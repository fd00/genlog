#ifndef GR_GRAPH_FUN_H
#define GR_GRAPH_FUN_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

GraphFrame *
create_current_graph(void);

void
init_basic_graph(GraphFrame *gf);

#ifdef XGR
void
set_drawing_area(GraphFrame *cgr, Widget w);
#endif

void
set_dimensions(GraphFrame * cgr, Dimension w, Dimension h);
void
set_pixmap_dimensions(GraphFrame * cgr, Dimension w, Dimension h);

#ifdef XGR
void
set_pixmap(GraphFrame * cgr, Pixmap pixmap);
void
set_font(GraphFrame * cgr, Font font);
#endif

void
set_cost_function(GraphFrame * cgr, long (*val)());

void
set_show_vertex_weight(GraphFrame * cgr, Boolean val);
void
set_show_edge_weight(GraphFrame * cgr, Boolean val);
void
set_shape(GraphFrame * cgr, char shape);

void
set_dim_vertex(GraphFrame *cgr,short vsize,short hsize);

void
set_scale(GraphFrame *cgr, short scale);

void
set_flag_draw(GraphFrame *cgr, Boolean flag);

Boolean
get_flag_draw(GraphFrame *cgr);

void
set_flag_pixdraw(GraphFrame *cgr, Boolean flag);

Boolean
get_flag_pixdraw(GraphFrame *cgr);

Boolean
get_flag_draw_highlight(GraphFrame *cgr);

void
set_flag_draw_highlight(GraphFrame *cgr, Boolean flag);

void
set_num_clusters(GraphFrame *cgr, int nclusters);
int
get_num_clusters(GraphFrame *cgr);
void
set_num_components(GraphFrame *cgr, int ncomponent);
int
get_num_components(GraphFrame *cgr);

void
set_num_vertex(GraphFrame *cgr, int nvertex);

void
set_num_ver_group(GraphFrame *cgr, int nvgroup);

void
set_count_vertex(GraphFrame *cgr,int count_v);

int
get_count_vertex(GraphFrame *cgr);

void
set_graph_dir(GraphFrame *cgr, int gdir);
void
set_graphic_flag(GraphFrame *cgr, int graphic);
void
set_current_group_vertex(GraphFrame *cgr, struct pt *cgv);

PNODE
get_root_vertex(GraphFrame *cgr);
void
set_root_vertex(GraphFrame *cgr, PNODE crv);

#ifdef XGR
void 
set_graph_gcs(GraphFrame *cgr, GC gc, GC pix_erase, GC pix_high, GC pix_color);
#endif

int
get_num_ver_group(GraphFrame *cgr);
int
get_num_vertex(GraphFrame *cgr);
int
get_ver_size(GraphFrame *cgr);
int
get_hor_size(GraphFrame *cgr);

int
get_graph_dir(GraphFrame *cgr);

int
get_graphic_flag(GraphFrame *cgr);

int
get_scale(GraphFrame *cgr);

LNode_type *
get_list_vertices(GraphFrame *cgr);

LNode_type *
get_list_selected_edges(GraphFrame *cgr);

LNode_type *
get_list_objects(GraphFrame *cgr);

LNode_type *
get_list_selected_vertices(GraphFrame *cgr);
LNode_type *
get_list_highlighted_vertices(GraphFrame *cgr);

#ifdef XGR
Pixmap
get_pixmap(GraphFrame *cgr);
#endif
Dimension
get_width(GraphFrame *cgr);
Dimension
get_height(GraphFrame *cgr);
Dimension
get_pixmap_width(GraphFrame *cgr);
Dimension
get_pixmap_height(GraphFrame *cgr);
int
get_name_display_mode(GraphFrame *cgr);
void
set_name_display_mode(GraphFrame *cgr, int dm);
void
set_name_edge_display_mode(GraphFrame *cgr, int em);
int
get_name_edge_display_mode(GraphFrame *cgr);

void
set_method_tree(GraphFrame *cgr, short mt);
short
get_method_tree(GraphFrame *cgr);
void
set_no_class_edge(GraphFrame *cgr, Boolean ncg);
Boolean
get_no_class_edge(GraphFrame *cgr);

extern PNODE
copy_complete_vertex(GraphFrame *ngf, PNODE v);

