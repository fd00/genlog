#include "llist.h"
#include "gr_graph_fun.h"

LNode_type *
init_linked_list(void)
{
  LNode_type *dummy = (LNode_type *)malloc(sizeof(LNode_type));
  if(!dummy)
  {
      fprintf(stderr, "List Out of Memory\n");
      abort();
  }
  else
    {
      dummy->info = NULL;
      dummy->next = dummy;
      dummy->back = dummy;
    }
  return dummy;
}

int
is_empty_list(LNode_type *head)
{
  if(head->next==head)
    return 1;
  return 0;
}


void
Insert_linked_list(LNode_type *head, LNode_type *node)
{
  node->next = head->next;
  head->next = node;
  node->back = head;
  node->next->back = node;
}
void
Remove_node_list(LNode_type *node)
{
    
}
void 
Delete_linked_list(LNode_type *node)
{
  /* Assume that head is different that node */
  
  node->back->next = node->next;
  node->next->back = node->back;
  /* Make sure to delete info :  delete_info_llist(node);*/

  free(node); node = NULL;
}

void
Delete_all_list(LNode_type *head)
{
  LNode_type *ptr;

  if(!head)
      return;

  for(ptr= head; ptr->next != head ;)
    Delete_linked_list(ptr->next);

  free(head); head = NULL;
}
LNode_type *
Search_Sequential_list(LNode_type * head, char *info)
{
  LNode_type *ptr;
  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    if (ptr->next->info == info)
      break;
  return ptr->next;
}

char *
next_element(LNode_type * head,LNode_type *node)
{
  LNode_type *ptr;
  if(node->next == head)
    ptr = head->next;
  else
    ptr = node->next;

  return ptr->info;
}

char *
back_element(LNode_type * head,LNode_type *node)
{
  LNode_type *ptr;
  if(node->back == head)
    ptr = head->back;
  else
    ptr = node->back;

  return ptr->info;
}
int
is_info_list(LNode_type * head, char *info)
{ 
  LNode_type *ptr;
  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    if (ptr->next->info == info)
      return 1;
  return 0;
}
LNode_type *
create_new_node_llist(char * info)
{
  LNode_type * node = (LNode_type *)malloc(sizeof(LNode_type));
  if(!node) {
      fprintf(stderr,"Out of Memory create_new_node_llist\n");
      abort();
  }
  node->next = NULL;
  node->back = NULL;
  node->info = info;
  return node;
}

int 
count_llist(LNode_type * head)
{
  LNode_type *ptr;
  int count = 0;

  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    count++;

  return count;
}
int
update_info_list(LNode_type *head, int pos, char *ninfo)
{
  LNode_type *ptr;
  int i=1;
  for(ptr= head->back; ptr != head ; ptr = ptr->back)
    {
      if(i==pos)
	{
	  if(ptr->info == NULL)
	    {
	      ptr->info=ninfo;
	      return 0;
	    }
	  else
	    return -2;
	}
      i++;
    }
  return -1;
}
char *
find_info_pos(LNode_type *head, int pos)
{
  LNode_type *ptr;
  int i=1;
  for(ptr= head->back; ptr != head ; ptr = ptr->back)
    {
      if(i==pos)
	return ptr->info;
      i++;
    }
  return NULL;
}
void
printing_linked_list(LNode_type * head)
{
  LNode_type *ptr;
  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    {
      struct pt *v= (struct pt *)(ptr->next->info);
      printf("%s-> ", v->label);
      /*struct edge *e = (struct edge *)(ptr->next->info);
      printf("%s-%s -> ", e->from->label, e->to->label);*/
    }
  printf("\n");  
  
  for(ptr= head; ptr->back != head ; ptr = ptr->back)
       printf("%s -> ", ptr->back->info);
  printf("\n");
  

}

char * 
GRSACopy (char ** dest, const char * src)
{
  if (*dest) free(*dest);
  if (! src)
    *dest = NULL;
  else {
    if ((*dest  = (char  *) malloc(strlen(src) + 1)) == NULL)
    {
        fprintf(stderr,"Out of Memory GRSACopy");
	abort();
    }
    strcpy (*dest, src);
  }
  return *dest;
}

char * 
GRSACat (char ** dest, const char * src)
{
  if (src && *src) {
    if (*dest) {
      int length = strlen (*dest);
      if ((*dest  = (char  *) realloc(*dest, length + strlen(src) + 1)) == NULL) {
        fprintf(stderr,"Out of Memory GRSACat");
	abort();
      }
      strcpy (*dest + length, src);
    } else {
	if ((*dest  = (char  *) malloc(strlen(src) + 1)) == NULL) {
        fprintf(stderr,"Out of Memory GRSACat");
	abort();
	}
      strcpy (*dest, src);
    }
  }
  return *dest;
}

/*
void main(void)
{
  LNode_type * head = init_linked_list();
  char str[30];
  while(str[0]!='q')
    {
      LNode_type *tmp;
      char * info = NULL;

      printf("Enter : ");
      scanf("%s",str);
      GRStrAllocCopy(info,str);
      tmp = create_new_node_llist(info);
      Insert_linked_list(head,tmp);
      printing_linked_list(head);
    }
  while(head->next != head)
    {
      char * info = head->next->info;
      printf("Deleting : ");
      printf("%s\n",info);
      free(info); info = NULL;
      Delete_linked_list(head->next);
      printing_linked_list(head);
    }
}
*/
