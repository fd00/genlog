#ifndef GR_RADTREE_H
#define GR_RADTREE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"
void 
compute_new_radial_positions(GraphFrame *gf, struct pt *root);

int
compute_radial_tree_positions(GraphFrame *gf, struct pt *sel);

#endif


