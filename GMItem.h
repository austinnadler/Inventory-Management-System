//File: GMItem.h

#ifndef GMITEM_H
#define GMITEM_H
#include <string>
using namespace std;
class GMItem {
    protected:
        string name;
        double price;
        int numOnHand;
        int code; // 5 digit (or less) code

    public:
        const int MAX_NAME_LENGTH = 20; // Name can be 20 characters long, or the method will return false. This is hard coded in error statements be
        const int CODE_MAX_DIGITS = 5;

        GMItem(const string& name = "N/A", 
               const double& price = 0.0,
               const int& numOnHand = 0,
               const int& code = -1)
        : name(name), price(price), numOnHand(numOnHand), code(code) {} //end ctor

        virtual ~GMItem() {}//end ~GMItem()

        virtual bool setItemName  (const string& name) final;
        virtual bool setItemPrice (const string& price) final;
        virtual bool setNumOnHand (const string& n) final;
        virtual bool setItemCode  (const string& code) final;

        virtual bool decreaseCount(const string& n = "1") final;
        virtual bool increaseCount(const string& n = "1") final;
        
        virtual string getItemName()   const final  { return name; }//end getItemName()
        virtual double getItemPrice()  const final  { return price; }//end getItemPrice()
        virtual int getItemCode()      const final  { return code; }//end getItemCode
        virtual int getNumOnHand()     const final  { return numOnHand; }//end getNumOnHand()
        
        virtual int getMaxNameLength() const final  { return MAX_NAME_LENGTH; }//end getMaxNameLength()
        virtual int getMaxCodeDigits() const final  { return CODE_MAX_DIGITS; }//end getMaxCodeDigits()

        virtual string toStringAdmin() const;
        virtual string toStringFile()  const;
        virtual string toStringBack()  const; // make a new string in the same format as the input file is read in
};

#endif