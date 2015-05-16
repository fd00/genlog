#ifndef GR_TREE_H
#define GR_TREE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#include "gr_graph_fun.h"

#define LevelSeparation 4*DSIDE
#define MaxDepth INFINITY
#define SiblingSeparation DSIDE
#define SubtreeSeparation 4*DSIDE

int 
compute_tree_positions(GraphFrame *gf, struct pt *sel,int RootOrientation);

struct pt *
PARENT(GraphFrame *gf,struct pt *Node);

struct pt *
FIRSTCHILD(GraphFrame *gf,struct pt *Node);

struct pt *
LEFTSIBLING(GraphFrame *gf,struct pt *Node);

struct pt *
RIGHTSIBLING(GraphFrame *gf,struct pt *Node);

Boolean
ISLEAF(struct pt *Node);

Boolean
HASCHILD(struct pt *Node);


void
SetPrevNodeAtLevel(GraphFrame *gf, int Level,struct pt *Node);

struct pt *
GetPrevNodeAtLevel(GraphFrame *gf, int Level);

void
InitPrevNodeList(GraphFrame *gf);

Boolean
PositionTree(GraphFrame *gf,struct pt *Node,int RootOrientation);

Boolean
CheckExtentsRange(GraphFrame *gf, int xValue, int yValue);

int
MeanNodeSize(GraphFrame *gf,struct pt *LeftNode, struct pt * RightNode);

struct pt *
GetLeftMost(GraphFrame *gf,struct pt * Node, int Level, int Depth);

void
Apportion(GraphFrame *gf,struct pt *Node, int Level);

Boolean
SecondWalk(GraphFrame *gf, struct pt *Node, int Level, int Modsum,
	   int RootOrientation);

void
FirstWalk(GraphFrame *gf,struct pt *Node, int Level);

void
print_prelims(GraphFrame *gf);

void 
dfs(struct pt *v);
void 
dfs_undirected(struct pt *v);

int
compute_tree_documents(GraphFrame *gf, int RootOrientation);

void 
dfs_highlight(struct pt *v);

PNODE
UPARENT(GraphFrame *gf,PNODE Node);

PNODE 
UFIRSTCHILD(GraphFrame *gf,PNODE Node);

PNODE 
ULEFTSIBLING(GraphFrame *gf,PNODE Node);

PNODE 
URIGHTSIBLING(GraphFrame *gf,PNODE Node);

Boolean
UISLEAF(GraphFrame *gf, PNODE Node);

Boolean
UHASCHILD(GraphFrame *gf, PNODE Node);

Boolean
UPositionTree(GraphFrame *gf,PNODE Node,int RootOrientation);

void
UFirstWalk(GraphFrame *gf,PNODE Node, int Level);

Boolean
USecondWalk(GraphFrame *gf, PNODE Node, int Level, int Modsum,
	    int RootOrientation);
void
UApportion(GraphFrame *gf,PNODE Node, int Level);
PNODE 
UGetLeftMost(GraphFrame *gf,PNODE  Node, int Level, int Depth);

#endif



