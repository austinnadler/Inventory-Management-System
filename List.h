// File: List.h

#ifndef LIST_H
#define LIST_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

template <typename E>
class List {
private:
    using elementPtr_t = E *;

    // This the the dynamic array.
    elementPtr_t elements = nullptr;
    int numberOfElements = 0;


    /*----------------------------------------------------------------*/
    // Utility methods
    void copyElements(const List& copy) {
        numberOfElements = copy.numberOfElements;
        elements = new E[numberOfElements];

        for (int i = 0; i <= numberOfElements - 1; i++) {
            elements[i] = copy.elements[i];
        }
    }// end

    void releaseElements() {
        delete [] elements;
        elements = nullptr;
        numberOfElements = 0;
    }// end
    /*----------------------------------------------------------------*/

public:
    List() { /* field declarations initialize the object. */ }// end List()

    // The big three.
    List(const List& copy) { copyElements(copy); }// end List()
    virtual ~List()        { releaseElements();  }// end ~List()
    const List& operator =(const List& rhs) {
        /* Check for self-assignment. */
        if (this != &rhs) {
            releaseElements();
            copyElements(rhs);
        }

        // Return a reference to the object itself.
        return *this;
    }// end =()
    /*----------------------------------------------------------------*/

    void pushBack(const E& element)  { pushAt(numberOfElements, element); }// end pushBack()
    void pushFront(const E& element) { pushAt(0, element);                }// end pushFront()
    void pushAt(const int index, const E& element) {
        if (index < 0 || index > numberOfElements) {
            throw range_error( "Invalid index: " + to_string(index) );
        }

        if ( isEmpty() ) {
            elements = new E[1] { element };

        } else {
            elementPtr_t tmpElements = new E[numberOfElements + 1];

            // Copy all elements to the tmp array, starting from the back.
            for (int i = numberOfElements - 1; i >= index; i--) {
                tmpElements[i + 1] = elements[i];
            }
            // Add the new value to the tmp array.
            tmpElements[index] = element;

            // Continue copying the elements array.
            for (int i = index - 1; i >= 0; i--) {
                tmpElements[i] = elements[i];
            }

            delete [] elements;
            elements = tmpElements;
            tmpElements = nullptr;
        }
        numberOfElements += 1;
    }// end
    /*----------------------------------------------------------------*/

    E popBack()  { return popAt(numberOfElements - 1); }// end popBack()
    E popFront() { return popAt(0);                    }// end popFront()
    E popAt(const int index) {
        if (index < 0 || index >= numberOfElements) {
            throw range_error( "Invalid index: " + to_string(index) );
        }

        elementPtr_t tmpElements = new E[numberOfElements - 1];

        // Copy all elements before index to the tmp array, starting from the back.
        for (int i = numberOfElements - 1; i > index; i--) {
            tmpElements[i - 1] = elements[i];
        }
        // Remove the value to the tmp array.
        E valuePopped = tmpElements[index];

        // Continue copying the elements array.
        for (int i = index - 1; i >= 0; i--) {
            tmpElements[i] = elements[i];
        }

        delete [] elements;
        elements = tmpElements;
        tmpElements = nullptr; // not delete because that would delete the memory pointed to by tmpElements

        numberOfElements -= 1;
        return valuePopped;
    }// end
    void deleteAt(const int index) {
        if (index < 0 || index >= numberOfElements) {
            throw range_error( "Invalid index: " + to_string(index) );
        }

        elementPtr_t tmpElements = new E[numberOfElements - 1];

        // Copy all elements before index to the tmp array, starting from the back.
        for (int i = numberOfElements - 1; i > index; i--) {
            tmpElements[i - 1] = elements[i];
        }

        // Continue copying the elements array.
        for (int i = index - 1; i >= 0; i--) {
            tmpElements[i] = elements[i];
        }

        delete [] elements;
        elements = tmpElements;
        tmpElements = nullptr; // not delete because that would delete the memory pointed to by tmpElements

        numberOfElements -= 1;
    }// end
    /*----------------------------------------------------------------*/

    E getBack() const  { return getAt(numberOfElements - 1); }// end getBack()
    E getFront() const { return getAt(0);                    }// end getFront()
    E getAt(const int index) {
        if (index < 0 || index >= numberOfElements) {
            throw range_error( "Invalid index: " + to_string(index) );
        }
        return elements[index];
    }// end
    /*----------------------------------------------------------------*/
    
    int size() const     { return numberOfElements;      }// end size()
    bool isEmpty() const { return numberOfElements == 0; }// end isEmpty()
    /*----------------------------------------------------------------*/

    virtual string toString() const {
        ostringstream oss;

        oss << "size: " << numberOfElements << ", [ ]";
        if ( !isEmpty() ) {
            // Remove ']' from the stream
            oss.seekp(-1, ios::end);

            for (int i = 0; i <= numberOfElements - 1; i++) {
                oss << elements[i] << ", ";
            }

            // Remove the ', ' from the stream.
            oss.seekp(-2, ios::end);
            oss << " ]";
        }

        return oss.str();
    }// end toString()
    /*----------------------------------------------------------------*/
};// end List


/*----------------------------------------------------------------*/
template <typename E>
ostream& operator <<(ostream& os, const List<E>& o) {
    os << o.toString();
    return os;
}// end <<()

/*----------------------------------------------------------------*/

#endif