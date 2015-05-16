#ifndef GR_NODE_H
#define GR_NODE_H

/* Structure of a Vertex */
struct  pt {
  struct  pr_pos  loc; /* Location in the drawing area */
  char    *label; /* Name */
  char    *weight; /* Weight */
  short color;
  Boolean  select; /* Selected */
  Boolean highlight; /* Hightlighted */
  char shape; /* Shape */
  Boolean group; /* Group */
  int num; /* Number */
  Boolean mark; /* Marked (Barycentric) */
  Boolean pick; /* Picked (Barycentric) */
  int level; /* Level (Barycentric) */
  int prelim; /* Tree drawing */ /* this is used as index */
  int modifier; /* Tree drawing */
  int counter; /* counter for node */
  void *context;
  struct pt *left_neighbor; /* Tree drawing */
  LNode_type *attrib; /* List of Attributes */
  LNode_type *ledges_in; /* List of the Edges in */
  LNode_type *ledges_out; /* List of the Edges out */
  LNode_type *lver_con; /* List of the Vertices clockwise */
};

typedef struct pt *PNODE;

#ifdef XGR
void
draw_text(
Widget	widget,
int	x, 
int     y,
GC	op,
Font	font,
char	*text,
GraphFrame *gf);

void
draw_vertex(
Widget	widget,
GC gc,
int x, 
int y, 
int side,
char	*text,char *weight, char shape,GraphFrame *gf);
#endif

void    
select_vertex(struct pt *v,GraphFrame *gf);
void
deselect_vertex(struct pt *v,GraphFrame *gf);

struct pt * 
copy_vertex(struct pt * ov);
void
copy_vertex_info(PNODE v, PNODE ov);

void    
highlight_vertex(struct pt *v,GraphFrame *gf);
void
dehighlight_vertex(struct pt *v,GraphFrame *gf);
struct pt *
create_node_memory(char *nname);

int
get_shape_vertex_number(struct pt *v);

char
get_shape_number(char *nshape);

char
get_shape(struct pt *v);

void
set_vertex_color(struct pt *v,short color);
short
get_color(struct pt *v);

void
set_vertex_shape(struct pt *v,char shape);

char *
get_weight_vertex(struct pt *v);
char *
get_label_vertex(struct pt *v);

void
delete_vertex_info(struct pt *v);

Boolean
change_label_vertex(GraphFrame *gf, struct pt *v, char * label);

void
change_weight_vertex(GraphFrame *gf, struct pt *v, char * weight);

void
change_selection_vertex(GraphFrame *gf, struct pt *v, Boolean sel);

void
change_highlight_vertex(GraphFrame *gf, struct pt *v, Boolean high);

void
change_shape_vertex(GraphFrame *gf, struct pt *v, char shape);

char *
get_string_shape(struct pt *v);

char *
get_string_shape_file(struct pt *v);

Boolean
is_vertex_selected(struct pt *v);

Boolean
is_selected(struct pt *v,GraphFrame *gf);

Boolean
is_vertex_highlighted(struct pt *v);

void
set_vertex_position(struct pt *v, int x, int y);
int
get_x(struct pt *v);
int
get_y(struct pt *v);
#ifdef XGR
void
redraw_vertex(GraphFrame *gf, struct pt *v);
#endif
char *
get_vertex_display_name(GraphFrame *gf, struct pt *v);

struct pt *
create_node_in_graph_name(GraphFrame *gf, char *name);

const char *
get_attribute(struct pt *v, const char *aname);

int
get_num_attribute(struct pt *v);

void
add_attribute(struct pt *v, const char *att_name, const char *att_value);

/*
 *	Added by Andrew Morrison (11.30.02) 
 */
void
change_attribute(struct pt *v, const char *att_name, const char *att_value);

void
add_copy_attribute(struct pt *v, const char *att_name, const char *att_value);

String
get_color_string(short node_col);

#ifdef XGR
void
change_gc(Widget w, GC gc, short node_col);
GC 
choose_gc(GraphFrame *gf, struct pt *v);
#endif

void
copy_attributes(struct pt *v, struct pt *ov);

LNode_type *
get_attribute_list(PNODE v);

int
get_vertex_number(struct pt *v);
void
increment_counter_vertex(struct pt *v);

void
set_vertex_counter(struct pt *v,int counter);
int
get_counter(struct pt *v);

LNode_type *
get_edges_in(struct pt *v);

LNode_type *
get_edges_out(struct pt *v);

int
get_num_edges_in(PNODE v);
int
get_num_edges_out(PNODE v);

void *
get_node_context(struct pt *v);

struct pt *
get_first_parent(struct pt *Node);

int
is_counter_lower(PNODE u, PNODE v);
int
is_counter_bigger(PNODE u,PNODE v);

int
is_vertex_in_region(PNODE v, int x, int y, int w, int h);

int
is_vertex_weight_lower(PNODE u, PNODE v);
int
is_vertex_weight_bigger(PNODE u,PNODE v);

int
is_vertex_label_lower(PNODE u, PNODE v);
int
is_vertex_label_bigger(PNODE u,PNODE v);

int
is_vertex_sink(PNODE v);
int
is_vertex_source(PNODE v);

int 
is_vertex_object(PNODE v);
int 
is_vertex_group(PNODE v);

int
is_vertex_num_edges_in_lower(PNODE u, PNODE v);
int
is_vertex_num_edges_in_bigger(PNODE u,PNODE v);
int
is_vertex_num_edges_out_lower(PNODE u, PNODE v);
int
is_vertex_num_edges_out_bigger(PNODE u,PNODE v);



#endif


