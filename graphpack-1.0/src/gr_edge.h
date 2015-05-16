#ifndef GR_EDGE_H
#define GR_EDGE_H

#define NO_EDGE_CLASS 0
#define TREE_CLASS 1
#define BACK_CLASS 2
#define FORWARD_CLASS 3 /* It could be also CROSS_CLASS */
#define LEVEL_CLASS 4
#define OUT_CLASS 5


#define MAX_UN_INT 65536
#define MAX_S_INT 32768
#define MIN_S_INT -32768

/* Structure of a Edge */
struct  edge    {
  struct  pt      *from; /* Vertex From */
  struct  pt      *to; /* Vertex to */
  Boolean touch; /* Visited */
  char *label;
  char    *weight; /* Weight */
  Boolean highlight; /* Highlighted */
  Boolean select;
  short lwidth;
  short color;
  char attrib; /* 'n' NEXT, 'p' previous, 'c' contents, 's' stylesheet 
		'u' up , 'm' made, 'z' included object*/
  short   class; /* Class of edges : NO_EDGE_CLASS, TREE_CLASS, BACK_CLASS,
		    FORWARD_CLASS */
  void *context;
};

typedef struct edge *PEDGE;

#ifdef XGR
void
draw_arc(
Widget	widget,
int	x1, int y1, int x2, int y2,
double	theta,
int	n,
double	tharrow,
int	larrow,
GC	op,
char *	weight,
GraphFrame *gf);

void 
draw_edge(GraphFrame *gf, struct edge *e, GC gc);
#endif
void
delete_edge_info(struct edge *e);
struct edge *
create_edge_memory(GraphFrame *gf, struct pt *v1,struct pt *v2);
struct edge * 
copy_edge(struct edge * e, struct pt *v1, struct pt *v2, GraphFrame *gf);
void
update_edge(GraphFrame *gf,struct edge *e);
short
get_edge_color(struct edge *e);
void
set_edge_color(struct edge *e, short col);
char
get_edge_attribute(struct edge *e);
char *
get_edge_name(PEDGE e);
char *
get_reverse_edge_name(PEDGE e);
char *
get_label_edge(struct edge *e);
void
set_edge_attrib(struct edge *e, char c);
char *
get_string_edge_attribute(struct edge *e);
char
get_char_edge_attribute(char *stratr);

void
change_highlight_edge(struct edge *e, Boolean high);
void 
change_weight_edge(struct edge *e, char *weight);
void 
change_label_edge(struct edge *e, char *label);
Boolean
is_edge_highlighted(struct edge *e);
#ifdef XGR
void
highlighting_edge(GraphFrame *gf, struct edge *e, GC gc);
#endif
char *
get_weight_edge(struct edge *e);

struct pt *
get_from_vertex(struct edge *e);

struct pt *
get_to_vertex(struct edge *e);

void 
select_edge_selecting_vertices(GraphFrame *gf, struct edge *e);

int 
is_weight_lower(PEDGE e, PEDGE f);
int 
is_weight_bigger(PEDGE e, PEDGE f);

#ifdef XGR
void
redraw_edge(GraphFrame *gf,struct edge *e);
GC 
choose_edge_gc(GraphFrame *gf, struct edge *e);
#endif

void
set_select_edge(struct edge *e, Boolean sel);
void
changing_select_edge(GraphFrame *gf, struct edge *e, Boolean sel);

Boolean
is_edge_selected(struct edge *e);
short
get_edge_class(struct edge *e);
void
set_edge_class(struct edge *e, short sc);
char *
get_string_edge_class(struct edge *e);

long
get_weight_node_to(PEDGE e);

long
get_long_weight_edge(PEDGE e);

long
get_simple_weight_edge(PEDGE e);

long
get_weight_out_degree_node_to(PEDGE e);

char *
get_edge_display_name(GraphFrame *gf, PEDGE e);

#endif


