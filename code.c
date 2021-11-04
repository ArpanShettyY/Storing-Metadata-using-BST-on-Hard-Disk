#include <stdio.h>
#include "assignment_4.h"

static void inorder_print(FILE *fp, int root);      //helper function to print inorder
static void preorder_print(FILE *fp, int root);     //helper function to print in preorder
static void delete_node(FILE *fp, int cur_offset, int par_offset, node_t cur, node_t parent);       //deletes the node which is specified by cur

FILE* init_tree(const char* filename)
{
    FILE *f_ptr=fopen(filename,"r+");               //initially opening in r+ mode so as to not overwrite file if it exists.
    if(f_ptr!=NULL)                                 //incase it already exists pointer is returned
    {
        return f_ptr;
    }
    f_ptr=fopen(filename,"w+");                     //else the file is created using w+ mode
    tree_t header;
    header.free_head=-1;                            //initializing the header
    header.root=-1;
    fwrite(&header,sizeof(tree_t),1,f_ptr);         //storing at start of file
    return f_ptr;
}

void close_tree(FILE *fp)
{
    fclose(fp);
}

void display_inorder(FILE *fp)
{
    tree_t header;
    fseek(fp,0,SEEK_SET);                           //reading header from file
    fread(&header,sizeof(tree_t),1,fp);         
    inorder_print(fp,header.root);
    printf("\n");
}

void display_preorder(FILE *fp)
{
    tree_t header;
    fseek(fp,0,SEEK_SET);                           //reading header from file
    fread(&header,sizeof(tree_t),1,fp);
    preorder_print(fp,header.root);
    printf("\n");   
}

void insert_key(int key, FILE *fp)
{
    int pos;                                        //variable to traverse till end, stores offset of nodes
    int offset;                                     //stores offset of the newly created node
    int dist;                                       //stores actual byte distance of parent node to which insertion will be done
    tree_t header;  
    node_t parent;                                  //stores the above mentioned parent node
    node_t node;                                    //the new node which is ready to be inserted
    fseek(fp,0,SEEK_SET);
    fread(&header,sizeof(tree_t),1,fp);
    pos=header.root;                                //starting at the root of the tree
    while(pos!=-1)                                  //untill a NULL node is reached
    {
        dist=sizeof(tree_t)+pos*sizeof(node_t);
        fseek(fp,dist,SEEK_SET);
        fread(&parent,sizeof(node_t),1,fp);         //read the parent node
        if(parent.key==key)                         //if node is already present do not do anything
        {
            return;
        } 
        else if(parent.key>key)
        {
            pos=parent.left_offset;                 //if key is lesser it needs to be stored in the left subtree of current node
        }
        else
        {
            pos=parent.right_offset;                //if not then to the right subtree
        }
    }

    if(header.free_head==-1)                                    //no free nodes present 
    {
        fseek(fp,0,SEEK_END);
        offset= (ftell(fp)-sizeof(tree_t))/sizeof(node_t);
        fseek(fp,ftell(fp),SEEK_SET);                           //go to end of the file
        node.key=key;
        node.left_offset=-1;
        node.right_offset=-1;
        fwrite(&node,sizeof(node_t),1,fp);                      //and create the new node here
    }
    else                                                        // if there are free nodes
    {
        offset= header.free_head;
        fseek(fp,sizeof(tree_t)+offset*sizeof(node_t),SEEK_SET);
        fread(&node,sizeof(node_t),1,fp);
        header.free_head=node.right_offset;                             //let the free head point to the next free node if any
        node.left_offset=-1;                                            //preparing the free node to be used in the tree
        node.right_offset=-1;
        node.key=key;
        fseek(fp,sizeof(tree_t)+offset*sizeof(node_t),SEEK_SET);        //store the new values of the unfree node and free head in the file
        fwrite(&node,sizeof(node_t),1,fp);
        fseek(fp,0,SEEK_SET);
        fwrite(&header,sizeof(tree_t),1,fp);
    }

    if(header.root==-1)                                                 //this is the first node
    {
        header.root=offset;                                             //then this node will be the root
        fseek(fp,0,SEEK_SET);
        fwrite(&header,sizeof(tree_t),1,fp);
    }
    else
    {
        if(parent.key>key)                                              //if not then find if it belongs to left/right of parent
        {
            parent.left_offset=offset;
        }
        else
        {
            parent.right_offset=offset;
        }
        fseek(fp,dist,SEEK_SET);
        fwrite(&parent,sizeof(node_t),1,fp);                            //store the changes made to parent node
    }
}

