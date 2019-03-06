// File: main.cpp

// FIX: debug saving further

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include "List.h"
#include "GMItem.h"
#include "PromptItem.h"
#include "AgeRestrictedItem.h"
using file_status_t = bool;
using total_price_t = double;

/* Loops that call their respective setters when editing items in the inventory system */
/* Created to simplify the main method                                                 */
void promptChangeCount(GMItem * itemPtr);
void promptChangeName(GMItem * itemPtr);    
void promptChangePrice(GMItem * itemPtr);
void promptChangeCode(GMItem * itemPtr, List<GMItem*> items);
void promptChangeNumberOnHand(GMItem * itemPtr);
// These get the List and the index because they use Lists pushers
void promptChangePrompt(List<GMItem*>& items, const int& index);
void promptChangeMinAge(List<GMItem*>& items, const int& index);
void promptDuplicateItem(List<GMItem*> items, const int& index);

/* Functions to add items to the inventory system */
/* Created to simplify the main method            */
void promptGeneralPrompt(string& code, string& name, string& price, string& numOnHand, bool& valid); //Prompt that all subclasses use first with error trapping
bool promptAddGMItem(List<GMItem*>& items);
bool promptAddPromptItem(List<GMItem*>& items);
bool promptAddAgeRestrictedItem(List<GMItem*>& items);
void promptDeleteItem(List <GMItem*>& items);                      
void writeBack(ofstream& ofs, List<GMItem*>& items);                 // writes to a new file with the same format as the example input file so it can be re-used.
void printAdminInfo(List<GMItem*>& items);                           // outputs to the screen the list of objects with all special information, for use in performAdminFunctions()
void printAdminSeperators();
void loadItemsFromFile(ifstream& ifs, List<GMItem*>& items);         // take an ifs and an empty array and fill said array with items from a FORMATTED file
void writeItems(ofstream& ofs, List<GMItem*>& items);                // take a provided ofstream and array of items and write them to the ofstream using the toStringFile() method
void save(ofstream& ofs, List<GMItem*>& items);                      // writeBack() and close file.
file_status_t openFileIn(ifstream& ifs, const string& fileName);     // use provided ifstream to open provided filename to read information from
file_status_t openFileOut(ofstream& ofs, const string& fileName);    // use provided ofstream to open provided filename to write data to
bool isCodeAvailible(List<GMItem*>& items, const int& code);             // Was planning to impliment a binary search, but this takes less than a second even with over 1 million items, so its good enough

/* Used in several functions */
string inFileName = "items.csv";
string outFileName = "itemsOut.csv";
ifstream ifs;
ofstream ofs;

using namespace std;

