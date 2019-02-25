// File: main.cpp

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "GMItem.h"
#include "PromptItem.h"
#include "AgeRestrictedItem.h"

using namespace std;
using file_status_t = bool;
using total_price_t = double;


// This program can really be broken into two programs: checkout() and performAdminFunctions()
// performAdminFunctions() and related functions:
void performAdminFunctions();
    void promptChangeCount(GMItem * itemPtr);             // The interface for these functions is seperate to slim down the body of performAdminFunctions()
    void promptChangeName(GMItem * itemPtr);
    void promptChangePrice(GMItem * itemPtr);
    void promptChangeCode(GMItem * itemPtr);
    void promptChangePrompt(vector<GMItem*>& items, const int& index);
    void promptChangeMinAge(vector<GMItem*>& items, const int& index);
    void promptAddGMItem(vector<GMItem*>& items);
    void promptAddPromptItem(vector<GMItem*>& items);
    void promptAddAgeRestrictedItem(vector<GMItem*>& items);
    void promptDeleteItem(vector <GMItem*> items);

    void writeBack(ofstream& ofs, vector<GMItem*> items); // writes to a new file with the same format as the example input file so it can be re-used.
    void printAdminInfo(vector<GMItem*> items);           // outputs to the screen the list of objects with all special information, for use in performAdminFunctions()

void checkout(); // create a new array of pointers and put the items that you want to check out into it. Program totals the purchase and adds tax, then decreases whatever counts need to be decreased. Uses vector functions for deletion and adding.
    const int MAX_CART_SIZE = 500; // I know from working at Wal-Mart that POS systems have built in caps around 400 or 500 so I figured I'd add one.
    const double TAX_RATE = 0.071;  // IL 7.1%
    void printItemsPOS(vector<GMItem*> items);          // To screen, displays only code, name, and price with periods for spacing, for use in checkout()
    void printPOSPriceSection(vector<GMItem*> items);   // print subtotal, taxes, and total, for use in checkout()
    void printPOSHeader();  
    total_price_t calculateTax(const double& subTotal);
    total_price_t calculateTotal(double& subTotal);
// FIX: writeItems needs to be updated to print the different types of items in distinct sections.
// FIX: add something to either verify that item numbers are unique, or present user with all of the items with that code and go from there.

// Testing file reading and writing
void testFileIOandPricing();

// Utilities:
void loadItemsFromFile(ifstream& ifs, vector<GMItem*> &items); 
void writeItems(ofstream& ofs, vector<GMItem*> items); // To file, includes expiration, age restrictions, etc. formatted for a csv file with a header
file_status_t openFileIn(ifstream& ifs, const string& fileName);
file_status_t openFileOut(ofstream& ofs, const string& fileName);

void sortItemsByName(vector<GMItem*> items);
void sortItemsByCode(vector<GMItem*> items);

// Default file names to make testing quicker.
string inFileName = "items.txt";
string outFileName = "itemsOut.csv";

int main() {
    string cmd;
    string stillstr;
    bool still = true;
    bool validCmd = false;

    do {
        cout << "Enter 'admin' to use the performAdminFunctions function" << endl
             << "Enter 'pos' to use the checkout function" << endl
             << "or enter 'exit' to exit." << endl
             << "Enter a command: ";
        getline(cin,cmd);

        if(cmd == "pos") {
            do {
                checkout();
                cout << "Keeping using POS? (y/n):";
                getline(cin, stillstr);
                if(stillstr == "y") {
                    still = true;
                } else if (stillstr == "n") {
                    still = false;
                }
            } while(still);
            validCmd = true;
        } else if(cmd == "admin") {
            validCmd = true;
            do {
                performAdminFunctions();
                cout << "Keep using admin? (y/n): ";
                getline(cin, stillstr);
                if(stillstr == "y") {
                    still = true;
                } else if (stillstr == "n") {
                    still = false;
                }
            } while(still);
        } else if(cmd == "exit") {
            validCmd = true; 
        }
    } while (!validCmd);

    return 0;
}// end main()

