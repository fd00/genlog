#ifndef GR_RANK_H
#define GR_RANK_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

void
compute_rank(GraphFrame *gf, int maxiter);

int
select_big_rank(GraphFrame *gf, int mah);

#endif
