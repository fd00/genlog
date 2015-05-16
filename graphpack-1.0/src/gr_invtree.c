#include "gr_invtree.h"
#include "gr_structure.h"
#include "gr_operations.h"
#include "gr_bary.h"

static int xTopAdjustment;
static int yTopAdjustment;

static void
get_inv_circle_coordinates(int r, struct pt *v,struct pt *c, int *a, int *b,
			   int maxp);


int 
compute_inv_tree_positions(GraphFrame *gf, struct pt *sel,int RootOrientation)
{
  if(!gf->level_zero)
    gf->level_zero = init_linked_list();

  compute_inv_tree_graph(gf,sel);

  IPositionTree(gf,sel,RootOrientation);
  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  /*print_prelims(gf);*/
  Delete_all_list(gf->level_zero);
  /*gf->level_zero = init_linked_list();*/
  gf->level_zero = NULL;
  return 0;
}
struct pt *
IPARENT(GraphFrame *gf,struct pt *Node)
{
  LNode_type *ptr;
  if(!Node)
    return NULL;

  for(ptr = Node->ledges_out->next ; ptr != Node->ledges_out ; ptr= ptr->next)
    {
      struct edge *e = (struct edge *)(ptr->info);
      if(e->class == TREE_CLASS)
	return (e->to);
    }
  return NULL;
}

struct pt *
IFIRSTCHILD(GraphFrame *gf,struct pt *Node)
{
  LNode_type *ptr;

  if(!Node)
    return NULL;

  for(ptr =Node->ledges_in->back; ptr != Node->ledges_in; ptr=ptr->back)
    {
      struct edge *e = (struct edge *)(ptr->info);
      if(e->class == TREE_CLASS)
	return (e->from);
    }
  return NULL;
}

struct pt *
ILEFTSIBLING(GraphFrame *gf,struct pt *Node)
{
  struct pt *Parent=NULL; 
  struct edge *te=NULL;
  LNode_type *ptr;
  if(!Node)
    return NULL;

  Parent = IPARENT(gf,Node);

  if(!Parent)
    return NULL;

  ptr = Parent->ledges_in->back;
  
  for(;ptr != Parent->ledges_in; ptr = ptr->back)
    {
      te = (struct edge *)ptr->info;
      if(te->class == TREE_CLASS && te->from == Node)
	break;
    }

  if(ptr == Parent->ledges_in)
    return NULL;

  for(;ptr->next != Parent->ledges_in; ptr = ptr->next)
    {
      te = (struct edge *)ptr->next->info;
      if(te->class == TREE_CLASS)
	return (te->from);
    }
  return NULL;
}

struct pt *
IRIGHTSIBLING(GraphFrame *gf,struct pt *Node)
{
  struct pt *Parent=NULL;
  struct edge *te=NULL;
  LNode_type *ptr;

  if(!Node)
    return NULL;

  Parent = IPARENT(gf,Node);

  if(!Parent)
    return NULL;

  ptr = Parent->ledges_in->back;
  for(;ptr != Parent->ledges_in; ptr = ptr->back)
    {
      te = (struct edge *)ptr->info;
      if(te->class == TREE_CLASS && te->from == Node)
	break;
    }

  if(ptr== Parent->ledges_in)
    return NULL;

  for(;ptr->back != Parent->ledges_in; ptr= ptr->back)
    {
      te = (struct edge *)ptr->back->info;
      if(te->class == TREE_CLASS)
	return (te->from);
    }
  return NULL;
}

Boolean
IISLEAF(struct pt *Node)
{
  LNode_type *ptr;
  if(is_empty_list(Node->ledges_in))
    return True;
  for(ptr= Node->ledges_in->back ; ptr != Node->ledges_in; ptr = ptr->back)
    {
      struct edge *e = (struct edge *)ptr->info;
      if(e->class == TREE_CLASS)
	return False;
    }
  return True;
}
Boolean
IHASCHILD(struct pt *Node)
{
  /*if(is_empty_list(Node->ledges_out))
    return False;
  return True;*/
  return (!IISLEAF(Node));
}


