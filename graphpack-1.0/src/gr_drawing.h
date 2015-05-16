#ifndef GR_DRAWING_H
#define GR_DRAWING_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

int
compute_circular_positions(GraphFrame *gf);
int
compute_horizontal_positions(GraphFrame *gf);
int
compute_diagonal_positions(GraphFrame *gf);
int
compute_random_positions(GraphFrame *gf);
int
compute_linear_positions(GraphFrame *gf);

void
print_matrix(double **a, int noc, int moc); /*private*/
void
create_adjacency_matrix(int **a, GraphFrame *gf,int noc);/*private*/
int **
floyd(GraphFrame *gf,int noc);/*private*/
int
compute_pretty_positions(GraphFrame *gf, Boolean initial);

int 
compute_force_directed_positions(GraphFrame *gf);

int
compute_layered_pretty_positions(GraphFrame *gf);
int
compute_radial_pretty_positions(GraphFrame *gf);

double 
delta_m(GraphFrame *gf,double **k,int **l, int m,int noc);/*private*/
void
new_coord(int m, double **k, int **l, GraphFrame *gf,int noc);/*private*/
int
maxm(GraphFrame *gf,double **k, int **l,int noc);/*private*/

void
create_adjacency_matrix_highlight(int **a, GraphFrame *gf,int noc); /*private*/
void 
enumerate_vertices(GraphFrame *gf);
struct pt *
get_vertex(int n, GraphFrame *gf);/*private*/

void 
delete_matrix_int(int **A, int m);/*private*/
void 
delete_matrix_double(double **A, int m);/*private*/

Boolean
find_empty_spot(struct pt *v, GraphFrame *gf);/*private*/

Boolean
find_near_spot(struct pt *v, GraphFrame *gf);
#endif
