#ifndef GR_ALGORITHMS_H
#define GR_ALGORITHMS_H
#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

void 
delete_short_distance_list(GraphFrame *gf);

void
Dijkstra(GraphFrame *gf, PNODE s,long (*comp_weight)());


long
select_shortest_path(GraphFrame *gf, PNODE s, PNODE t,long (*comp_weight)());

long
select_shortest_path_to(GraphFrame *gf, PNODE t,long (*comp_weight)());

int
draw_radial_tree_from_shortest_path(GraphFrame *gf, PNODE sel,
				    long (*comp_weight)());
void
print_shortest_path(PNODE t);

long
get_value_shortest_path(PNODE t);

long
get_number_edges_shortest_path(PNODE t);

void
apply_function_to_shortest_path(PNODE t, void (*funct)());

void
pick_objects_in_path(PNODE v);

int 
select_biggest_shortest_path(GraphFrame *gf);


int
draw_hierarchical_radial_tree_graph(GraphFrame *gf, PNODE sel);

int
compute_tree_from_shortest_path(GraphFrame *gf, PNODE sel,
				long (*comp_weight)());

LNode_type *
classify_hierarchical_edges(GraphFrame *gf, PNODE sel);

double
compute_cluster_coefficient(GraphFrame *gf);

#endif

