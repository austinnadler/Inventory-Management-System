// File: AgeRestrictedItem.cpp

#include "AgeRestrictedItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

string AgeRestrictedItem::toStringPOS() const {
    ostringstream oss;
    oss << GMItem::toStringPOS();
    return oss.str();
}// end toStringPOS()

bool AgeRestrictedItem::setMinAge(const string& minAge) {
    try {
        this -> minAge = stoi(minAge);
        return true;
    } catch(invalid_argument e) {
        return false;
    }
}

string AgeRestrictedItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() <<  " Minimum Age: " << setw(10) << right << getMinAge() ;
    return oss.str();
}

string AgeRestrictedItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getMinAge();
    return oss.str();
}// end toStringFile()
