#ifndef GR_INVTREE_H
#define GR_INVTREE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"
#include "queue.h"

#define LevelSeparation 4*DSIDE
#define MaxDepth INFINITY
#define SiblingSeparation DSIDE
#define SubtreeSeparation 4*DSIDE

int 
compute_inv_tree_positions(GraphFrame *gf, struct pt *sel,int RootOrientation);
struct pt *
IPARENT(GraphFrame *gf,struct pt *Node);
struct pt *
IFIRSTCHILD(GraphFrame *gf,struct pt *Node);
struct pt *
ILEFTSIBLING(GraphFrame *gf,struct pt *Node);
struct pt *
IRIGHTSIBLING(GraphFrame *gf,struct pt *Node);
Boolean
IISLEAF(struct pt *Node);
Boolean
IHASCHILD(struct pt *Node);
Boolean
IPositionTree(GraphFrame *gf,struct pt *Node,int RootOrientation);
void
IInitPrevNodeList(GraphFrame *gf);
struct pt *
IGetPrevNodeAtLevel(GraphFrame *gf, int Level);
void
ISetPrevNodeAtLevel(GraphFrame *gf, int Level,struct pt *Node);
void
IFirstWalk(GraphFrame *gf,struct pt *Node, int Level);
Boolean
ISecondWalk(GraphFrame *gf, struct pt *Node, int Level, int Modsum,
	   int RootOrientation);
void
IApportion(GraphFrame *gf,struct pt *Node, int Level);
struct pt *
IGetLeftMost(GraphFrame *gf,struct pt * Node, int Level, int Depth);
int
IMeanNodeSize(GraphFrame *gf,struct pt *LeftNode, struct pt * RightNode);
Boolean
ICheckExtentsRange(GraphFrame *gf, int xValue, int yValue);
void
Iprint_prelims(GraphFrame *gf);
void 
inv_dfs(struct pt *v);
void
compute_inv_tree_graph(GraphFrame *gf, struct pt *v);
int
inv_bfs(GraphFrame *gf, struct pt *root);
int
compute_radial_inv_tree_positions(GraphFrame *gf, struct pt *sel);
void 
compute_new_inv_radial_positions(GraphFrame *gf, struct pt *root);

#endif



