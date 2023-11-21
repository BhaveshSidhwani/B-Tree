# CS539 - Database System

## Programming Project 1 - B+Tree

### Project Description

This programming project is to implement an index for fast data retrieval without having to search through every row in a database table.

You will need to implement the B+Tree dynamic index structure. It is a balanced tree in which the internal nodes direct the search and leaf nodes contains record pointers to actual data entries. Since the tree structures grows and shrink dynamically, you are required to handle the logic of split and merge.

We are providing you the *BPlusTree* classes that contain the API that you need to implement. If a class already contains certain member variables, you should **not** remove them. But you are free to add helper functions/member variables to these classes in order to correctly realize the functionality.

You are only allowed to modify the header file (`include/b_plus_tree.h`) and its corresponding source file (`src/b_plus_tree.cpp`).

We assume the keys are unique integers. We simulate the disk pages using two node types and the MAX_FANOUT parameter.

### **Submission Format Attention:**

🚨 **CRITICAL INSTRUCTION** 🚨

- **Files to Modify**: You are **ONLY** allowed to modify the header file (`include/b_plus_tree.h`) and its corresponding source file (`src/b_plus_tree.cpp`).
  
- **Submission Format**: Prepare to submit by **ONLY** zipping and uploading these two files. Ensure they're not nested inside any folders.

🛑 **WARNING**: Any deviation from this exact submission format will result in a **ZERO** score for this assignment. Adhere strictly to the submission guidelines!

### Build

run the following commands to build the system:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```


### Testing

Evaluate the components of this assignment using our test file (`test/b_plus_tree_test.cpp`). You're free to adjust this file for testing and debugging.

While we offer elementary test cases within this file, be aware that our grading will incorporate more intricate cases.

Execute the following commands to run the test:

```
$ cd build
$ make bplustree-test
$ ./bplustree-test
```

### Grading Rubric

Your submission will be assessed on:

1. **Functionality**: Does your code execute all test cases and yield correct results?
2. **B+ Tree Integrity**: Post insertion and deletion, do the tree nodes uphold the B+ tree definition, i.e., are they at least half full?
3. **Performance**: Is the latency for queries acceptable? Are insertion and deletion operations reasonably quick?
4. **Tree Structure**: Considerations will include tree balance and tree height.

Note that we will use additional test cases that are more complex and go beyond the sample test cases that we provide you.

**WARNING: All of the code for this project must be your own. You may not copy source code from other students or other sources that you find on the web. Plagiarism will not be tolerated. See [Rutgers University Academic Integrity Policy](http://nbacademicintegrity.rutgers.edu/) for additional information.**