Boolean
IPositionTree(GraphFrame *gf,struct pt *Node,int RootOrientation)
{
  if(!Node)
    return True;

  /* Initialize the list of previous nodes at each level */

  IInitPrevNodeList(gf);
  
  /* Do the preliminary positioning with a psotorder walk */

  IFirstWalk(gf,Node, 0);
  
  /*print_prelims(gf);*/

  /* Determine how to adjust all the nodes with respect to
     the location of the root */

  if((RootOrientation == NORTH) || (RootOrientation == SOUTH))
    {
      xTopAdjustment = Node->loc.x - Node->prelim;
      yTopAdjustment = Node->loc.y;
    }
  else if((RootOrientation == WEST) || (RootOrientation == EAST))
    {
      xTopAdjustment = Node->loc.x;
      yTopAdjustment = Node->loc.y + Node->prelim;;
    }

  /* Do the final positioning with a preorder walk */
  
  return ISecondWalk(gf,Node,0,0,RootOrientation);
}

void
IInitPrevNodeList(GraphFrame *gf)
{
  /* Start with the node at level 0 - the apex of the tree */
  LNode_type *TempPtr = gf->level_zero->back;
  while(TempPtr != gf->level_zero)
    {
      TempPtr->info = NULL; /* PREVNODE(Temptr) <- 0 */
      TempPtr = TempPtr->back;
    }
}

struct pt *
IGetPrevNodeAtLevel(GraphFrame *gf, int Level)
{
  /* Start with the node at level 0 - the apex of the tree */
  LNode_type *TempPtr = gf->level_zero->back;
  int i=0;
  while(TempPtr!= gf->level_zero)
    {
      if(i==Level)
	return((struct pt *)TempPtr->info);
      TempPtr = TempPtr->back;
      i++;
    }
  return NULL;
}



void
ISetPrevNodeAtLevel(GraphFrame *gf, int Level,struct pt *Node)
{
  /* Start with the node at level 0 - the apex of the tree */
  LNode_type *TempPtr = gf->level_zero->back;
  int i=0;

  while(TempPtr!= gf->level_zero)
    {
      if(i==Level)
	{
	  /* At this level, replace the existing list */
	  /* element with the passed-in node */
	  TempPtr->info = (char *)Node;
	  return;
	}
      else if(TempPtr->back == gf->level_zero)
	{
	  /* There isn't a list element yet at this level, so */
	  /* add one. The following instructions prepare the  */
	  /* list element at the next level, not at this one  */
	  Insert_linked_list(gf->level_zero,
			     create_new_node_llist((char *)NULL));
	}
      /* Prepare to move to the next level, to look again */
      TempPtr = TempPtr->back;
      i++;
    }
  /* Should only get here is LevelZeroPtr is nil */
  Insert_linked_list(gf->level_zero,
		     create_new_node_llist((char *)Node));
}

void
IFirstWalk(GraphFrame *gf,struct pt *Node, int Level)
{
  struct pt *ls = ILEFTSIBLING(gf,Node);
   int ss = scale_dim(gf->scale,SiblingSeparation);
  /*printf("FW: Node : %s \n", Node->label);*/

  /* Set the pointer to the previous node at this level */
  Node->left_neighbor = IGetPrevNodeAtLevel(gf,Level);
  ISetPrevNodeAtLevel(gf,Level,Node); /* This is now the previous */
  Node->modifier = 0; /* Set the default modifier value */

  if((IISLEAF(Node) || Level == MaxDepth))
    {
      if(ls) /* Node has left sibling */
	{
	  /* Determine the preliminary x-coordinate based on: */
	  /* the preliminary x-coordinate of the left sibling, */
	  /* the separation between sibling nodes, and */
	  /* the mean size of left sibling and current node */
	  Node->prelim = ls->prelim + ss 
	    +IMeanNodeSize(gf,ls, Node);
	}
      else /* No sibling on the left to worry about */
	Node->prelim = 0;
    }
  else 
    {
      /* This Node is not a leaf, so call this procedure */
      /* recursively for each of its offspring */
      struct pt *rrs;
      int Midpoint;
      struct pt *Leftmost; 
      struct pt *Rightmost;

      Rightmost = Leftmost = IFIRSTCHILD(gf,Node);

      IFirstWalk(gf,Leftmost, Level + 1);

      rrs = IRIGHTSIBLING(gf,Rightmost);
      while(rrs)
	{
	  Rightmost = rrs;
	  IFirstWalk(gf,Rightmost,Level +1);
	  rrs = IRIGHTSIBLING(gf,Rightmost);
	}

      Midpoint = (Leftmost->prelim + Rightmost->prelim)/2;

      if(ls)
	{
	  Node->prelim = ls->prelim + ss
	    +IMeanNodeSize(gf,ls,Node);
	  Node->modifier = Node->prelim - Midpoint;
	  IApportion(gf,Node,Level);
	}
      else
	Node->prelim = Midpoint;
    }
}

