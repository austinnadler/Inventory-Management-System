// File: GMItem.cpp

// General Merchandise Items (Non-grocery)
#include "GMItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

GMItem::GMItem(const string& name, const double& price, const int& numOnHand, const int& code) 
: name(name), price(price), numOnHand(numOnHand), code(code)
{}


string GMItem::toStringPOS() const {
    ostringstream oss;
    oss << left << code << "\t" << setw(25) << setfill('.') << name << right << fixed << setprecision(2) << price;
    return oss.str();
}// end toStringPOS()

string GMItem::toStringAdmin() const {
    ostringstream oss;
    oss << setw(10) << left << code << "| " << setw(20) << name  << right << "|" << setw(10) << fixed << setprecision(2) << price << " | " << left << setw(6) << numOnHand << " |";
    return oss.str();
}//end toStringAdmin()


string GMItem::toStringFile() const {
    ostringstream oss;
    oss << code << "," <<  name << "," << fixed << setprecision(2) << price << "," << numOnHand;
    return oss.str();
}// end toStringFile()

// Overload not implimented yet, need to learn more about pointers
// ostream& operator <<(ostream& os, const GMItem& o) {}// end <<()