#ifndef _BINARY_SEARCH_TREE_HPP
#define _BINARY_SEARCH_TREE_HPP

#include "../enabled_modules.h"

#ifdef BINARY_SEARCH_TREE_MODULE

#include <Arduino.h>

/*!
*   @brief Node Pointer node module that helps us manange a binary search tree. 
*/
struct BinarySearchTreePointerNode{
    /*!
    *   @brief Node reference number that we calculate our sorting arithmatic on 
    */
    int key; 

    /*!
    *   @brief General purpose pointer that let's us fetch anything around the systm 
    */
    void *ptr = NULL;

    /*!
    *   @brief Left, right and parent pointer nodes for the tree. 
    */
    struct BinarySearchTreePointerNode *left = NULL;   
    struct BinarySearchTreePointerNode *right = NULL; 
};

/*!
*   @brief The module that lets us generate binary search trees. It's worth noting that we can't 
*/
class BinarySearchTreePointerModule{
public:
    /*!
    *   @brief Insert operation that lets us insert stimuli into the binary tree
    *   @param int (key that we are inserting that lets us manage computations)
    *   @param void *ptr(general purpose pointer so that we can have data attached to our tree
    */
    void insert(int key, void *ptr); 

    /*!
    *   @brief Lets us delete a given node, and return the data. 
    *   @param int(key that we are using to search for the module)
    */
    void remove(int key); 

    /*!
    *   @brief Searches
    *   @param int key(that we are looking for data with)
    *   @return BinarySearchTreePointerNode*(pointer to the node in question). Will return NULL if nothing is found
    */
    BinarySearchTreePointerNode* search(int key); 

    /*!
    *   @brief The smallest value the binary ree
    *   @returns Pointer to the smallest node in the binary tree
    */
    BinarySearchTreePointerNode* minimum(void); 
    
    /*!
    *   @brief The largest value the binary ree
    *   @returns Pointer to the largest node in the binary tree
    */
    BinarySearchTreePointerNode* maximum(void); 

private: 

    // Root node of the binary search tree.
    BinarySearchTreePointerNode *root; 

}; 

#endif

#endif