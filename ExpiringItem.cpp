// File: ExpiringItem.cpp

#include "ExpiringItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

string ExpiringItem::toStringPOS() const {
    ostringstream oss;
    oss << GMItem::toStringPOS();
    return oss.str();
}// end toStringPOS()



string ExpiringItem::toStringBack() const {
    ostringstream oss;
    oss << "pa," << warning << "," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}



bool ExpiringItem::setWarning(const string& warning) {
    if(!(warning.length() > MAX_WARNING_LENGTH)) {
        try {
            this -> warning = warning;
            return true;
        } catch (invalid_argument e) {
            return false;
        }
    } else {
        return false;
    }
}// end setWarning()



string ExpiringItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() << " Warning: " << "\"" << getWarning() << "\"";
    return oss.str();
}// and toStringAdmin()



string ExpiringItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getWarning();
    return oss.str();
}// end toStringFile() 