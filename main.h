#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
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
void promptChangeCode(GMItem * itemPtr);
bool isCodeTaken(vector<GMItem*>& inventory, const int& code);       // Was planning to impliment a binary search, but this takes less than a second even with over 1 million items, so its good enough
void promptChangePrompt(vector<GMItem*>& items, const int& index);
void promptChangeMinAge(vector<GMItem*>& items, const int& index);

/* Functions to add items to the inventory system */
/* Created to simplify the main method            */
void promptGeneralPrompt(string& code, string& name, string& price, string& numOnHand, bool& valid); //Prompt that all subclasses use first with error trapping
bool promptAddGMItem(vector<GMItem*>& items);
bool promptAddPromptItem(vector<GMItem*>& items);
bool promptAddAgeRestrictedItem(vector<GMItem*>& items);
void promptDeleteItem(vector <GMItem*>& items);                      
void writeBack(ofstream& ofs, vector<GMItem*>& items);               // writes to a new file with the same format as the example input file so it can be re-used.
void printAdminInfo(vector<GMItem*>& items);                         // outputs to the screen the list of objects with all special information, for use in performAdminFunctions()
void loadItemsFromFile(ifstream& ifs, vector<GMItem*>& items);       // take an ifs and an empty array and fill said array with items from a FORMATTED file
void writeItems(ofstream& ofs, vector<GMItem*>& items);              // take a provided ofstream and array of items and write them to the ofstream using the toStringFile() method
void sort(vector<GMItem*>& items);
void save(ofstream& ofs, vector<GMItem*>& items);                    // writeBack() and close file.
file_status_t openFileIn(ifstream& ifs, const string& fileName);     // use provided ifstream to open provided filename to read information from
file_status_t openFileOut(ofstream& ofs, const string& fileName);    // use provided ofstream to open provided filename to write data to

/* Used in several functions */
string inFileName = "items.csv"; // only used in loadItemFromFile(), here for organization
string outFileName = "itemsOut.csv";
ifstream ifs; // only used in loadItemFromFile(), here for organization
ofstream ofs;
vector<GMItem*> inventory;
using namespace std;

#endif