int main() {
    List<GMItem*> inventory;
    string input;
    // cout << "Enter the inventory list file name with extension: ";
    // getline(cin, inFileName);
    bool found = false;
    cout << "Opening file " << inFileName << "..." << endl;
    file_status_t fInStatus = openFileIn(ifs, inFileName);
    if(!fInStatus) {
        cout << "Error opening file \"" << inFileName << "\"" << endl;
        exit(1);
    } else {
        cout << "File " << inFileName << " opened succesfully" << endl;
    }

    loadItemsFromFile(ifs, inventory); 
    do {
        cout << "INDEX      CODE         NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(inventory);
        printAdminSeperators();
        cout << "This is the only menu from which you should try to exit this program!" << endl
             << "Enter the number of the action you want to perform." << endl
             << "1. Manage current inventory" << endl
             << "2. Add item" << endl
             << "3. Delete an item" << endl << endl
             << "Enter your choice: ";
        getline(cin, input);

        if(input == "exit") {
            save(ofs, inventory);
            return 0;
        }
        
        if(input == "1") {
            // I found it better when testing to not have these as loops to avoid having to restart the program constantly
            // This can easily be changed to accept an item code instead of an index, that was how I had it initially. I changed it when codes got bigger than indexes because I found it easier to use that way.
            string input;
            int index;
            int maxIndex = inventory.size() - 1;
                do {
                    try {
                        cout << "Enter the index of them item you want to edit: ";
                        getline(cin, input);
                        index = stoi(input);
                        if(index < 0 || index > maxIndex) {
                            cerr << "Invalid index: " << input << endl;
                        }
                    }catch(invalid_argument e) {
                        cerr << "Invalid input: " << input << endl;
                    }
                }while( index < 0 || index > maxIndex);

                GMItem * itemPtr = inventory.getAt(index);
                found = true;
                bool doneWithThisItem = false;
                do {
                    do {
                        cout << "Item found: " << itemPtr->getItemName() << endl << endl
                         << "1. Change number on hand" << endl
                         << "2. Change price" << endl
                         << "3. Change item name" << endl
                         << "4. Change item code" << endl
                         << "5. Change item prompt" << endl
                         << "6. Change item minimum age" << endl
                         << "7. Duplicate this item (when creating similar item)" << endl
                         << "Enter the number of the action you want to perform: ";    
                        getline(cin,input);
                    } while(input != "1" & input != "2" & input != "3" & input != "4" & input != "5" & input != "6" & input != "7"); // Better than a bunch of if else
                    
                    if(input == "1") {
                        promptChangeNumberOnHand(itemPtr);
                    } else if(input == "2") {
                        promptChangePrice(itemPtr);    
                        save(ofs, inventory);    
                    } else if(input == "3") {
                        promptChangeName(itemPtr);
                        save(ofs, inventory);    
                    } else if (input == "4") {
                        promptChangeCode(itemPtr, inventory);
                        save(ofs, inventory);    
                    } else if(input == "5") {
                        promptChangePrompt(inventory, index);
                        save(ofs, inventory);    
                    } else if(input == "6") {
                        promptChangeMinAge(inventory, index);
                        save(ofs, inventory);    
                    } else if(input == "7") {
                        promptDuplicateItem(inventory, index);
                        save(ofs, inventory);
                    }
                    cout << "Done editing this item? (y/n): ";
                    getline(cin, input);
                    if(input == "n") {
                        doneWithThisItem = false; // just in case
                    } else if(input == "y") {
                        doneWithThisItem = true;
                    } 
                } while(!doneWithThisItem); 
                found = false;
        } else if(input == "2") {
            bool valid = false;
            do {
                cout << "a. A GMItem with no special characteristics." << endl
                     << "b. A PromptItem with a prompt to be stored." << endl
                     << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl
                     << "Enter the character of your choice: ";
                getline(cin, input);
                if(input == "a") {
                    valid = promptAddGMItem(inventory);
                } else if (input == "b") {
                    valid = promptAddPromptItem(inventory);                    
                } else if(input == "c") {
                    valid = promptAddAgeRestrictedItem(inventory);  
                }
            } while((input != "a" || input != "b" || input != "c") && !valid);
        } else if(input == "3") {
            promptDeleteItem(inventory);
        }
    } while(input != "exit");
}// end main()

/*--------------------------------------------------------------*/
/*------------------------ User Actions ------------------------*/
/*--------------------------------------------------------------*/

void promptChangeNumberOnHand(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << endl
                << "a. Add to the current count" << endl
                << "b. Subtract from the current count" << endl
                << "c. Enter a completely new count" << endl
                << "Enter the character of the action you want to perform: ";
        getline(cin, input);
        if(input == "a") {
            do {
                cout << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                getline(cin, input);
                valid = itemPtr->increaseCount(input);
                if(!valid) {
                    cerr << "Invalid input: " << input << endl;
                }   
            } while(!valid);
        } else if (input == "b") {
            do {
                cout << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                getline(cin, input);
                valid = itemPtr->decreaseCount(input);
                if(!valid) {
                    cerr << "Invalid input: " << input << endl;
                }
            } while(!valid);
        } else if(input == "c") {
            do {
                cout << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                getline(cin, input);
                valid = itemPtr->setNumOnHand(input);
                if(!valid) {
                    cerr << "Invalid input: " << input << endl;
                } 
            } while(!valid);
        } else {
            valid = false;
        }                       
    } while(!valid);
}
void promptChangeName(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << "\nEnter the new item name for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        valid = itemPtr->setItemName(input);
        if(!valid) {
            cerr << "Invalid input: " << input << endl;
        }
    } while(!valid);
}//end promptChangeName()

