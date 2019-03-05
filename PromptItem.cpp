// File: PromptItem.cpp

#include "PromptItem.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

bool PromptItem::setPrompt(const string& prompt) {
    if(!(prompt.length() > MAX_PROMPT_LENGTH)) {
        try {
            this -> prompt = prompt;
            return true;
        } catch (invalid_argument& e) {
            return false;
        }
    } else {
        return false;
    }
}// end setprompt()

string PromptItem::toStringBack() const {
    ostringstream oss;
    oss << "pr," << prompt << "," << name << "," << price << "," << numOnHand << "," << code;
    return oss.str();
}//end toStringBack()

string PromptItem::toStringAdmin() const {
    ostringstream oss;
    oss << GMItem::toStringAdmin() << " Prompt: " << "\"" << getPrompt() << "\"";
    return oss.str();
}// and toStringAdmin()

string PromptItem::toStringFile() const {
    ostringstream oss;
    oss << GMItem::toStringFile() << "," << getPrompt();
    return oss.str();
}// end toStringFile() 

void PromptItem::operator =(const PromptItem& o) {
    name = o.name;
    price = o.price;
    numOnHand = o.numOnHand;
    code = o.code;
    prompt = o.prompt;
}//end =()
