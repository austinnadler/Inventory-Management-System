// File: main.cpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "GMItem.h"
#include "ExpiringItem.h"
#include "AgeRestrictedItem.h"

using namespace std;
using file_status_t = bool;
using num_items_t = int;
using total_price_t = double;

const int MAX_SIZE = 5000;

const double TAX_RATE = 0.071; // Illinois 7.1%

void testFileIOandPricing(); // Original main method for testing file reading and writing 
void performAdminFunctions();
void checkout(); // create a new array of pointers and put the items that you want to buy into it. Program totals the purchase and adds tax
// ADD: sorting by name and number when printing or writing items
// FIX: writeItems needs to be updated to print the different types of items in their own tables to seperate the special fields
void writeItems(ofstream& ofs, GMItem * cart[], const int& numItems); // To file, includes expiration, age restrictions, etc. formatted for a csv file
void writeBack(ofstream& ofs, GMItem * items[], const int& numItems);
void writeAdminInfo(ofstream& ofs, GMItem * cart[], const int& numItems); //To file, all admin info. works as keeping a log of current inventory

void printItems(GMItem * cart[], const int& numItems);                // To screen, displays only code, name, and price with periods for spacing
void printAdminInfo(GMItem * cart[], const int& numItems); // outputs list with all special information
void printPOSPriceSection(GMItem * items[], const int& numItems);// print subtotal, taxes, and total
void printPOSHeader(); // print the header: CODE    NAME   .... and so on

void sortItemsByName(GMItem * items[], const int& numItems);
void sortItemsByCode(GMItem * items[], const int& numItems);

bool addItemToList(GMItem * items[], int& numItems, GMItem * newItem);
bool removeLastItem(GMItem * items[], int& numItems);

total_price_t calculateTax(const double& subTotal);
total_price_t calculateTotal(double& subTotal);

num_items_t loadItemsFromFile(ifstream& ifs, GMItem * items[]); // counts passed by reference so they can both be changed
file_status_t openFileIn(ifstream& ifs, const string& fileName);
file_status_t openFileOut(ofstream& ofs, const string& fileName);

string inFileName = "items.txt";
string outFileName = "itemsOut.csv";

