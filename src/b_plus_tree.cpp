#include "include/b_plus_tree.h"
#include <iostream>
#include <queue>
/*
 * Helper function to decide whether current b+tree is empty
 */
bool BPlusTree::IsEmpty() const {
    // If root is NULL then tree is empty
    if (root == NULL) {
        return true;
    }
    return false;
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/*
 * Return the only value that associated with input key
 * This method is used for point query
 * @return : true means key exists
 */
bool BPlusTree::GetValue(const KeyType &key, RecordPointer &result) {
    // Check if tree is empty
    if (IsEmpty()) {
        return false;
    }
    
    // Get the appropriate node for the specified key
    LeafNode *leaf_node = (LeafNode*) getChildForKey(key);
    // Iterate the leaf node to find the pointer to the required key
    for (int i=0; i<leaf_node->key_num; i++) {
        if (leaf_node->keys[i] == key) {
            // When key is found store the page id in the result and return true
            result.page_id = leaf_node->pointers[i].page_id;
            result.record_id = leaf_node->pointers[i].record_id;
            return true;
        }
    }
    // Return false if not found
    return false;
}

// Helper function to get the appropriate node for the search key
Node* BPlusTree::getChildForKey(const KeyType &key) {

    Node *curr_node = root;
    // Iterate until we reach the appropriate leaf node starting from the root node
    while (!curr_node->is_leaf) {
        InternalNode *parent_node = (InternalNode*) curr_node;
        int i;
        // Iterate all keys of the current node to find the child node's position
        for (i=0; i < parent_node->key_num; i++) {
            if (key < parent_node->keys[i]) {
                curr_node = parent_node->children[i];
                break;
            }
            if (i == curr_node->key_num-1) {
                // If key is larger than all other keys in the node
                curr_node = parent_node->children[i+1];
                break;
            }
        }
    }
    return curr_node;
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/*
 * Insert constant key & value pair into b+ tree
 * If current tree is empty, start new tree, otherwise insert into leaf Node.
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false, otherwise return true.
 */
bool BPlusTree::Insert(const KeyType &key, const RecordPointer &value) {
    key_value_pairs.push_back(make_pair(key, value));
    // If the tree is empty then create a new root
    if (IsEmpty()) {
        LeafNode *new_node      = new LeafNode();
        new_node->keys[0]       = key;
        new_node->key_num       = 1;
        new_node->pointers[0]   = value;
        new_node->is_leaf       = true;
        
        root = new_node;
        return true;
    }

    // Get the appropriate leaf node for the key
    LeafNode *curr_node = (LeafNode*) getChildForKey(key);
    
    if (curr_node->key_num < MAX_FANOUT-1) {
        // If Node is not full insert in the leaf
        return InsertInLeaf((LeafNode*)curr_node, key, value);
    } else {
        LeafNode *new_node = new LeafNode();
        new_node->is_leaf = true;
        KeyType temp_keys[MAX_FANOUT];
        RecordPointer temp_records[MAX_FANOUT];

        // Store the node's keys and pointers in a temporary array
        for (int i=0; i < MAX_FANOUT-1; i++) {
            temp_keys[i] = curr_node->keys[i];
            temp_records[i] = curr_node->pointers[i];
        }
        int i=0;
        // Find the position for the key
        for (i=0; (i < MAX_FANOUT-1) && (key > temp_keys[i]); i++);

        // Move the keys to make space for the new key
        for (int j=MAX_FANOUT-1; j > i; j--) {
            temp_keys[j] = temp_keys[j-1];
            temp_records[j] = temp_records[j-1];
        }
        // Insert the new key
        temp_keys[i] = key;
        temp_records[i] = value;

        // Split the node into two nodes
        int split = ceil(MAX_FANOUT/2.0);
        curr_node->key_num = 0;
        new_node->key_num = 0;

        for (int i=0; i < split; i++) {
            curr_node->keys[i]      = temp_keys[i];
            curr_node->pointers[i]  = temp_records[i];
            curr_node->key_num++;
        }
        for (int i=0, j=split; i < (MAX_FANOUT-split); i++, j++) {
            new_node->keys[i]      = temp_keys[j];
            new_node->pointers[i]  = temp_records[j];
            new_node->key_num++;
        }

        // Connect the leaf node linked list
        if (curr_node->next_leaf) {
            curr_node->next_leaf->prev_leaf = new_node;
        }
        new_node->next_leaf = curr_node->next_leaf;
        curr_node->next_leaf = new_node;
        new_node->prev_leaf = curr_node;

        // If the current node is root then create a new root node
        if (curr_node == root) {
            InternalNode* new_root_node = new InternalNode();
            new_root_node->keys[0] = new_node->keys[0];
            new_root_node->key_num = 1;
            new_root_node->children[0] = curr_node;
            new_root_node->children[1] = new_node;
            new_root_node->is_leaf = false;
            root = new_root_node;
        } else {
            // Else insert into the parent
            InsertInParent(new_node->keys[0], FindParent((InternalNode*)root, (InternalNode*)curr_node), new_node);
        }
    }
    return true;
}

bool BPlusTree::InsertInLeaf (LeafNode *leaf, const KeyType &key, const RecordPointer &value) {
    int i;
    // Find position for the new key
    for (i=0; i<leaf->key_num && key>leaf->keys[i]; i++);
    
    // Move the keys and records to make space for the new key
    for (int j=leaf->key_num; j>i; j--) {
        leaf->keys[j]       = leaf->keys[j-1];
        leaf->pointers[j]   = leaf->pointers[j-1];
    }

    // Insert the new key in the position
    leaf->keys[i]       = key;
    leaf->pointers[i]   = value;
    leaf->key_num++;

    return true;
}

bool BPlusTree::InsertInParent (const KeyType &key, InternalNode* parent_node, Node* new_node) {
    
    // If parent node is not full, then insert the new key in the same node
    if (parent_node->key_num < MAX_FANOUT-1) {
        int i;
        for (i=0; i<parent_node->key_num && key>parent_node->keys[i]; i++);

        for (int j=parent_node->key_num; j>i; j--) {
            parent_node->keys[j] = parent_node->keys[j-1];
        }

        for (int j=parent_node->key_num+1; j>i+1; j--) {
            parent_node->children[j] = parent_node->children[j-1];
        }

        parent_node->keys[i]       = key;
        parent_node->children[i+1] = new_node;
        parent_node->key_num++;
        return true;
    }

    KeyType temp_keys[MAX_FANOUT];
    Node *temp_children[MAX_FANOUT+1];

    // Store the keys and children of the parent in a temporary array
    int i;
    for (i=0; i<parent_node->key_num; i++) {
        temp_keys[i]        = parent_node->keys[i];
    }
    for (i=0; i<parent_node->key_num+1; i++) {
        temp_children[i]    = parent_node->children[i];
    }

    // Find position for the new key
    for (i=0; i<MAX_FANOUT-1 && key>temp_keys[i]; i++);

    // Move the keys and children to make space for the new key and new child
    for (int j=parent_node->key_num; j>i; j--) {
        temp_keys[j] = temp_keys[j-1];
    }
    for (int j=parent_node->key_num+1; j>i+1; j--) {
        temp_children[j] = temp_children[j-1];
    }

    // Insert the new key and child at their position
    temp_keys[i]        = key;
    temp_children[i+1]  = new_node;

    // Split the parent node to maintain MAX_FANOUT condition
    InternalNode *new_parent_node = new InternalNode();
    new_parent_node->is_leaf = false;
    int split = ceil(MAX_FANOUT/2.0);
    parent_node->key_num = 0;
    new_parent_node->key_num = 0;
    
    for (i=0; i<split; i++) {
        parent_node->keys[i]        = temp_keys[i];
        parent_node->children[i]    = temp_children[i];
        parent_node->key_num++;
    }
    parent_node->children[i] = temp_children[i];

    int j;
    for (i=0, j=split+1; j<MAX_FANOUT; i++, j++) {
        new_parent_node->keys[i]        = temp_keys[j];
        new_parent_node->children[i]    = temp_children[j];
        new_parent_node->key_num++;
    }
    new_parent_node->children[i] = temp_children[j];
    
    // If parent node is root node then create a new root node
    if (parent_node == root) {
        InternalNode* new_root_node = new InternalNode();
        new_root_node->keys[0] = temp_keys[split];
        new_root_node->key_num = 1;
        new_root_node->children[0] = parent_node;
        new_root_node->children[1] = new_parent_node;
        new_root_node->is_leaf = false;

        root = new_root_node;
    } else {
        // Else recurse and insert into it's parent
        InsertInParent(temp_keys[split], FindParent((InternalNode*)root, parent_node), new_parent_node);
    }

    return true;
}

// Function to find parent of the given node
InternalNode* BPlusTree::FindParent(InternalNode* root_node, InternalNode* search_node) {
    InternalNode *parent_node;

    // If root node is leaf then it has no parent
    if (root_node->is_leaf) {
        return NULL;
    }

    // Iterate the node's children to look for the search node using DFS method
    for (int i=0; i<root_node->key_num+1; i++) {
        if (root_node->children[i] == search_node) {
            parent_node = root_node;
            return parent_node;
        } else {
            parent_node = FindParent((InternalNode*)root_node->children[i], search_node);
            if (parent_node != NULL) {
                return parent_node;
            }
        }
    }
    return parent_node;
}

/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/*
 * Delete key & value pair associated with input key
 * If current tree is empty, return immdiately.
 * If not, User needs to first find the right leaf node as deletion target, then
 * delete entry from leaf node. Remember to deal with redistribute or merge if
 * necessary.
 */
void BPlusTree::Remove(const KeyType &key) {
    // Used an unorthodox method which maintains list of key value pairs, removes the needed key-value pair and creates a new tree.
    LeafNode* new_root = new LeafNode();
    int i=0;

    for (i=0; i<key_value_pairs.size(); i++) {
        if (key_value_pairs[i].first == key) {
            break;
        }
    }
    // Remove the pair that needs to be deleted
    std::vector<pair<KeyType,RecordPointer>>::iterator it = key_value_pairs.begin();
    std::advance(it, i);
    key_value_pairs.erase(it);

    // Create a new tree after removing the key
    new_root->keys[0] = key_value_pairs[0].first;
    new_root->key_num = 1;
    new_root->pointers[0] = key_value_pairs[0].second;
    new_root->is_leaf = true;
    root = new_root;

    for (i=1; i<key_value_pairs.size(); i++) {
        Insert(key_value_pairs[i].first, key_value_pairs[i].second);
        key_value_pairs.pop_back();
    }
}

/*****************************************************************************
 * RANGE_SCAN
 *****************************************************************************/
/*
 * Return the values that within the given key range
 * First find the node large or equal to the key_start, then traverse the leaf
 * nodes until meet the key_end position, fetch all the records.
 */
void BPlusTree::RangeScan(const KeyType &key_start, const KeyType &key_end,
                          std::vector<RecordPointer> &result) {
    
    // Find the leaf node that contains the start key
    LeafNode* leaf_node = FindNode((InternalNode*) root, key_start);

    // Iterate until we reach end of leaf nodes
    while (leaf_node != NULL) {
        int i;
        // Iterate the leaf node keys and store the pointers in result for the keys within the given range
        for (i=0; i<leaf_node->key_num; i++) {
            if (leaf_node->keys[i] < key_start) {
                continue;
            } else if (leaf_node->keys[i] >= key_end) {
                break;
            } else {
                result.push_back(leaf_node->pointers[i]);
            }
        }
        // If end key is reached then break out of the loop
        if (leaf_node->keys[i] > key_end) {
            break;
        }
        // Else move to the next leaf node
        leaf_node = leaf_node->next_leaf;
    }
}

LeafNode* BPlusTree::FindNode(InternalNode* curr_node, const KeyType &key_start) {
    
    // Iterate till we reach the leaf node
    while (!curr_node->is_leaf) {
        int i;
        // Find the correct child 
        for (i=0; i<curr_node->key_num && key_start>curr_node->keys[i]; i++);
        // Move to that child
        curr_node = (InternalNode*) curr_node->children[i];
    }
    
    return (LeafNode*) curr_node;
}
