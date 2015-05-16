#ifndef _USER_H_
# define _USER_H_

#include <gr_graph_fun.h>

typedef struct _robo_user {
	char *address;
	char *agent;
	char *referer;
	char *version;

	time_t start;
	PNODE node;

} robo_user;


char *
make_address(void);

robo_user *
make_random_user(void);

void
destroy_user(LNode_type *users, robo_user *user);

LNode_type *
create_random_user_list(int num_users);

robo_user *
get_random_user_from_list(LNode_type *users, int num_users);

#endif