int numGMItems = 0;
int numExpiringItems = 0;

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
num_items_t loadItemsFromFile(ifstream& ifs, GMItem * items[]) {
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
    type_id_t warning = "warn";
    type_id_t general = "gm";

    getline(ifs, itemType, ',');
    while(!ifs.eof()) {
        if(itemType == warning) {
            getline(ifs, expirationDate, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            items[numItems++] = new ExpiringItem(expirationDate, name, stod(price), stoi(qtyOnHand), stoi(code));
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
    GMItem * inventory[MAX_SIZE];
    GMItem * cart[MAX_SIZE];
    ifstream ifs;
    ofstream ofs;

    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    }
    num_items_t numItems = loadItemsFromFile(ifs, inventory); 

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
                    printPOSPriceSection(cart, numItemsInCart);
                    for(int i = 0; i < numItemsInCart; i++) {
                        cart[i]->decreaseCount();
                    }
            cout << endl;
        } else if (codeString == "undo") {
                if(numItemsInCart < 1) {
                    cout << endl << "The list is already empty!" << endl;
                } else {
                    removeLastItem(cart, numItemsInCart);
                    printPOSHeader();
                    printItems(cart, numItemsInCart);
                    printPOSPriceSection(cart, numItemsInCart);
                }

        } else {
                try {
                    code = stoi(codeString);
                    for(int i = 0; i < numItems; i++) {
                        
                        // FIX: This code does not work
                        // if (foundItem == false && i == numItems) { 
                        //     cout << "No item with that code was found in the inventory file." << endl;
                        // } 

                        if(code == inventory[i]->getItemCode()) {
                            cart[numItemsInCart] = inventory[i];

                            // RTTI Run Time Type Idenification used to determine if the item is age restricted or not and take steps to verify age

                            GMItem *gm = inventory[i];
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
                        }                     
                    }
                } catch(exception e) {
                    cout << "Invalid Code Entered!" << endl;
                }
        }

        cout << endl;
                printPOSHeader();
                printItems(cart, numItemsInCart);
            cout << endl;
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
    ifstream ifs;
    ofstream ofs;
    GMItem * items[MAX_SIZE];
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



void printAdminInfo(GMItem * items[], const int& numItems) {
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
    ifstream ifs;
    ofstream ofs;
    GMItem * inventory[MAX_SIZE];

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

    num_items_t numItems = loadItemsFromFile(ifs, inventory); 
    do {
        cout << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(inventory, numItems);
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
                if(inventory[i]->getItemCode() == stoi(input)) {
                    GMItem * itemPtr = inventory[i];
                    found = true;
                    cout << endl 
                        << "Item found: " << itemPtr->getItemName() << endl << endl
                        << "1. Change number on hand" << endl
                        << "2. Change price" << endl
                        << "3. Change item name" << endl
                        << "4. Change item code" << endl
                        << "5. Change item warning prompt" << endl
                        << "6. Change item minimum age" << endl
                        << "Enter the number of the action you want to perform: ";    
                    getline(cin,input);

                    if(input == "1") {
                                       cout << endl
                                            << "a. Add to the current count" << endl
                                            << "b. Subtract from the current count" << endl
                                            << "c. Enter a completely new count" << endl
                                            << "Enter the number of the action you want to perform: ";
                                        getline(cin, input);
                                        if(input == "a") {
                                            do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                                getline(cin, input);
                                                validIn = itemPtr->increaseCount(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                                        } else if (input == "b") {
                                            do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                                getline(cin, input);
                                                validIn = itemPtr->decreaseCount(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;     
                                        } else if(input == "c") {
                                            do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = itemPtr->setNumOnHand(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                                        }
                    } else if(input == "2") {
                                            do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Enter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = itemPtr->setItemPrice(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;                    
                    } else if(input == "3") {
                                             do {
                                                cout << "Enter 'exit' to quit. The name can be anything, the shorter the better." << endl;
                                                cout << "Enter the new item namefor item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = itemPtr->setItemName(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                    } else if (input == "4") {
                                             do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Enter new code for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = itemPtr->increaseCount(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                    } else if(input == "5") {
                                        //More RTTI to check if the item has an expiration before trying to change it.
                                        ExpiringItem * ex = dynamic_cast<ExpiringItem*>(itemPtr);
                                        if(ex != NULL) {
                                             do {
                                                cout << "Enter 'exit' to quit. Warning should be 20 characters or shorter and on one line." << endl;
                                                cout << "Enter the new warning for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = ex->setWarning(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                                        } else {
                                            cout << "This item doesn't have a warning. Warnings must be 20 characters or shorer." << endl
                                                 << "Enter 'add' to add one, or anything else to back out: ";
                                            getline(cin, input);
                                            if(input == "add") {
                                                do {
                                                    cout << "Enter the new minimum age: ";
                                                    getline(cin, input);
                                                    ExpiringItem * newEx = new ExpiringItem;
                                                    bool testr = newEx->setWarning(input);
                                                    if(testr) {
                                                        newEx = new ExpiringItem(input, itemPtr->getItemName(), itemPtr->getItemPrice(), itemPtr->getNumOnHand(), itemPtr->getItemCode());
                                                        inventory[i] = newEx;
                                                        validIn = true;
                                                    } else {
                                                        cout << "Invalid input: " << input << endl;
                                                    }
                                                } while (!validIn && input != "exit");
                                            }
                                        }
                    } else if(input == "6") {
                                        //More RTTI to check if the item has an expiration before trying to change it.
                                            AgeRestrictedItem * ar = dynamic_cast<AgeRestrictedItem*>(itemPtr);
                                        if(ar != NULL) {
                                             do {
                                                cout << "Enter 'exit' to quit. Only integer values accepted." << endl;
                                                cout << "Enter the new minmum age for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                validIn = ar->setMinAge(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            } while(!validIn && input != "exit");
                                            validIn = false;
                                        } else {
                                            cout << "This item doesn't have a minimum age." << endl
                                                 << "Enter 'add' to add one, or anything else to back out: ";
                                            getline(cin, input);
                                            if(input == "add") {
                                                do {
                                                    cout << "Enter the new minimum age: ";
                                                    getline(cin, input);
                                                    AgeRestrictedItem * newAr = new AgeRestrictedItem;
                                                    bool testr = newAr->setMinAge(input);
                                                    if(testr) {
                                                        newAr = new AgeRestrictedItem(stoi(input), itemPtr->getItemName(), itemPtr->getItemPrice(), itemPtr->getNumOnHand(), itemPtr->getItemCode());
                                                        inventory[i] = newAr;
                                                        validIn = true;
                                                    } else {
                                                        cout << "Invalid input: " << input << endl;
                                                    }
                                                } while (!validIn && input != "exit");
                                            }
                                        }
                    }
                }
            }
            found = false;
            writeBack(ofs, inventory, numItems);
    } else if(input == "2") {
        cout << "a. A GMItem with no special characteristics." << endl
             << "b. A ExpiringItem with an expiration date to be stored." << endl
             << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl;
        getline(cin, input);
        bool valid;
        string code, name, price;
        if(input == "a") {




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


bool removeLastItem(GMItem * items[], int& numItems) {
    if(numItems < 1) {
        return false;
    } else {
        items[numItems] = nullptr;
        numItems--;
        return true;
    }
}


bool addItemToList(GMItem * items[], int& numItems, GMItem * newItem) {
    if(numItems == MAX_SIZE - 1) {
        return false;
    } else {
        items[numItems + 1] = newItem;
        numItems++;
        return true;
    }
}

