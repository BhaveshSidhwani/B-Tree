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

/*
//=========================================================================//
//
//  test_functions.h
//
//  Description:
//  This header file contains a suite of utility functions specifically designed 
//  for testing the functionality and integrity of the B+Tree implementation. 
//
//  Features:
//  - Methods for batch insertions and deletions.
//  - Verification of tree properties.
//  - Checking for value retrievals and more.
//
//  Important:
//  Students are NOT required to modify anything in this file.
//  This file is solely used to test the student's B+Tree implementation. 
//  Modifying this file might lead to unexpected testing outcomes.
//
//=========================================================================//
*/

#pragma once

#include "../include/para.h"
#include "../include/b_plus_tree.h"
#include <iostream>

using namespace std;

// get the height of the tree
// This function computes the height of the tree by traversing down its leftmost branch until a leaf node is reached.
int getHeight(Node *root)
{
    if (root == NULL)
    {
        return 0;
    }
    if (root->is_leaf == true)
    {
        return 1;
    }

    int height = 1;

    Node *cursor = root;
    while (cursor->is_leaf == false)
    {
        int key_num = cursor->key_num;
        cursor = ((InternalNode *)cursor)->children[0];
        height++;
    }

    return height;
}

// judge if the tree is balance
// This function verifies if the tree is balanced. A B+ tree is balanced when all its leaves are at the same depth.
bool verifyIfBalance(Node *node, const int height, int current_depth)
{
    Node *cursor = node;
    if (cursor->is_leaf == true)
    {
        return current_depth == height;
    }

    bool res;
    for (int i = 0; i <= node->key_num; i++)
    {
        cursor = ((InternalNode *)node)->children[i];
        res = verifyIfBalance(cursor, height, current_depth + 1);
    }
    return res;
}

// judge if every node's key num is lower than max_key
// This function ensures that each node (except root) has a key count between min_key_num and max_key_num. This is a property of B+ trees to ensure balanced data distribution.
bool verifyNodesKeyNum(Node *root, Node *node, const int max_key_num, const int min_key_num)
{
    Node *cursor = node;
    if (cursor != root && cursor->is_leaf == true)
    {
        return max_key_num >= cursor->key_num && cursor->key_num >= min_key_num;
    }

    bool res = true;
    if (node != root)
    {
        res = max_key_num >= cursor->key_num && cursor->key_num >= min_key_num;
    }
    for (int i = 0; i <= node->key_num; i++)
    {
        cursor = ((InternalNode *)node)->children[i];
        res = res & verifyNodesKeyNum(root, cursor, max_key_num, min_key_num);
    }

    return res;
}

// get every leaf node of the tree
// This function populates a vector with all the leaf nodes. Useful for certain validations.
void getLeavesNodes(Node *node, vector<Node *> &leaves)
{
    Node *cursor = node;
    if (cursor->is_leaf == true)
    {
        leaves.emplace_back(cursor);
        return;
    }
    else
    {
        for (int i = 0; i <= node->key_num; i++)
        {
            cursor = ((InternalNode *)node)->children[i];
            getLeavesNodes(cursor, leaves);
        }
    }
}

// to easily implement, just check if their page_ids are the same
// This function verifies if the page_ids stored in the tree's leaf nodes match the provided ground truth.
bool verifyLeavesNodes(Node *root, const vector<vector<int>> &ground_truth)
{
    vector<Node *> leaves;
    getLeavesNodes(root, leaves);
    if (leaves.size() != ground_truth.size())
    {
        return false;
    }
    for (int i = 0; i < ground_truth.size(); i++)
    {
        auto pointers_1 = ((LeafNode *)leaves[i])->pointers;
        auto real_page_ids = ground_truth[i];

        // if(leaves[i]->key_num!=ground_truth[i].size()){
        //     return false;
        // }
        for (int j = 0; j < real_page_ids.size(); j++)
        {
            if (pointers_1[j].page_id != real_page_ids[j])
                return false;
        }
    }
    return true;
}

