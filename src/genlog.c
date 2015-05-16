#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include <time.h>

#include <gr_graph_fun.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "getopt.h"
#include "perror_exit.h"
#include "user.h"

void
version(void)
{
	printf("%s\n",PACKAGE_STRING);
	printf("Send patches and bug reports to %s\n",PACKAGE_BUGREPORT);
}

void
help(void)
{
	version();
	printf("\n");
	printf("Usage: %s [--version] [--help] --graph\n",PACKAGE_NAME);
	printf("\t[--run-for-seconds] [--number-requests] [--requests-per-min] \n");
	printf("\t[--user-timeout] [--num-users] [--no-wait]\n\n");
	
	printf("  -v,--version                          Print version information and quit\n");
	printf("  -h,--help                             Print this help information and quit\n");
	printf("  -q,--quiet                            Be quiet ( no debug output to stderr )\n");
	printf("  -s,--run-for-seconds=<SECONDS>        Generate log entries for SECONDS seconds\n");
	printf("  -n,--number-requests=<REQUESTS>       Generate REQUESTS log entries\n");
	printf("  -r,--requests-per-min=<RPM>           Generate RPM requests per minute\n");
	printf("  -t,--user-timeout=<TIMEOUT>           Have users tend to leave by TIMEOUT seconds\n");
	printf("  -u,--num-users=<USERS>                Try to have USERS random active users at any time\n");
	printf("  -w,--no-wait                          Do not pause between log entries ( timestamps are\n");
	printf("                                         not adjusted )\n");
}

GraphFrame *
init_graph(void)
{
	GraphFrame *gf = NULL;

	gf = create_current_graph();

	set_dimensions(gf, 1000,1000);
	set_shape(gf, BOX);
	set_dim_vertex(gf,BSIDE,HSIDE);
	set_scale(gf, 0);
	set_num_vertex(gf, 0);
	set_graph_dir(gf, DIRECTED);

	return gf;
}

GraphFrame *
load_graph_file(char *path)
{
	GraphFrame *graph;
	FILE *f;
	long len = 0,tlen = 0;
	char *data = NULL;
	char tmp[512];

	assert(path != NULL);

	graph = init_graph();

	if(!(f = fopen(path,"r")))
		perror_exit("fopen() (%s:%d)",__FILE__,__LINE__);

	while((tlen = fread((void *)tmp,sizeof(char),512,f))>0) {
		if((data = (char *)realloc(data,sizeof(char)*(len+tlen+1)))<0)
			perror_exit("realloc() (%s:%d)",__FILE__,__LINE__);

		memcpy(&data[len],tmp,tlen);
		len += tlen;
	}
	parse_xml_text_graph(graph,data,len);
	
	return graph;
}

const char *
relative_url(const char *url)
{
	char *s;

	if(!strstr(url,"http://"))
		return url;
	
	if(!(s = strchr(url,'/')))
		return url;
	++s;
	if(!(s = strchr(s,'/')))
		return url;
	++s;
	if(!(s = strchr(s,'/')))
		return url;
	return s;	
}

PNODE
get_random_node(GraphFrame *graph)
{
	PNODE node = (PNODE)find_info_pos(graph->list_vertices,(rand()%graph->count_vertex)+1);
	return node;
}

/* void
move_user_random_out_link_html(robo_user *user)
{
	int links;
	int i;
	LNode_type *eout = NULL, *ptr, *eohtml = NULL;
	const char *mime;
	int eohtmlcnt = 0;
	PNODE node = user->node;
	PEDGE edge;
	
	if(!node)
		return;
	
	if(!(links = num_hyperlinks_out(node))) {
		user->node = NULL;
		return;
	}

	if(!(eout = get_edges_out(node))) {
		user->node = NULL;
		return;
	}

	// slowly prune out non html links
	eohtml = init_linked_list();
	for(ptr = eout->back; ptr != eout; ptr = ptr->back) {
		PEDGE e = (PEDGE) ptr->info;
		PNODE u = e->to;
		if((mime = get_attribute(u,"mime"))){
			Insert_linked_list(eohtml,create_new_node_llist((char *)u));
			++eohtmlcnt;
		}
	}

	if(user->referer)
		free(user->referer);
	if(node->label)
		user->referer = strdup(node->label);
	else
		user->referer = strdup("-");

	if(!(user->node = (PNODE)find_info_pos(eohtml,(rand()%eohtmlcnt)+1)))
		user->node = NULL;

	Delete_all_list(eohtml);
} */

void
move_user_random_out_link(robo_user *user)
{
	int links;
	char *referer;
	PNODE node;
	PEDGE edge;
	int ran;

	if(!(links = num_hyperlinks_out(user->node))) {
		user->node = NULL;
		return;
	}
	
	referer = user->node->label;
	
	ran = (rand()%links)+1;
	edge = (PEDGE)find_info_pos(get_edges_out(user->node),ran);
	node = edge->to;

	if(!(user->node = node))
		return;
	
	if(user->referer)
		free(user->referer);
	user->referer = strdup(referer);
}