extern PEDGE
copy_complete_edge(GraphFrame *ngf, PEDGE e, PNODE nv, PNODE nu);

extern int 
move_graph_to_given_position(GraphFrame *gf, int xi, int yi);

extern int 
rotate_graph(GraphFrame *gf, int degs);

extern int 
find_graph_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

extern int 
find_selected_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

extern int 
find_highlighted_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h);

extern int
move_graph_lin(GraphFrame *gf, int x,int y);

extern int
move_selected_nodes_lin(GraphFrame *gf, int x,int y);

extern int
select_double_linked_nodes_and_edges(GraphFrame *gf);

#ifdef XGR
extern void 
refresh_graph_window(GraphFrame *gf);
void
move_vertex_draw(GraphFrame *cgr,struct pt *ov,struct pr_pos loc);
#endif
extern struct pt *
find_vertex(GraphFrame *cgr, struct pr_pos *loc);

extern struct pt *
find_vertex_by_name(GraphFrame *cgr, char *vlabel);
extern PEDGE
find_edge_by_name(GraphFrame *cgr,char *sname, char *tname);

extern struct pt *
find_vertex_by_number(GraphFrame *cgr, int number);

extern struct edge *
find_edge(GraphFrame *cgr,struct pt *v1, struct pt *v2);

extern int
adding_edge(GraphFrame *gf,struct pt *v1,struct pt *v2, char *weight);

extern int
adding_vertex(GraphFrame *gf, char *name, char *weight, struct pr_pos *loc);

extern void 
deleting_vertex(GraphFrame *cgr,struct pt *v);

extern void
deleting_edge(GraphFrame *gf, struct edge *e);

extern void
move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc);

extern void
verify_move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc);

extern struct pt *
get_first_vertex(GraphFrame *gf);

extern struct pt *
get_last_vertex(GraphFrame *gf);

extern int 
zooming_graph(GraphFrame *gf);

extern int 
expanding_graph(GraphFrame *gf,int escale);

extern int
expanding_graph_without_moving(GraphFrame *gf, int escale,int xc, int yc);

extern int
zooming_graph_without_moving(GraphFrame *gf, int xc, int yc);

extern void
delete_graph_resources(GraphFrame *gf); /*public*/

extern void
highlight_selected_nodes(GraphFrame *gf);

extern void
highlight_selected_edges(GraphFrame *gf);

extern void
dehighlight_all_nodes(GraphFrame *gf);

extern void
dehighlight_all_edges(GraphFrame *gf);

extern void
select_all_nodes(GraphFrame *gf);
extern int
select_all_sources(GraphFrame *gf);

extern int
select_single_component(GraphFrame *gf, PNODE v);

extern int
select_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

extern int
deselect_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

extern int
select_only_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h);

extern int
select_all_edges_in_region(GraphFrame *gf, int x, int y, int w, int h);

extern int
select_only_edges_in_region(GraphFrame *gf, int x, int y, int w, int h);

extern int
select_all_nodes_in_degree(GraphFrame *gf,int indeg);

extern int
select_all_nodes_out_degree(GraphFrame *gf,int outdeg);

extern int
select_complement(GraphFrame *gf);
extern int
select_complement_edges(GraphFrame *gf);
int
select_all_inside_nodes(GraphFrame *gf);
extern int
select_all_leaves(GraphFrame *gf);
extern int
select_shape_vertices(GraphFrame *gf, char shape);
extern int
select_edge_by_class(GraphFrame *gf, short class);

extern int
select_most_visited_vertices(GraphFrame *gf, int mv);

extern int
select_most_visited_edges(GraphFrame *gf, int mv);
extern void
reset_counter_nodes(GraphFrame *gf);
extern void
reset_weight_edges(GraphFrame *gf);
extern void
deselect_all_nodes(GraphFrame *gf);
extern int
deselect_all_edges(GraphFrame *gf);
extern void
reset_color_edges(GraphFrame *gf);

extern long
get_count_edges(GraphFrame *gf);
extern long
get_count_selected_edges(GraphFrame *gf);

extern void 
reset_graph_info(GraphFrame *gf); /*public*/