void performAdminFunctions() {
    string input;
    string thisInfileName = inFileName;
    string thisOutFileName = "newlog.txt";
    bool found = false;
    int index;
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
        //FIX: Maybe use iomanip here, but i found it easier to just have this fixed and have the objects' output be controlled by iomanip to fit here
        cout << "INDEX      CODE         NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(inventory);
        cout << "----------|------------|---------------------|-----------|--------|--------------" << endl
             << "Enter the number of the action you want to perform." << endl
             << "1. Manage current inventory" << endl
             << "2. Add item" << endl
             << "3. Delete an item" << endl
             << "and at any time, you can enter 'exit' to back out to this menu." << endl << endl
             << "Enter your choice: ";
        getline(cin, input);
        
        if(input == "1") {
            // I found it better when testing to not have these as loops to avoid having to restart the program constantly
            // This can easily be changed to accept an item code instead of an index, that was how I had it initially. I changed it when codes got bigger than indexes because I found it easier to use that way.
            bool validIndex = false;
            do {
                cout << "\nEnter index of the item that you want to edit: ";
                getline(cin, input);
                try {
                    validIndex = true;
                    int index = stoi(input);
                    if(index < 0 || index > inventory.size()) {
                        cout << "Invalid index: " << input << endl;
                    }
                } catch (invalid_argument e) {
                    if(input == "exit") {
                        return;
                    } else {
                        cout << "Invalid input: " << input << endl;
                    }
                }

            } while(!validIndex);

            try {
                GMItem * itemPtr = inventory[stoi(input)];
                index = stoi(input);
                found = true;
                cout << endl 
                     << "Item found: " << itemPtr->getItemName() << endl << endl
                     << "1. Change number on hand" << endl
                     << "2. Change price" << endl
                     << "3. Change item name" << endl
                     << "4. Change item code" << endl
                     << "5. Change item warning prompt" << endl
                     << "6. Change item minimum age" << endl << endl
                     << "Enter the number of the action you want to perform: ";    
                    getline(cin,input);

                    if(input == "exit") {
                        return;
                    } else {
                        if(input == "1") {
                            string input;
                            bool validIn = false;
                            do {
                                cout << endl
                                     << "a. Add to the current count" << endl
                                     << "b. Subtract from the current count" << endl
                                     << "c. Enter a completely new count" << endl
                                     << "Enter the character of the action you want to perform: ";
                                getline(cin, input);
                                
                                if(input == "exit") {
                                    return;
                                } else {
                                    if(input == "a") {
                                        do {
                                            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                 << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                            getline(cin, input);
                                            if(input == "exit") {
                                                return;
                                            } else {
                                                validIn = itemPtr->increaseCount(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }  
                                            }
                                        } while(!validIn);
                                        validIn = false;
                                    } else if (input == "b") {
                                        do {
                                            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                 << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                            getline(cin, input);
                                            if(input == "exit") {
                                                return;
                                            } else {
                                                validIn = itemPtr->decreaseCount(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            }
                                        } while(!validIn);
                                        validIn = false;     
                                    } else if(input == "c") {
                                        do {
                                            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                 << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                            getline(cin, input);
                                            if(input == "exit") {
                                                return;
                                            } else {
                                                validIn = itemPtr->setNumOnHand(input);
                                                if(!validIn) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            }
                                        } while(!validIn);
                                        validIn = false;
                                    } else {
                                        validIn = false;
                                    }
                                }
                            } while(!validIn);
                        } else if(input == "2") {
                            promptChangePrice(itemPtr);            
                        } else if(input == "3") {
                            promptChangeName(itemPtr);
                        } else if (input == "4") {
                            promptChangeCode(itemPtr);
                        } else if(input == "5") {
                            promptChangePrompt(inventory, index);
                        } else if(input == "6") {
                            promptChangeMinAge(inventory, index);
                        } else if (input == "exit") {
                               return;
                        }

                    }

                } catch(invalid_argument e) {
                        cout << "Invalid input: " << input << endl;
                }   
                
                found = false;
        } else if(input == "2") {
                cout << "a. A GMItem with no special characteristics." << endl
                     << "b. A PromptItem with a warning to be stored." << endl
                     << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl;
                getline(cin, input);
                if(input == "a") {
                    promptAddGMItem(inventory);
                } else if (input == "b") {
                    promptAddPromptItem(inventory);                    
                } else if(input == "c") {
                    promptAddAgeRestrictedItem(inventory);  
                }
        } else if(input == "3") {
            string input;
            bool valid = false;
            do {
                cout << "Enter the index of them item you want to delete: ";
                getline(cin, input);
                try {
                    int index = stoi(input);
                    if(index < 0 || index > inventory.size()) {
                        cout << "Invalid index: " << index << endl;
                    } else {
                        valid = true;
                        cout << "Are you sure you want to delete this item from the inventory system?:" << endl
                            << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                            << inventory.at(index)->toStringAdmin() << endl
                            << "Enter 'y' or 'n':";
                        getline(cin, input);
                        if(input == "y") {                                  
                            inventory.erase(inventory.begin() + index);  // vector built in erase 
                        } 
                    }
                } catch(invalid_argument e) {
                    cout << "Invalid input: " << input << endl;
                }
    } while (!valid && input != "exit" && input != "n");
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
    // After making all the changes, write the new vector to the file and close the stream
    writeBack(ofs, inventory);
    ofs.close();
}   



void promptChangeCount(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    cout << endl
         << "a. Add to the current count" << endl
         << "b. Subtract from the current count" << endl
         << "c. Enter a completely new count" << endl
         << "Enter the character of the action you want to perform: ";
    getline(cin, input);
    if(input == "a") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nIncrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            validIn = itemPtr->increaseCount(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;
    } else if (input == "b") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nDecrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            validIn = itemPtr->decreaseCount(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;     
    } else if(input == "c") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nEnter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
            getline(cin, input);
            validIn = itemPtr->setNumOnHand(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
    validIn = false;
    }
}



void promptChangePrice(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "Enter an integer or decimal value (e.g. 5, or 4.99)" << endl
             << "\nEnter 'exit' to quit." << endl
             << "\nEnter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        validIn = itemPtr->setItemPrice(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!validIn && input != "exit");
        validIn = false;  
}



void promptChangeName(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "\nEnter 'exit' to quit. The name can be anything 20 characters or less." << endl
             << "\nEnter the new item namefor item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        if(input == "exit") {
            validIn = true;
        } else {
            validIn = itemPtr->setItemName(input);
            if(!validIn) {
                cout << "Invalid input: " << input << endl;
            }
        }
    } while(!validIn);
}



