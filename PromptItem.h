// File: PromptItem.h
// Parishable items / grocery
#ifndef PROMPTITEM_H
#define PROMPTITEM_H

#include <string>
#include <iostream>
#include "GMItem.h"
using namespace std;

class PromptItem : public GMItem {
private:
    string warning;
    
public:
    const int MAX_WARNING_LENGTH = 35;
    PromptItem(const PromptItem& copy) { 
        name = copy.name;
        price = copy.price;
        numOnHand = copy.numOnHand;
        code = copy.code;
        warning = copy.warning; 
    } // end copy ctor

    PromptItem(const string& warning = "Check for expiration", 
               const string& name = "NA", 
               const double& price = 0.0, 
               const int& numOnHand = 0, 
               const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), warning(warning)  {}//end ctor

    virtual ~PromptItem() {}//end PromptItem()

    virtual int getMaxWarningLength()   { return MAX_WARNING_LENGTH; }
    virtual string getWarning() const   { return warning; }//end getExpirationDate()
    virtual bool setWarning(const string& warning); // any format, but must be short than 20 characters

    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
    virtual string toStringBack() const;

};

#endif