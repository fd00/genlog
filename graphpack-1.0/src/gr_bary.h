#ifndef GR_BARY_H
#define GR_BARY_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"
int 
compute_bary_positions(GraphFrame *gf, int cycle_len);
void 
find_cycle(GraphFrame *gf, struct pt *v, int level, int cycle_len);
void
save_new_cycle(GraphFrame *gf,struct pt *begin, struct pt *end, int cycle_len);
void
reset_mark_vertices(GraphFrame *gf);
void
reset_mark_pick_vertices(GraphFrame *gf);
void
reset_level_vertices(GraphFrame *gf);

void
get_cycle(GraphFrame *gf, int cycle_len);

int
circularize(GraphFrame *gf, LNode_type *the_cycle, int node_cnt);
int
calculate_window_size(GraphFrame *gf, Dimension *X, Dimension *Y);
void 
get_rest(GraphFrame *gf);
int
layout_rest(GraphFrame *gf, LNode_type *the_rest, int rest_cnt);
void
solve_gauss_jordan(double **M, int n, int m);

void
get_sel_cycle(GraphFrame *gf);

int 
compute_sel_bary_positions(GraphFrame *gf);

#endif
