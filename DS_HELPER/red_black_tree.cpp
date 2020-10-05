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

static void deletion_helper(struct RedBlackTreePointerNode *node, uint32_t reference_number){
    while(1){
        if(node->reference_number > reference_number)
            node = node->left
        else if(node->reference_number < reference_number)
            node = node->right; 
        else if(node->reference_number == reference_number)
            
    }
}
/*!
*   @brief Inserts a node into the tree with a given reference number and value
*   @param uint32_t reference number
*   @param void* general purpose pointer
*/  
void RedBlackTreePointerModule::insert(void *ptr, uint32_t reference_number){
    // Handles edge case where our head is null(aka no modules in the data yet)
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
    // Since we want to keep track of our general purpose pointer
    new_node->ptr = ptr; 
    // We default to red. 
    new_node->red_black = true;

    // Helper function that deals with flagging and moving nodes around to keep tree balanced. 
    // check_uncle_red_black(new_node); 
}

/*!
*   @brief Helper function that lets us handle flaging when our uncle is either red or black
*   @param struct RedBlackTreePointerNode* (current node that we are checking, so we flag up the stack)
*/
static void check_uncle_red_black(struct RedBlackTreePointerNode* node){
    // Checking to see if we have a parent, if not we are done
    if(node->parent == NULL)
        return; 

    // We define the parent
    struct RedBlackTreePointerNode *parent = node->parent; 
    
    // If our parent is red, then we check out, since 
    if(parent->red_black)
        return; 
    
    // If we don't have a grand parent, we check out
    if(node->parent->parent == NULL)
        return; 

    // Then we define the grandparent
    struct RedBlackTreePointerNode *grand_parent = node->parent->parent; 

    // Instantiate the uncle
    struct RedBlackTreePointerNode *uncle; 
    bool uncle_left = true; 

    // Depending on which node pointer matches with the parent
    // We point the uncle to something relevant. 
    if(node->parent == grand_parent->left){
        uncle = grand_parent->right;
        uncle_left = false;  
    }
    else
        uncle = grand_parent->left; 

    // If our uncle is flagged as red_black
    if(uncle != NULL){
        if(uncle->red_black){
            parent->red_black = false; 
            grand_parent->red_black = true; 
            uncle->red_black = false; 
            check_uncle_red_black(grand_parent); 
        }
    }

    // LEFT_LEFT_CASE
    if(uncle_left == false && parent->left == node){
        

    }

    // LEFT_RIGHT_CASE
    if(uncle_left == false && parent->right == node){

    }

    // RIGHT_RIGHT_CASE
    if(uncle_left == true && parent->right == node){

    }

    // RIGHT_LEFT_CASE
    if(uncle_left == true && parent->left == node){
        
    }
}


#endif