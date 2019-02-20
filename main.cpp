// File: main.cpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "GMItem.h"
#include "ExpiringItem.h"
#include "AgeRestrictedItem.h"

using namespace std;
using file_status_t = bool;
using num_items_t = int;
using total_price_t = double;

const int MAX_SIZE = 500;

const double TAX_RATE = 0.071; // Illinois 7.1%

void testFileIOandPricing(); // Original main method for testing file reading and writing 
void performAdminFunctions();
void checkout(); // create a new array of pointers and put the items that you want to "buy" into it. Program totals the purchase and adds tax.

// FIX: writeItems needs to be updated to print the different types of items in distinct sections.
// FIX: add something to either verify that item numbers are uniqe, or present user with all of the items with that code and go from there.
void writeItems(ofstream& ofs, vector<GMItem*> items); // To file, includes expiration, age restrictions, etc. formatted for a csv file
void writeBack(ofstream& ofs, vector<GMItem*> items);

// Functions created to increase readability in performAdminFunctions()
void changeCount(GMItem * itemPtr);
void changeName(GMItem * itemPtr);
void changePrice(GMItem * itemPtr);
void changeCode(GMItem * itemPtr);
void changeWarning(GMItem * itemPtr);
void changeMinAge(GMItem * itemPtr);

void printItems(vector<GMItem*> items);                // To screen, displays only code, name, and price with periods for spacing
void printAdminInfo(vector<GMItem*> items); // outputs list with all special information
void printAdminSep() { cout << "----------|------------|---------------------|-----------|--------|--------------" << endl; };
void printPOSPriceSection(vector<GMItem*> items);// print subtotal, taxes, and total
void printPOSHeader(); // print the header: CODE    NAME   .... and so on

void sortItemsByName(vector<GMItem*> items);
void sortItemsByCode(vector<GMItem*> items);

bool addItemToList(vector<GMItem*> items, GMItem * newItem);

total_price_t calculateTax(const double& subTotal);
total_price_t calculateTotal(double& subTotal);

void loadItemsFromFile(ifstream& ifs, vector<GMItem*> &items); 
file_status_t openFileIn(ifstream& ifs, const string& fileName);
file_status_t openFileOut(ofstream& ofs, const string& fileName);

string inFileName = "items.txt";
string outFileName = "itemsOut.csv";

int main() {
    // testFileIOandPricing();
    // checkout();
    performAdminFunctions();
    return 0;
}// end main()


// Take the array of items and go through it adding all of the prices up and return it as a double
total_price_t calculateSubtotal(vector<GMItem*> items){
    total_price_t total = 0;
    for(int i = 0; i < items.size(); i++) {
        total += items.at(i)->getItemPrice();
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
void loadItemsFromFile(ifstream& ifs, vector<GMItem*> &items) {
    string itemType;
    string name;
    string price;
    string qtyOnHand;
    string expirationDate;
    string code;
    string minAge;

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

            ExpiringItem * ex = new ExpiringItem(expirationDate, name, stod(price), stoi(qtyOnHand), stoi(code));
            items.push_back(ex);
            getline(ifs, itemType, ',');
        } else if (itemType == general) {
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            GMItem * gm = new GMItem(name, stod(price), stoi(qtyOnHand), stoi(code));
            items.push_back(gm);
            getline(ifs, itemType, ',');
        } 
        else if (itemType == ageRestricted) {
            getline(ifs, minAge, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            AgeRestrictedItem * ar = new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(qtyOnHand), stoi(code));
            items.push_back(ar);
            getline(ifs, itemType, ',');
        }
    }
}// end loadItemsFromFile()


// Take a provided ofstream and array of items and write them to the ofstream using the toStringFile() method
void writeItems(ofstream& ofs, vector<GMItem*> items) {
    for(int i = 0; i < items.size(); i++) {
        ofs << items.at(i)->toStringFile() << endl;
    }
    ofs.close();
}// end writeBack()


void writeBack(ofstream& ofs, vector<GMItem*> items) {
    for(int i = 0; i < items.size(); i++) {
        ofs << items.at(i)->toStringBack() << endl;
    }
    ofs.close();
}// end writeItems()


