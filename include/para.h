// Maximum number of children a node in the B+ tree can have. 
// This defines the branching factor for the tree. For a B+ tree,
// a node can have a maximum of (MAX_FANOUT - 1) keys and MAX_FANOUT children.
#define MAX_FANOUT 4

// The data type for keys used in the B+ tree. In this case, the keys are of type 'int'.
// This could be modified to use other types if needed, such as float, string, etc.
#define KeyType int
