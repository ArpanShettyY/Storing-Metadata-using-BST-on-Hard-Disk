# Storing-Metadata-using-BST-on-Hard-Disk

The tree should support the following operations (mentioned in the header file)
1. insert
2. delete based on the key
3. traversal - inorder : space separated values followed by \n
4. traversal - pre-order : space separated values followed by \n

As you might know we can’t just store pointers directly in a file; they’d have no meaning even if you do write them. When it comes to a file, we represent addresses with file offsets. So, data structures like linked lists or trees will use offsets instead of pointers.


This program was tested in Ubuntu 20.04 gcc compiler version 9.3.0)


Implementation:
1)FILE* init_tree(const char* filename);
    Uses the "r+" or "w+" mode so that we can do both write and read.
    Initially open in "r+" so as to check if the file exists or not and not overwrite the file if it exists.
    If it doesn't exist we create a new one by using "r+" mode. 

2)void insert_key(int key, FILE *fp);
    Checks for the key
    if not present finds position where it can be inserted
    Then tries to prepare a node.
    It is either removed from the list of free node or a new one is created at the end of current file if no free nodes already exists
    Then this is inserted to its appropriate position.

3)void delete_key(int key, FILE *fp);
    Check if key is present.
    If it is then track the node and its parent.
    Replace the node by its inorder successor if any.
    Take care of conditions if tree has just one node and if no inorder successor is present.
    Then send the node deleted to the free node list for further use.

Rest of the functions are self-explanatory
