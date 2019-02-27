# InventorySystem

This project is currently has 2 main functions: inventory tracking(performAdminFunctions()), and point of sale (POS) (checkout())

The inventory tracking aspect is much more complicated than the POS aspect of the project. It starts out by opening a file that is assumed to have been created before hand for the first use, but after the first use, the program is able to read your file and process it so that you can save it and open it back up later to make more changes.

In total, there are 10 "changes" that can be made to the items in the file. The first two are simple, adding and removing GMItems or any item of a subclass of GMItem to and from it. For the others, you will first need to specify what item you want to modify. After selecting an item by providing its index in the inventory vector, you can do the following:
  
 1) Increase the on hand count by any integer
 2) Decrease the on hand count by any integer
 3) Change the on hand number to a completely new integer
 4) Change the item price
 5) Change the item name
 6) Change the item code
 7) Change the item's warning prompt (if the item is of class ExpiringItem) or add one to it, which converts the item to a subclass ExpiringItem which means that the item has a special message attatched to it that will be displayed when it is entered into the seperate POS System project to warn the theoretical cashier to ensure that the item is not expired.
 8) Change the item's minimum purchase age (if the item is of class AgeRestrictedItem) or add one to it, which converts the item to a subclass AgeRestrictedItem which means that it has as special field that stores and integer value representing the minimum age one must be to purchase it (for things like alcohol or M-rated games)
 
  
The checkout function loads the inventory file specified, and then allows the user to enter codes. The program compares the entered code to those stored in the input vector, and if a match is found, places a pointer to that location in the input vector in a new vector that is reffered to as the cart. Thanks to vectors, the checkout() functions also easily implements a method of removing items from the shopping cart by simply entering the command "undo" to remove the most recent item and erase() to erase an item given it's index in the vector.
