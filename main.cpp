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
// FIX: writeItems needs to be updated to print the different types of items in their own tables to seperate the special fields
void writeItems(ofstream& ofs, GMItem * cart[], const int& numItems); // To file, includes expiration, age restrictions, etc. formatted for a csv file
void printItems(GMItem * cart[], const int& numItems);                // To screen, displays only code, name, and price with periods for spacing
void writeBack(ofstream& ofs, GMItem * items[], const int& numItems);
void printAdminInfo(GMItem * cart[], const int& numItems); // outputs list with all special information
void writeAdminInfo(ofstream& ofs, GMItem * cart[], const int& numItems); //To file, all admin info. works as keeping a log of current inventory
void checkout(); // create a new array of pointers and put the items that you want to buy into it. Program totals the purchase and adds tax
void printPOSPriceSection(GMItem * items[], const int& numItems);// print subtotal, taxes, and total
void printPOSHeader(); // print the header: CODE    NAME   .... and so on
void sortItemsByName(GMItem * items[], const int& numItems);
void sortItemsByCode(GMItem * items[], const int& numItems);
void addItemToList(GMItem * items[], const int& numItems, GMItem * newItem);

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


// Take the array of items and go through it adding all of the prices up and return it as a double
total_price_t calculateSubtotal(GMItem * items[], const int& numItems){
    total_price_t total = 0;
    for(int i = 0; i < numItems; i++) {
        total += items[i]->getItemPrice();
    }
    return total;
}// end calculateSubtotal()


// Take the subtotal and calculate the tax, add it to the subtotal and return it as a double
total_price_t calculateTotal(double& subtotal){
    return subtotal + calculateTax(subtotal);
}// end calculateTotal()


// Take the subTotal and calculate the tax that needs to be added based on the constant declared TAX_RATE. Program doesn't support multiple rates but could be implimented later
total_price_t calculateTax(const double& subTotal) {
    total_price_t taxes = subTotal * TAX_RATE;
    return taxes;
}// end calculateTax()


// Take an ifs and an empty array and fill said array with items from a FORMATTED file
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


// Take a provided ofstream and array of items and write them to the ofstream using the toStringFile() method
void writeItems(ofstream& ofs, GMItem * items[], const int& numItems) {
    for(int i = 0; i < numItems; i++) {
        ofs << items[i]->toStringFile() << endl;
    }
    ofs.close();
}// end writeBack()


void writeBack(ofstream& ofs, GMItem * items[], const int& numItems) {
    for(int i = 0; i < numItems; i++) {
        ofs << items[i]->toStringBack() << endl;
    }
    ofs.close();
}// end writeItems()


// Take a provided array of items and print it to the screen using the toStringPOS() method
void printItems(GMItem * items[], const int& numItems) {
    for(int i = 0; i < numItems; i++) {
        cout  << items[i]->toStringPOS() << endl;
    }
}// end printItems()


// use provided ifstream to open provided filename to read information from
file_status_t openFileIn(ifstream& ifs, const string& fileName) {
    ifs.open(fileName);
    return ifs.is_open();
}// end openFileIn()


// use provided ofstream to open provided filename to write data to
file_status_t openFileOut(ofstream& ofs, const string& fileName){
    ofs.open(fileName);
    return ofs.is_open();
}// end openFileOut()


// a pos system for selling inventory. 
//FIX: items are currently removed from inventory as they are added to the list, and not as they are actually sold
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

                    // RTTI Run Time Type Idenification used to determine if the item is age restricted or not and take steps to verify age

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


// print the formatted POS header
void printPOSHeader() {
    cout << left << "CODE\t" << "NAME" <<  right << setw(26) << "PRICE" << endl;
}


// test some of the base functions -- doesn't include subclass info just code, name, price
void testFileIOandPricing() {
    // cout << "Enter the input file name with extension: ";
    // getline(cin, inFileName);
    printPOSHeader();
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
    sortItemsByName(items, numItems);
    printItems(items, numItems);
    printPOSPriceSection(items, numItems);   
}


// do the end of the transaction totaling and outputting all at once given the array of items and actual size size
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
        cout << "----------|---------------------|-----------|--------|-------------------------" << endl;
        cout  << items[i]->toStringAdmin() << endl;
    }
}



