#include "hash_table.h"
#include "llist.h"

void 
init_hash_table(Hashtable_type H)
{
  int i;
  for(i=0; i< HASHSIZE; i++)
    H[i] = NULL;
}

int 
Hash (Key_type s)
{
  int h = 0;
  while(*s)
    h+= *s++;
  return abs(h % HASHSIZE);
}
      

Node_type *
Sequentialsearch(Node_type *head, Key_type target, Node_type **aptr)
{
  Node_type *location = head;
  if(EQ(location->info.key,target))
    {
      *aptr = location;
      return location;
    }
  for(location = head; location->next != NULL; location = location->next)
    if(EQ(location->next->info.key,target))
      {
	*aptr = location;
	return location->next;
      }
  return NULL;
}

Node_type *
Retrieve_back_pointer(Hashtable_type H, Key_type target,Node_type **aptr)
{
  Node_type *head = H[Hash(target)];
  if(head)
    return Sequentialsearch(head,target,aptr);
  return NULL;
}

Node_type *
Retrieve(Hashtable_type H, Key_type target)
{
  Node_type *aptr;
  return Retrieve_back_pointer(H,target,&aptr);

}

void 
Insert(Hashtable_type H, Node_type *node)
{
  int h;
  h = Hash(node->info.key);
  node->next = H[h];
  H[h] = node;
}

void 
delete_info(Item_type info)
{
  /* This function depends on the info of the node */
  free(info.key); info.key = NULL;
  info.gr = NULL;
}

void 
Delete(Hashtable_type H, int h, Node_type *node, Node_type *anode)
{
  delete_info(node->info);
  if(node == anode)
    {
      H[h]=node->next;
      free(node); node = NULL;
    }
  else
    {
      anode->next = node->next;
      free(node); node = NULL;
    }
}

void
Delete_target(Hashtable_type H,Key_type target)
{
  int h= Hash(target);
  Node_type *aptr=NULL;
  Node_type *nptr = Retrieve_back_pointer(H, target,&aptr);

  if(nptr)
    {
      Delete(H,h,nptr,aptr);
    }
}
  
void 
copy_info(Item_type *dest, Item_type src)
{
  /* This function depends on the info of the node */
  dest->key = src.key;
  dest->gr = src.gr;
}
  
Node_type *
create_new_node(Item_type info)
{
  Node_type *nnode;
  nnode = (Node_type *)malloc(sizeof(Node_type));
  if(!nnode) {
      fprintf(stderr,"Out of Memory create_new_node\n");
      abort();
  }
  copy_info(&(nnode -> info),info);
  nnode -> next = NULL;
  return nnode;
}

void
Delete_hash_table(Hashtable_type H)
{
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(H[i] != NULL)
	{
	  Node_type * loc = H[i];
	  while (H[i] != NULL)
	    {
	      Delete(H,i,loc,loc);
	      loc = H[i];
	    }
	}
    }
}

void
insert_hash_info(Hashtable_type H, char *key_info, char *info_save)
{
  Node_type *nptr;
  Item_type item;
  Key_type key = NULL;
  GRStrAllocCopy(key,key_info);
  item.key = key;
  item.gr = info_save;
  nptr = create_new_node(item);
  Insert(H, nptr);
}

void 
printing_hash_table(Hashtable_type H)
{
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(H[i] != NULL)
	{
	  Node_type * loc = H[i];
	  printf(" %d : ", i);
	  for(;loc!=NULL;loc=loc->next)
	    {
	      /*struct pt *gr = (struct pt *)loc->info.gr;*/
	      printf("%s -> ", loc->info.key);
	      /*printf("(%s)",gr->label);*/
	    }
	  printf("|\n");
	}
    }
}

void *
get_element_hashed(Hashtable_type H, Key_type key)
{
  Node_type *nptr = Retrieve(H,key);
  if(nptr)
      return (void*)(nptr->info.gr);
  return NULL;
}

LNode_type *
get_list_elements(Hashtable_type H)
{
  LNode_type *l = init_linked_list();
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(H[i] != NULL)
	{
	  Node_type * loc = H[i];
	  for(;loc!=NULL;loc=loc->next)
	    {
	      void *ele = (void *)loc->info.gr;
	      Insert_linked_list(l,create_new_node_llist((char *)ele));
	    }
	}
    }
  return l;
}

/*
void 
main(void)
{ 
  Hashtable_type H;
  char str[20];
  Item_type item;
  Node_type * nptr;

  item.key = str;

  init_hash_table(H);
  while(str[0]!='0')
    {
      printf("string : ");
      scanf("%s", str);
      if(Retrieve(H,str) == NULL)
	{
	  nptr = create_new_node(item);
	  Insert(H,nptr);
	}
      printing_hash_table(H);
    }
  while(str[0]!='1')
    {
      printf("find : ");
      scanf("%s", str);
      nptr = Retrieve(H,str);
      if (nptr != NULL)
	{
	  printf("found %s\n",nptr->info.key);
	  Delete_target(H,str);
	}
      else
	printf("not found\n");
      printing_hash_table(H);
    }
      
}
*/