void
generate_request(GraphFrame *graph, robo_user *user)
{
	char date[80];
	time_t now;
	struct tm *t;
	
	if(user->node)
		move_user_random_out_link(user);
	
	if(!user->node)
		user->node = get_random_node(graph);

	now = time(NULL);
	t   = gmtime(&now);
	strftime(date,80,"[%d/%b/%Y:%H:%M:%S ",t);
	snprintf(&date[strlen(date)],80-strlen(date),"-0500]");
	/*
	snprintf(&(date[strlen(date)]),80-strlen(date),"%c%.2d%.2d]",
                 t->tm_gmtoff < 0 ? '-' : '+',
                 t->tm_gmtoff / (60*60), t.tm_gmtoff % (60*60));
	*/
	
	printf("%s - - %s \"GET %s %s\" %s %d \"%s\" \"%s\"\n",
			user->address,
			date,
			user->node->label ? relative_url(user->node->label) : "/",
			user->version,
			get_attribute(user->node,"code") ? get_attribute(user->node,"code") : "200",
			512,
			user->referer,
			user->agent);
	fflush(stdout);

}

int
main(int argc, char **argv)
{
	int i;
	robo_user *user = NULL;

	char *graphfile = NULL;
	int run_seconds = 0;
	int num_requests = 100;
	int rpm = 100;
	int uto = 60;
	int num_users = 10;
	short quiet = 0;
	short wait  = 1;
	
	unsigned long usec = (1000000*60)/rpm;
	
	time_t stop_time;
	
	LNode_type *users = NULL;
	GraphFrame *graph = NULL;

	char o;
	struct option opt_long[] = {
		{"version",no_argument,NULL,'v'},
		{"help",no_argument,NULL,'h'},
		{"quiet",no_argument,NULL,'q'},
		{"graph",required_argument,NULL,'g'},
		{"run-for-seconds",required_argument,NULL,'s'},
		{"number-requests",required_argument,NULL,'n'},
		{"requests-per-min",required_argument,NULL,'r'},
		{"user-timeout",required_argument,NULL,'t'},
		{"num-users",required_argument,NULL,'u'},
		{"no-wait",no_argument,NULL,'w'},
		{0,0,0,0}
	};

	while((o = getopt_long(argc,argv,"vhqg:s:n:r:t:u:w",opt_long,NULL)) != EOF) {
		switch(o) {
		case 'v':
			version();
			exit(EXIT_SUCCESS);
			break;
		case 'h':
			help();
			exit(EXIT_SUCCESS);
			break;
		case 'q':
			quiet = 1;
			break;
		case 'g':
			graphfile = strdup(optarg);
			break;
		case 's':
			run_seconds = atoi(optarg);
			num_requests = 0;
			break;
		case 'n':
			num_requests = atoi(optarg);
			run_seconds = 0;
			break;
		case 'r':
			rpm = atoi(optarg);
			if(!rpm)
				error_exit("--requests-per-min requires a numeric argument\n");
			usec = (1000000*60)/rpm;
			break;
		case 't':
			uto = atoi(optarg);
			break;
		case 'u':
			num_users = atoi(optarg);
			if(num_users < 1)
				error_exit("--num-users must be an integer greater than 0\n");
			break;
		case 'w':
			wait = 0;
			break;
		}
	}

	if(!graphfile) {
		fprintf(stderr,"XGMML Webgraph file required\n");
		help();
		exit(EXIT_FAILURE);
	}

	srand(time(NULL));
		
	if(!quiet) {
		fprintf(stderr,"Using:\n");
		fprintf(stderr,"\t         Graph File : %s\n", graphfile);
		if(run_seconds)
			fprintf(stderr,"\t           Run Time : %d\n", run_seconds);
		if(num_requests)
			fprintf(stderr,"\t    Number Requests : %d\n", num_requests);
		fprintf(stderr,"\tRequests Per Minute : %d\n", rpm);
		fprintf(stderr,"\t       User Timeout : %d\n\n", uto);
	}
	
	if(!quiet)
		fprintf(stderr,"Initializing %d random users\n",num_users);
	users = create_random_user_list(num_users);

	if(!quiet)	
		fprintf(stderr,"Loading graph file (%s)\n",graphfile);
	graph = load_graph_file(graphfile);
	free(graphfile);
	graphfile = NULL;
		
	if(!quiet)
		fprintf(stderr,"\n--- Begin Output on stdout ---\n\n");

		
	if(run_seconds) {
		
		stop_time = time(NULL) + run_seconds;
		
		while(time(NULL) < stop_time) {
			user = get_random_user_from_list(users,num_users);
			generate_request(graph,user);
			check_expired(users,user,uto);

			if(wait)
				if(usleep(usec)<0)
					perror_exit("usleep() (%s:%d)",__FILE__,__LINE__);
		}
		
		
	}
	else if(num_requests < 0) {
		while(1) {
			user = get_random_user_from_list(users,num_users);
			generate_request(graph,user);
			check_expired(users,user,uto);

			if(wait)
				if(usleep(usec)<0)
					perror_exit("usleep() (%s:%d)",__FILE__,__LINE__);
		}
	}
	else if(num_requests) {
		
		for(i=0;i<num_requests;i++) {
			
			user = get_random_user_from_list(users,num_users);
			generate_request(graph,user);
			check_expired(users,user,uto);

			if(wait)
				if(usleep(usec)<0)
					perror_exit("usleep() (%s:%d)",__FILE__,__LINE__);
		}
		
	}

	return EXIT_SUCCESS;
}
