#ifndef ALIST_H
#define ALIST_H

#include "llist.h"

LNode_type *
init_assoc_list(void);
int
delete_assoc_list(LNode_type *alist);
int
add_object_assoc_list(LNode_type *alist, const char *aname, 
		      const char *avalue);
int 
replace_object_assoc_list(LNode_type *alist, const char *aname, 	
			  const char *avalue);
const char *
find_value_assoc_list(LNode_type *alist, const char *aname);
int
remove_object_assoc_list(LNode_type *alist, const char *aname);

LNode_type *
copy_assoc_list(LNode_type *alist);

int
get_node_info_assoc_list(LNode_type *node, char **aname, char **avalue);

#endif



