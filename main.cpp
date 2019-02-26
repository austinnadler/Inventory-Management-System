// File: main.cpp

// FIX: debug saving further
// FIX: crea

#include "main.h"

int main() {
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
    sort(inventory);
    do {
        //FIX: Maybe use iomanip here, but i found it easier to just have this fixed and have the objects' output be controlled by iomanip to fit here
        cout << "INDEX      CODE         NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl;
        printAdminInfo(inventory);
    // MENU 1
        cout << "----------|------------|----------------------|-----------|--------|--------------" << endl
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
            string input;
            int index;
            int maxIndex = inventory.size() - 1;
                do {
                    try {
                        cout << "Enter the index of them item you want to edit: ";
                        getline(cin, input);
                        if(input == "exit") {
                            save(ofs, inventory);
                            return 0;
                        }
                        index = stoi(input);

                        if(index < 0 || index > maxIndex) {
                            cerr << "Invalid index: " << input << endl;
                        }
                    }catch(invalid_argument e) {
                        cerr << "Invalid input: " << input << endl;
                    }
                }while( index < 0 || index > maxIndex);

                GMItem * itemPtr = inventory.at(index);
                found = true;
                bool doneWithThisItem = false;
                do {
                    cout << "Item found: " << itemPtr->getItemName() << endl << endl
                         << "1. Change number on hand" << endl
                         << "2. Change price" << endl
                         << "3. Change item name" << endl
                         << "4. Change item code" << endl
                         << "5. Change item warning prompt" << endl
                         << "6. Change item minimum age" << endl << endl
                         << "Enter the number of the action you want to perform: ";    
                    getline(cin,input);

                    if(input == "exit") {
                        save(ofs, inventory);
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
                                    save(ofs, inventory);
                                    return 0;
                                    } else {
                                        if(input == "a") {
                                            do {
                                                cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                     << "Increase count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                                getline(cin, input);
                                                valid = itemPtr->increaseCount(input);
                                                if(!valid) {
                                                    if(input == "exit") {
                                                        save(ofs, inventory);
                                                        return 0;
                                                    } else {
                                                        cerr << "Invalid input: " << input << endl;
                                                    }  
                                                }   
                                            } while(!valid);
                                        } else if (input == "b") {
                                            do {
                                                cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                    << "Decrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
                                                getline(cin, input);
                                                save(ofs, inventory);

                                                if(input == "exit") {
                                                    save(ofs, inventory);
                                                    return 0;
                                                } else {
                                                    valid = itemPtr->decreaseCount(input);
                                                    if(!valid) {
                                                        cerr << "Invalid input: " << input << endl;
                                                    }
                                                }
                                            } while(!valid);
                                        } else if(input == "c") {
                                            do {
                                                cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                                                    << "Enter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
                                                getline(cin, input);
                                                if(input == "exit") {
                                                    save(ofs, inventory);
                                                    return 0;
                                                } else {
                                                    valid = itemPtr->setNumOnHand(input);
                                                    if(!valid) {
                                                        cerr << "Invalid input: " << input << endl;
                                                    }
                                                }
                                            } while(!valid);
                                        } else {
                                            valid = false;
                                        }
                                    }                              
                                } while(!valid && input != "exit");
                                save(ofs, inventory); 
                            } else if(input == "2") {
                                promptChangePrice(itemPtr);    
                                save(ofs, inventory);    
                            } else if(input == "3") {
                                promptChangeName(itemPtr);
                                save(ofs, inventory);    
                            } else if (input == "4") {
                                promptChangeCode(inventory, itemPtr);
                                save(ofs, inventory);    
                            } else if(input == "5") {
                                promptChangePrompt(inventory, index);
                                save(ofs, inventory);    
                            } else if(input == "6") {
                                promptChangeMinAge(inventory, index);
                                save(ofs, inventory);    
                            } else if (input == "exit") {
                                save(ofs, inventory);
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
                    
                found = false;
        } else if(input == "2") {
            do {
                cout << "a. A GMItem with no special characteristics." << endl
                    << "b. A PromptItem with a warning to be stored." << endl
                    << "c. An AgeRestrictedItem that has a minimum purchaser age to store." << endl
                    << "Enter the character of your choice: ";
                getline(cin, input);
                if(input == "exit") {
                    save(ofs, inventory);
                    return 0;
                }
                if(input == "a") {
                    promptAddGMItem(inventory);
                } else if (input == "b") {
                    promptAddPromptItem(inventory);                    
                } else if(input == "c") {
                    promptAddAgeRestrictedItem(inventory);  
                }
            } while(input != "a" || input != "b" || input != "c");
        } else if(input == "3") {
            promptDeleteItem(inventory);
        }
    
    } while(input != "exit");

    cout << "Exiting..." << endl;
    file_status_t fOutStatus = openFileOut(ofs, outFileName);


    // cout << "Enter the name of your outfile: ";
    // getline(cin, outFileName);

    if(!fOutStatus) {
        cout << "Error opening file \"" << outFileName << "\"" << endl;
        exit(1);
    }
    // After making all the changes, write the new vector to the file and close the stream
    save(ofs, inventory);
    return 0;
}// end main()

