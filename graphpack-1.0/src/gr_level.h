#ifndef GR_LEVEL_H
#define GR_LEVEL_H
#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

int
compute_level_positions(GraphFrame *gf);
int
Assign_level_nodes(GraphFrame *gf);
void 
make_levels(GraphFrame *gf);
int
place_nodes(GraphFrame *gf);
void
delete_levels( LNode_type ** levels);
int
calculate_hight_level(GraphFrame *gf, double *L,  double *ystep);
int
calculate_width_level(GraphFrame *gf, double *L,  double *xstep,int levelcnt);

#endif
