//===----------------------------------------------------------------------===//
//
//                         Rutgers CS539 - Database System
//                         ***DO NOT SHARE PUBLICLY***
//
// Identification:   test/b_plus_tree_delete_test.cpp
//
// Copyright (c) 2023, Rutgers University
//
//===----------------------------------------------------------------------===//

#include "../include/b_plus_tree.h"
#include "../include/test_functions.h"
#include "../include/para.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

// Function to perform the test on a given tree with the specified batches and range
void RunTest(BPlusTree& tree, const vector<int>& insertBatch, const vector<int>& deleteBatch, int range_l, int range_r, int range_num, const std::string& testName) {
    cout << testName << endl;
    testOneCase(tree, insertBatch, deleteBatch, range_l, range_r, range_num);
}

int main() {
  
    // This is a B+Tree delete test program.
    // Students' B+Tree implementations will be graded based on how they perform on these tests.
    // The score is determined by the number of passed test cases.
    // Below are three sample test cases. In the actual grading, there will be 100 test cases.

    // Test Case 0: Basic insertions followed by deletions at intervals of 4.
    BPlusTree tree_0;
    vector<int> insertBatch_0;
    for (int i = 100; i < 500; i++) {
        insertBatch_0.push_back(i);
    }
    vector<int> deleteBatch_0;
    for (int i = 100; i < 110; i += 4) {
        deleteBatch_0.push_back(i);
    }
    RunTest(tree_0, insertBatch_0, deleteBatch_0, 100, 112, 9, "B+Tree Test Case 0...");

    // Test Case 1: Insertions in random order with no deletions. 
    BPlusTree tree_1;
    vector<int> insertBatch_1{7,3,9,4,8,5,6,10,2};
    // vector<int> insertBatch_1{7,3,4,5,6};
    vector<int> deleteBatch_1; // Empty batch
    RunTest(tree_1, insertBatch_1, deleteBatch_1, 2, 6, 5, "B+Tree Test Case 1...");

    // Test Case 2: Larger mixed insertions followed by specific deletions.
    BPlusTree tree_2;
    vector<int> insertBatch_2{7,3,9,4,8,5,6,10,2,345,34,1,999,555,333,444,777,888,111,11,12,222,666};
    vector<int> deleteBatch_2{3,9,999};
    RunTest(tree_2, insertBatch_2, deleteBatch_2, 34, 500, 6, "B+Tree Test Case 2...");

    return 0;
}
