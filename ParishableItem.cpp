// File: ParishableItem.cpp

#include "ParishableItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

string ParishableItem::toStringPOS() const {
    ostringstream oss;
    oss << GMItem::toStringPOS();
    return oss.str();
}// end toStringPOS()


string ParishableItem::toStringBack() const {
    ostringstream oss;
    oss << "pa," << expirationDate << "," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}

bool ParishableItem::setExpirationDate(const string& expirationDate) {
    if(!(expirationDate.length() > MAX_EXPDATE_LENGTH)) {
        try {
            this -> expirationDate = expirationDate;
            return true;
        } catch (invalid_argument e) {
            return false;
        }
    } else {
        return false;
    }
}// end setExpirationDate

string ParishableItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() << " Expiration: "  << setw(11) << right << getExpirationDate();
    return oss.str();
}// and toStringAdmin()

string ParishableItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getExpirationDate();
    return oss.str();
}// end toStringFile()
