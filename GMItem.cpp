// File: GMItem.cpp

// General Merchandise Items (Non-grocery)
#include "GMItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

string GMItem::toStringBack() const {
    ostringstream oss;
    oss << "gm," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}



bool GMItem::setItemName(const string& name)  {
    if(!(name.length() > MAX_NAME_LENGTH)) {
        this -> name = name;
        return true;
    } else {
        return false;
    }
}



bool GMItem::setItemPrice(const string& price){
        try {
            if(stod(price) < 0) {
                return false;
            } else {
                this -> price = stod(price);
                return true; 
            } 
        } catch (invalid_argument e) {
            return false;
        }
    
}



bool GMItem::setNumOnHand(const string& n) {
    try {
        if(stoi(n) < 0) {
                return false;
        } else {
            this -> numOnHand = stoi(n);
            return true;
        }
    } catch (invalid_argument e) {
         return false;
    }
    
}



bool GMItem::decreaseCount(const string& n) {
    try {
        if(stoi(n) < 0) {
            return false;
        } else {
            this -> numOnHand -= stoi(n);
            return true;
        }
    } catch (invalid_argument e) {
        return false;
    }
}



bool GMItem::increaseCount(const string& n) {
    try {
        if(stoi(n) < 0) {
            return false;
        } else {
            this -> numOnHand += stoi(n);
            return true;
        }
    } catch (invalid_argument e) {
        return false;
    }
}



bool GMItem::setItemCode(const string& code) {
        try {
            if(code.length() > 5 || stoi(code) < 0) {
                return false;
            } else {
                this -> code = stoi(code);
                return true;
            }
        } catch (invalid_argument e) {
            return false;
        }
    
}


string GMItem::toStringAdmin() const {
    ostringstream oss;
    oss << setw(10) << left << code << " | " << setw(20) << name  << right << "|" << setw(10) << fixed << setprecision(2) << price << " | " << left << setw(6) << numOnHand << " |";
    return oss.str();
}//end toStringAdmin()


string GMItem::toStringFile() const {
    ostringstream oss;
    oss << code << "," <<  name << "," << fixed << setprecision(2) << price << "," << numOnHand;
    return oss.str();
}// end toStringFile()