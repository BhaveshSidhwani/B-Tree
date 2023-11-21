//===----------------------------------------------------------------------===//
//
//                         Rutgers CS539 - Database System
//                         ***DO NO SHARE PUBLICLY***
//
// Identification:   include/b_plus_tree.h
//
// Copyright (c) 2023, Rutgers University
//
//===----------------------------------------------------------------------===//
#pragma once

#include <queue>
#include <string>
#include <vector>
#include "para.h"

using namespace std;

// Value structure we insert into BPlusTree
struct RecordPointer {
    int page_id;
    int record_id;
    RecordPointer() : page_id(0), record_id(0){};
    RecordPointer(int page, int record) : page_id(page), record_id(record){};
};

// BPlusTree Node
class Node {
public:
    Node(bool leaf) : key_num(0), is_leaf(leaf) {};
    bool is_leaf;
    int key_num;
    KeyType keys[MAX_FANOUT - 1];
};

// internal b+ tree node
class InternalNode : public Node {
public:
    InternalNode() : Node(false) {};
    Node * children[MAX_FANOUT];
};

class LeafNode : public Node {
public:
    LeafNode() : Node(true) {};
    RecordPointer pointers[MAX_FANOUT - 1];
    // pointer to the next/prev leaf node
    LeafNode *next_leaf = NULL;
    LeafNode *prev_leaf = NULL;
};


/**
 * Main class providing the API for the Interactive B+ Tree.
 *
 * Implementation of simple b+ tree data structure where internal pages direct
 * the search and leaf pages contain record pointers
 * (1) We only support (and test) UNIQUE key
 * (2) Support insert & remove
 * (3) Support range scan, return multiple values.
 * (4) The structure should shrink and grow dynamically
 */

class BPlusTree {
public:
    BPlusTree(){};

    std::vector<std::pair<KeyType, RecordPointer>> key_value_pairs;

    // Returns true if this B+ tree has no keys and values
    bool IsEmpty() const;
        
    // Insert a key-value pair into this B+ tree.
    bool Insert(const KeyType &key, const RecordPointer &value);

    // Remove a key and its value from this B+ tree.
    void Remove(const KeyType &key);

    // return the value associated with a given key
    bool GetValue(const KeyType &key, RecordPointer &result);

    // return the values within a key range [key_start, key_end) not included key_end
    void RangeScan(const KeyType &key_start, const KeyType &key_end,
                    std::vector<RecordPointer> &result);
    
    // pointer to the root node.
    Node *root = NULL;

private:
    // Function to get the leaf node for the specified key
    Node* getChildForKey(const KeyType &key);
    
    // Function to insert the new key in the leaf node
    bool InsertInLeaf(LeafNode *leaf, const KeyType &key, const RecordPointer &value);

    // Function to insert the new key in the parent node
    bool InsertInParent(const KeyType &key, InternalNode *parent_node, Node *new_node);

    // Find the parent for the specified node
    InternalNode* FindParent(InternalNode* root_node, InternalNode *search_node);

    // Function to find the node that contains the start key
    LeafNode* FindNode(InternalNode* curr_node, const KeyType &key_start);
};