/*--------------------------------------------------------------*/
/*------------------------ User Actions ------------------------*/
/*--------------------------------------------------------------*/

void promptChangeName(GMItem * itemPtr) {
    string input;
    bool valid = false;
    GMItem * test = new GMItem();
    do {
        cout << "\nEnter 'exit' to quit. The name can be anything " << itemPtr->getMaxNameLength() << " characters or less." << endl
             << "\nEnter the new item namefor item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        } else {
            valid = test->setItemName(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } else {
                itemPtr->setItemName(input);
                save(ofs, inventory);
            }
        }
    } while(!valid);
    delete test;
}//end promptChangeName()

void promptChangePrice(GMItem * itemPtr) {
    string input;
    bool valid = false;
    GMItem * test = new GMItem();
    do {
        cout << "Enter an integer or decimal value (e.g. 5, or 4.99)" << endl
             << "\nEnter 'exit' to quit." << endl
             << "\nEnter the new price for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
        getline(cin, input);
        valid = test->setItemPrice(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } else {
                itemPtr->setItemPrice(input);
                save(ofs, inventory);
            }
        } while(!valid && input != "exit");
    valid = false;  
    delete test;
}//end promptChangePrice()

void promptChangeCount(GMItem * itemPtr) {
    string input;
    bool valid = false;
    GMItem * test = new GMItem();
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
            valid = test->increaseCount(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } else {
                itemPtr->decreaseCount(input);
                save(ofs, inventory);
            }
        } while(!valid && input != "exit");
    valid = false;
    } else if (input == "b") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nDecrease count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << " by: ";
            getline(cin, input);
            valid = test->decreaseCount(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } else {
                itemPtr->decreaseCount(input);
                save(ofs, inventory);
            }
        } while(!valid && input != "exit");
    valid = false;     
    } else if(input == "c") {
        do {
            cout << "\nEnter 'exit' to quit. Only integer values accepted." << endl
                 << "\nEnter the new on hand count for item " << itemPtr->getItemName() << " " << itemPtr->getItemCode() << ": ";
            getline(cin, input);
            valid = test->setNumOnHand(input);
            if(!valid) {
                cerr << "Invalid input: " << input << endl;
            } else {
                itemPtr->decreaseCount(input);
                save(ofs, inventory);
            }
        } while(!valid && input != "exit");
    valid = false;
    }
    delete test;
}//end promptChangeCount()

void promptChangeCode(vector<GMItem*>& inventory, GMItem * item) {
    string input;
    bool isInt = false;
    bool isTaken = true;
    bool allGood = false;
    GMItem * test = new GMItem(); // Test with new item for safety
    do {
        do {
            cout << "Enter the new item code. This must be " << item->CODE_MAX_DIGITS <<  " digits or shorter: ";
            getline(cin, input);
            isInt = test->setItemCode(input);
        } while(!isInt);

        if(isInt) {
            isTaken = isCodeTaken(inventory, stoi(input));
            if(isTaken) {
                cout << "Code " << input << " is already taken." << endl;
            } else {
                item->setItemCode(input);
            }
        }

        if(isInt && !isTaken) {
            allGood = true;
            save(ofs, inventory);
        }
    } while(!allGood);

    delete test;
}//propmtChangeCode()

void promptChangePrompt(vector<GMItem*>& items, const int& index) {
    string input;
    bool valid = false;
    PromptItem * newPtr = new PromptItem();
    PromptItem * prPtr = nullptr;
    prPtr = dynamic_cast<PromptItem*>(prPtr);
    do {
        cout << "Enter 'r' to remove the warning from this item, or 'a' or 'c' to add or change the prompt." << endl;
        getline(cin, input);
        if(input == "exit") {
            valid = true;
        }
        if(input == "a" || input == "c") {
            cout << "Warning prompts must be " << newPtr->getMaxWarningLength() << " characters or shorter." << endl
                 << "Enter the new warning for item " << items.at(index)->getItemName() << " - " << items.at(index)->getItemCode() << ": ";
            getline(cin, input);
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
   
        } else if(input == "r") {
            GMItem * newItemPtr = new GMItem(items.at(index)->getItemName(),
                                             items.at(index)->getItemPrice(),
                                             items.at(index)->getNumOnHand(),
                                             items.at(index)->getItemCode());
            delete items.at(index);
            items.at(index) = newItemPtr;
            save(ofs, items);
            valid = true;
        }
    } while(!valid);
}//end promptChangePrompt()

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
            save(ofs, items);
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
                save(ofs, items);
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

