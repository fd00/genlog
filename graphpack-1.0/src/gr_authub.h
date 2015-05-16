#ifndef GR_AUTHUB_H
#define GR_AUTHUB_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

void
compute_author_hub(GraphFrame *gf, int maxiter,int sel);
void
normalize_x_and_y(GraphFrame *gf);
void
normalize2_x_and_y(GraphFrame *gf);
void
normalize3_x_and_y(GraphFrame *gf);

void
apply_O_operation(GraphFrame *gf);

void
apply_I_operation(GraphFrame *gf);

void
initialize_author_hub_list(GraphFrame *gf);

void 
delete_author_hub_list(GraphFrame *gf);
int
how_many_nodes_are_big_auth(GraphFrame *gf);
int
how_many_nodes_are_big_hub(GraphFrame *gf);
int
select_big_auth(GraphFrame *gf, int mah);
int
select_big_hub(GraphFrame *gf, int mah);

void
initialize_author_hub_list_edges(GraphFrame *gf);
void
apply_I_operation_edges(GraphFrame *gf);
void
apply_O_operation_edges(GraphFrame *gf);
void 
print_values_hub_edges(GraphFrame *gf);
void
compute_author_hub_edges(GraphFrame *gf, int maxiter, int sel);
int
select_big_auth_edges(GraphFrame *gf, int mah);
int
select_big_hub_edges(GraphFrame *gf, int mah);
int
select_big_nodes_from_hub_edges(GraphFrame *gf, int mah);

int
select_biggest_in_degree(GraphFrame *gf, int nbigin);

int
select_biggest_out_degree(GraphFrame *gf, int nbigout);

#endif