void promptChangeCode(GMItem * itemPtr) {
    string input;
    bool validIn = false;
    do {
        cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
             << "\nEnter new code for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        if(input == "exit") {
            validIn = true;
        } else {
            try {
                validIn = itemPtr->setItemCode(input);
                if(!validIn) {
                    cout << "Invalid code: " << input << endl
                        << "Codes must be positive digits and 5 digits (or shorter)" << endl;
                } else {
                    validIn = itemPtr->setItemCode(input);
                    if(!validIn) {
                        cout << "Invalid input: " << input << endl;
                    }
                }
            } catch(invalid_argument e) {
                cout << "Invalid input: " << input << endl
                    << "Codes must be positive digits and 5 digits (or shorter)";
            }
        }
    } while(!validIn);
}



void promptChangePrompt(vector<GMItem*>& items, const int& index) {
    string input;
    bool valid = false;
    PromptItem * newPtr = new PromptItem();
    PromptItem * prPtr = nullptr;
    prPtr = dynamic_cast<PromptItem*>(prPtr);
    do {
        cout << "Enter 'r' to remove the warning from this item, or 'a' or 'c' to add or change the prompt." << endl;
        getline(cin, input);
        if(input == "a" || input == "c") {
            cout << "Warning prompts must be 20 characters or shorter." << endl
                 << "Enter the new warning for item " << items.at(index)->getItemName() << " - " << items.at(index)->getItemCode() << ": ";
            getline(cin, input);
            if(input == "exit") {
                valid = true;
            } else {
                valid = newPtr->setWarning(input);
                if(valid && prPtr == nullptr) {
                    newPtr->setWarning(input);
                    newPtr->setItemName(items.at(index)->getItemName());
                    newPtr->setItemCode(to_string(items.at(index)->getItemCode()));
                    newPtr->setItemPrice(to_string(items.at(index)->getItemPrice()));
                    newPtr->setNumOnHand(to_string(items.at(index)->getNumOnHand()));
                    items.at(index) = newPtr;
                } else if(valid && prPtr != nullptr) {
                    valid = prPtr->setWarning(input);
                }
            }
        } else if(input == "r") {
            GMItem * newItemPtr = new GMItem(items.at(index)->getItemName(),
                                             items.at(index)->getItemPrice(),
                                             items.at(index)->getNumOnHand(),
                                             items.at(index)->getItemCode());
            delete items.at(index);
            items.at(index) = newItemPtr;
            valid = true;
        }
    } while(!valid);
}