void promptChangePrice(GMItem * itemPtr) {
    string input;
    bool valid = false;
    do {
        cout << "Enter an integer or decimal value (e.g. 5, or 4.99)" << endl
             << "\nEnter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        valid = itemPtr->setItemPrice(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            }
        } while(!valid);
    valid = false;  
}//end promptChangePrice()

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
            cout << "\nIncrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            valid = itemPtr->increaseCount(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            }
        } while(!valid);
        valid = false;
    } else if (input == "b") {
        do {
            cout << "\nDecrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            valid = itemPtr->decreaseCount(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } 
        } while(!valid);
        valid = false;     
    } else if(input == "c") {
        do {
            cout << "\nEnter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
            getline(cin, input);
            valid = itemPtr->setNumOnHand(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            }
        } while(!valid);
        valid = false;
    }
}//end promptChangeCount()

void promptChangeCode(GMItem * itemPtr, List<GMItem*> inventory) {
    string input;
    bool isInt = false;
    bool isTaken = true;
    bool allGood = false;
    do {
        do {
            cout << "Enter the new item code. This must be " << itemPtr->CODE_MAX_DIGITS <<  " digits or shorter: ";
            getline(cin, input);
            isInt = itemPtr->setItemCode(input);
        } while(!isInt);

        if(isInt) {
            isTaken = isCodeAvailible(inventory, stoi(input));
            if(isTaken) {
                cout << "Code " << input << " is already taken." << endl;
            } else {
                itemPtr->setItemCode(input);
            }
        }

        if(isInt && !isTaken) {
            allGood = true;
            save(ofs, inventory);
        }
    } while(!allGood);
}//promptChangeCode()

void promptChangePrompt(List<GMItem*>& items, const int& index) {
    string input;
    bool valid = false;
    PromptItem * newPtr = new PromptItem();
    PromptItem * prPtr = nullptr;
    prPtr = dynamic_cast<PromptItem*>(items.getAt(index));
    do {
        cout << "Enter 'r' to remove the prompt from this item, or 'a' or 'c' to add or change the prompt." << endl;
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        }
        if(input == "a" || input == "c") {
            cout << "Prompts must be " << newPtr->MAX_PROMPT_LENGTH << " characters or shorter." << endl
                 << "Enter the new prompt for item " << items.getAt(index)->getItemName() << " - " << items.getAt(index)->getItemCode() << ": ";
            getline(cin, input);
            valid = newPtr->setPrompt(input);
            if(valid && prPtr == nullptr) {
                newPtr->setPrompt(input);
                newPtr->setItemName(items.getAt(index)->getItemName());
                newPtr->setItemCode(to_string(items.getAt(index)->getItemCode()));
                newPtr->setItemPrice(to_string(items.getAt(index)->getItemPrice()));
                newPtr->setNumOnHand(to_string(items.getAt(index)->getNumOnHand()));
                items.deleteAt(index);
                items.pushAt(index, newPtr);
            } else if(valid && prPtr != nullptr) {
                valid = prPtr->setPrompt(input);
            }
        } else if(input == "r") {
            GMItem * newGMPtr = new GMItem(items.getAt(index)->getItemName(),
                                           items.getAt(index)->getItemPrice(),
                                           items.getAt(index)->getNumOnHand(),
                                           items.getAt(index)->getItemCode());
            items.deleteAt(index);
            items.pushAt(index, newGMPtr);
            valid = true;
        }
    } while(!valid);
}//end promptChangePrompt()

