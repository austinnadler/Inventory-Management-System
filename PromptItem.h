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
    string prompt;
    
public:
    const int MAX_PROMPT_LENGTH = 35;
    PromptItem(const PromptItem& copy) { 
        name = copy.name;
        price = copy.price;
        numOnHand = copy.numOnHand;
        code = copy.code;
        prompt = copy.prompt; 
    } // end copy ctor

    PromptItem(const string& prompt = "Check for expiration", 
               const string& name = "NA", 
               const double& price = 0.0, 
               const int& numOnHand = 0, 
               const int& code = -1)  
    :   GMItem(name, price, numOnHand, code), prompt(prompt)  {}//end ctor

    virtual ~PromptItem() {}//end PromptItem()

    virtual string getPrompt() const   { return prompt; }//end getExpirationDate()
    virtual bool setPrompt(const string& prompt); // any format, but must be short than 20 characters

    virtual string toStringAdmin() const;
    virtual string toStringFile() const;
    virtual string toStringBack() const;

    void operator =(const PromptItem& o);
};

#endif