void promptChangeMinAge(vector<GMItem*>& items, const int& index) {
    string input;
    bool valid = false;;
    AgeRestrictedItem * newPtr = new AgeRestrictedItem();
    AgeRestrictedItem * arPtr = nullptr;
    arPtr = dynamic_cast<AgeRestrictedItem*>(arPtr);
    do {
        cout << "Enter 'r' to remove the warning from this item, or 'a' or 'c' to add or change the prompt." << endl;
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        } else {
            if(input == "a" || input == "c") {
                cout << "Input only integer values such as '18' or '21'."<< endl
                    << "Enter the new minium age for item " << items.at(index)->getItemName() << " - " << items.at(index)->getItemCode() << ": ";    
                getline(cin, input);
                valid = newPtr->setMinAge(input);
                if(valid && arPtr == nullptr) {
                    newPtr->setMinAge(input);
                    newPtr->setItemName(items.at(index)->getItemName());
                    newPtr->setItemCode(to_string(items.at(index)->getItemCode()));
                    newPtr->setItemPrice(to_string(items.at(index)->getItemPrice()));
                    newPtr->setNumOnHand(to_string(items.at(index)->getNumOnHand()));
                    items.at(index) = newPtr;
                } else if(valid && arPtr != nullptr) {
                    valid = arPtr->setMinAge(input);
                }
            } else if(input == "r") {
                GMItem * newItemPtr = new GMItem(items.at(index)->getItemName(),
                                                items.at(index)->getItemPrice(),
                                                items.at(index)->getNumOnHand(),
                                                items.at(index)->getItemCode());
                delete items.at(index);
                items.at(index) = newItemPtr;
                valid = true;
            }
        }
    } while(!valid);
}



void promptAddGMItem(vector<GMItem*>& items) {
    bool valid;
    string input, code, name, price, numOnHand;
    do {
        cout << "\nEnter item information as follows: " << endl
             << "\nEnter the code or 'exit' to back out (only chance, after inputting name you have to finish or restart the program.): ";
        getline(cin, input);
        code = input;
        input = "";
        cout << "Enter the name: ";
        getline(cin, input);
        if(input == "exit") { valid = true; }
        name = input;
        input = "";
        cout << "Enter the quantity on hand: ";
        getline(cin, input);
        numOnHand = input;
        input = "";
        cout << "Enter the price: ";
        getline(cin, input);
        price = input;
        input = "";
        try {
            items.push_back(new GMItem(name, stod(price), stoi(numOnHand), stoi(code)));
            valid = true;
        } catch (invalid_argument e) {
            cout << "One or more arguments were invalid. Try again." << endl;
            valid = false;
        }
    } while(!valid);    
}



void promptAddPromptItem(vector<GMItem*>& items) {
    bool valid;
    string input, code, name, price, numOnHand, warning;
    do {
        cout << "Enter item information as follows: " << endl
             << "Enter the code or 'exit' to back out (only chance, after inputting name you have to finish or restart the program.): ";
        getline(cin, input);
        if(input == "exit") { valid = true; }
        code = input;
        input = "";
        cout << "Enter the name: ";
        getline(cin, input);
        name = input;
        input = "";
        cout << "Enter the quantity on hand: ";
        getline(cin, input);
        numOnHand = input;
        input = "";
        cout << "Enter the price: ";
        getline(cin, input);
        price = input;
        input = "";
        cout << "Enter the 20 character maximum warning: ";
        getline(cin, warning);
        try {
            items.push_back(new PromptItem(warning, name, stod(price), stoi(numOnHand), stoi(code)));
            valid = true;
        } catch (exception e) {
            cout << "One or more arguments were invalid. Try again." << endl;
            valid = false;
        }
    } while(!valid);           
}



void promptAddAgeRestrictedItem(vector<GMItem*>& items) {
    bool valid;
    string input, code, name, price, numOnHand, minAge;
    do {
        cout << "Enter item information as follows: " << endl;
        cout << "Enter the code: ";
        getline(cin, input);
        code = input;
        input = "";
        cout << "Enter the name: ";
        getline(cin, input);
        name = input;
        input = "";
        cout << "Enter the quantity on hand: ";
        getline(cin, input);
        numOnHand = input;
        input = "";
        cout << "Enter the price: ";
        getline(cin, input);
        price = input;
        input = "";
        cout << "Enter the minimum age: ";
        getline(cin, input);
        minAge = input;
        input = "";
        try {
            items.push_back(new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(numOnHand), stoi(code)));
            valid = true;
        } catch (exception e) {
            cout << "One or more arguments were invalid. Try again." << endl;
            valid = false;
        }
    } while(!valid);  
}



void promptDeleteItem(vector<GMItem*> items) {
    string input;
    bool valid = false;
    do {
        cout << "Enter the index of them item you want to delete: ";
        getline(cin, input);
        try {
            int index = stoi(input);
            cout << "Are you sure you want to delete this item from the inventory system?:" << endl
                 << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                 << items.at(index)->toStringAdmin() << endl
                 << "Enter 'y' or 'n':";
            getline(cin, input);
            if(input == "y") {
                items.erase(items.begin() + index);
            } else if (input == "n") {
                return;
            }       
            valid = true;
        } catch (invalid_argument e) {
             cout << "Invalid code entered: " << input;
        }
    } while (!valid && input != "exit");
}