Boolean
ISecondWalk(GraphFrame *gf, struct pt *Node, int Level, int Modsum,
	   int RootOrientation)
{
  Boolean Result=True;
  int xTemp, yTemp;
  struct pt *rs = IRIGHTSIBLING(gf,Node);
  int ls = scale_dim(gf->scale,LevelSeparation);

  /*printf("SW : %s \n", Node->label);*/

  if(Level <= MaxDepth)
    {
      if(RootOrientation == NORTH)
	{
	  xTemp = xTopAdjustment + Node->prelim + Modsum;
	  yTemp = yTopAdjustment + (Level * ls);
	}
      else if(RootOrientation == SOUTH)
	{
	  xTemp = xTopAdjustment + Node->prelim + Modsum;
	  yTemp = yTopAdjustment - (Level * ls);
	}
      else if(RootOrientation == EAST)
	{
	  xTemp = xTopAdjustment + (Level * ls);
	  yTemp = yTopAdjustment - (Node->prelim + Modsum);
	}
      else if(RootOrientation == WEST)
	{
	  xTemp = xTopAdjustment - (Level * ls);
	  yTemp = yTopAdjustment - (Node->prelim + Modsum);
	}

      /* Check to see that xTemp and yTemp are of the proper */
      /* size for your application */
      if(ICheckExtentsRange(gf,xTemp,yTemp))
	{
	  Node->loc.x = xTemp;
	  Node->loc.y = yTemp;
	  if(IHASCHILD(Node))
	    {
	      /* Apply the Modifier value for this node to */
	      /* all its offspring */
	      Result = ISecondWalk(gf,IFIRSTCHILD(gf,Node),Level+1, 
				  Modsum + Node->modifier,RootOrientation);
	    }
	    
	  if((Result == True) && (rs != NULL))
	    Result = ISecondWalk(gf,rs, Level, Modsum,RootOrientation);
	      
	}
      else
	/* continuing would but the tree outside of the drawable
	   extents range */
	Result = False;
    }
  else /* We are at a level deeper than what we want to draw */
    Result = True;

  return Result;
}

void
IApportion(GraphFrame *gf,struct pt *Node, int Level)
{
  struct pt *Leftmost = IFIRSTCHILD(gf,Node);
  struct pt *Neighbor = Leftmost->left_neighbor;
  struct pt *AncestorLeftmost, *AncestorNeighbor;
  int CompareDepth = 1;
  int DepthToStop = MaxDepth - Level;

  while((Leftmost != NULL) && (Neighbor != NULL) 
	&& (CompareDepth <= DepthToStop))
    {
      /* Compute the location of Leftmost and where it should */
      /* be with respect to Neighbor */
      int LeftModsum = 0;
      int RightModsum = 0;
      int i,MoveDistance;
      AncestorLeftmost = Leftmost;
      AncestorNeighbor = Neighbor;
      for( i = 0; i< CompareDepth ; i++)
	{
	  AncestorLeftmost = IPARENT(gf,AncestorLeftmost);
	  AncestorNeighbor = IPARENT(gf,AncestorNeighbor);
	  RightModsum += AncestorLeftmost->modifier;
	  LeftModsum += AncestorNeighbor->modifier;
	}
      /* Find the MoveDistance, and apply it to Node's subtree */
      /* Add appropriate portions to smaller interior subtrees */
      MoveDistance = (Neighbor->prelim + LeftModsum + 
		      scale_dim(gf->scale,SubtreeSeparation) +
		      IMeanNodeSize(gf,Leftmost, Neighbor)) - 
	             (Leftmost->prelim + RightModsum);
      if(MoveDistance > 0)
	{
	  /* Count interior sibling subtrees in LeftSiblings */
	  struct pt *TempPtr = Node;
	  int LeftSiblings = 0;
	  while((TempPtr != NULL) && (TempPtr != AncestorNeighbor))
	    {
	      LeftSiblings++;
	      TempPtr = ILEFTSIBLING(gf,TempPtr);
	    }
	  if(TempPtr != NULL)
	    {
	      /* Apply portions to appropriate leftsibling subtrees */
	      int Portion = MoveDistance/LeftSiblings;
	      TempPtr = Node;
	      while(TempPtr != AncestorNeighbor)
		{
		  TempPtr->prelim += MoveDistance;
		  TempPtr->modifier += MoveDistance;
		  MoveDistance -= Portion;
		  TempPtr = ILEFTSIBLING(gf,TempPtr);
		}
	    }
	  else
	    /* Don't need to move anything -- it needs to */
	    /* be done by an ancestor because */
	    /* AncestorNeighbor and AncestorLeftmost are */
	    /* not siblings of each other */
	    return;
	}
      /* Determine the leftmost descendant of Node at the next */
      /* lover level to compare its positioning against that of */
      /* its Neighbor */
      CompareDepth++;
      if(IISLEAF(Leftmost))
	Leftmost = IGetLeftMost(gf,Node, 0, CompareDepth);
      else
	Leftmost = IFIRSTCHILD(gf,Leftmost);

      /* This is missing in the original algorithm */

      if(Leftmost)
	Neighbor = Leftmost->left_neighbor; 
      else
	Neighbor = NULL;
    }
}

