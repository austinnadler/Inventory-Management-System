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

string ParishableItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() << " Expiration: "  << setw(11) << right << getExpirationDate();
    return oss.str();
}

string ParishableItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getExpirationDate();
    return oss.str();
}// end toStringFile()
