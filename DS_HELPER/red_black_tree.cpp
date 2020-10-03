#include "red_black_tree.hpp"

#ifdef RED_BLACK_TREE_MODULE

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

    // If the number we are looking for is greater than the current nodes number, we check the right node
    if(node->reference_number < reference_number)
        return search_helper(node->right, reference_number); 

    // Otherwise we check the left node, and return it's output. 
    return search_helper(node->left, reference_number); 
}

/*!
*   @returns Return as node based off the inputed reference number
*   @param uint32_t reference_number we are searching for
*/
RedBlackTreePointerNode* RedBlackTreePointerModule::search(uint32_t reference_number){
    return search_helper(this->head, reference_number);
}

/*!
*   @returns Find the largest known value in the tree. Returns as a pointer to relevant node. 
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

/*!
*   @brief Allows us to generate a node with a given parent and reference number
*   @param struct RedBlackTreePointerNode* parent(parent node of the node we are generating)
*   @param uint32_t reference number
*/
static inline RedBlackTreePointerNode* node_generation(struct RedBlackTreePointerNode* parent, uint32_t reference_number){
    RedBlackTreePointerNode *new_node = new RedBlackTreePointerNode; 
    new_node->parent = parent; 
    new_node->reference_number = reference_number; 
    new_node->red_black = true; 
    return new_node; 
}

/*!
*   @brief Helper function that recursively iterates through the tree until it either finds a spot to put the new node, or stops looking if it finds a node with equal value. 
*   @param RedBlackTreePointerNode* node (node that we are comparing at current moment)
*   @param uint32_t Particular key we want to insert into the system
*/
static RedBlackTreePointerNode* insertion_helper(struct RedBlackTreePointerNode* node, uint32_t reference_number){
    if(reference_number < node->reference_number){
        if(node->left == NULL){
            node->left = node_generation(node, reference_number); 
            return node->left;
        }
        node->left = insertion_helper(node->left, reference_number); 
    }
    else if(reference_number > node->reference_number){
        if(node->right == NULL){
            node->right = node_generation(node, reference_number); 
            return node->right; 
        }
        node->right = insertion_helper(node->right, reference_number);
    }

    return node; 
}

static void check_case_a(struct RedBlackTreePointerNode* node){
    if(node->parent == NULL)
        return; 
    
    if(node->parent->parent == NULL)
        return; 

    struct RedBlackTreePointerNode *grand_parent = node->parent->parent; 

    if(grand_parent->left == NULL) 
    
    if(grand_parent->right == NULL)
    

     node->parent->parent == NULL || node->parent->parent->left == NULL || node->parent->parent)
     
}

/*!
*   @brief Inserts a node into the tree with a given reference number and value
*   @param uint32_t reference number
*   @param void* general purpose pointer. 
*/  
void RedBlackTreePointerModule::insert(void *ptr, uint32_t reference_number){
    if(this->head == NULL){
        RedBlackTreePointerNode *new_node = new RedBlackTreePointerNode; 
        new_node->ptr = ptr; 
        new_node->reference_number = reference_number;
        // Set new node to red
        new_node->red_black = false;
        new_node->parent = NULL; 
        this->head = new_node; 
        return; 
    }

    // We are changing the pointer to the inputed key
    RedBlackTreePointerNode *new_node = insertion_helper(this->head, reference_number); 
    new_node->ptr = ptr; 
    new_node->red_black = true;

}

#endif