void promptAddGMItem(vector<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand;
    promptGeneralPrompt(code, name, price, numOnHand, valid);
    GMItem * newPtr = new GMItem(name, stod(price), stoi(numOnHand), stoi(code));
    items.push_back(newPtr);
    save(ofs, items);
}//end promptAddGMItem()

void promptAddPromptItem(vector<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand, warning;
    PromptItem * test = new PromptItem();
    
    do {
        promptGeneralPrompt(code, name, price, numOnHand, valid);
        cout << "Enter the 20 character maximum warning: ";
        getline(cin, warning);
        valid = test->setWarning(warning);
        if (valid) {
            try {
                PromptItem * newPtr = new PromptItem(warning, name, stod(price), stoi(numOnHand), stoi(code));
                items.push_back(newPtr);
                valid = true;
            } catch (invalid_argument& e) {
                cerr << "One or more arguments were invalid. Try again." << endl;
                valid = false;
            }
        }
    } while(!valid);     
    save(ofs, items);
    delete test;
}//end promptAddPromptItem()

void promptAddAgeRestrictedItem(vector<GMItem*>& items) {
    bool valid = false;
    string code, name, price, numOnHand, minAge;
    AgeRestrictedItem * test = new AgeRestrictedItem();
    
    do {
        promptGeneralPrompt(code, name, price, numOnHand, valid);
        cout << "Enter the new minimum age: ";
        getline(cin, minAge);
        if(minAge == "exit") {
            save(ofs, items);
            return;
        }
        valid = test->setMinAge(minAge);
        if (valid) {
            try {
                AgeRestrictedItem * newPtr = new AgeRestrictedItem(stoi(minAge), name, stod(price), stoi(numOnHand), stoi(code));
                items.push_back(newPtr);
                valid = true;
            } catch (invalid_argument& e) {
                cerr << "One or more arguments were invalid. Try again." << endl;
                valid = false;
            }
        }
    } while(!valid);           
    save(ofs, items);
    delete test;
}//end promptAddAgeRestrictedItem()

void promptDeleteItem(vector<GMItem*>& items) {
    string input;
    int index;
    int maxIndex = items.size() - 1;
    do {
        do {
            try {
                cout << "Enter the index of them item you want to delete from the system: ";
                getline(cin, input);
                index = stoi(input);
                if(index < 0 || index > maxIndex) {
                    cerr << "Invalid index: " << input << endl;
                }
            }catch(invalid_argument e) {
                cerr << "Invalid input: " << input << endl;
            }
        }while( index < 0 || index > maxIndex);

        try {
            index = stoi(input);
            cout << "CODE       NAME                  PRICE       QTY OH   EXPIRATION / MIN. AGE" << endl
                 << items.at(index)->toStringAdmin() << endl
                 << "Delete item? (y/n): ";
            getline(cin, input);
            if(input == "y") {
                items.erase(items.begin() + index);
                save(ofs, items);
            } else if (input == "n") {
                return;
            }       
        } catch (invalid_argument& e) {
             cerr << "Invalid code entered: " << input;
        }
    } while (input != "y" && input != "n" && input != "exit");
}//end promptDeleteItem()

/*-----------------------------------------------------------*/
/*------------------------ Utilities ------------------------*/
/*-----------------------------------------------------------*/

void loadItemsFromFile(ifstream& ifs, vector<GMItem*>& items) {
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
}//end loadItemsFromFile()

void writeBack(ofstream& ofs, vector<GMItem*>& items) {
    for(int i = 0; i < items.size(); i++) {
        ofs << items.at(i)->toStringBack() << endl;
    }
    // DOES NOT CLOSE FILE! THIS IS INTENTIONAL SO THAT MULTIPLE CHANGES CAN BE MADE!
}//end writeItems()

void printAdminInfo(vector<GMItem*>& items) {
     for(int i = 0; i < items.size(); i++) {
        cout << "----------|------------|----------------------|-----------|--------|--------------" << endl;
        cout << setw(10) << left << i << "| " << items.at(i)->toStringAdmin() << endl;
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

bool isCodeTaken(vector<GMItem*>& items, const int& code) { 
    for(int i = 0; i < items.size(); i++) {
        if(items.at(i)->getItemCode() == code) {
            return true;
        }
    }
    return false;
}//end isCodeTaken() // FIX: Plan to impliment binary search

void sort(vector<GMItem*>& items) {
    sort(items.begin(), items.end(), [](const GMItem* lhs, const GMItem* rhs) {
        return lhs->getItemCode() < rhs->getItemCode();
    });
}//end sort()

void save(ofstream& ofs, vector<GMItem*>& items) {
    openFileOut(ofs, outFileName);
    writeBack(ofs, items);
    ofs.close();
}