// File: ParishableItem.h
// Parishable items / grocery
#ifndef PARISHABLEITEM_H
#define PARISHABLEITEM_H

#include <string>
#include <iostream>
#include "GMItem.h"
using namespace std;

class ParishableItem : public GMItem {
private:
    string expirationDate;
public:
    const int MAX_EXPDATE_LENGTH = 20;
    ParishableItem(const string& expirationDate = "MM/DD/YY", const string& name = "NA", const double& price = 0.0, const int& numOnHand = 0, const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), expirationDate(expirationDate)
    {}//end ParishableItem()
    ~ParishableItem() {}//end ParishableItem()

    bool setExpirationDate(const string& expirationDate); // any format, but must be short than 20 characters

    int getMaxDateLength() { return MAX_EXPDATE_LENGTH; }

    string getExpirationDate() const                     { return expirationDate; }//end getExpirationDate()
    
    virtual string toStringPOS() const;
    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
};

#endif