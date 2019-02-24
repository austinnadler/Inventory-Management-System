// File: PromptItem.cpp

#include "PromptItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

string PromptItem::toStringBack() const {
    ostringstream oss;
    oss << "pa," << warning << "," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}



bool PromptItem::setWarning(const string& warning) {
    if(!(warning.length() > MAX_WARNING_LENGTH)) {
        try {
            this -> warning = warning;
            return true;
        } catch (invalid_argument e) {
            return false;
        }
    } else {
        return false;
    }
}// end setWarning()



string PromptItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() << " Prompt: " << "\"" << getWarning() << "\"";
    return oss.str();
}// and toStringAdmin()



string PromptItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getWarning();
    return oss.str();
}// end toStringFile() 