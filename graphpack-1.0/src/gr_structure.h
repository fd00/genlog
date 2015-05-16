#ifndef GR_STRUCTURE_H
#define GR_STRUCTURE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

void
save_list_selected(struct pt *v,GraphFrame *gf);

void
delete_list_selected(struct pt *v,GraphFrame *gf);

void
insert_hash_vertex_name(GraphFrame *gf, char *vertex_name, struct pt *v);
void
change_names_edges(GraphFrame *gf, struct pt *v, char * label);

void
save_list_highlight(struct pt *v,GraphFrame *gf);

void
delete_list_highlight(struct pt *v,GraphFrame *gf);

Node_type *
select_nearest_vertex(struct pr_pos *loc, GraphFrame *gf);

Node_type *
select_nearest_edge(struct pt *v1, struct pt *v2, GraphFrame *gf);

void
insert_e( struct edge *e,GraphFrame *gf );

Key_type
calculate_key_pos(struct pr_pos	*loc, int side, int rel_width);

Key_type 
get_key_string_edge(char *sname, char *tname);

Key_type
calculate_key_edge(struct pt *v1, struct pt *v2);

void
update_pos_grid(struct pr_pos *loc, int side);

void
insert_v(struct	pt *elem,GraphFrame *gf);

void
update_list_edges(LNode_type *ledges, struct edge *e); /* private */

void
update_list(LNode_type *lhead, char * elem); /*private*/

void
delete_v(GraphFrame *gf,Node_type *nptr,struct pt *v); /*private */

void
delete_e(GraphFrame *gf,struct edge *e);

int
compute_clusters(GraphFrame *gf,int k);

int
compute_connected_component(GraphFrame *gf);

int
compute_sel_connected_components(GraphFrame *gf);

int
select_max_component_of_selected_vertices(GraphFrame *gf);

int
select_tree_of_selected_vertices(GraphFrame *gf, PNODE root);

#endif


