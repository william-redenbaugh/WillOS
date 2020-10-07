#ifndef AVL_TREE_HPP
#define AVL_TREE_HPP

#include "enabled_modules.h"

#ifdef AVL_TREE_MODULE 

#include <Arduino.h> 

/*!
*   @brief Node Pointer node module that helps us manage the Red black tree. 
*/
struct AVLPointerNode{
    /*!
    *   @brief Node reference number that we calculate our sorting arithmatic on 
    */
    uint32_t reference_number; 

    /*!
    *   @brief General purpose pointer that let's us fetch anything around the systm 
    */
    void *ptr;

    /*!
    *   @brief Flag that determines whether or not node is red or black
    *   @note  True: Red. False: Black,
    */
    int height = false; 

    /*!
    *   @brief Left and right pointer nodes for the tree. 
    */
    struct AVLPointerNode *left;   
    struct AVLPointerNode *right; 
};

class AVLPointerModule{
    public: 
    /*!
    *   @returns Return as node based off the inputed reference number
    *   @param uint32_t reference_number we are searching for
    */
    AVLPointerNode* search(uint32_t reference_number); 

    /*!
    *   @returns Find the largest known value in the tree. 
    */
    AVLPointerNode* max(void); 

    /*!
    *   @returns the smallest known value in the tree
    */
    AVLPointerNode* min(void); 

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
}; 

#endif 

#endif 