/**************************************************************************************************************************************/
/********************************************************* END ADMIN FUNCTION *********************************************************/
/**************************************************************************************************************************************/

// In checkout, i imagine the code being scanned in from a barcode. When testing, I just have the inventory file open and enter codes from there.
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
        if(numItemsInCart == MAX_CART_SIZE) {
            cout << endl << "Maximum cart sized reached. Totaling the order..." << endl << endl;
                    printPOSHeader();
                    printItemsPOS(cart);
                    printPOSPriceSection(cart);
            cout << endl;
            return;
        }                        

        cout << "Enter product code (Type 'total' when done entering items): ";
        getline(cin, codeString);
        // FIX: add else if clause to do returns!
        if(codeString == "total") {
            cout << endl;
            try {
                    printPOSHeader();
                    printItemsPOS(cart);
                    printPOSPriceSection(cart);

                    // Change the on hand quantity.
                    for(int i = 0; i < numItemsInCart; i++) {
                        cart.at(i)->decreaseCount();
                    }
            } catch (exception e) {
                cout << "Invalid input: " << codeString << endl;
            }
            cout << endl;
        } else if (codeString == "undo") {
                if(cart.size() < 1) {
                    cout << endl << "The list is already empty!" << endl;
                } else {
                    cart.pop_back();
                    printPOSHeader();
                    printItemsPOS(cart);
                    printPOSPriceSection(cart);
                }
        } else {
            try {
                code = stoi(codeString);
                for(int i = 0; i < inventory.size(); i++) {
                        
                        // FIX: This code does not work
                        if (foundItem == false && i == inventory.size()) { 
                            cout << "No item with that code was found in the inventory file." << endl;
                        } 

                    if(code == inventory.at(i)->getItemCode()) {
                        
                        // RTTI Run Time Type Idenification used to determine if the item is age restricted or not and take steps to verify age

                        GMItem *gm = inventory.at(i);
                        // Try to convert to subclass to check type
                        AgeRestrictedItem *ar = dynamic_cast<AgeRestrictedItem*>(gm);
                        PromptItem *pi = dynamic_cast<PromptItem*>(gm);
                        string input;
                        bool validIn = false;

                        if(ar != nullptr) {
                            do {
                                cout << "This is an age restricted item. Verify with a valid photo ID." << endl;
                                cout << "Verify customer is at least " << ar->getMinAge() << " years or older (y/n): ";
                                getline(cin, input);
                                if(input == "y") {
                                    oss << inventory.at(i)->toStringPOS();
                                    subTotal += inventory.at(i)->getItemPrice();
                                    numItemsInCart++;
                                    cart.push_back(inventory.at(i));
                                    foundItem = true;
                                    validIn = true;
                                } else if(input == "n") {
                                    cout << "Sale not allowed." << endl;
                                    validIn = true;
                                }
                            } while(!validIn);
                        }
                        
                        if(pi != nullptr) {
                            do {
                                cout << pi->getWarning() << endl;
                                cout << "Ackowledge? (y/n): ";
                                getline(cin, input);
                                if(input == "y") {
                                    oss << inventory.at(i)->toStringPOS();
                                    subTotal += inventory.at(i)->getItemPrice();
                                    numItemsInCart++;
                                    cart.push_back(inventory.at(i));
                                    foundItem = true;
                                    validIn = true;
                                } else if(input == "n") {
                                    cout << "Sale not allowed." << endl;
                                    validIn = true;
                                }
                            } while(!validIn);      
                        }
                        
                        
                        if(ar == NULL && pi == NULL) {
                            cart.push_back(inventory.at(i));
                        }

                    }                     
                }
            } catch(exception e) {
                cout << "Invalid Code Entered!" << endl;
            }
        }

        cout << endl;
                printPOSHeader();
                printItemsPOS(cart);
            cout << endl;
                printPOSPriceSection(cart);
        cout << endl;
        
    } while (codeString != "total" && numItemsInCart < MAX_CART_SIZE);
}   


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

            PromptItem * pi = new PromptItem(expirationDate, name, stod(price), stoi(qtyOnHand), stoi(code));
            items.push_back(pi);
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
void printItemsPOS(vector<GMItem*> items) {
    for(int i = 0; i < items.size(); i++) {
        cout  << items.at(i)->toStringPOS() << endl;
    }
}// end printItemsPOS()


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
    printItemsPOS(items);
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
        cout << "----------|------------|---------------------|-----------|--------|--------------" << endl;
        cout << setw(10) << left << i << "| " << items.at(i)->toStringAdmin() << endl;
    }
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



void sortItems(vector<GMItem*> items) {
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