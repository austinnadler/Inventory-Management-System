// File: main.cpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "GMItem.h"
#include "ParishableItem.h"
#include "AgeRestrictedItem.h"

using namespace std;
using file_status_t = bool;
using num_items_t = int;
using total_price_t = double;

const int MAX_SIZE = 500;
const double TAX_RATE = 0.071; // Illinois 7.1%

void testFileIOandPricing(); // Original main method for testing file reading and writing 
void performAdminFunctions();
// ADD: sorting by name and number when printing or writing items
//writeItems and printItems output the items in the order they were added to the array
void writeItems(ofstream& ofs, GMItem * cart[], const int& numItems); // To file, includes expiration, age restrictions, etc. formatted for a csv file
void printItems(GMItem * cart[], const int& numItems);                // To screen, displays only code, name, and price with periods for spacing
void printAdminInfo(GMItem * cart[], const int& numItems); // outputs list with all special information
void writeAdminInfo(ofstream& ofs, GMItem * cart[], const int& numItems); //To file, all admin info. works as keeping a log of current inventory
void checkout(); // create a new array of pointers and put the items that you want to buy into it. Program totals the purchase and adds tax
void printPOSPriceSection(GMItem * items[], const int& numItems);// print subtotal, taxes, and total
void printPOSHeader(); // print the header: CODE    PRICE   .... and so on

total_price_t calculateTax(const double& subTotal);
total_price_t calculateSubtotal(GMItem items[], ParishableItem parsihableItems[], const int& numGMItems, const int& numParishableItems);
total_price_t calculateTotal(double& subTotal);

num_items_t loadItemsFromFile(ifstream& ifs, GMItem * items[]); // counts passed by reference so they can both be changed
file_status_t openFileIn(ifstream& ifs, const string& fileName);
file_status_t openFileOut(ofstream& ofs, const string& fileName);

GMItem * items [MAX_SIZE];

ifstream ifs;
ofstream ofs;

string inFileName = "items.txt";
string outFileName = "itemsOut.csv";

int numGMItems = 0;
int numParishableItems = 0;

int main() {
    // testFileIOandPricing();
    // checkout();
    performAdminFunctions();
    return 0;
}// end main()



total_price_t calculateSubtotal(GMItem * items[], const int& numItems){
    total_price_t total = 0;
    for(int i = 0; i < numItems; i++) {
        total += items[i]->getItemPrice();
    }
    return total;
}// end calculateSubtotal()



total_price_t calculateTotal(double& subtotal){
    return subtotal + calculateTax(subtotal);
}// end calculateTotal()



total_price_t calculateTax(const double& subTotal) {
    total_price_t taxes = subTotal * TAX_RATE;
    return taxes;
}// end calculateTax()



num_items_t loadItemsFromFile(ifstream& ifs, GMItem * gmItems[]) {
    string itemType;
    string name;
    string price;
    string qtyOnHand;
    string expirationDate;
    string code;
    string minAge;
    num_items_t numItems = 0;

    using type_id_t = string;
    type_id_t ageRestricted = "ar";
    type_id_t parishable = "pa";
    type_id_t general = "gm";

    getline(ifs, itemType, ',');
    while(!ifs.eof()) {
        if(itemType == parishable) {
            getline(ifs, expirationDate, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            items[numItems++] = new ParishableItem(expirationDate, name, stod(price), stoi(qtyOnHand), stoi(code));
            getline(ifs, itemType, ',');
        } else if (itemType == general) {
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            items[numItems++] = new GMItem(name, stod(price), stoi(qtyOnHand), stoi(code));
            getline(ifs, itemType, ',');
        } 
        else if (itemType == ageRestricted) {
            getline(ifs, minAge, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            items[numItems++] = new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(qtyOnHand), stoi(code));
            getline(ifs, itemType, ',');
        }
    }
    return numItems;
}// end loadItemsFromFile()



void writeItems(ofstream& ofs, GMItem * items[], const int& numItems) {
    for(int i = 0; i < numItems; i++) {
        ofs << items[i]->toStringFile() << endl;
    }
    ofs.close();
}// end writeItems()



void printItems(GMItem * items[], const int& numItems) {
    for(int i = 0; i < numItems; i++) {
        cout  << items[i]->toStringPOS() << endl;
    }
}// end printItems()



file_status_t openFileIn(ifstream& ifs, const string& fileName) {
    ifs.open(fileName);
    return ifs.is_open();
}// end openFileIn()



file_status_t openFileOut(ofstream& ofs, const string& fileName){
    ofs.open(fileName);
    return ofs.is_open();
}// end openFileOut()



void checkout() {
    ostringstream oss;
    string codeString;
    int code = 0;
    int numItemsInCart = 0;
    double subTotal = 0;
    bool foundItem = false;
    GMItem * cart[MAX_SIZE];

    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    }
    num_items_t numItems = loadItemsFromFile(ifs, items); 

    do { 
        foundItem = false;
        if(numItemsInCart == MAX_SIZE) {
            cout << endl << "Maximum cart sized reached. Totaling the order..." << endl << endl;
                    printPOSHeader();
                    printItems(cart, numItemsInCart);
                    printPOSPriceSection(cart, numItemsInCart);
            cout << endl;
            return;
        }                        

        cout << "Enter product code (This code should be only numbers.) (Type \"total\" when done entering items): ";
        getline(cin, codeString);
        if(codeString == "total") {
            cout << endl;
                    printPOSHeader();
                    printItems(cart, numItemsInCart);
                    subTotal = calculateSubtotal(cart, numItemsInCart);  
                    printPOSPriceSection(cart, numItemsInCart);
                    for(int i = 0; i < numItemsInCart; i++) {
                        cart[i]->decreaseCount();
                    }
            cout << endl;
        }

        try {
            code = stoi(codeString);
            for(int i = 0; i < numItems; i++) {
                if(code == items[i]->getItemCode()) {
                    cart[numItemsInCart] = items[i];

                    // Run Time Type Idenification used to determine if the item is age restricted or not and take steps to verify age

                    GMItem *gm = items[i];
                    AgeRestrictedItem *ar = dynamic_cast<AgeRestrictedItem*>(gm);
                    if(ar != NULL) {
                        string valid;
                        cout << "This is an age restricted item. Verify with a valid photo ID." << endl;
                        cout << "Then enter 'y' if the customer is at least " << ar->getMinAge() << " years or older, or 'n': ";
                        getline(cin, valid);
                        if(valid == "y") {
                            oss << cart[numItemsInCart]->toStringPOS();
                            subTotal += cart[numItemsInCart++]->getItemPrice();
                            foundItem = true;
                        } else {
                            cout << "Customer cannot purchase this item." << endl;
                        }
                    } else {
                        oss << cart[numItemsInCart]->toStringPOS();
                        subTotal += cart[numItemsInCart++]->getItemPrice();
                        foundItem = true;
                    }
                } else { foundItem = false; } 
            }
        // add something to catch and notify if code wasn't found
        
        } catch(exception e) {
            cout << "Invalid Code Entered!" << endl;
        }

        cout << endl;
                printPOSHeader();
                printItems(cart, numItemsInCart);
            cout << endl;
                subTotal = calculateSubtotal(cart, numItemsInCart);     
                printPOSPriceSection(cart, numItemsInCart);
        cout << endl;
        
    } while (codeString != "total" && numItemsInCart < MAX_SIZE);
}   