// conveniently insert a batch of record pointers
bool batchInsert(BPlusTree &tree, const vector<int> &batch)
{
    bool flag = true;
    for (auto const &toInsert : batch)
    {
        RecordPointer one_record(toInsert, toInsert);
        tree.Insert(toInsert, one_record);
    }
    return flag;
}

// conveniently delete a batch of record pointers
void batchDelete(BPlusTree &tree, const vector<int> &batch)
{
    for (auto const &toInsert : batch)
    {
        tree.Remove(toInsert);
    }
}

// This function checks two key properties of the B+ tree: balance and node key count.
void verifyTreeProperty(BPlusTree &tree)
{
    const int min_keyNum = int((MAX_FANOUT - 1) / 2.0);
    const int max_keyNum = MAX_FANOUT - 1;
    int height = getHeight(tree.root);
    // printf("Tree Height: %d\n", height);
    if (verifyIfBalance(tree.root, height, 1) == false)
    {
        printf("ERROR: The tree is not balanced\n");
    }
    if (verifyNodesKeyNum(tree.root, tree.root, max_keyNum, min_keyNum) == false)
    {
        printf("ERROR: The key num of each node in this tree is not legal\n");
    }
}

/**
 * Test a single B+Tree operation scenario.
 * 
 * This function serves as a testing mechanism for the B+Tree implementation. It tests the following:
 * 1. Initial tree empty status.
 * 2. Insertion of a batch of records.
 * 3. Verification of successful insertions using GetValue.
 * 4. Deletion of a batch of records.
 * 5. Verification of successful deletions.
 * 6. RangeScan function to ensure records fall within the desired range.
 * 7. Ensures that the tree maintains its properties post operations.
 * 
 * If any errors are detected, an "ERROR: ..." message will be displayed.
 * Otherwise, the test case is considered passed.
 * 
 * @param tree The B+Tree to be tested.
 * @param insertBatch Batch of keys to be inserted.
 * @param deleteBatch Batch of keys to be deleted.
 * @param range_l Lower bound for the range scan.
 * @param range_r Upper bound for the range scan.
 * @param range_num Expected number of records within the given range.
 */
void testOneCase(BPlusTree &tree, const vector<int> &insertBatch, const vector<int> &deleteBatch, const int range_l, const int range_r, const int range_num)
{
    // Check if the tree is initially empty.
    if (!tree.IsEmpty())
    {
        cout << "ERROR: IsEmpty() test fail!" << endl;
    }

    // Insert a batch of records.
    bool insert_flag = batchInsert(tree, insertBatch);
    // Check if insertions were successful and tree is not empty.
    if (!insert_flag || tree.IsEmpty())
    {
        cout << "ERROR: Insert() test fail!" << endl;
    }

    cout << "Insert Completed\n" << endl;

    bool getValue_flag = true;
    // Verify if all inserted keys can be fetched.
    for (auto inserted : insertBatch)
    {
        RecordPointer one_record;
        tree.GetValue(inserted, one_record);
        if (one_record.page_id != inserted)
        {
            getValue_flag = false;
            cout << "ERROR: GetValue Not Found: " << inserted << endl;
            break;
        }
    }

    if (getValue_flag == false)
        cout << "ERROR: GetValue() test fail!" << endl;

    // Delete a batch of records.
    batchDelete(tree, deleteBatch);
    cout << "Delete Completed" << endl;

    // Verify if the first two keys from the delete batch were successfully removed.
    RecordPointer temp;
    if(deleteBatch.size() > 2)
        if (tree.GetValue(deleteBatch[0], temp) || tree.GetValue(deleteBatch[1], temp))
        {
            cout << "ERROR: Remove() test fail!" << endl;
        }

    // Perform a range scan and verify if the records are within the given range.
    vector<RecordPointer> records;
    tree.RangeScan(range_l, range_r, records);
    if (records.size() != range_num && records.size() != range_num-1)
    {
        cout << "ERROR: RangeScan() test fail!" << endl;
    }

    // Check B+Tree properties to ensure its integrity after operations.
    cout << "Verifying Tree Property" << endl;
    verifyTreeProperty(tree);
}
