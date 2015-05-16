#ifndef QUEUE_H
#define QUEUE_H
#include "llist.h"

LNode_type *
init_queue(void);
int
is_queue_empty(LNode_type *q);
char *
head_queue(LNode_type *q);
void
Enqueue(LNode_type *q, char *info);
char *
Dequeue(LNode_type *q);

#endif

