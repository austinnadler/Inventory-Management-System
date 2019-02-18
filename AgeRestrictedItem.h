// File: AgeRestrictedItem.h

#ifndef AGERESTRICTEDITEM_H
#define AGERESTRICTEDITEM_H

#include <string>
#include <iostream>
#include "GMItem.h"
using namespace std;

class AgeRestrictedItem : public GMItem {
private:
    int minAge;
public:
    AgeRestrictedItem(const int& minAge = 18, const string& name = "NA", const double& price = 0.0, const int& numOnHand = 0, const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), minAge(minAge)
    {}
    ~AgeRestrictedItem() {}//end AgeRestrictedItem()
    
    bool setMinAge(const string& minAge);

    int getMinAge() const  { return minAge; }//end getExpirationDate()
    
    virtual string toStringPOS() const;
    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
    virtual string toStringBack() const; 
};

#endif