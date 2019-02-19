// File: ExpiringItem.h
// Parishable items / grocery
#ifndef EXPIRINGITEM_H
#define EXPIRINGITEM_H

#include <string>
#include <iostream>
#include "GMItem.h"
using namespace std;

class ExpiringItem : public GMItem {
private:
    string warning;
public:
    const int MAX_WARNING_LENGTH = 20;
    ExpiringItem(const string& warning = "Check for expiration", const string& name = "NA", const double& price = 0.0, const int& numOnHand = 0, const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), warning(warning)
    {}//end ExpiringItem()

    ~ExpiringItem() {}//end ExpiringItem()

    bool setWarning(const string& warning); // any format, but must be short than 20 characters

    int getMaxWarningLength() { return MAX_WARNING_LENGTH; }

    string getWarning() const                     { return warning; }//end getExpirationDate()
    
    virtual string toStringPOS() const;
    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
    virtual string toStringBack() const;

};

#endif