void promptChangeMinAge(List<GMItem*>& items, const int& index) {
    string input;
    bool valid = false;;
    AgeRestrictedItem * newPtr = new AgeRestrictedItem();
    AgeRestrictedItem * arPtr = nullptr;
    arPtr = dynamic_cast<AgeRestrictedItem*>(items.getAt(index));
    do {
        cout << "Enter 'r' to remove the prompt from this item, or 'a' or 'c' to add or change the prompt." << endl;
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        } else {
            if(input == "a" || input == "c") {
                cout << "Input only integer values such as '18' or '21'."<< endl
                    << "Enter the new minium age for item " << items.getAt(index)->getItemName() << " - " << items.getAt(index)->getItemCode() << ": ";    
                getline(cin, input);
                valid = newPtr->setMinAge(input);
                if(valid && arPtr == nullptr) {
                    newPtr->setMinAge(input);
                    newPtr->setItemName(items.getAt(index)->getItemName());
                    newPtr->setItemCode(to_string(items.getAt(index)->getItemCode()));
                    newPtr->setItemPrice(to_string(items.getAt(index)->getItemPrice()));
                    newPtr->setNumOnHand(to_string(items.getAt(index)->getNumOnHand()));
                    items.deleteAt(index);
                    items.pushAt(index, newPtr);
                } else if(valid && arPtr != nullptr) {
                    valid = arPtr->setMinAge(input);
                }
            } else if(input == "r") {
                GMItem * newItemPtr = new GMItem(items.getAt(index)->getItemName(),
                                                items.getAt(index)->getItemPrice(),
                                                items.getAt(index)->getNumOnHand(),
                                                items.getAt(index)->getItemCode());
                items.deleteAt(index);
                items.pushAt(index, newItemPtr);
                valid = true;
            }
        }
    } while(!valid);
}//end promptChangeMinAge()

void promptGeneralPrompt(string& code, string& name, string& price, string& numOnHand, bool& valid) {
    string input;
    GMItem * test = new GMItem();
    
    do {
        cout << "Enter the code (" << test->CODE_MAX_DIGITS << " digits or less): ";
        getline(cin, input);
        valid = test->setItemCode(input);
        if(valid) {
            code = input;
        }
    } while(!valid);
    valid = false;

    do {
        cout << "Enter the name ("  << test->MAX_NAME_LENGTH << " characters or less): ";
        getline(cin, input);
        valid = test->setItemName(input);
        if(valid) {
            name = input;
        }
    } while(!valid);
    valid = false;
    
    do {
        cout << "Enter the quantity on hand (postive integers only): ";
        getline(cin, input);
        valid = test->setNumOnHand(input);
        if(valid) {
            numOnHand = input;
        }
    } while(!valid);

    do {
        cout << "Enter the price (postive integers or decimal numbers only): ";
        getline(cin, input);
        valid = test->setItemPrice(input);
        if(valid) {
            price = input;
        }
    } while(!valid);     

    delete test;
}//end promptChangeMinAge()

bool promptAddGMItem(List<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand;
    promptGeneralPrompt(code, name, price, numOnHand, valid);
    GMItem * newPtr = new GMItem(name, stod(price), stoi(numOnHand), stoi(code));
    try {
        items.pushBack(newPtr);
        save(ofs, items);
        return true;
    } catch (exception e) {
        cerr << "Error adding to list" << endl;
        return false;
    }
}//end promptAddGMItem()

bool promptAddPromptItem(List<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand, prompt;
    PromptItem * newPtr = new PromptItem();
    // temp pointer used to validate
    do {
        promptGeneralPrompt(code, name, price, numOnHand, valid);
        cout << "Enter the 50 character maximum prompt: ";
        getline(cin, prompt);
        valid = newPtr->setPrompt(prompt);
        if (valid) {
            try {
                delete newPtr;
                newPtr = new PromptItem(prompt, name, stod(price), stoi(numOnHand), stoi(code));
                items.pushBack(newPtr);
                valid = true;
            } catch (invalid_argument& e) {
                cerr << "One or more arguments were invalid. Try again." << endl;
                valid = false;
            }
        }
    } while(!valid);     
    save(ofs, items);
    return valid;
}//end promptAddPromptItem()

bool promptAddAgeRestrictedItem(List<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand, minAge;
    AgeRestrictedItem * newPtr = new AgeRestrictedItem();
    
    do {
        promptGeneralPrompt(code, name, price, numOnHand, valid);
        cout << "Enter the new minimum age: ";
        getline(cin, minAge);
        valid = newPtr->setMinAge(minAge);
        if (valid) {
            try {
                delete newPtr;
                AgeRestrictedItem * newPtr = new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(numOnHand), stoi(code));
                items.pushBack(newPtr);
                valid = true;
            } catch (invalid_argument& e) {
                cerr << "One or more arguments were invalid. Try again." << endl;
                valid = false;
            }
        }
    } while(!valid);           
    save(ofs, items);
    return valid;
}//end promptAddAgeRestrictedItem()

