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
    ExpiringItem(const ExpiringItem& copy) { 
        name = copy.name;
        price = copy.price;
        numOnHand = copy.numOnHand;
        code = copy.code;
        warning = copy.warning; 
    } // end copy ctor
    ExpiringItem(const string& warning = "Check for expiration", 
                 const string& name = "NA", const double& price = 0.0, 
                 const int& numOnHand = 0, 
                 const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), warning(warning)  {}//end ctor

    virtual ~ExpiringItem() {}//end ExpiringItem()

    virtual int getMaxWarningLength()   { return MAX_WARNING_LENGTH; }
    virtual string getWarning() const   { return warning; }//end getExpirationDate()

    virtual bool setWarning(const string& warning); // any format, but must be short than 20 characters

    
    virtual string toStringPOS() const;
    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
    virtual string toStringBack() const;

};

#endif