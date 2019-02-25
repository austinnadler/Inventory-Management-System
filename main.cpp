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

// FIX: writeItems needs to be updated to print the different types of items in distinct sections.
// FIX: add something to either verify that item numbers are unique, or present user with all of the items with that code and go from there.
// FIX: add returns to checkout()

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


// Utilities:
void loadItemsFromFile(ifstream& ifs, vector<GMItem*> &items); 
void writeItems(ofstream& ofs, vector<GMItem*> items); // To file, includes expiration, age restrictions, etc. formatted for a csv file with a header
file_status_t openFileIn(ifstream& ifs, const string& fileName);
file_status_t openFileOut(ofstream& ofs, const string& fileName);
void sortItemsByName(vector<GMItem*> items);
void sortItemsByCode(vector<GMItem*> items);

// Default file names to make testing quicker.
string inFileName = "items.csv";
string outFileName = "itemsOut.csv";

int main() {
    string input;
    string thisInfileName = inFileName;
    string thisOutFileName = "newlog.csv";
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
                        return 0;
                    } else {
                        cout << "Invalid input: " << input << endl;
                    }
                }

            } while(!validIndex);

            try {
                GMItem * itemPtr = inventory[stoi(input)];
                index = stoi(input);
                found = true;
                bool doneWithThisItem = false;
                do {
                    cout << endl << "INDEX      CODE         NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
                    printAdminInfo(inventory);
                    cout << "----------|------------|---------------------|-----------|--------|--------------" << endl
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
                        return 0;
                    } else {
                        if(input == "1") {
                            string input;
                            bool valid = false;
                            do {
                                cout << endl
                                    << "a. Add to the current count" << endl
                                    << "b. Subtract from the current count" << endl
                                    << "c. Enter a completely new count" << endl
                                    << "Enter the character of the action you want to perform: ";
                                getline(cin, input);
                                
                                if(input == "exit") {
                                    return 0;
                                    } else {
                                        if(input == "a") {
                                            do {
                                                cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                    << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                                getline(cin, input);
                                                if(input == "exit") {
                                                    return 0;
                                                } else {
                                                    valid = itemPtr->increaseCount(input);
                                                if(!valid) {
                                                    cout << "Invalid input: " << input << endl;
                                                }  
                                            }
                                        } while(!valid);
                                    } else if (input == "b") {
                                        do {
                                            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                            getline(cin, input);
                                            if(input == "exit") {
                                                return 0;
                                            } else {
                                                valid = itemPtr->decreaseCount(input);
                                                if(!valid) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            }
                                        } while(!valid);
                                    } else if(input == "c") {
                                        do {
                                            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                            getline(cin, input);
                                            if(input == "exit") {
                                                return 0;
                                            } else {
                                                valid = itemPtr->setNumOnHand(input);
                                                if(!valid) {
                                                    cout << "Invalid input: " << input << endl;
                                                }
                                            }
                                        } while(!valid);
                                    } else {
                                        valid = false;
                                    }
                                }
                            } while(!valid);
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
                            return 0;
                        }
                    } 
                    cout << "Done editing this item? (y/n): ";
                    getline(cin, input);
                    if(input == "n") {
                        doneWithThisItem = false;
                    } else if(input == "y") {
                        doneWithThisItem = true;
                    } 
                } while(!doneWithThisItem);

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
                        cout << "Are you sure you want to delete this item from the inventory system?: " << endl
                            << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                            << inventory.at(index)->toStringAdmin() << endl
                            << "Enter 'y' or 'n': ";
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
    return 0;
}// end main()

/*------------------------ User actions ------------------------ */

void promptChangeName(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << "\nEnter 'exit' to quit. The name can be anything 20 characters or less." << endl
             << "\nEnter the new item namefor item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        } else {
            valid = itemPtr->setItemName(input);
            if(!valid) {
                cout << "Invalid input: " << input << endl;
            }
        }
    } while(!valid);
}

void promptChangePrice(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << "Enter an integer or decimal value (e.g. 5, or 4.99)" << endl
             << "\nEnter 'exit' to quit." << endl
             << "\nEnter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        valid = itemPtr->setItemPrice(input);
            if(!valid) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!valid && input != "exit");
        valid = false;  
}
void promptChangeCount(GMItem * itemPtr) {
    string input;
    bool valid = false;
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
            valid = itemPtr->increaseCount(input);
            if(!valid) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!valid && input != "exit");
    valid = false;
    } else if (input == "b") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nDecrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            valid = itemPtr->decreaseCount(input);
            if(!valid) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!valid && input != "exit");
    valid = false;     
    } else if(input == "c") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nEnter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
            getline(cin, input);
            valid = itemPtr->setNumOnHand(input);
            if(!valid) {
                cout << "Invalid input: " << input << endl;
            }
        } while(!valid && input != "exit");
    valid = false;
    }
}

void promptChangeCode(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
             << "\nEnter new code for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        } else {
            try {
                valid = itemPtr->setItemCode(input);
                if(!valid) {
                    cout << "Invalid code: " << input << endl
                        << "Codes must be positive digits and 5 digits (or shorter)" << endl;
                } else {
                    valid = itemPtr->setItemCode(input);
                    if(!valid) {
                        cout << "Invalid input: " << input << endl;
                    }
                }
            } catch(invalid_argument e) {
                cout << "Invalid input: " << input << endl
                    << "Codes must be positive digits and 5 digits (or shorter)";
            }
        }
    } while(!valid);
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
            cout << "Are you sure you want to delete this item from the inventory system?: " << endl
                 << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                 << items.at(index)->toStringAdmin() << endl
                 << "Enter 'y' or 'n': ";
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

/*------------------------ Utilities ------------------------*/

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