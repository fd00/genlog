#ifndef GR_OPERATIONS_H
#define GR_OPERATIONS_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

struct pt *
find_vertex(GraphFrame *cgr, struct pr_pos *loc);

struct edge *
find_edge(GraphFrame *cgr,struct pt *v1, struct pt *v2);

struct pt *
find_vertex_by_name(GraphFrame *cgr, char *vlabel);

struct pt *
find_vertex_by_number(GraphFrame *cgr, int number);

PEDGE
find_edge_by_name(GraphFrame *cgr,char *sname, char *tname);

int
adding_vertex(GraphFrame *gf, char *name, char *weight, struct pr_pos *loc);

void
add_get_vertex_name(char **vertex_name, char *src_name,GraphFrame *gf);
int
adding_edge(GraphFrame *gf,struct pt *v1,struct pt *v2, char *weight);

int 
rotate_graph(GraphFrame *gf, int degs);

void
move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc);

void
verify_move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc);

struct pt *
get_first_vertex(GraphFrame *gf);

struct pt *
get_last_vertex(GraphFrame *gf);

int 
zooming_graph(GraphFrame *gf);

int
expanding_graph(GraphFrame *gf,int escale);

int
expanding_graph_without_moving(GraphFrame *gf, int escale,int xc, int yc);

int
zooming_graph_without_moving(GraphFrame *gf, int xc, int yc);

int
scaling_graph(GraphFrame *gf);

int
calculate_size(int scale);

int 
scale_dim(int scale, int x);

void
highlight_selected_nodes(GraphFrame *gf);

void
highlight_selected_edges(GraphFrame *gf);

void
dehighlight_all_nodes(GraphFrame *gf);

void
dehighlight_all_edges(GraphFrame *gf);

void
select_all_nodes(GraphFrame *gf);
int 
select_objects(GraphFrame *gf);

int
select_single_component(GraphFrame *gf, PNODE v);

int
select_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

int
deselect_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

int
select_only_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

int
select_all_edges_in_region(GraphFrame *gf, int x, int y, int w, int h);

int
select_only_edges_in_region(GraphFrame *gf, int x, int y, int w, int h);

int
select_all_nodes_out_degree(GraphFrame *gf,int outdeg);

int
select_all_nodes_in_degree(GraphFrame *gf,int indeg);

int
select_complement(GraphFrame *gf);
int
select_complement_edges(GraphFrame *gf);
void
deselect_all_nodes(GraphFrame *gf);
int
deselect_all_edges(GraphFrame *gf);

long
get_count_edges(GraphFrame *gf);

long
get_count_selected_edges(GraphFrame *gf);


void
reset_color_edges(GraphFrame *gf);
int
select_all_sources(GraphFrame *gf);
int
select_all_leaves(GraphFrame *gf);
int
select_all_inside_nodes(GraphFrame *gf);
int
select_all_documents(GraphFrame *gf);
void
dehigh_all_nodes(GraphFrame *gf);
void 
deleting_vertex(GraphFrame *cgr,struct pt *v);
void
delete_edges_in_out_vertex(GraphFrame *gf,struct pt *v); /*private */

void
deleting_edge(GraphFrame *gf, struct edge *e);

void 
reset_graph_info(GraphFrame *gf); /*public*/

#ifdef XGR
void
clear_graph_area(GraphFrame *gf); /*private*/
#endif

void 
delete_edges(Hashtable_type H); /*private */
void 
delete_vertices2(Hashtable_type H); /*private*/
void 
delete_vertices(LNode_type *lver);
void
delete_graph_resources(GraphFrame *gf); /*public*/
int 
decrease_window_size(GraphFrame *gf, Dimension *X, Dimension *Y);
int 
increase_window_size(GraphFrame *gf, Dimension *X, Dimension *Y);
int
move_graph_ver(GraphFrame *gf, int y);
int
move_graph_hor(GraphFrame *gf, int x);
int
move_graph_lin(GraphFrame *gf, int x,int y);

int
move_selected_nodes_lin(GraphFrame *gf, int x,int y);

int
move_neg_graph(GraphFrame *gf);
void
insert_all_vertices_hash_table(GraphFrame *gf);
int 
verify_insert_v(GraphFrame *gf, struct pt *v);

int
select_nodes_by_string_template(GraphFrame *gf, char *tmpl); /* public */
int
select_immediate_children(GraphFrame *gf, struct pt *v); /* public */
int
select_immediate_parents(GraphFrame *gf, struct pt *v); /* public */
int 
select_neighbors(GraphFrame *gf);/*public */

int
select_tree(GraphFrame *gf, struct pt *v); 

int
dfs_visit_tree(GraphFrame *gf, struct pt *v);

int
dfs_undirected_visit_tree(GraphFrame *gf, struct pt *v);
int
delete_selected_nodes(GraphFrame *cgr);

struct pt *
choose_best_tree(GraphFrame *gf, struct pt *v, int gmax);

int 
select_tree_by_template(GraphFrame *gf, struct pt *v);

int
dfs_visit_tree_by_template(GraphFrame *gf, struct pt *v, char *tmpl);

struct pt *
choose_best_tree_by_template(GraphFrame *gf, struct pt *v, int gmax);

struct pt *
choose_first_tree_by_template(GraphFrame *gf, struct pt *v, int gmax);

int
count_objects_tree(GraphFrame *gf, PNODE v);
int
dfs_visit_tree_counting_objects(GraphFrame *gf, struct pt *v);

int
select_leaves_children(GraphFrame *gf, struct pt *v);

LNode_type *
linear_dfs_visit_tree(GraphFrame *gf, struct pt *v, int *count,char shape);

int
rec_linear_dfs_visit_tree(GraphFrame *gf, struct pt *v, LNode_type *lnodes,
			  char shape);
int
select_tree_levels(GraphFrame *gf, struct pt *v, int levels);
int
dfs_visit_tree_levels(GraphFrame *gf, struct pt *v,int levels);
int 
dfs_undirected_visit_tree_levels(GraphFrame *gf, struct pt *v,int levels);

int
select_edge_by_class(GraphFrame *gf, short class);

int
select_shape_vertices(GraphFrame *gf, char shape);
void
reset_counter_nodes(GraphFrame *gf);
void
reset_weight_edges(GraphFrame *gf);

int
how_many_edges_are_visited(GraphFrame *gf);
void
assign_visited_edges(GraphFrame *gf,PEDGE *A);

int
select_most_visited_edges(GraphFrame *gf, int mv);

int
how_many_nodes_are_visited(GraphFrame *gf);
void
assign_visited_nodes(GraphFrame *gf,PNODE *A);
int
select_most_visited_vertices(GraphFrame *gf, int mv);
int 
select_i_cluster_vertices(GraphFrame *gf, int i);
int
select_i_connected_component_vertices(GraphFrame *gf, int i);

void
swap(GPTR *a, GPTR *b);
int
partition(GPTR *A, int p, int r, int (*isl)(), int (*isb)());
int
randomized_partition(GPTR *A, int p, int r,int (*isl)(), int (*isb)());


void 
randomized_quicksort(GPTR *A, int p, int r,int (*isl)(), int (*isb)());


GPTR
randomized_select(GPTR *A, int p, int r, int i,int (*isl)(), int (*isb)());

void
reset_node_context(GraphFrame *gf);

int 
number_of_edges_selected_vertices(GraphFrame *gf);

int
select_double_linked_nodes_and_edges(GraphFrame *gf);


#endif