struct pt *
IGetLeftMost(GraphFrame *gf,struct pt * Node, int Level, int Depth)
{
  if(Level >= Depth)
    return Node;
  else if (IISLEAF(Node)) 
    return NULL;
  else
    {
      struct pt *Rightmost = IFIRSTCHILD(gf,Node);
      struct pt *Leftmost = IGetLeftMost(gf,Rightmost, Level + 1, Depth);
      struct pt *rs = IRIGHTSIBLING(gf,Rightmost);
      /* Do a postorder walk of the subtree below Node */
      while((Leftmost == NULL) && (rs != NULL))
	{
	  Rightmost = rs;
	  Leftmost = IGetLeftMost(gf,Rightmost, Level + 1, Depth);
	  rs = IRIGHTSIBLING(gf,Rightmost);
	}
      return Leftmost;
    }
}


int
IMeanNodeSize(GraphFrame *gf,struct pt *LeftNode, struct pt * RightNode)
{
  /*int NodeSize = 0;
  if(LeftNode!=NULL)
    NodeSize += RIGHTSIZE(LeftNode);
  if(RightNode!=NULL)
    NodeSize += LEFTSIZE(RightNode);
  return NodeSize;*/
  return (gf->vsize *2);
}

Boolean
ICheckExtentsRange(GraphFrame *gf, int xValue, int yValue)
{
  if(xValue >= 0 && xValue <= gf->w && yValue >= 0 && yValue <= gf->h)
    return True;
  else
    {
      /*printf("Out of range\n");*/
      return True;
    }
}


void
Iprint_prelims(GraphFrame *gf)
{
  LNode_type *ptr;
  printf("\nGRAPH\n");

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *ov = (struct pt *)ptr->back->info;
      printf("Node: %s, x: %d, prelim %d, modifier %d \n", 
	     ov->label,ov->loc.x, ov->prelim, ov->modifier);
    }
}

void 
inv_dfs(struct pt *v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->from;
      if( tv->mark == False)
	{
	  e->class = TREE_CLASS;
	  inv_dfs(tv);
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else
	e->class = BACK_CLASS;
    }
  v->pick = True;
}

void
compute_inv_tree_graph(GraphFrame *gf, struct pt *v)
{
  reset_mark_pick_vertices(gf);
  reset_edges(gf);
  if(gf->mtree == DFS_TREE)
    {
      if(gf->graph_dir == DIRECTED)
	inv_dfs(v);
    }
  else if(gf->mtree == BFS_TREE)
    inv_bfs(gf,v);
}

int
inv_bfs(GraphFrame *gf, struct pt *root)
{
  /* White - no mark, no pick
     Gray  - mark , no pick
     Black - mark, pick */
  LNode_type *Q = init_queue();
  int max;

  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  root->mark = True; /* root is Gray */
  root->level = 0;
  Enqueue(Q,(char *)root);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_in->back; poed != u->ledges_in; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->info;
	      struct pt * tv = e->from;
	      if(!(tv->mark || tv->pick)) /* Is vertex white? */
		{
		  tv->mark = True; /* Vertex is gray */
		  tv->level = u->level + 1;
		  e->class = TREE_CLASS;
		  Enqueue(Q,(char *)tv);
		}
	      else if(tv->mark)
		{
		  if(tv->pick)
		      e->class = BACK_CLASS;
		  else
		      e->class = FORWARD_CLASS;
		}
	    }
	}
      else
	return -1;

      Dequeue(Q);
      u->pick = True; /* Vertex u is Black */
      max = u->level;
    }
  Delete_linked_list(Q);
  return max;
}

