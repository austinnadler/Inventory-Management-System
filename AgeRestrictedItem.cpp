// File: AgeRestrictedItem.cpp

#include "AgeRestrictedItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

bool AgeRestrictedItem::setMinAge(const string& minAge) {
    try {
        this -> minAge = stoi(minAge);
        return true;
    } catch(invalid_argument& e) {
        return false;
    }
}//end setMinAge()

string AgeRestrictedItem::toStringBack() const {
    ostringstream oss;
    oss << "pa," << minAge << "," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}//end toStringBack()

string AgeRestrictedItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() <<  " Minimum Age: " << right << getMinAge() << " years" ;
    return oss.str();
}//end toStringAdmin()

string AgeRestrictedItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getMinAge();
    return oss.str();
}// end toStringFile()

void AgeRestrictedItem::operator =(const AgeRestrictedItem& o) {
    name = o.name;
    price = o.price;
    numOnHand = o.numOnHand;
    code = o.code;
    minAge = o.minAge;
}//end =()