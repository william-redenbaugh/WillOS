#ifndef _READ_BLACK_TREE_HPP
#define _READ_BLACK_TREE_HPP

#include "../enabled_modules.h"

#ifdef RED_BLACK_TREE_MODULE

#include <Arduino.h>

/*!
*   @brief Node Pointer node module that helps us manage the Red black tree. 
*/
struct RedBlackTreePointerNode{
    /*!
    *   @brief Node reference number that we calculate our sorting arithmatic on 
    */
    uint32_t reference_number; 

    /*!
    *   @brief General purpose pointer that let's us fetch anything around the systm 
    */
    void *ptr;

    /*!
    *   @brief Flag that determines whether or not node is red or black. True: Red. False: Black
    *   @note  True: Red. False: Black. 
    */
    bool red_black = false; 

    /*!
    *   @brief Left, right and parent pointer nodes for the tree. 
    */
    struct RedBlackTreePointerNode *left = NULL;   
    struct RedBlackTreePointerNode *right = NULL; 
    struct RedBlackTreePointerNode *parent = NULL; 
};

/*!
*   @brief Module that lets us generate a red-black tree. 
*/
class RedBlackTreePointerModule{
public: 
    /*!
    *   @returns Return as node based off the inputed reference number
    *   @param uint32_t reference_number we are searching for
    */
    RedBlackTreePointerNode* search(uint32_t reference_number); 

    /*!
    *   @returns Find the largest known value in the tree. 
    */
    RedBlackTreePointerNode* max(void); 

    /*!
    *   @returns the smallest known value in the tree
    */
    RedBlackTreePointerNode* min(void); 

    /*!
    *   @brief Inserts a node into the tree with a given reference number and value
    *   @param uint32_t reference number
    *   @param void* general purpose pointer. 
    */  
    void insert(void *ptr, uint32_t reference_number); 

    /*!
    *   @brief deletes a node with a given reference number
    *   @param uint32_t reference number
    *   @returns bool whether or not we could remove node. 
    */
    bool remove(uint32_t reference_number); 

private: 

    RedBlackTreePointerNode *head; 

};

#endif 
#endif