int
compute_radial_inv_tree_positions(GraphFrame *gf, struct pt *sel)
{
  if(!gf->level_zero)
    gf->level_zero = init_linked_list();

  compute_inv_tree_graph(gf,sel);

  IPositionTree(gf,sel,NORTH);

  compute_new_inv_radial_positions(gf, sel);

  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  /*print_prelims(gf);*/
  Delete_all_list(gf->level_zero);
  gf->level_zero = NULL;
  /*gf->level_zero = init_linked_list();*/
  return 0;

}

#define MAX_LEVEL 200

void 
compute_new_inv_radial_positions(GraphFrame *gf, struct pt *root)
{
  int *perimeters,*radius;
  PNODE *lnodes;
  int i,max;
  int pmax = 0;
  int ls = scale_dim(gf->scale, LevelSeparation);
  int ss = scale_dim(gf->scale, SubtreeSeparation);
  LNode_type *Q = init_queue();

  perimeters = (int *)calloc(sizeof(int),MAX_LEVEL);
  if(!perimeters) {
      fprintf(stderr,"Out of Memory compute_new_inv_radial_positions\n");
      abort();
  }

  radius = (int *)calloc(sizeof(int),MAX_LEVEL);
  if(!radius) {
      fprintf(stderr,"Out of Memory compute_new_inv_radial_positions\n");
      abort();
  }

  lnodes = (PNODE *)calloc(sizeof(PNODE), MAX_LEVEL);
  if(!lnodes) {
      fprintf(stderr,"Out of Memory compute_new_inv_radial_positions\n");
      abort();
  }

  root->level = 0;
  
  Enqueue(Q,(char *)root);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);
      
      lnodes[u->level]=u;
      if(u->left_neighbor)
	{
	  perimeters[u->level]+=(int)abs(u->left_neighbor->loc.x - u->loc.x);

	  lnodes[u->level]=u;
	}
      
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_in->back; poed != u->ledges_in; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->info;
	      struct pt * tv = e->from;

	      if(e->class == TREE_CLASS)
		{
		  tv->level = u->level + 1;
		  Enqueue(Q,(char *)tv);
		}
	    }
	}
      else
	{
	  free(perimeters); perimeters = NULL;
	  free(radius); radius = NULL;
	  Delete_linked_list(Q); Q = NULL;
	  return;
	}

      Dequeue(Q);
      max = u->level;
    }

  for(i=1;i<= max; i++)
    {
      int lr = (lnodes[i])->loc.x - root->loc.x;
      int ll = perimeters[i] - lr;
      if(lr  > ll )
	perimeters[i] = 2 * lr;
      else
	perimeters[i] = 2 * ll;

      if(perimeters[i] > pmax)
	pmax = perimeters[i];
    }

  for(i=1;i<= max; i++)
    {
      struct pt *cnode = lnodes[i];
      int maxp = pmax/2 + (int)ss;
      int rad = (int)(((double)perimeters[i]+(double)ss)/(double)(2*PI));
      int rad2 = radius[i-1]+ls;

      radius[i] = rad > rad2 ? rad : rad2;

      /*printf("Level %d per = %d rad = %d", i, perimeters[i],radius[i]);
	printf(" %d\n", lnodes[i] ? lnodes[i]->num : -1);*/

      while(cnode!= NULL)
	{
	  int a, b;
	  get_inv_circle_coordinates(radius[i], cnode,root,&a, &b, maxp);
	  cnode->loc.x = root->loc.x +a;
	  cnode->loc.y = root->loc.y +b;
	  cnode = cnode->left_neighbor;
	}
    }
  
  free(perimeters); perimeters=NULL;
  free(radius); radius=NULL;
  Delete_linked_list(Q); Q=NULL;
  return;
}

static void
get_inv_circle_coordinates(int r, struct pt *v, struct pt *c, int *a, int *b, int maxp)
{
  double tn = (double)(v->loc.x - c->loc.x) * PI / (double)maxp;
  *a = (int)((double)r*sin(tn));
  *b = (int)((double)r*cos(tn));
}