extern int
compute_linear_positions(GraphFrame *gf);
extern int
compute_circular_positions(GraphFrame *gf);
extern int
compute_horizontal_positions(GraphFrame *gf);
extern int
compute_diagonal_positions(GraphFrame *gf);
extern int
compute_random_positions(GraphFrame *gf);
extern int
compute_radial_tree_positions(GraphFrame *gf, struct pt *sel);
extern int
compute_pretty_positions(GraphFrame *gf, Boolean initial);
extern int 
compute_force_directed_positions(GraphFrame *gf);
extern int
compute_layered_pretty_positions(GraphFrame *gf);
extern int
compute_radial_pretty_positions(GraphFrame *gf);
extern int
compute_bary_positions(GraphFrame *gf,int cycle_len);
extern int
compute_sel_bary_positions(GraphFrame *gf);
extern int
compute_level_positions(GraphFrame *gf);
extern int 
compute_tree_positions(GraphFrame *gf, struct pt *sel,int);

extern int 
compute_inv_tree_positions(GraphFrame *gf, struct pt *sel,int RootOrientation);
extern int
compute_radial_inv_tree_positions(GraphFrame *gf, struct pt *sel);
extern int 
compute_radial_positions(GraphFrame *gf, struct pt *sel);

extern int 
compute_geometry_positions(GraphFrame *gf,struct pt *v, char *const);
extern int 
compute_geometry_positions_cycle(GraphFrame *gf,struct pt *v, struct pt *v2,
				 char *const);
extern void
grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf);
extern void
extended_grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf); /* public */
extern PNODE
insert_vertex_in_graph(GraphFrame *ngf, PNODE ov);

extern void
insert_selected_nodes(GraphFrame *ngf,GraphFrame *ogf); /*public*/

void
insert_selected_edges(GraphFrame *ngf, GraphFrame *ogf);/*public*/

extern void
extended_browsing(GraphFrame *ngf,GraphFrame *ogf); /* public */

extern void
resize_window_size(GraphFrame *gf,Dimension x, Dimension y);

extern void 
reenumerate_vertices(GraphFrame *gf); /*public */
#ifdef XGR
extern void
color_edges_by_class(GraphFrame *gf, short nc, short tc, short bc, short fc,
		     short oc, short lc);
extern short
gr_get_color_int(char *colorstr);
#endif
/* public */

extern int
select_nodes_by_string_template(GraphFrame *gf, char *tmpl); /* public */
extern int
select_immediate_children(GraphFrame *gf, struct pt *v);
extern int
select_immediate_parents(GraphFrame *gf, struct pt *v); /* public */
extern int 
select_neighbors(GraphFrame *gf);
extern int
select_tree(GraphFrame *gf, struct pt *v);
extern int 
select_objects(GraphFrame *gf);

extern int
select_all_documents(GraphFrame *gf);
extern int
select_tree_levels(GraphFrame *gf, struct pt *v, int levels);

extern int
delete_selected_nodes(GraphFrame *cgr);
extern int 
delete_nodes_from_to(GraphFrame *gf, int fromno, int tono);
extern struct pt *
choose_best_tree(GraphFrame *gf, struct pt *v, int gmax);

extern int 
select_tree_by_template(GraphFrame *gf, struct pt *v);

extern struct pt *
choose_best_tree_by_template(GraphFrame *gf, struct pt *v, int gmax);

extern struct pt *
choose_first_tree_by_template(GraphFrame *gf, struct pt *v, int gmax);

extern int
bfs(GraphFrame *gf, struct pt *root);

extern int
compute_tree_documents(GraphFrame *gf, int RootOrientation);

extern void
compute_tree_graph(GraphFrame *gf, struct pt *v);

extern int
dfs_visit_tree_by_template(GraphFrame *gf, struct pt *v, char *tmpl);

extern void
reset_mark_pick_vertices(GraphFrame *gf);
extern void
reset_edges(GraphFrame *gf);

extern int
complete_bfs_directed(GraphFrame *gf, struct pt *root);

extern LNode_type *
linear_dfs_visit_tree(GraphFrame *gf, struct pt *v, int *count,char shape);

extern void
compute_rank(GraphFrame *gf, int maxiter);

extern int
select_big_rank(GraphFrame *gf, int mah);

extern void
compute_author_hub(GraphFrame *gf, int maxiter,int sel);

