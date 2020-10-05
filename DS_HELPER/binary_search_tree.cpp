#include "binary_search_tree.hpp"

#ifdef BINARY_SEARCH_TREE_MODULE

/*!
*   @brief Insert operation that lets us insert stimuli into the binary tree
*   @param int (key that we are inserting that lets us manage computations)
*   @param void *ptr(general purpose pointer so that we can have data attached to our tree
*/
void BinarySearchTreePointerModule::insert(int key, void *ptr){

    // Iterate through the binary tree. 
    // NOTE** we iterate through the tree to prevent stack space from being used too much
    // Even if one compiler handle's edge cases well, another compiler may not 
    // And we want to mitigate use of stack space as much as possible
    BinarySearchTreePointerNode *node = this->root;     
    while(1){
        // If we reach the end of the branch of the tree, then we break out of the loop
        if(node == NULL)
            break;  

        // If the current node has a key larger than the key we are trying to insert
        // Then we switch to the left branch
        if(node->key > key)
            node = node->left;
 
        // If the current node has a key smaller than the key we are trying to insert
        // Then we switch to the right branch
        else if(node->key < key)
            node = node->right; 

        // If by some chance the key we request is the same as the key found in a node, we return
        else
            break; 
    }

    // We generate a new node, with a given key and ptr module
    node = new BinarySearchTreePointerNode; 
    node->key = key; 
    node->ptr = ptr; 
}

/*!
*   @brief Helper function that lets us find the minimum node value in a binary tree
*   @param BinarySearchTreePointerNode* root of the tree we are looking for
*   @return BinarySearchTreePointerNode* the node with the lowest valiue
*/
static struct BinarySearchTreePointerNode* min_node(struct BinarySearchTreePointerNode* root){
    if(root == NULL)
        return NULL; 

    while(root->left != NULL)
        root = root->left; 

    return root; 
}

/*!
*   @brief Helper function that lets us find the maximum node value in a binary tree
*   @param BinarySearchTreePointerNode* root of the tree we are looking for
*   @return BinarySearchTreePointerNode* the node with the highest valiue
*/
static struct BinarySearchTreePointerNode* max_node(struct BinarySearchTreePointerNode* root){
    if(root == NULL)
        return NULL; 

    while(root->right != NULL)
        root = root->right; 

    return root; 
}

static struct BinarySearchTreePointerNode* node_delete_helper(struct BinarySearchTreePointerNode* root, int key){
    // base case 
    if (root == NULL) return root; 
  
    // If the key to be deleted is smaller than the root's key, 
    // then it lies in left subtree 
    if (key < root->key) 
        root->left = node_delete_helper(root->left, key); 
  
    // If the key to be deleted is greater than the root's key, 
    // then it lies in right subtree 
    else if (key > root->key) 
        root->right = node_delete_helper(root->right, key); 
  
    // if key is same as root's key, then This is the node 
    // to be deleted 
    else
    { 
        // node with only one child or no child 
        if (root->left == NULL) 
        { 
            struct BinarySearchTreePointerNode *temp = root->right; 
            free(root); 
            return temp; 
        } 
        else if (root->right == NULL) 
        { 
            struct BinarySearchTreePointerNode *temp = root->left; 
            free(root); 
            return temp; 
        } 
  
        // node with two children: Get the inorder successor (smallest 
        // in the right subtree) 
        struct BinarySearchTreePointerNode* temp = min_node(root->right); 
  
        // Copy the inorder successor's content to this node 
        root->key = temp->key; 
  
        // Delete the inorder successor 
        root->right = node_delete_helper(root->right, temp->key); 
    } 
    return root;
}

/*!
*   @brief Lets us delete a given node, and return the data. 
*   @param int(key that we are using to search for the module)
*   @return void *ptr(pointer to the data)
*/
void BinarySearchTreePointerModule::remove(int key){
    this->root = node_delete_helper(this->root, key); 
}

/*!
*   @brief Searches
*   @param int key(that we are looking for data with)
*   @return BinarySearchTreePointerNode*(pointer to the node in question). Will return NULL if nothing is found
*/
BinarySearchTreePointerNode* BinarySearchTreePointerModule::search(int key){
    BinarySearchTreePointerNode *node = this->root;     
    while(1){
        // If we reach the end of the branch of the tree, then we break out of the loop
        if(node == NULL)
            return NULL;  

        // If the current node has a key larger than the key we are trying to search for
        // Then we switch to the left branch
        if(node->key > key)
            node = node->left;
 
        // If the current node has a key smaller than the key we are trying to search for
        // Then we switch to the right branch
        else if(node->key < key)
            node = node->right; 

        // If by some chance the key we request is the same as the key found in a node, we return
        else
            break; 
    } 

    // If we were able to fine a matching key
    return node; 
}

/*!
*   @brief The smallest value the binary ree
*   @returns Pointer to the smallest node in the binary tree
*/
BinarySearchTreePointerNode* BinarySearchTreePointerModule::minimum(void){
    return min_node(this->root); 
}

/*!
*   @brief The largest value the binary ree
*   @returns Pointer to the largest node in the binary tree
*/
BinarySearchTreePointerNode* BinarySearchTreePointerModule::maximum(void){
    return max_node(this->root); 
}

#endif