# InventorySystem

This program includes Dynamic Array List, which is a dynamically allocated array template class.

There are several changes that can be made to the items in the file. The first two are simple, adding and removing GMItems or any item of a subclass of GMItem to and from it. For the others, you will first need to specify what item you want to modify. After selecting an item by providing its index in the inventory List, you can do the following:
  
 1) Change on hand count.
    a) Increase by some integer amount.
    b) Decrease by some integer amount.
    c) Set the on hand count to a new integer.
 2) Change the item price.
 3) Change the item name.
 4) Change the item code.
 5) Change the item's warning prompt (if the item is of class ExpiringItem) or add one to it, which converts the item to a
    subclass ExpiringItem which means that the item has a special message attatched to it that will be displayed when it is
    entered into the POS System to warn the user to ensure that the item is not expired, or inform the customer of the        warning associated with the item.
 6) Change the item's minimum purchase age (if the item is of class AgeRestrictedItem) or add one to it, which converts the
    item to a subclass AgeRestrictedItem which means that it has as special field that stores and integer value representing
    the minimum age one must be to purchase it (for things like alcohol or M-rated games).
 7) Duplicate the item, providing a new code that will be checked for uniqueness.
