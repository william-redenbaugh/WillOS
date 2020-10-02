#include "red_black_tree.hpp"

/*!
*   @brief Helper function that handles most of the process of finding a given node based off a reference number
*   @param RedBlackTreePointerNode* pointer to the node with the matching reference number 
*   @param uint32_t reference_number number we are looking for 
*/
static RedBlackTreePointerNode *search_helper(RedBlackTreePointerNode *node, uint32_t reference_number){
    // If we can't find anything, we return null
    if(node == NULL)
        return NULL; 

    // If we find the node with our reference number 
    if(node->reference_number == reference_number)
        return node; 

    // Checking if the left or right sides found the value. 
    RedBlackTreePointerNode *left = search_helper(node->left, reference_number); 
    RedBlackTreePointerNode *right = search_helper(node->right, reference_number); 

    // If either left or right sides of the node found anything
    // We return it!
    if(left != NULL)
        return left; 
    else if(right != NULL)
        return right;

    // If nothing is found, we return null  
    return NULL; 
}

/*!
*   @returns Return as node based off the inputed reference number
*   @param uint32_t reference_number we are searching for
*/
RedBlackTreePointerNode* RedBlackTreePointerModule::search(uint32_t reference_number){
    return search_helper(this->head, reference_number);
}

/*!
*   @returns Find the largest known value in the tree. 
*/
RedBlackTreePointerNode* RedBlackTreePointerModule::max(void){
    RedBlackTreePointerNode *current = this->head; 
    // Siphs through the rightmost nodes until we have reached the rightmostnode. 
    while(current != NULL)
        current = current->right; 

    return current; 
}

/*!
*   @returns Find the smallest known value in the tree. 
*/
RedBlackTreePointerNode* RedBlackTreePointerModule::min(void){
    RedBlackTreePointerNode *current = this->head; 
    // Siphs through the leftmost nodes until we have reached the rightmostnode. 
    while(current != NULL)
        current = current->left; 

    return current; 
}