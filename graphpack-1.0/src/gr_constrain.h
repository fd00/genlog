#ifndef GR_CONSTRAIN_H
#define GR_CONSTRAIN_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

typedef struct edge_con{
  struct edge *e;
  int lface;
  int unv;
  Boolean cw;
  Boolean touch;
} Edge_con;

#define NEAST 0
#define NWEST 2
#define SEAST 1
#define SWEST 3

#define CON_UNIT 6

int 
compute_geometry_positions(GraphFrame *gf, struct pt *v, char *sconst);
int
parse_constraints(char *sconst,int **ncons,int n);
int
parse_line_constr(char *line, int *ncons);
void
set_constraints_nodes(GraphFrame *gf, int **ncons, int n);
int *
find_line_constr(struct pt *v, int **ncons, int n);
int 
layout_graph_constrains(GraphFrame *gf,struct pt *v);
void
print_vertex_const(struct pt *v);
Boolean
is_const_ok(struct pt *v);
void
report_nodes_constraints(GraphFrame *gf);
void
place_rest_vertices(GraphFrame *gf);
int
find_first_face(GraphFrame *gf, LNode_type *Q,struct pt *v);
void 
queue_loop(GraphFrame *gf,LNode_type *Q);
void
place_vertices_face(GraphFrame *gf, Edge_con * ec, LNode_type *Q);
void
place_triangle(GraphFrame *gf, Edge_con * ec, LNode_type *Q);
int
compute_triangle(struct pt *v, struct pt *v1, struct pt *v2,int cw,int d);

void
try_place_two_vertices(GraphFrame *gf, Edge_con * ec, LNode_type *Q);
void
place_square(GraphFrame *gf, Edge_con * ec, LNode_type *Q);
void
compute_square(struct pt *v1, struct pt *v2, struct pt *v3, struct pt *v4, 
	       int cw,int d);
void
place_rhombus(GraphFrame *gf, Edge_con * ec, LNode_type *Q);
Boolean
find_route_4(GraphFrame *gf,struct pt *v, LNode_type *troute);
Boolean
find_route_3(GraphFrame *gf,struct pt *v, LNode_type *troute);
struct pt *
next_vertex_const(struct pt *par, struct pt *v);
struct pt *
back_vertex_const(struct pt *par, struct pt *v);
void
place_route(GraphFrame *gf,LNode_type *troute);
void
save_edges_queue(GraphFrame *gf, LNode_type *troute, LNode_type *Q);
Boolean
recompute_faces_queue(GraphFrame *gf,LNode_type *Q);
Boolean
recompute_unplaced_vertex(GraphFrame *gf, Edge_con *ec);
void
set_edge_con(GraphFrame *gf,Edge_con *ec, struct edge *e);
void
compute_faces_vertex(GraphFrame *gf, Edge_con *ec, struct edge *e);

void
reorder_queue(GraphFrame *gf,LNode_type *Q);
void
Enqueue_prior(LNode_type *Q, Edge_con *ec);
void
print_queue(GraphFrame *gf, LNode_type *Q);
void 
print_ec(Edge_con *ec);

struct pt *
find_unplaced_vertex(GraphFrame *gf, Edge_con * ec);
void
find_two_unplaced_vertex(GraphFrame *gf, Edge_con * ec,struct pt **v3,
			 struct pt **v4);
double
cal_angle(struct pt *v,struct pt *r);
double
distance(double x1, double y1, double x2, double y2);

void
initialize_const(int **ncons, int n);

void
satisfy_const(GraphFrame *gf, struct pt *v1, struct pt *v2, struct pt *v,
	      int dire);
void
satisfy_const_rh(GraphFrame *gf, struct pt *v1, struct pt *v2, struct pt *v3,
		 struct pt *v,int dire);
Boolean
clock_wise(double a, double b,double c);
Boolean
cc_clock_wise(double a, double b,double c);
Boolean
clock_wise_4(double a, double b,double c,double d);
Boolean
cc_clock_wise_4(double a, double b,double c,double d);
double
ang_sup(double angle);
Boolean
is_increased(double ang);
double
min_d(double a, double b);
double
max_d(double a, double b);
double
average_angle(double a, double b);
void
find_bounding_box(struct pt *v1, struct pt *v2,struct pt *v3,struct pt *v4,
		  int *minx, int *miny, int *maxx, int *maxy);

void
delete_constrains_vertex(GraphFrame *gf);
void
delete_constrains_array(int **ncons,int n);

#endif