// Take a provided array of items and print it to the screen using the toStringPOS() method
void printItems(vector<GMItem*> items) {
    for(int i = 0; i < items.size(); i++) {
        cout  << items.at(i)->toStringPOS() << endl;
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
    vector<GMItem*> inventory;
    vector<GMItem*> cart;
    ifstream ifs;
    ofstream ofs;

    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    }
    loadItemsFromFile(ifs, inventory); 

    do { 
        foundItem = false;
        if(numItemsInCart == MAX_SIZE) {
            cout << endl << "Maximum cart sized reached. Totaling the order..." << endl << endl;
                    printPOSHeader();
                    printItems(cart);
                    printPOSPriceSection(cart);
            cout << endl;
            return;
        }                        

        cout << "Enter product code (This code should be only numbers.) (Type \"total\" when done entering items): ";
        getline(cin, codeString);
        if(codeString == "total") {
            cout << endl;
                    printPOSHeader();
                    printItems(cart);
                    printPOSPriceSection(cart);
                    for(int i = 0; i < numItemsInCart; i++) {
                        cart.at(i)->decreaseCount();
                    }
            cout << endl;
        } else if (codeString == "undo") {
                if(cart.size() < 1) {
                    cout << endl << "The list is already empty!" << endl;
                } else {
                    cart.pop_back();
                    printPOSHeader();
                    printItems(cart);
                    printPOSPriceSection(cart);
                }

        } else {
            try {
                code = stoi(codeString);
                for(int i = 0; i < inventory.size(); i++) {
                        
                        // FIX: This code does not work
                        // if (foundItem == false && i == numItems) { 
                        //     cout << "No item with that code was found in the inventory file." << endl;
                        // } 

                    if(code == inventory.at(i)->getItemCode()) {
                        cart.push_back(inventory.at(i));

                                    // RTTI Run Time Type Idenification used to determine if the item is age restricted or not and take steps to verify age

                        GMItem *gm = inventory.at(i);
                        AgeRestrictedItem *ar = dynamic_cast<AgeRestrictedItem*>(gm);
                        if(ar != NULL) {
                        string valid;
                        cout << "This is an age restricted item. Verify with a valid photo ID." << endl;
                        cout << "Then enter 'y' if the customer is at least " << ar->getMinAge() << " years or older, or 'n': ";
                        getline(cin, valid);
                            if(valid == "y") {
                                oss << cart.back()->toStringPOS();
                                subTotal += cart.back()->getItemPrice();
                                numItemsInCart++;
                                foundItem = true;
                            } else {
                                cout << "Customer cannot purchase this item." << endl;
                            }
                        } else {
                            oss << cart.back()->toStringPOS();
                            subTotal += cart.back()->getItemPrice();
                            numItemsInCart++;
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
                printItems(cart);
            cout << endl;
                printPOSPriceSection(cart);
        cout << endl;
        
    } while (codeString != "total" && numItemsInCart < MAX_SIZE);
}   


// print the formatted POS header
void printPOSHeader() {
    cout << left << "INDEX\t" << "CODE\t" << "NAME" <<  right << setw(26) << "PRICE" << endl;
}


// test some of the base functions -- doesn't include subclass info just code, name, price
void testFileIOandPricing() {
    ifstream ifs;
    ofstream ofs;
    vector<GMItem*> items;
    // cout << "Enter the input file name with extension: ";
    // getline(cin, inFileName);
    printPOSHeader();
    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    }
    loadItemsFromFile(ifs, items); 

    // cout << "Enter the output file name with extension: ";
    // getline(cin, outFileName);
    file_status_t fOutStatus = openFileOut(ofs, outFileName);
    if(!fOutStatus) {
        cout << "Error opening file \"" << outFileName << "\"" << endl;
        exit(1);
    }

    ofs << "Code,Name,Price,On Hand,Expiration" << endl;
    writeItems(ofs, items);
    sortItemsByName(items);
    printItems(items);
    printPOSPriceSection(items);   
}


// do the end of the transaction totaling and outputting all at once given the array of items and actual size size
void printPOSPriceSection(vector<GMItem*> items) {
    total_price_t subTotal = calculateSubtotal(items);
    total_price_t taxes = calculateTax(subTotal);
    total_price_t totalPrice = calculateTotal(subTotal);

    cout << "Total number of items: " << items.size() << endl
         << "Subtotal: $" << subTotal << endl
         << "Tax: $" << fixed << setprecision(2) << taxes << " (at " << TAX_RATE*100 << "%)" << endl
         << "Total: $" << fixed << setprecision(2) << totalPrice << endl;
}



void printAdminInfo(vector<GMItem*> items) {
     for(int i = 0; i < items.size(); i++) {
        printAdminSep;
        cout << setw(10) << left << i << "| " << items.at(i)->toStringAdmin() << endl;
    }
}



void performAdminFunctions() {
    string input;
    string thisInfileName = inFileName;
    string thisOutFileName = "newlog.txt";
    bool found = false;
    ifstream ifs;
    ofstream ofs;
    vector<GMItem*> inventory;

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

    loadItemsFromFile(ifs, inventory); 
    do {
        //FIX: Maybe use iomanip here, but i found it easier to just have this fixed and have the output be controlled by iomanip
        cout << "INDEX      CODE         NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(inventory);
        printAdminSep();

        cout << "1. Manage current inventory" << endl
             << "2. Add item" << endl
             << "3. Delete an item" << endl
             << "and at any time, you can enter 'exit' to quit the program." << endl
             << "Enter your choice: ";
        getline(cin, input);
        if(input == "1") {
                cout << "Enter index of the number of the item that you want to edit: ";
                getline(cin, input);

                if(input == "exit") {
                    return;
                }

                for(int i = 0; i < inventory.size() && found == false; i++) {
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
                            changeCount(itemPtr);
                        } else if(input == "2") {
                            changePrice(itemPtr);            
                        } else if(input == "3") {
                            changeName(itemPtr);         // These are all defined below to slim down paF()
                        } else if (input == "4") {
                            changeCode(itemPtr);
                        } else if(input == "5") {
                            changeWarning(itemPtr);
                        } else if(input == "6") {
                            changeMinAge(itemPtr);
                        }
                    }
                }
                found = false;
        } else if(input == "2") {
            cout << "All options unimplemented \na. A GMItem with no special characteristics." << endl
                << "b. A ExpiringItem with an expiration date to be stored." << endl
                << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl;
            getline(cin, input);
            bool valid;
            string code, name, price, numOnHand;
            if(input == "a") {
                do {
                    cout << "Enter item information as such <code,name,numberOnHand,price>";
                    getline(cin, code, ',');
                    getline(cin, name, ',');
                    getline(cin, numOnHand, ',');
                    getline(cin, price);
                    try {
                        inventory.push_back(new GMItem(name, stod(price), stoi(numOnHand), stoi(code)));
                        valid = true;
                    } catch (invalid_argument e) {
                        cout << "One or more arguments were invalid. Try again." << endl;
                        valid = false;
                    }
                } while(!valid);
            } else if (input == "b") {
                string code, name, price, numOnHand, warning;
                do {
                    cout << "Enter item information as follows <code,name,numberOnHand,price,20 char warning msg>" << endl
                         << "Enter item information: ";
                    getline(cin, code, ',');
                    getline(cin, name, ',');
                    getline(cin, numOnHand, ',');
                    getline(cin, price, ',');
                    getline(cin, warning);
                    try {
                        inventory.push_back(new ExpiringItem(warning, name, stod(price), stoi(numOnHand), stoi(code)));
                        valid = true;
                    } catch (exception e) {
                        cout << "One or more arguments were invalid. Try again." << endl;
                        valid = false;
                    }
                } while(!valid);                               
            } else if(input == "c") {
                string code, name, price, numOnHand, minAge;
                do {
                    cout << "Enter item information as follows <code,name,numberOnHand,price,minAge>"<< endl
                         << "Enter item information: ";
                    getline(cin, code, ',');
                    getline(cin, name, ',');
                    getline(cin, numOnHand, ',');
                    getline(cin, price, ',');
                    getline(cin, minAge);
                    try {
                        inventory.push_back(new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(numOnHand), stoi(code)));
                        valid = true;
                    } catch (exception e) {
                        cout << "One or more arguments were invalid. Try again." << endl;
                        valid = false;
                    }
                } while(!valid);    
            }
        } else if(input == "3") {
            string input;
            bool valid = false;
            do {
                cout << "Enter the code of them item you want to delete: ";
                getline(cin, input);
                try {
                    int code = stoi(input);
                    for(int i = 0; i < inventory.size(); i++) {
                        if(code == inventory.at(i)->getItemCode()) {
                            cout << "Are you sure you want to delete this item from the inventory system?:" << endl
                                 << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                                 << inventory.at(i)->toStringAdmin() << endl
                                 << "Enter 'y' or 'n':";
                            getline(cin, input);
                            if(input == "y") {
                                inventory.erase(inventory.begin() + i);
                            } else if (input == "n") {
                                return;
                            }
                        }
                    }
                    valid = true;
                } catch (invalid_argument e) {
                    cout << "Invalid code entered: " << input;
                }
            } while (!valid);
        }
    
    } while(input != "exit");

    cout << "Exiting..." << endl;
    file_status_t fOutStatus = openFileOut(ofs, thisOutFileName);


    // cout << "Enter the name of your outfile: ";
    // getline(cin, thisOutFileName);

    if(!fOutStatus) {
        cout << "Error opening file \"" << thisOutFileName << "\"" << endl;
        exit(1);
    }

    writeBack(ofs, inventory);
    ofs.close();
}   


// Insertion sort used 
void sortItemsByName(vector<GMItem*> items) {
    int i, j; 
    GMItem * key;
    for (i = 1; i < items.size(); i++) { 
       key = items.at(i); 
       j = i-1; 
       while (j >= 0 && items.at(i)->getItemName() > key->getItemName()) { 
           items.at(i+1) = items.at(j); 
           j = j-1; 
       } 
       items.at(j+1) = key; 
   } 
}



void sortItemsByCode(vector<GMItem*> items) {
    int i, j; 
    GMItem * key;
    for (i = 1; i < items.size(); i++) { 
       key = items.at(i); 
       j = i-1; 
       while (j >= 0 && items.at(i)->getItemCode() > key->getItemCode()) { 
           items.at(i+1) = items.at(j); 
           j = j-1; 
       } 
       items.at(j+1) = key; 
   } 
}


bool addItemToList(vector<GMItem*> items, GMItem * newItem) {
    if(items.size() == MAX_SIZE) {
        return false;
    } else {
        items.push_back(newItem);
        return true;
    }
}



void changeCount(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    cout << endl
         << "a. Add to the current count" << endl
         << "b. Subtract from the current count" << endl
         << "c. Enter a completely new count" << endl
         << "Enter the number of the action you want to perform: ";
    getline(cin, input);
    if(input == "a") {
        do {
            cout << "Enter 'exit' to quit. Only integer values accepted." << endl
                 << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            validIn = itemPtr->increaseCount(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;
    } else if (input == "b") {
        do {
            cout << "Enter 'exit' to quit. Only integer values accepted." << endl
                 << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            validIn = itemPtr->decreaseCount(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;     
    } else if(input == "c") {
        do {
            cout << "Enter 'exit' to quit. Only integer values accepted." << endl
                 << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
            getline(cin, input);
            validIn = itemPtr->setNumOnHand(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;
    }
}



void changePrice(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "Enter 'exit' to quit. Only integer values accepted." << endl
             << "Enter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        validIn = itemPtr->setItemPrice(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
        validIn = false;  
}



void changeName(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "Enter 'exit' to quit. The name can be anything, the shorter the better." << endl
             << "Enter the new item namefor item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        validIn = itemPtr->setItemName(input);
        if(!validIn) {
            cout << "Invalid input: " << input << endl;
        }
    } while(!validIn && input != "exit");
    validIn = false;
}



void changeCode(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "Enter 'exit' to quit. Only integer values accepted." << endl
             << "Enter new code for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        validIn = itemPtr->increaseCount(input);
        if(!validIn) {
            cout << "Invalid input: " << input << endl;
        }
    } while(!validIn && input != "exit");
    validIn = false;
}



void changeWarning(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    //More RTTI to check if the item has an expiration before trying to change it.
    ExpiringItem * ex = dynamic_cast<ExpiringItem*>(itemPtr);
    if(ex != NULL) {
        do {
            cout << "Enter 'exit' to quit. Warning should be 20 characters or shorter and on one line." << endl
                 << "Enter the new warning for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
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
                    itemPtr = newEx;
                    validIn = true;
                } else {
                    cout << "Invalid input: " << input << endl;
                }
            } while (!validIn && input != "exit");
        }
    }
}



void changeMinAge(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    //More RTTI to check if the item has an expiration before trying to change it.
    AgeRestrictedItem * ar = dynamic_cast<AgeRestrictedItem*>(itemPtr);
    if(ar != NULL) {
        do {
            cout << "Enter 'exit' to quit. Only integer values accepted." << endl
                 << "Enter the new minmum age for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
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
                    itemPtr = newAr;
                    validIn = true;
                } else {
                    cout << "Invalid input: " << input << endl;
                }
            } while (!validIn && input != "exit");
        }
    }
}