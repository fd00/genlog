#ifndef LLIST_H
#define LLIST_H


#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct lnode_tag {
  char * info;
  struct lnode_tag *next;
  struct lnode_tag *back;
} LNode_type;

#define GRStrAllocCopy(dest, src) GRSACopy (&(dest), src)
#define GRStrAllocCat(dest, src)  GRSACat  (&(dest), src)

char * 
GRSACopy (char ** dest, const char * src);
char * 
GRSACat (char ** dest, const char * src);

LNode_type *
init_linked_list(void);
int
is_empty_list(LNode_type *head);
void
Insert_linked_list(LNode_type *head, LNode_type *node);
void 
Delete_linked_list(LNode_type *node);
LNode_type *
create_new_node_llist(char * info);
void
printing_linked_list(LNode_type * head);
LNode_type *
Search_Sequential_list(LNode_type * head, char *info);
void
Delete_all_list(LNode_type *head);
int 
count_llist(LNode_type * head);
int
is_info_list(LNode_type * head, char *info);
char *
next_element(LNode_type * head,LNode_type *node);
char *
back_element(LNode_type * head,LNode_type *node);

int
update_info_list(LNode_type *head, int pos, char *ninfo);
char *
find_info_pos(LNode_type *head, int pos);
#endif


