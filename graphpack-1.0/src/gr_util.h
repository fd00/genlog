#ifndef GR_UTIL_H
#define GR_UTIL_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#ifndef TOLOWER
#define TOLOWER(c) tolower(c)
#define TOUPPER(c) toupper(c)
#endif

void
grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf); /*public*/

void
extended_grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf); /* public */

void
create_edges_for_extended_group_graph(GraphFrame *ngf,GraphFrame *ogf);


void 
copy_unselected_vertices(GraphFrame *ngf,GraphFrame *ogf);/*private*/

struct pt *
create_group_vertex(GraphFrame *ngf,GraphFrame *ogf);/*private*/

void
create_edges_for_group_graph(GraphFrame *ngf,GraphFrame *ogf);/*private*/

void
get_new_location_group_vertex(struct pr_pos *loc,GraphFrame *ogf);/*private*/

PNODE
insert_vertex_in_graph(GraphFrame *ngf, PNODE ov);

void
insert_selected_nodes(GraphFrame *ngf,GraphFrame *ogf); /*public*/

void
insert_selected_edges(GraphFrame *ngf, GraphFrame *ogf);/*public*/

void
extended_browsing(GraphFrame *ngf,GraphFrame *ogf); /* public */

void 
reenumerate_vertices(GraphFrame *gf); /*public */


void
color_edges_by_class(GraphFrame *gf, short nc, short tc, short bc, short fc,
		     short oc, short lc);

short
gr_get_color_int(char *colorstr);

/* public */

char *
GRStrMatch (const char * tmpl, const char * name); /* private */

char * 
GRStrCaseMatch (const char * tmpl, const char * name); /*private */

void
untouch_edges(GraphFrame *gf);

void
compute_tree_graph(GraphFrame *gf, struct pt *v);

void
compute_tree_graph_highlight(GraphFrame *gf, struct pt *v);

void
link_tree(GraphFrame *gf);

LNode_type *
list_sources_nodes(GraphFrame *gf,int *count);

void
reset_pick_in_objects(GraphFrame *gf);

void
update_list_objects(GraphFrame *gf);

int 
move_graph_to_given_position(GraphFrame *gf, int xi, int yi);

int 
find_graph_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

int 
find_selected_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

int 
find_highlighted_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

LNode_type *
get_list_of_equivalent_nodes(GraphFrame *gf);

Boolean
compare_vertices_equivalents(PNODE v , PNODE u, GraphFrame *gf);

int
reduce_graph_with_equivalent_nodes(GraphFrame *ogf, GraphFrame *ngf, 
				   LNode_type *leqnodes);
void
copy_unmarked_vertices(GraphFrame *ngf, GraphFrame *ogf);

void
copy_edges_between_unmarked_vertices(GraphFrame *ngf, GraphFrame *ogf);

void
copy_verify_edges_in(GraphFrame *ngf, PNODE v, PNODE vr, LNode_type *leqnodes);
void
copy_verify_edges_out(GraphFrame *ngf, PNODE v, PNODE vr, LNode_type *leqnodes);

void
copy_complete_edges_in(GraphFrame *ngf, PNODE v, LNode_type *leqnodes);

void
copy_complete_edges_out(GraphFrame *ngf, PNODE v, LNode_type *leqnodes);

PEDGE
copy_complete_edge(GraphFrame *ngf, PEDGE e, PNODE nv, PNODE nu);

PNODE
copy_complete_vertex(GraphFrame *ngf, PNODE v);
void
copy_reducing_marked_vertices(GraphFrame *ngf, GraphFrame *ogf, 
			      LNode_type *leqnodes);

long
gr_factorial(long n);

#endif






