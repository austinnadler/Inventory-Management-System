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
        GMItem(const string& name = "NA", const double& price = 0.0, const int& numOnHand = 0, const int& code = -1);
        ~GMItem() {}//end ~GMItem()

        virtual void setItemName(const string& name) final  { this -> name  = name; }//end setItemName()
        virtual void setItemPrice(const double& price) final { this -> price = price; }//end setItemPrice()
        virtual void setNumOnHand(const int& n) final { numOnHand = n; }//end setNumOnHand()
        virtual void decreaseCount(const int& n = 1) final { numOnHand -= n; } //end decreaseCount()
        virtual void increaseCount(const int& n = 1) final { numOnHand += n; }//end increaseCount()
        
        virtual string getItemName()  const final { return name; }//end getItemName()
        virtual double getItemPrice() const final { return price; }//end getItemPrice()
        virtual int getItemCode()     const final { return code; }//end getItemCode
        virtual int getNumOnHand()    const final { return numOnHand; }//end getNumOnHand()
        
        virtual string toStringPOS() const;
        virtual string toStringAdmin() const;
        virtual string toStringFile() const;
};

ostream& operator <<(ostream& os, const GMItem& o);

#endif