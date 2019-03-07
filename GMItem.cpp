// File: GMItem.cpp

// General Merchandise Items (Non-grocery)
#include "GMItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

bool GMItem::setItemName(const string& name)  {
    if(!(name.length() > MAX_NAME_LENGTH)) {
        this -> name = name;
        return true;
    } else {
        return false;
    }
}//end setItemName()

bool GMItem::setItemPrice(const string& price){
        try {
            if(stod(price) < 0) {
                return false;
            } else {
                this -> price = stod(price);
                return true; 
            } 
        } catch (invalid_argument& e) {
            return false;
        }
    
}//end setItemPrice()

bool GMItem::setNumOnHand(const string& n) {
    try {
        if(stoi(n) < 0) {
                return false;
        } else {
            this -> numOnHand = stoi(n);
            return true;
        }
    } catch (invalid_argument& e) {
         return false;
    }
    
}//end setNumOnHand()

bool GMItem::decreaseCount(const string& n) {
    try {
        if(stoi(n) < 0) {
            return false;
        } else {
            this -> numOnHand -= stoi(n);
            return true;
        }
    } catch (invalid_argument& e) {
        return false;
    }
}//end decreaseCount()

bool GMItem::increaseCount(const string& n) {
    try {
        if(stoi(n) < 0) {
            return false;
        } else {
            this -> numOnHand += stoi(n);
            return true;
        }
    } catch (invalid_argument& e) {
        return false;
    }
}//end increaseCount()

bool GMItem::setItemCode(const string& code) {
        try {
            if(code.length() > 5 || stoi(code) < 0) {
                return false;
            } else {
                this -> code = stoi(code);
                return true;
            }
        } catch (invalid_argument& e) {
            return false;
        }
    
}//end setItemCode()

string GMItem::toStringBack() const {
    ostringstream oss;
    oss << "gm," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}//end toStringBack()

string GMItem::toStringAdmin() const {
    ostringstream oss;
    oss << setw(10) << left << code << " | " << setw(20) << name  << right << " |" << setw(10) << fixed << setprecision(2) << price << " | " << left << setw(6) << numOnHand << " |" << "                                                              " << "|";
    return oss.str();
}//end toStringAdmin()

string GMItem::toStringFile() const {
    ostringstream oss;
    oss << code << "," <<  name << "," << fixed << setprecision(2) << price << "," << numOnHand;
    return oss.str();
}//end toStringFile()

void GMItem::operator =(const GMItem& o) {
    name = o.name;
    price = o.price;
    numOnHand = o.numOnHand;
    code = o.code;
}//end =()

bool GMItem::operator ==(const GMItem& o) const {
    return price == o.price;
}//end ==()

bool GMItem::operator !=(const GMItem& o) const {
    return price != o.price;
}//end !=()

bool GMItem::operator <(const GMItem& o) const {
    return price < o.price;
}//end <()

bool GMItem::operator <=(const GMItem& o) const {
    return price <= o.price;
}//end <=()

bool GMItem::operator >(const GMItem& o) const {
    return price > o.price;
}//end >()

bool GMItem::operator >=(const GMItem& o) const {
    return price >= o.price;
}//end >=()