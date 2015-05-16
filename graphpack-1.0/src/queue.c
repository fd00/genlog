#include "queue.h"

LNode_type *
init_queue(void)
{
  LNode_type *q = init_linked_list();
  return q;
}

char *
head_queue(LNode_type *q)
{
  LNode_type *node = q->back;
  char *info = node->info;
  return info;
}
int
is_queue_empty(LNode_type *q)
{
  return(is_empty_list(q));
}

void
Enqueue(LNode_type *q, char *info)
{
  LNode_type *node = create_new_node_llist(info);
  Insert_linked_list(q,node);
}

char *
Dequeue(LNode_type *q)
{
  LNode_type *node = q->back;
  char *info = node->info;
  Delete_linked_list(node);
  return info;
}






