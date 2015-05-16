#include "alist.h"

typedef struct assoc_node {
  char *name;
  char *value;
} Ainfo;

LNode_type *
init_assoc_list(void)
{
  LNode_type *al = init_linked_list();
  return al;
}

Ainfo*
create_node_assoc_list(const char *aname, const char *avalue)
{
  Ainfo *ai = (Ainfo *)calloc(1,sizeof(Ainfo));
  if(!ai) {
    fprintf(stderr, "Association List Out of Memory\n");
    abort();
  }
  else
    {
      GRStrAllocCopy(ai->name,(char *)aname);
      GRStrAllocCopy(ai->value,(char *)avalue);
    }
  return ai;
}
void 
delete_node_assoc_list(Ainfo *ai)
{
  if(ai)
    {
      if(ai->value)
      {
	free(ai->value);
	ai->value = NULL;
      }
      if(ai->name)
      {
	free(ai->name);
	ai->name = NULL;
      }
      free(ai); ai = NULL;
    }
}

int
delete_assoc_list(LNode_type *alist)
{
  if(alist)
    {
      LNode_type *ptr;
      for(ptr= alist->next; ptr != alist ; ptr = ptr->next)
	{
	  Ainfo *ai = (Ainfo *)ptr->info;
	  delete_node_assoc_list(ai);
	}
      Delete_all_list(alist);
      return 0;
    }
  return 1;
}

int
add_object_assoc_list(LNode_type *alist, const char *aname, 
		      const char *avalue)
{
  if(alist && aname)
    {
      Ainfo *ai = (Ainfo *)calloc(1,sizeof(Ainfo));
      if(!ai)
	fprintf(stderr, "Association List Out of Memory\n");
      else
	{
	  LNode_type *node = create_new_node_llist((char *)ai);
	  GRStrAllocCopy(ai->name,aname);
	  if(avalue)
	      GRStrAllocCopy(ai->value,avalue);
	  Insert_linked_list(alist,node);
	}
      return 0;
    }
  return 1;
}

Ainfo *
find_object_assoc_list(LNode_type *alist, const char *aname)
{
  if(alist && aname)
    {
      LNode_type *ptr;
      for(ptr= alist->next; ptr != alist ; ptr = ptr->next)
	{
	  Ainfo *ai = (Ainfo *)ptr->info;
	  if(!strcasecmp(ai->name,aname))
	    return ai;
	}
    }  
  return NULL;
}
int 
replace_object_assoc_list(LNode_type *alist, const char *aname, 	
			  const char *avalue)
{
  if(alist && aname)
    {
      Ainfo *ai = find_object_assoc_list(alist,aname);
      if(ai)
	{
	  GRStrAllocCopy(ai->value,avalue);
	  return 0;
	}
    }
  return 1;
}

const char *
find_value_assoc_list(LNode_type *alist, const char *aname)
{
  if(alist && aname)
    {
      Ainfo *ai = find_object_assoc_list(alist,aname);
      if(ai)
	return ai->value;
    }
  return NULL;
}

int
remove_object_assoc_list(LNode_type *alist, const char *aname)
{
  if(alist && aname)
    {
      LNode_type *dptr,*ptr;
      for(ptr= alist->next; ptr != alist ; ptr = ptr->next)
	{
	  Ainfo *ai = (Ainfo *)ptr->info;
	  if(ai && !strcmp(ai->name,aname))
	    {
	      dptr = ptr;
	      ptr = ptr->next;
	      delete_node_assoc_list(ai);
	      Delete_linked_list(dptr);
	    }
	}
      return 0;
    }
  return 1;
}


LNode_type *
copy_assoc_list(LNode_type *alist)
{
  if(alist)
    {
      LNode_type *nalist = init_assoc_list();
      LNode_type *ptr;
      for(ptr= alist->back; ptr != alist ; ptr = ptr->back)
	{
	  Ainfo *ai = (Ainfo *)ptr->info;
	  Ainfo *nai = create_node_assoc_list(ai->name,ai->value);
	  Insert_linked_list(nalist,create_new_node_llist((char *)nai));
	}
      return nalist;
    }
  return NULL;
}

int
get_node_info_assoc_list(LNode_type *node, char **aname, char **avalue)
{
  if(node)
    {
      Ainfo *ai = (Ainfo *)node->info;
      *aname = ai->name;
      *avalue = ai->value;
      return 0;
    }
  return 1;
}
