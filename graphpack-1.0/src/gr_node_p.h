#ifndef GR_NODE_P_H
#define GR_NODE_P_H

#ifdef XGR
void
draw_box(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);

void
draw_circular(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);

void
draw_h_ellipsis(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);

void
draw_v_ellipsis(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);

void
draw_rhombus(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);

void
draw_triangle(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);
void
draw_octagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);
void
draw_hexagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);
void
draw_pentagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf);
#endif

#endif