void promptDuplicateItem(List<GMItem*> items, const int& index) {
    string input;
    bool validCode = false;
    GMItem * duplicate = new GMItem();
    int newCode = 0;
    do {
        try {
            cout << "Enter the new unique item code: ";
            getline(cin, input);
            newCode = stoi(input);
            validCode = isCodeAvailible(items, stoi(input));
        } catch(invalid_argument e) {
            validCode = false;
            cout << "Invalid input: " << input << endl;
        }
    } while(!validCode);
    delete duplicate;
    duplicate = items.getAt(index);
    duplicate->setItemCode(to_string(newCode));
    items.pushBack(duplicate);
}

void promptDeleteItem(List<GMItem*>& items) {
    string input;
    int index;
    do {
        try {
            cout << "Enter the index of them item you want to delete from the system: ";
            getline(cin, input);
            index = stoi(input);
            if(index < 0 || index > items.size()) {
                cerr << "Invalid index: " << input << endl;
            }
        }catch(invalid_argument e) {
            cerr << "Invalid input: " << input << endl;
        }
    } while( index < 0 || index > items.size());

    index = stoi(input);
    cout << "CODE        NAME                   PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
            << items.getAt(index)->toStringAdmin() << endl;
    do {
        cout << "Delete item? (y/n): ";
        getline(cin, input);
        if(input == "y") {
            items.deleteAt(index);
            save(ofs, items);
        } else if (input == "n") {
            return;
        }
    } while(input != "y" && input != "n");
}//end promptDeleteItem()

/*-----------------------------------------------------------*/
/*------------------------ Utilities ------------------------*/
/*-----------------------------------------------------------*/

void loadItemsFromFile(ifstream& ifs, List<GMItem*>& items) {
    string itemType;
    string name;
    string price;
    string qtyOnHand;
    string expirationDate;
    string code;
    string minAge;

    using type_id_t = string;
    type_id_t ageRestricted = "ar";
    type_id_t prompt = "pr";
    type_id_t general = "gm";

    getline(ifs, itemType, ',');
    while(!ifs.eof()) {
        if(itemType == prompt) {
            getline(ifs, expirationDate, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            PromptItem * pi = new PromptItem(expirationDate, name, stod(price), stoi(qtyOnHand), stoi(code));
            items.pushBack(pi);
            getline(ifs, itemType, ',');
        } else if (itemType == general) {
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            GMItem * gm = new GMItem(name, stod(price), stoi(qtyOnHand), stoi(code));
            items.pushBack(gm);
            getline(ifs, itemType, ',');
        } 
        else if (itemType == ageRestricted) {
            getline(ifs, minAge, ',');
            getline(ifs, name, ',');
            getline(ifs, price, ',');
            getline(ifs, qtyOnHand, ',');
            getline(ifs, code);

            AgeRestrictedItem * ar = new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(qtyOnHand), stoi(code));
            items.pushBack(ar);
            getline(ifs, itemType, ',');
        }
    }
}//end loadItemsFromFile()

void writeBack(ofstream& ofs, List<GMItem*>& items) {
    for(int i = 0; i < items.size(); i++) {
        ofs << items.getAt(i)->toStringBack() << endl;
    }
    // Doesn't close the file.
}//end writeItems()

void printAdminSeperators() {
    cout << "----------|------------|----------------------|-----------|--------|--------------" << endl;
}

void printAdminInfo(List<GMItem*>& items) {
     for(int i = 0; i < items.size(); i++) {
        printAdminSeperators();
        cout << setw(10) << left << i << "| " << items.getAt(i)->toStringAdmin() << endl;
    }
}

file_status_t openFileIn(ifstream& ifs, const string& fileName) {
    ifs.open(fileName);
    return ifs.is_open();
}//end openFileIn()

file_status_t openFileOut(ofstream& ofs, const string& fileName){
    ofs.open(fileName);
    return ofs.is_open();
}//end openFileOut()

bool isCodeAvailible(List<GMItem*>& items, const int& code) { 
    for(int i = 0; i < items.size(); i++) {
        if(items.getAt(i)->getItemCode() == code) {
            return false;
        }
    }
    return true;
}//end isCodeAvailible() // FIX: Plan to impliment binary search

void save(ofstream& ofs, List<GMItem*>& items) {
    openFileOut(ofs, outFileName);
    writeBack(ofs, items);
    ofs.close();
}