extern int
select_big_auth(GraphFrame *gf, int mah);
extern int 
select_big_hub(GraphFrame *gf, int mah);

extern int
select_biggest_in_degree(GraphFrame *gf, int nbigin);
extern int
select_biggest_out_degree(GraphFrame *gf, int nbigout);

extern void
compute_author_hub_edges(GraphFrame *gf, int maxiter, int sel);
extern int
select_big_auth_edges(GraphFrame *gf, int mah);
extern int
select_big_hub_edges(GraphFrame *gf, int mah);
int
select_big_nodes_from_hub_edges(GraphFrame *gf, int mah);

extern void
Dijkstra(GraphFrame *gf, PNODE s,long (*comp_weight)());

extern long
get_value_shortest_path(PNODE t);

extern long
get_number_edges_shortest_path(PNODE t);

extern long
select_shortest_path(GraphFrame *gf, PNODE s, PNODE t,long (*comp_weight)());
extern long
select_shortest_path_to(GraphFrame *gf, PNODE t,long (*comp_weight)());
extern int
draw_radial_tree_from_shortest_path(GraphFrame *gf, PNODE sel,
				    long (*comp_weight)());
extern int
draw_hierarchical_radial_tree_graph(GraphFrame *gf, PNODE sel);

extern LNode_type *
classify_hierarchical_edges(GraphFrame *gf, PNODE sel);

extern double
compute_cluster_coefficient(GraphFrame *gf);

extern void
reset_pick_in_objects(GraphFrame *gf);
extern void
apply_function_to_shortest_path(PNODE t, void (*funct)());

extern void
pick_objects_in_path(PNODE v);
extern int 
select_biggest_shortest_path(GraphFrame *gf);

extern int
bfs_first_variation(GraphFrame *gf, struct pt *root);
extern Boolean
there_are_no_pick_nodes(GraphFrame *gf);

LNode_type *
get_list_of_equivalent_nodes(GraphFrame *gf);

int
reduce_graph_with_equivalent_nodes(GraphFrame *ogf, GraphFrame *ngf, 
				   LNode_type *leqnodes);

extern int
compute_clusters(GraphFrame *gf,int k);
extern int
select_i_cluster_vertices(GraphFrame *gf, int i);

extern int
compute_connected_components(GraphFrame *gf);
extern int
compute_sel_connected_components(GraphFrame *gf);
extern int
select_i_connected_component_vertices(GraphFrame *gf, int i);

extern int
select_max_component_of_selected_vertices(GraphFrame *gf);
extern int
select_tree_of_selected_vertices(GraphFrame *gf, PNODE root);
extern void
reset_node_context(GraphFrame *gf);

extern int 
number_of_edges_selected_vertices(GraphFrame *gf);

extern void
generate_line_graph(GraphFrame *ngf,GraphFrame *ogf);

extern int 
classify_edges_by_directory(GraphFrame *gf);

extern int
num_hyperlinks_out(PNODE v);
extern int
num_hyperlinks_to_ext(PNODE v);
extern int
num_hyperlinks_to_nhtml(PNODE v);
extern int
num_hyperlinks_to_html(PNODE v);
extern int 
vertex_http_code(PNODE v);
extern void
GR_set_shape_from_attributes(GraphFrame *gf);


extern int 
num_vertices_http_code(GraphFrame *gf, int code);
extern int
num_html_pages_with_error_http_code(GraphFrame *gf, int code);
extern int
num_orphan_html_pages(GraphFrame *gf);
extern int
num_html_pages_without_title(GraphFrame *gf);
extern int
num_html_pages_with_images_without_alt(GraphFrame *gf);
extern int
num_objects_http_code(GraphFrame *gf, int code);


extern int
is_vertex_num_hyperlinks_lower(PNODE u, PNODE v);
extern int
is_vertex_num_hyperlinks_bigger(PNODE u,PNODE v);
extern int
num_ref_objects_out(PNODE v);
extern int
is_vertex_num_ref_objects_lower(PNODE u, PNODE v);
extern int
is_vertex_num_ref_objects_bigger(PNODE u,PNODE v);


extern void 
randomized_quicksort(GPTR *A, int p, int r,int (*isl)(), int (*isb)());

extern GPTR
randomized_select(GPTR *A, int p, int r, int i,int (*isl)(), int (*isb)());


#endif






