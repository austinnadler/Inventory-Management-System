# InventorySystem

This program includes Dynamic Array List, which is a dynamically allocated array template class.

In total, there are 10 changes that can be made to the items in the file. The first two are simple, adding and removing GMItems or any item of a subclass of GMItem to and from it. For the others, you will first need to specify what item you want to modify. After selecting an item by providing its index in the inventory List, you can do the following:
  
 1) Increase the on hand count by any integer
 2) Decrease the on hand count by any integer
 3) Change the on hand number to a completely new integer
 4) Change the item price
 5) Change the item name
 6) Change the item code
 7) Change the item's warning prompt (if the item is of class ExpiringItem) or add one to it, which converts the item to a
    subclass ExpiringItem which means that the item has a special message attatched to it that will be displayed when it is
    entered into the seperate POS System project to warn the theoretical cashier to ensure that the item is not expired.
 8) Change the item's minimum purchase age (if the item is of class AgeRestrictedItem) or add one to it, which converts the
    item to a subclass AgeRestrictedItem which means that it has as special field that stores and integer value representing
    the minimum age one must be to purchase it (for things like alcohol or M-rated games)