void delete_key(int key, FILE *fp)
{
    int par_offset=-1,cur_offset, cur_dist;                             //offset of the parent node(-1 since no parent to the root), offset of the node to be deleted and its distance in bytes from start
    tree_t header;                                              
    node_t cur, parent;                                                 //the node to be deleted and its parent
    fseek(fp,0,SEEK_SET);
    fread(&header,sizeof(tree_t),1,fp);
    cur_offset=header.root;                                             //starting from root
    while(cur_offset!=-1)
    {
        cur_dist=sizeof(tree_t)+cur_offset*sizeof(node_t);
        fseek(fp,cur_dist,SEEK_SET);
        fread(&cur,sizeof(node_t),1,fp);
        if(cur.key==key)                                                //if the key 
        {
            
            delete_node(fp, cur_offset, par_offset, cur, parent);       //delete the node from the active tree
            cur.left_offset=-1;
            cur.right_offset=header.free_head;                          //store the newly deleted node into the list of free nodes
            fseek(fp,cur_dist,SEEK_SET);
            fwrite(&cur,sizeof(node_t),1,fp);
            fseek(fp,0,SEEK_SET);
            fread(&header,sizeof(tree_t),1,fp);
            header.free_head=cur_offset;
            fseek(fp,0,SEEK_SET);
            fwrite(&header,sizeof(tree_t),1,fp);
            break;
        }
        par_offset=cur_offset;                                          //store the parent details before finding its child to move on to
        parent=cur;
        if(cur.key>key)
        {
            cur_offset=cur.left_offset;
        }
        else if(cur.key<key)
        {
            cur_offset=cur.right_offset;
        }
    }                                                                   //indicating the key is not found so no actions performed
}

static void inorder_print(FILE *fp, int root)
{
    if(root!=-1)
    {
        node_t node;
        int offset=sizeof(tree_t)+root*sizeof(node_t);          //get the node at offset root
        fseek(fp,offset,SEEK_SET);
        fread(&node,sizeof(node_t),1,fp);
        inorder_print(fp, node.left_offset);                    //print its left subtree
        printf("%d ",node.key);                                 // then itself
        inorder_print(fp, node.right_offset);                   // and then right subtree
    }
}

static void preorder_print(FILE *fp, int root)
{
    if(root!=-1)
    {
        node_t node;
        int offset=sizeof(tree_t)+root*sizeof(node_t);
        fseek(fp,offset,SEEK_SET);
        fread(&node,sizeof(node_t),1,fp);
        printf("%d ",node.key);                                 //print the node itself
        preorder_print(fp, node.left_offset);                   // then its left subtree
        preorder_print(fp, node.right_offset);                  // then its right subtree
    }
}

static void delete_node(FILE *fp, int cur_offset, int par_offset, node_t cur, node_t parent)
{
    int pos;                                        //store offset of the replacement node
    if(cur.right_offset==-1)
    {
        pos=cur.left_offset;                        // if no inorder successor just delete directly and the left child of it will be taken by its parent
    }
    else                                            //else find its inorder successor
    {
        int p_off=-1,d;                             //offset of parent of the successor and successor's byte distance from start
        node_t c,p;                                 //successor node and its parent node
        pos=cur.right_offset;                       //starting at right subtree of node to be deleted
        d=sizeof(tree_t)+pos*sizeof(node_t);
        fseek(fp,d,SEEK_SET);
        fread(&c,sizeof(node_t),1,fp);

        while(c.left_offset!=-1)                    //until the left most node of right subtree is reached
        {
            p_off=pos;                              //store its parent position
            p=c;                                    //and value
            pos=c.left_offset;                      //go further left
            d=sizeof(tree_t)+pos*sizeof(node_t);    
            fseek(fp,d,SEEK_SET);
            fread(&c,sizeof(node_t),1,fp);
        }

        if(p_off!=-1)                                                   //if the successor is not the direct right child of the deleted node
        {
            p.left_offset=c.right_offset;                               //remove it from its position
            int p_dist=sizeof(tree_t)+p_off*sizeof(node_t);
            fseek(fp,p_dist,SEEK_SET);
            fwrite(&p,sizeof(node_t),1,fp);
            c.right_offset=cur.right_offset;                            //let the successor take the left child and right child of the deleted node
        }
        c.left_offset=cur.left_offset;
        int c_dist=sizeof(tree_t)+pos*sizeof(node_t);
        fseek(fp,c_dist,SEEK_SET);
        fwrite(&c,sizeof(node_t),1,fp);
    }
    if(par_offset==-1)                                                  //if its the only node in the tree
    {
        tree_t header;
        fseek(fp,0,SEEK_SET);
        fread(&header,sizeof(tree_t),1,fp);
        header.root=pos;
        fseek(fp,0,SEEK_SET);
        fwrite(&header,sizeof(tree_t),1,fp);
    }
    else                                                                //if not the only node, then make the successor take the place of the deleted node
    {
        if(parent.left_offset==cur_offset)
        {
            parent.left_offset=pos;
        }
        else
        {
            parent.right_offset=pos;
        }
        int par_dist=sizeof(tree_t)+par_offset*sizeof(node_t);
        fseek(fp,par_dist,SEEK_SET);
        fwrite(&parent,sizeof(node_t),1,fp);
    }
}
