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
        int code;   // UPC
    public:
        const int MAX_NAME_LENGTH = 20; // Name can be 20 characters long, or the method will return false. This is hard coded in error statements be
        const int MAX_PRICE_LENGTH = 9; // 123456.89 Price can be 9 characters long total, or the method will return false. 
        const int MAX_CODE_LENGTH = 15;

        GMItem(const string& name = "N/A", const double& price = 0.0, const int& numOnHand = 0, const int& code = -1);
        ~GMItem() {}//end ~GMItem()

        virtual bool setItemName(const string& name) final;
        virtual bool setItemPrice(const string& price) final;
        virtual bool setNumOnHand(const string& n) final;
        virtual bool decreaseCount(const string& n = "1") final;
        virtual bool increaseCount(const string& n = "1") final;
        virtual bool setItemCode(const string& code) final;
        
        virtual string getItemName()  const final { return name; }//end getItemName()
        virtual double getItemPrice() const final { return price; }//end getItemPrice()
        virtual int getItemCode()     const final { return code; }//end getItemCode
        virtual int getNumOnHand()    const final { return numOnHand; }//end getNumOnHand()
        virtual int getMaxNameLength() const final { return MAX_NAME_LENGTH; }//end getMaxNameLength()
        virtual int getMaxPriceLength() const final { return MAX_PRICE_LENGTH; }//end getMaxPriceLength()
        virtual int getMaxCodeLength() const final { return MAX_CODE_LENGTH; }//end getMaxCodeLength()

        
        virtual string toStringPOS() const;
        virtual string toStringAdmin() const;
        virtual string toStringFile() const;
};

ostream& operator <<(ostream& os, const GMItem& o);

#endif