void performAdminFunctions() {
    string input;
    string thisInfileName = inFileName;
    string thisOutFileName = "newlog.txt";
    bool found = false;
    bool validIn;

    // cout << "Enter the inventory list file name with extension: ";
    // getline(cin, thisInfileName);

    cout << "Opening file " << thisInfileName << "..." << endl;
    file_status_t fInStatus = openFileIn(ifs, thisInfileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << thisInfileName << "\"" << endl;
        exit(1);
    } else {
        cout << "File " << thisInfileName << " opened succesfully" << endl;
    }

    // cout << "Enter the name of your outfile: ";
    // getline(cin, thisOutFileName);

    file_status_t fOutStatus = openFileOut(ofs, thisOutFileName);
    if(!fOutStatus) {
        cout << "Error opening file \"" << thisOutFileName << "\"" << endl;
        exit(1);
    }

    num_items_t numItems = loadItemsFromFile(ifs, items); 
    do {
        cout << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(items, numItems);
        cout << "----------|---------------------|-----------|--------|-------------------------" << endl;





    cout << "1. Manage current inventory" << endl
         << "2. Add item" << endl
         << "Enter your choice: ";
    getline(cin, input);
    if(input == "1") {
            cout << "Enter the number of the item you want to edit. Enter 'exit' to quit." << endl;

            if(input == "exit") {
                return;
            }

            getline(cin, input);
            for(int i = 0; i < numItems && found == false; i++) {
                if(items[i]->getItemCode() == stoi(input)) {
                    found = true;
                    cout << endl 
                        << "Item found: " << items[i]->getItemName() << endl << endl
                        << "1. Change number on hand" << endl
                        << "2. Change price" << endl
                        << "3. Change item name" << endl
                        << "4. Change item code" << endl
                        << "5. Change item expiration date" << endl
                        << "6. Change item minimum age" << endl
                        << "Enter the number of the action you want to perform: ";
                    getline(cin,input);
                    if(input == "1") {
                                        cout << endl
                                            << "1. Add to the current count" << endl
                                            << "2. Subtract from the current count" << endl
                                            << "3. Enter a completely new count" << endl
                                            << "Enter the number of the action you want to perform: ";
                                        getline(cin, input);
                                        if(input == "1") {
                                            cout << "Enter the number by which you want to increase the on hand count: ";
                                            getline(cin, input);
                                            
                                            validIn = items[i]->increaseCount(input);
                                            while(!validIn) {
                                                cout << "Couldn't convert to an int, enter 0 to exit this command or enter a new integer value: ";
                                                getline(cin, input);
                                                validIn = items[i]->increaseCount(input);
                                            }  
                                        } else if (input == "2") {
                                            cout << "Enter the number by which you want to decrease the on hand count: ";
                                            getline(cin, input);
                                            validIn = items[i]->decreaseCount(input);
                                            while(!validIn) {
                                                cout << "Couldn't convert to an int, enter 0 to exit this command or enter a new integer value: ";
                                                getline(cin, input);
                                                validIn = items[i]->decreaseCount(input);
                                            }             
                                        } else if(input == "3") {
                                            cout << "Enter the new on hand count: ";
                                            getline(cin, input);
                                            validIn = items[i]->setNumOnHand(input);
                                            while(!validIn) {
                                                cout << "Couldn't convert to an int, or the new number was too large or too small." << endl
                                                    << "Enter 0 to exit this command or enter a new integer value: ";
                                                getline(cin, input);
                                                validIn = items[i]->setNumOnHand(input);
                                            }  
                                        }
                    } else if(input == "2") {
                                            cout << "Enter the new price (ex. 123456.89): ";
                                            getline(cin, input);
                                            validIn = items[i]->setItemPrice(input);
                                            while(!validIn) {
                                                cout << "Couldn't convert to a double" << endl
                                                    << "Enter a new integer value or 0 to exit this command: ";
                                                getline(cin, input);
                                                validIn = items[i]->setItemPrice(input);
                                            }                       
                    } else if(input == "3") {
                                            cout << "Enter the new product name: ";
                                            getline(cin, input);
                                            validIn = items[i]->setItemName(input);
                                            while(!validIn) {
                                                cout << "That name was too long, max length is " << items[i]->getMaxNameLength() << " characters long" << endl
                                                    << "Enter a new name: ";
                                                getline(cin, input);
                                                validIn = items[i]->setItemName(input);
                                            }
                    } else if (input == "4") {
                                            cout << "Enter the new item code: ";
                                            getline(cin, input);
                                            validIn = items[i]->setItemCode(input);
                                            while(!validIn) {
                                                cout << "That code couldn't be converted to an integer" << endl
                                                    << "Enter a new code: ";
                                                getline(cin, input);
                                                validIn = items[i]->setItemName(input);
                                            }
                    } else if(input == "5") {
                                        //More RTTI to check if the item has an expiration before trying to change it.
                                            GMItem *gm = items[i];
                                            ParishableItem *pa = dynamic_cast<ParishableItem*>(gm);
                                            if(pa != NULL) {
                                                cout << "Enter the new minimum age: ";
                                                getline(cin, input);
                                                validIn = pa->setExpirationDate(input);
                                                while(!validIn) {
                                                    cout << "That expiration date was too long. The max expiration date length is " << pa->getMaxDateLength() << " characters long." << endl
                                                        << "Enter the new minimum age or 0 to exit this command: ";
                                                    getline(cin, input);
                                                    validIn = pa->setItemName(input);
                                                }
                                            } else {
                                                cout << "This item does not have an expiration date. Delete it and recreate it if it needs one, or use a physical label." << endl;
                                            }
                    } else if(input == "6") {
                                        //More RTTI to check if the item has an expiration before trying to change it.
                                            GMItem *gm = items[i];
                                            AgeRestrictedItem *ar = dynamic_cast<AgeRestrictedItem*>(gm);
                                            if(ar != NULL) {
                                                cout << "Enter the new minimum age: ";
                                                getline(cin, input);
                                                validIn = ar->setMinAge(input);
                                                while(!validIn) {
                                                    cout << "Couldn't convert that to an int. " << endl
                                                        << "Enter the new minimum age or 0 to exit this command: ";
                                                    getline(cin, input);
                                                    validIn = ar->setItemName(input);
                                                }
                                            } else {
                                                cout << "This item does not have a minimum age. Delete it and recreate it if it needs one. DO NOT RELY ON A PHYSICAL LABEL" << endl;
                                            }

                    }
                }
            }
            found = false;
            writeBack(ofs, items, numItems);
    } else if(input == "2") {
        cout << "a. A GMItem with no special characteristics." << endl
             << "b. A ParishableItem with an expiration date to be stored." << endl
             << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl;
        getline(cin, input);
        bool valid;
        if(input == "a") {

                            GMItem * item;
                            cout << "Enter the item code: ";
                            getline(cin, input);
                            valid = item->setItemCode(input);
                            while(!valid) {
                                cout << "Invalid code, couldn't be converted to an int" << endl;
                                cout << "Enter new code: ";
                                getline(cin, input);
                                item ->setItemCode(input);
                            }
                            
                            cout << "Enter the item name: ";
                            getline(cin, input);
                            valid = item->setItemName(input);
                            while(!valid) {
                                cout << "Invalid name, too long" << endl;
                                cout << "Enter new name: ";
                                getline(cin, input);
                                item ->setItemName(input);
                            }

                            cout << "Enter the item price: ";
                            getline(cin, input);
                            valid = item->setItemPrice(input);
                            while(!valid) {
                                cout << "Invalid price, couldn't be converted to a double" << endl;
                                cout << "Enter new price: ";
                                getline(cin, input);
                                item ->setItemPrice(input);
                            }

                            cout << "Enter the qty on hand: ";
                            getline(cin, input);
                            valid = item->setNumOnHand(input);
                            while(!valid) {
                                cout << "Invalid quantity, couldn't be converted to an int" << endl;
                                cout << "Enter new quantity: ";
                                getline(cin, input);
                                item ->setNumOnHand(input);
                            } 
                            numItems++;
                            items[numItems] = item;

        } else if (input == "b") {

                            


        } else if(input == "c") {

                            

        }
    }
        } while(input != "exit");
        cout << "Exiting..." << endl;
}   


// Insertion sort used 
void sortItemsByName(GMItem * items[], const int& numItems) {
    int i, j; 
    GMItem * key;
    for (i = 1; i < numItems; i++) { 
       key = items[i]; 
       j = i-1; 
       while (j >= 0 && items[j]->getItemName() > key->getItemName()) 
       { 
           items[j+1] = items[j]; 
           j = j-1; 
       } 
       items[j+1] = key; 
   } 
}



void sortItemsByCode(GMItem * items[], const int& numItems) {
    int i, j; 
    GMItem * key;
    for (i = 1; i < numItems; i++) { 
       key = items[i]; 
       j = i-1; 
       while (j >= 0 && items[j]->getItemCode() > key->getItemCode()) 
       { 
           items[j+1] = items[j]; 
           j = j-1; 
       } 
       items[j+1] = key; 
   } 
}