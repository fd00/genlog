#ifndef GR_RADIAL_H
#define GR_RADIAL_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

#define CONS_RADIUS 10

typedef struct level_info {
  int num_nodes;
  int max_node;
  int level;
  int radius;
} LEVEL_INFO;

int 
compute_radial_positions(GraphFrame *gf, struct pt *sel);

int
count_not_marked(LNode_type *ledges);
int
count_not_marked_undir(struct pt *par, LNode_type *ledges);

void
sub_embed(GraphFrame *gf,double total, int r, struct pt *v, 
	  long precount,int x, int y, LNode_type *l);
void
position_vertex_radial(GraphFrame *gf, struct pt *v, int r, double total,
		       int x, int y);
int
bfs(GraphFrame *gf, struct pt *root);
void
reset_edges(GraphFrame *gf);
void
add_num_child(LNode_type *num_node_level, int cchild, int level);
int
bfs_count_nodes(GraphFrame *gf, struct pt *sel,LNode_type * num_node_level);
void
print_level_info(LNode_type *num_node_level);
int
find_optimal_radius(LNode_type *num_node_level, int level);
void
calculate_radius_level(GraphFrame *gf, LNode_type *num_node_level);
void
delete_level_info(LNode_type *num_node_level);
int
complete_bfs_directed(GraphFrame *gf, struct pt *root);

int
bfs_first_variation(GraphFrame *gf, struct pt *root);

Boolean
there_are_no_pick_nodes(GraphFrame *gf);

int
bfs_highlight(GraphFrame *gf, struct pt *root);

#endif