void printPOSHeader() {
    cout << left << "CODE\t" << "NAME" <<  right << setw(27) << "PRICE" << setw(10) << "MINIMUM AGE" << endl;
}



void testFileIOandPricing() {
    // cout << "Enter the input file name with extension: ";
    // getline(cin, inFileName);
    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    }
    num_items_t numItems = loadItemsFromFile(ifs, items); 

    // cout << "Enter the output file name with extension: ";
    // getline(cin, outFileName);
    file_status_t fOutStatus = openFileOut(ofs, outFileName);
    if(!fOutStatus) {
        cout << "Error opening file \"" << outFileName << "\"" << endl;
        exit(1);
    }

    ofs << "Code,Name,Price,On Hand,Expiration" << endl;
    writeItems(ofs, items, numItems);
    printAdminInfo(items, numItems);
    printPOSPriceSection(items, numItems);    
}



void printPOSPriceSection(GMItem * items[], const int& numItems) {
    total_price_t subTotal = calculateSubtotal(items, numItems);
    total_price_t taxes = calculateTax(subTotal);
    total_price_t totalPrice = calculateTotal(subTotal);

    cout << "Total number of items: " << numItems << endl
         << "Subtotal: $" << subTotal << endl
         << "Tax: $" << fixed << setprecision(2) << taxes << " (at " << TAX_RATE*100 << "%)" << endl
         << "Total: $" << fixed << setprecision(2) << totalPrice << endl;
}



void printAdminInfo(GMItem * cart[], const int& numItems) {
     for(int i = 0; i < numItems; i++) {
        cout << "----------+---------------------+-----------+--------+-------------------------" << endl;
        cout  << items[i]->toStringAdmin() << endl;
    }
}

void performAdminFunctions() {
    string input;
    string fileName = "items.txt";
    int iCode;
    bool found = false;

    // cout << "Enter the inventory lsti file name: ";
    // getline(cin, fileName);

    cout << "Opening file " << fileName << "..." << endl;
    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    } else {
        cout << "File " << fileName << " opened succesfully" << endl;
    }

    num_items_t numItems = loadItemsFromFile(ifs, items); 
    // do {
        printAdminInfo(items, numItems);
        cout << "----------+---------------------+-----------+--------+-------------------------" << endl;

    //     cout << "Enter the number of the item you want to edit. Enter 'exit' to quit." << endl;

    //     if(input == "quit") {
    //         return;
    //     }

    //     try {
    //         getline(cin, input);
    //         iCode = stoi(input);
    //     } catch (exception e) {
    //         cout << "Your input was not recognized." << endl;
    //     }

    //     for(int i = 0; i < numItems && found == false; i++) {
    //         if(items[i]->getItemCode() == iCode) {
    //             cout << "Item found: " << items[i]->getItemName() << endl;
    //             cout << items[i]->toStringAdmin() << endl;
    //             found = true;
    //             int theOne = i;
    //             cout << "1. Change price"
    //                  << "2. Change number on hand"
    //                  << "3. Delete item from system"
    //                  << "Enter the number of the action you want to perform: ";
    //             getline(cin,input);
                
    //         }
    //     }
        
    // } while(input != "exit");
}