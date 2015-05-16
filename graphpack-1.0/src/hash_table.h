#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "llist.h"

#define EQ(a,b) (!strcmp((a),(b)))

#define HASHSIZE 997

typedef char *Key_type;

typedef struct item_tag {
  Key_type key;
  char * gr; /* Pointer to edge or vertex information */
} Item_type;

typedef struct node_tag {
  Item_type info;
  struct node_tag *next;
} Node_type;

typedef Node_type *Hashtable_type[HASHSIZE];

void 
init_hash_table(Hashtable_type H);
int 
Hash (Key_type s);
Node_type *
Sequentialsearch(Node_type *head, Key_type target, Node_type **aptr);
Node_type *
Retrieve_back_pointer(Hashtable_type H, Key_type target,Node_type **aptr);
Node_type *
Retrieve(Hashtable_type H, Key_type target);
void 
Insert(Hashtable_type H, Node_type *node);

void 
delete_info(Item_type info);

void 
Delete(Hashtable_type H, int h, Node_type *node, Node_type *anode);
void
Delete_target(Hashtable_type H,Key_type target);
void 
copy_info(Item_type *dest, Item_type src);
Node_type *
create_new_node(Item_type info);
void 
printing_hash_table(Hashtable_type H);

void
Delete_hash_table(Hashtable_type H);
void
insert_hash_info(Hashtable_type H, char *key_info, char *info_save);

void *
get_element_hashed(Hashtable_type H, Key_type key);

LNode_type *
get_list_elements(Hashtable_type H);


#endif
