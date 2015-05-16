#ifndef GR_GRAPH_H
#define GR_GRAPH_H
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#ifdef XGR
#include <Xm/DrawingA.h>
#endif

#ifndef XGR
typedef unsigned short      Dimension;
typedef char            Boolean;
typedef char *          String;
#define False 0
#define True 1
#endif

#include <math.h>

#include "hash_table.h"
#include "llist.h"
#include "alist.h"

#define GR_WIDTH 1000    /* arbitrary width and height values */
#define GR_HEIGHT 1000

#define GR_STANDARD_FORMAT 0
#define GR_STRUCTURAL_FORMAT 1

#define GR_MAX_LENGTH 8000

#define GR_MWIDTH 700
#define GR_MHEIGHT 700

#define POS_NODE -1
#define NAME_NODE -2
#define MEMORY_NODE -3
#define MEMORY_EDGE -1

#define MEMORY_MATRIX -2
#define GRAPH_NOT_CONNECTED -3
#define WINDOW_FULL -1

#define NO_CYCLE -3

#define ERROR_TREE -3
#define ERROR_HIGH_TREE -4

#define ERROR_HIGHT_LEVEL_WINDOW -3
#define ERROR_WIDTH_LEVEL_WINDOW -4

#define ERROR_CONST_FILE -4
#define NO_GRAPH -5
#define NO_FACE -6

#define RESIZE_WINDOW 1

#define NONE            0
#define DIRECTED        1
#define UNDIRECTED      2

#define GRAPHIC_ON       1
#define GRAPHIC_OFF      2

#define MAX_NODES_WIN 50
#define MAX_NODES_FULL_WIN 110
#define MAX_NODES_DOUBLE_WIN 220

#define HOR_MAX_NODES_WIN 20
#define HOR_MAX_NODES_FULL_WIN 40
#define HOR_MAX_NODES_DOUBLE_WIN 80

#define RAN_MAX_NODES_WIN 200
#define RAN_MAX_NODES_FULL_WIN 1000
#define RAN_MAX_NODES_DOUBLE_WIN 2000

#define BOX 'b' /* shapes of nodes */
#define CIRCULAR 'c'
#define VER_ELLIPSIS 'v'
#define HOR_ELLIPSIS 'h'
#define RHOMBUS 'r'
#define TRIANGLE 't'
#define PENTAGON 'p'
#define HEXAGON 'x'
#define OCTAGON 'o'

#define GR_BG 0
#define GR_GREEN 1
#define GR_NAVY 2
#define GR_YELLOW 3
#define GR_MAGENTA 4
#define GR_CYAN 5
#define GR_GRAY 6
#define GR_ORANGE 7
#define GR_BLACK 8
#define GR_MAROON 9
#define GR_WHITE 10
#define GR_BLUE 11
#define GR_PINK 12
#define GR_BROWN 13
#define GR_GREY 14
#define GR_LIMEGREEN 15
#define GR_TURQUOISE 16
#define GR_VIOLET 17
#define GR_WHEAT 18
#define GR_PURPLE 19
#define GR_BEIGE 20
#define GR_RED 21


#define HSCALE 20
#define LSCALE -9

#define     PI      3.14159265353279502884
#define INFINITY 99999999
#define MAX_INT_POS 2147483640
#define MAX_INT_NEG -2147483640
#define	EPSILON	 9.0

#define BSIDE 10
#define HSIDE 1.2*BSIDE
#define DSIDE BSIDE*2
#define MDIGITS 10

#define EAST 1
#define NORTH 2
#define WEST 3
#define SOUTH 4


#define DIS_NUM 0
#define DIS_LABEL 1
#define DIS_NO_LABEL 2
#define DIS_SHORT_LABEL 3
#define DIS_SHORT_TITLE 4
#define DIS_TITLE 5

#define DIS_ENUM 0
#define DIS_ELABEL 1
#define DIS_ENO_LABEL 2
#define DIS_ESHORT_LABEL 3
#define DIS_EWEIGHT 4

#define DFS_TREE 0
#define BFS_TREE 1
#define BFS_FV_TREE 2
#define SHORTEST_PATH_TREE 3
#define SHORTEST_PATH_FV_TREE 4

#define     Strcpy(a,b)     (((b)[0]!='\0')?strcpy(a,b):(((a)[0]='\0'),a))

typedef void *GPTR;

/* Location of a Vertex */
struct	pr_pos	{
  int	x, y;
};
 
/* Graph Structure */
typedef struct _frame {
#ifdef XGR
  Pixmap pixmap; /* used to redraw the DrawingArea */
  Widget drawing_a; /* Drawing area */
  GC gc; /* Normal Graphic Context */
  GC pix_erase; /* Graphic Context to erase elements */
  GC pix_high; /* Graphic Context to highlight elements */
  GC pix_color; /* Graphic Context to color elements */
  Font font; /* Font */
#endif
  Dimension w,h; /* Dimensions of the Drawing Area */
  Dimension wp,hp; /* Dimensions of the Pixmap Area */
  Boolean show_vweight; /* Show weight of vertices */
  Boolean show_eweight; /* Shoe weight of Edges */
  Boolean draw; /* Draw in the Drawing Area */
  Boolean pdraw; /* Draw in the Pixmap Area */
  Boolean no_class_edge; /* classify edges or not */
  Boolean drawhighlight;
  char shape; /* General shape of vertices */
  short vsize; /* Dimensions of a vertex */
  short hsize;
  short mtree; /* Tree of Graph (DFS or BFS) */
  int dmode; /* Mode Display label of Node */
  int emode; /* Mode Display label of Edge */
  short scale; /* scale of the graph */
  int nvertex; /* number of last vertex */
  int nvgroup; /* number of last group vertex */
  int count_vertex; /* number of vertices */
  int graph_dir; /* Graph Direction (Directed or Undirected ) */
  int graphic; /* Graphic representation is included or not */
  int num_clusters;
  int num_components;
  struct pt  *cgv; /* current group vertex */
  struct pt  *rootv; /* current root vertex */
  Node_type *nver; /* vertex to move */
  long (*cost_weight)(); /* function for computing cost of edges */
  LNode_type *list_vertices; /* List of Vertices */
  LNode_type * list_sel_vertex; /* List of Selected Vertices */
  LNode_type * list_high_vertex; /* List of Highlighted Vertices */
  LNode_type * attrib_name; /* List of names of attributes of Vertices */
  LNode_type * list_visited_vertex; /* Barycentric drawing */
  LNode_type * the_cycle;/* Barycentric drawing */
  LNode_type * the_rest;/* Barycentric drawing */
  LNode_type * level_zero; /* Tree drawing */
  LNode_type * lauthub; /* List for author hub algorithm */
  LNode_type * lrank; /* List for ranking algorithm */
  short alg_node_edge;
  LNode_type * lsdist; /* List for computing shortest distances */
  LNode_type * lobjects; /* List for including objects in documents */
  Hashtable_type HV; /* Hash table of vertices */
  Hashtable_type HVname; /* Hash table of vertex names */
  Hashtable_type Hedge; /* Hash table of Edges */
} GraphFrame;


#endif



