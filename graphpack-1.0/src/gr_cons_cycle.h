#ifndef GR_CONS_CYCLE_H
#define GR_CONS_CYCLE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

int
compute_geometry_positions_cycle(GraphFrame *gf, struct pt *v, struct pt *v2,
			   char *sconst);
int 
layout_graph_cons_cycle(GraphFrame *gf,struct pt *v,struct pt *v2);

int
find_first_face_cycle(GraphFrame *gf, LNode_type *Q,struct pt *v,
		      struct pt *v2);
Boolean
find_face_cycle(GraphFrame *gf,struct pt *v,struct pt *v2,LNode_type *troute);

void
place_route_cycle(GraphFrame *gf, LNode_type *troute);



#endif
