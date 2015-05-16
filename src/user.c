#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "perror_exit.h"
#include "user.h"

static char *agents[] = {
	"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.4) Gecko/20030624",
	"Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.5) Gecko/20031007 Firebird/0.7",
	"FAST-WebCrawler/3.8 (atw-crawler at fast dot no; http://fast.no/support/crawler.asp)",
	"Wget/1.8.2",
	"-",
	"Googlebot/2.1 (+http://www.googlebot.com/bot.html)",
	"Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.3b) Gecko/20030112",
	"Scooter/3.2",
	"Mozilla/4.0 (compatible; grub-client-1.4.3; Crawl your own stuff with http://grub.org)",
	"Mozilla/4.0 compatible ZyBorg/1.0 DLC (wn.zyborg@looksmart.net; http://www.WISEnutbot.com)",
	NULL,
	};

static char *referers[] = {
	"http://www.google.com/search?q=Rensselaer%20Polytechnic%20Institute",
	"http://www.google.com/search?q=Web%20Usage%20Mining%20+%20Dr%20Moorthy",
	"http://www.google.com/search?q=e^(pi*i)+1",
	"http://www.cs.rpi.edu/~moorthy",
	"http://www.organix.tv/research/w3live",
	"http://www.theonion.com",
	"http://www.cs.stanford.edu/~morria/SemanticWeb/SearchEngine/Prototype/",
	"http://www.csmonitor.com/articles/index.php?article=Happy%20Student%20Makes%20Semantic%20Search%20Engine",
	NULL
	};

char *
make_address(void)
{
	unsigned int a,b,c,d;
	char *address;
 
	a = rand()%0xff;
	b = rand()%0xff;
	c = rand()%0xff;
	d = rand()%0xff;

	if(!(address = (char *)malloc(sizeof(char)*16)))
		perror_exit("malloc() (%s:%d)",__FILE__,__LINE__);

	snprintf(address,16,"%d.%d.%d.%d",a,b,c,d);

	return address;
}

robo_user *
make_random_user(void)
{
	robo_user *user = NULL;

	static int agentlen = 0;
	static int reflen = 0;

	if(!agentlen)
		while(agents[agentlen]!=NULL)
			++agentlen;

	if(!reflen)
		while(referers[reflen]!=NULL)
			++reflen;

	if(!(user = (robo_user *)malloc(sizeof(robo_user))))
		perror_exit("malloc() (%s:%d)",__FILE__,__LINE__);

	user->address = make_address();
	user->agent   = strdup(agents[(rand()%agentlen)]);
	user->referer = strdup(referers[(rand()%reflen)]);
	user->version = strdup("HTTP/1.1");

	user->start = time(NULL);
	user->node = NULL;

	return user;
}

void
destroy_user(LNode_type *users, robo_user *user)
{
	LNode_type *ptr;
	
	assert(user != NULL);

	for(ptr=users->back;ptr!=users;ptr=ptr->back) {
		if(((robo_user *)ptr->info)->address == user->address) {
			Delete_linked_list(ptr);
			break;
		}
	}
	
	
	assert(user->address != NULL);
	free(user->address);
	user->address = NULL;

	assert(user->agent != NULL);
	free(user->agent);
	user->agent = NULL;

	assert(user->referer != NULL);
	free(user->referer);
	user->referer = NULL;

	free(user);
	user = NULL;
}

LNode_type *
create_random_user_list(int num_users)
{
	LNode_type *users = NULL;
	int i;

	users = init_linked_list();

	for(i=0;i<num_users;i++)
		Insert_linked_list(users,create_new_node_llist((char *)make_random_user()));

	return users;
}

robo_user *
get_random_user_from_list(LNode_type *users, int num_users)
{
	robo_user *user = NULL;
	user = (robo_user *)find_info_pos(users,(rand()%num_users)+1);
	assert(user != NULL);
	return user;
}

void
check_expired(LNode_type *users, robo_user *user,int timeout)
{
	if((time(NULL) - user->start) > timeout) {
		destroy_user(users, user);
		Insert_linked_list(users,create_new_node_llist((char *)make_random_user()));
	}
}
