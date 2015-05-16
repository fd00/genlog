#ifndef GR_WINDOW_H
#define GR_WINDOW_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#ifdef XGR
void
redraw_vertex_edges(GraphFrame *gf,GC gc,struct pt *v,Boolean erase);
void
move_vertex_draw(GraphFrame *cgr,struct pt *ov, struct pr_pos loc);
void 
Draw_all_edges(GraphFrame *gf);
void 
Draw_all_vertices(GraphFrame *gf);
void 
refresh_graph_window(GraphFrame *gf); /*public*/
int 
line_is_in_box(int x1, int y1, int x2, int y2, int w, int h);
int 
point_is_in_box(int x1, int y1, int w, int h);

#endif

#endif

