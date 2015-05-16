#include "gr_tree.h"
#include "gr_structure.h"
#include "gr_operations.h"
#include "gr_bary.h"
#include "gr_radial.h"
#include "gr_util.h"

static int xTopAdjustment;
static int yTopAdjustment;
static PNODE rootnode = NULL;

int 
compute_tree_positions(GraphFrame *gf, struct pt *sel,int RootOrientation)
{
  if(!gf->level_zero)
    gf->level_zero = init_linked_list();


  if(!gf->drawhighlight)
      compute_tree_graph(gf,sel);
  else
      compute_tree_graph_highlight(gf, sel);

  if(gf->graph_dir == UNDIRECTED) 
  {
      sel->context = NULL; /* parent of root is NULL */
      UPositionTree(gf,sel,RootOrientation);
  }
  else
      PositionTree(gf,sel,RootOrientation);
  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  /*print_prelims(gf);*/
  Delete_all_list(gf->level_zero);
  /*gf->level_zero = init_linked_list();*/
  gf->level_zero = NULL;
  return 0;
}

struct pt *
PARENT(GraphFrame *gf,struct pt *Node)
{
  LNode_type *ptr;
  if(!Node)
    return NULL;

  if(Node == rootnode)
      return NULL;

  for(ptr = Node->ledges_in->next ; ptr != Node->ledges_in ; ptr= ptr->next)
    {
      struct edge *e = (struct edge *)(ptr->info);
      if(e->class == TREE_CLASS)
	return (e->from);
    }
  return NULL;
}

struct pt *
FIRSTCHILD(GraphFrame *gf,struct pt *Node)
{
  LNode_type *ptr;

  if(!Node)
    return NULL;

  for(ptr =Node->ledges_out->back; ptr != Node->ledges_out; ptr=ptr->back)
    {
      struct edge *e = (struct edge *)(ptr->info);
      if(e->class == TREE_CLASS)
	return (e->to);
    }
  return NULL;
}

struct pt *
LEFTSIBLING(GraphFrame *gf,struct pt *Node)
{
  struct pt *Parent=NULL; 
  struct edge *te=NULL;
  LNode_type *ptr;
  if(!Node)
    return NULL;

  Parent = PARENT(gf,Node);

  if(!Parent)
    return NULL;

  ptr = Parent->ledges_out->back;
  
  for(;ptr != Parent->ledges_out; ptr = ptr->back)
    {
      te = (struct edge *)ptr->info;
      if(te->class == TREE_CLASS && te->to == Node)
	break;
    }

  if(ptr == Parent->ledges_out)
    return NULL;

  for(;ptr->next != Parent->ledges_out; ptr = ptr->next)
    {
      te = (struct edge *)ptr->next->info;
      if(te->class == TREE_CLASS)
	return (te->to);
    }
  return NULL;
}

struct pt *
RIGHTSIBLING(GraphFrame *gf,struct pt *Node)
{
  struct pt *Parent=NULL;
  struct edge *te=NULL;
  LNode_type *ptr;

  if(!Node)
    return NULL;

  Parent = PARENT(gf,Node);

  if(!Parent)
    return NULL;

  ptr = Parent->ledges_out->back;
  for(;ptr != Parent->ledges_out; ptr = ptr->back)
    {
      te = (struct edge *)ptr->info;
      if(te->class == TREE_CLASS && te->to == Node)
	break;
    }

  if(ptr== Parent->ledges_out)
    return NULL;

  for(;ptr->back != Parent->ledges_out; ptr= ptr->back)
    {
      te = (struct edge *)ptr->back->info;
      if(te->class == TREE_CLASS)
	return (te->to);
    }
  return NULL;
}

Boolean
ISLEAF(struct pt *Node)
{
  LNode_type *ptr;
  if(!Node)
      return True;
  if(is_empty_list(Node->ledges_out))
    return True;
  for(ptr= Node->ledges_out->back ; ptr != Node->ledges_out; ptr = ptr->back)
    {
      struct edge *e = (struct edge *)ptr->info;
      if(e->class == TREE_CLASS)
	return False;
    }
  return True;
}
Boolean
HASCHILD(struct pt *Node)
{
  /*if(is_empty_list(Node->ledges_out))
    return False;
  return True;*/
  return (!ISLEAF(Node));
}


Boolean
PositionTree(GraphFrame *gf,struct pt *Node,int RootOrientation)
{
  if(!Node)
    return True;

  rootnode = Node;

  /* Initialize the list of previous nodes at each level */

  InitPrevNodeList(gf);
  
  /* Do the preliminary positioning with a psotorder walk */

  FirstWalk(gf,Node, 0);
  
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
      yTopAdjustment = Node->loc.y + Node->prelim;
    }

  /* Do the final positioning with a preorder walk */
  
  return SecondWalk(gf,Node,0,0,RootOrientation);
}

void
InitPrevNodeList(GraphFrame *gf)
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
GetPrevNodeAtLevel(GraphFrame *gf, int Level)
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
SetPrevNodeAtLevel(GraphFrame *gf, int Level,struct pt *Node)
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
FirstWalk(GraphFrame *gf,struct pt *Node, int Level)
{
  struct pt *ls = LEFTSIBLING(gf,Node);
   int ss = scale_dim(gf->scale,SiblingSeparation);
  /*printf("FW: Node : %s \n", Node->label);*/

  /* Set the pointer to the previous node at this level */
  Node->left_neighbor = GetPrevNodeAtLevel(gf,Level);
  SetPrevNodeAtLevel(gf,Level,Node); /* This is now the previous */
  Node->modifier = 0; /* Set the default modifier value */

  if((ISLEAF(Node) || Level == MaxDepth))
    {
      if(ls) /* Node has left sibling */
	{
	  /* Determine the preliminary x-coordinate based on: */
	  /* the preliminary x-coordinate of the left sibling, */
	  /* the separation between sibling nodes, and */
	  /* the mean size of left sibling and current node */
	  Node->prelim = ls->prelim + ss 
	    +MeanNodeSize(gf,ls, Node);
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

      Rightmost = Leftmost = FIRSTCHILD(gf,Node);

      FirstWalk(gf,Leftmost, Level + 1);

      rrs = RIGHTSIBLING(gf,Rightmost);
      while(rrs)
	{
	  Rightmost = rrs;
	  FirstWalk(gf,Rightmost,Level +1);
	  rrs = RIGHTSIBLING(gf,Rightmost);
	}

      Midpoint = (Leftmost->prelim + Rightmost->prelim)/2;

      if(ls)
	{
	  Node->prelim = ls->prelim + ss
	    +MeanNodeSize(gf,ls,Node);
	  Node->modifier = Node->prelim - Midpoint;
	  Apportion(gf,Node,Level);
	}
      else
	Node->prelim = Midpoint;
    }
}

Boolean
SecondWalk(GraphFrame *gf, struct pt *Node, int Level, int Modsum,
	   int RootOrientation)
{
  Boolean Result=True;
  int xTemp, yTemp;
  struct pt *rs = RIGHTSIBLING(gf,Node);
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
      if(CheckExtentsRange(gf,xTemp,yTemp))
	{
	  Node->loc.x = xTemp;
	  Node->loc.y = yTemp;
	  if(HASCHILD(Node))
	    {
	      /* Apply the Modifier value for this node to */
	      /* all its offspring */
	      Result = SecondWalk(gf,FIRSTCHILD(gf,Node),Level+1, 
				  Modsum + Node->modifier,RootOrientation);
	    }
	    
	  if((Result == True) && (rs != NULL))
	    Result = SecondWalk(gf,rs, Level, Modsum,RootOrientation);
	      
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
Apportion(GraphFrame *gf,struct pt *Node, int Level)
{
  struct pt *Leftmost = FIRSTCHILD(gf,Node);
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
	  AncestorLeftmost = PARENT(gf,AncestorLeftmost);
	  AncestorNeighbor = PARENT(gf,AncestorNeighbor);
	  RightModsum += AncestorLeftmost->modifier;
	  LeftModsum += AncestorNeighbor->modifier;
	}
      /* Find the MoveDistance, and apply it to Node's subtree */
      /* Add appropriate portions to smaller interior subtrees */
      MoveDistance = (Neighbor->prelim + LeftModsum + 
		      scale_dim(gf->scale,SubtreeSeparation) +
		      MeanNodeSize(gf,Leftmost, Neighbor)) - 
	             (Leftmost->prelim + RightModsum);
      if(MoveDistance > 0)
	{
	  /* Count interior sibling subtrees in LeftSiblings */
	  struct pt *TempPtr = Node;
	  int LeftSiblings = 0;
	  while((TempPtr != NULL) && (TempPtr != AncestorNeighbor))
	    {
	      LeftSiblings++;
	      TempPtr = LEFTSIBLING(gf,TempPtr);
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
		  TempPtr = LEFTSIBLING(gf,TempPtr);
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
      if(ISLEAF(Leftmost))
	Leftmost = GetLeftMost(gf,Node, 0, CompareDepth);
      else
	Leftmost = FIRSTCHILD(gf,Leftmost);

      /* This is missing in the original algorithm */

      if(Leftmost)
	Neighbor = Leftmost->left_neighbor; 
      else
	Neighbor = NULL;
    }
}

struct pt *
GetLeftMost(GraphFrame *gf,struct pt * Node, int Level, int Depth)
{
  if(Level >= Depth)
    return Node;
  else if (ISLEAF(Node)) 
    return NULL;
  else
    {
      struct pt *Rightmost = FIRSTCHILD(gf,Node);
      struct pt *Leftmost = GetLeftMost(gf,Rightmost, Level + 1, Depth);
      struct pt *rs = RIGHTSIBLING(gf,Rightmost);
      /* Do a postorder walk of the subtree below Node */
      while((Leftmost == NULL) && (rs != NULL))
	{
	  Rightmost = rs;
	  Leftmost = GetLeftMost(gf,Rightmost, Level + 1, Depth);
	  rs = RIGHTSIBLING(gf,Rightmost);
	}
      return Leftmost;
    }
}


int
MeanNodeSize(GraphFrame *gf,struct pt *LeftNode, struct pt * RightNode)
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
CheckExtentsRange(GraphFrame *gf, int xValue, int yValue)
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
print_prelims(GraphFrame *gf)
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
dfs(struct pt *v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      if( tv->mark == False)
	{
	  e->class = TREE_CLASS;
	  dfs(tv);
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else
	e->class = BACK_CLASS;
    }
  v->pick = True;
}

void 
dfs_undirected(struct pt *v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      if(tv == v)
	tv = e->from;

      if( tv->mark == False)
	{
	  if(e->class == NO_EDGE_CLASS)
	      e->class = TREE_CLASS;
	  tv->context = v; /* set parent for tree drawing */
	  dfs_undirected(tv);
	}
      else if (tv->pick == True)
	{
	  if(e->class == NO_EDGE_CLASS)
	    e->class = FORWARD_CLASS;
	}
      else
	{
	  if(e->class == NO_EDGE_CLASS)
	    e->class = BACK_CLASS;
	}
    }
  v->pick = True;
}

void 
dfs_link_attrib(struct pt *v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;

      if(e->attrib == 'n')
	{
	  if( tv->mark == False)
	    {
	      e->class = TREE_CLASS;
	      dfs_link_attrib(tv);
	    }
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else if (e->attrib == 'p')
	e->class = BACK_CLASS;
    }
  v->pick = True;
}

int
compute_tree_documents(GraphFrame *gf, int RootOrientation)
{
  int count=0;
  LNode_type *ptr,*lsnodes;
  int xi = 10*gf->vsize;
  int yi = xi;
  int inc = 10*gf->vsize;

  link_tree(gf);
  lsnodes = list_sources_nodes(gf,&count);
  /*printf("SOURCES NODES %d\n", count);*/
  if(!lsnodes)
    return -1;

  reset_edges(gf);

  for(ptr= lsnodes->next; ptr != lsnodes ; ptr = ptr->next)
    {
      PNODE sel = (PNODE)ptr->info;
      sel->loc.x = xi;
      sel->loc.y = yi;
      xi += inc;
    }

  for(ptr= lsnodes->next; ptr != lsnodes ; ptr = ptr->next)
    {
      PNODE sel = (PNODE)ptr->info;
      reset_mark_pick_vertices(gf);      
      dfs_link_attrib(sel);
      if(!gf->level_zero)
	gf->level_zero = init_linked_list();
      /*printf("drawing root %d\n", sel->num);*/
      PositionTree(gf,sel,RootOrientation);
      Delete_all_list(gf->level_zero);
      gf->level_zero = NULL;
    }
  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  Delete_all_list(lsnodes);
  return 0;
}


void 
dfs_highlight(struct pt *v)
{
  LNode_type *poed;
  v->mark = True;

  if(!v->highlight)
      return;
  
  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;

      if(!tv->highlight)
	  continue;

      if( tv->mark == False)
	{
	  e->class = TREE_CLASS;
	  dfs(tv);
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else
	e->class = BACK_CLASS;
    }
  v->pick = True;
}

/* ------------------------------------------------------------------------ */


PNODE
UPARENT(GraphFrame *gf,PNODE Node)
{
  if(!Node)
    return NULL;

  if(Node == rootnode)
      return NULL;

  return ((PNODE)Node->context);
}

/*PNODE
UPARENT(GraphFrame *gf,PNODE Node)
{
  LNode_type *ptr;
  if(!Node)
    return NULL;

  if(Node == rootnode)
      return NULL;

  for(ptr = Node->ledges_in->next ; ptr != Node->ledges_in ; ptr= ptr->next)
    {
      PEDGE e = (PEDGE)(ptr->info);
      if(e->class == TREE_CLASS) {
	  if(e->to == Node)
	      return (e->from);
	  else
	      return (e->to);
      }
      
    }
  return NULL;
}*/

PNODE 
UFIRSTCHILD(GraphFrame *gf,PNODE Node)
{
  LNode_type *ptr;
  PNODE Parent = NULL;

  if(!Node)
    return NULL;
  
  Parent = UPARENT(gf,Node);

  for(ptr =Node->ledges_in->back; ptr != Node->ledges_in; ptr=ptr->back)
    {
      PEDGE e = (PEDGE )(ptr->info);
      PNODE Child = NULL;
      if(e->class == TREE_CLASS) {
	  Child = (e->to == Node ? e->from : e->to);
	  if(Child != Parent)
	      return Child;
      }
    }
  return NULL;
}

PNODE 
ULEFTSIBLING(GraphFrame *gf,PNODE Node)
{
  PNODE GParent = NULL;
  PNODE Parent=NULL;
  PNODE Child = NULL;
  PEDGE te=NULL;
  LNode_type *ptr;
  if(!Node)
    return NULL;

  Parent = UPARENT(gf,Node);

  if(!Parent)
    return NULL;

  GParent = UPARENT(gf,Parent);

  ptr = Parent->ledges_in->back;
  
  for(;ptr != Parent->ledges_in; ptr = ptr->back)
    {
      te = (PEDGE )ptr->info;
      Child = (te->to == Parent ? te->from : te->to);
      if(te->class == TREE_CLASS && Child == Node)
	break;
    }

  if(ptr == Parent->ledges_in)
    return NULL;

  for(;ptr->next != Parent->ledges_in; ptr = ptr->next)
    {
      te = (PEDGE )ptr->next->info;
      Child = (te->to == Parent ? te->from : te->to);
      if(te->class == TREE_CLASS && Child != GParent)
	return (Child);
    }
  return NULL;
}

PNODE 
URIGHTSIBLING(GraphFrame *gf,PNODE Node)
{
  PNODE GParent = NULL;
  PNODE Parent = NULL;
  PNODE Child = NULL;
  PEDGE te=NULL;
  LNode_type *ptr;

  if(!Node)
    return NULL;

  Parent = UPARENT(gf,Node);

  if(!Parent)
    return NULL;

  GParent = UPARENT(gf,Parent);

  ptr = Parent->ledges_in->back;
  for(;ptr != Parent->ledges_in; ptr = ptr->back)
    {
      te = (PEDGE )ptr->info;
      Child = (te->to == Parent ? te->from : te->to);
      if(te->class == TREE_CLASS && Child == Node)
	break;
    }

  if(ptr== Parent->ledges_in)
    return NULL;

  for(;ptr->back != Parent->ledges_in; ptr= ptr->back)
    {
      te = (PEDGE )ptr->back->info;
      Child = (te->to == Parent ? te->from : te->to);
      if(te->class == TREE_CLASS && Child != GParent)
	return (Child);
    }
  return NULL;
}

Boolean
UISLEAF(GraphFrame *gf, PNODE Node)
{
    PNODE Child = UFIRSTCHILD(gf,Node);
    if(Child)
	return False;
    return True;
}

/*Boolean
UISLEAF(GraphFrame *gf, PNODE Node)
{
  LNode_type *ptr;
  PNODE Parent=NULL;

  if(!Node)
      return True;
  if(is_empty_list(Node->ledges_in))
    return True;
  Parent = UPARENT(gf,Node);
  for(ptr= Node->ledges_in->back ; ptr != Node->ledges_in; ptr = ptr->back)
    {
      PEDGE e = (PEDGE )ptr->info;
      Child = (e->to == Node ? e->from : e->to);
      if(e->class == TREE_CLASS && Child != Parent)
	return False;
    }
  return True;
}*/

Boolean
UHASCHILD(GraphFrame *gf, PNODE Node)
{
  /*if(is_empty_list(Node->ledges_out))
    return False;
  return True;*/
  return (!UISLEAF(gf,Node));
}


Boolean
UPositionTree(GraphFrame *gf,PNODE Node,int RootOrientation)
{
  if(!Node)
    return True;

  rootnode = Node;

  /* Initialize the list of previous nodes at each level */

  InitPrevNodeList(gf);
  
  /* Do the preliminary positioning with a psotorder walk */

  UFirstWalk(gf,Node, 0);
  
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
      yTopAdjustment = Node->loc.y + Node->prelim;
    }

  /* Do the final positioning with a preorder walk */
  
  return USecondWalk(gf,Node,0,0,RootOrientation);
}


void
UFirstWalk(GraphFrame *gf,PNODE Node, int Level)
{
  PNODE ls = ULEFTSIBLING(gf,Node);
   int ss = scale_dim(gf->scale,SiblingSeparation);
  /*printf("FW: Node : %s \n", Node->label);*/

  /* Set the pointer to the previous node at this level */
  Node->left_neighbor = GetPrevNodeAtLevel(gf,Level);
  SetPrevNodeAtLevel(gf,Level,Node); /* This is now the previous */
  Node->modifier = 0; /* Set the default modifier value */

  if((UISLEAF(gf,Node) || Level == MaxDepth))
    {
      if(ls) /* Node has left sibling */
	{
	  /* Determine the preliminary x-coordinate based on: */
	  /* the preliminary x-coordinate of the left sibling, */
	  /* the separation between sibling nodes, and */
	  /* the mean size of left sibling and current node */
	  Node->prelim = ls->prelim + ss 
	    +MeanNodeSize(gf,ls, Node);
	}
      else /* No sibling on the left to worry about */
	Node->prelim = 0;
    }
  else 
    {
      /* This Node is not a leaf, so call this procedure */
      /* recursively for each of its offspring */
      PNODE rrs;
      int Midpoint;
      PNODE Leftmost; 
      PNODE Rightmost;

      Rightmost = Leftmost = UFIRSTCHILD(gf,Node);

      UFirstWalk(gf,Leftmost, Level + 1);

      rrs = URIGHTSIBLING(gf,Rightmost);
      while(rrs)
	{
	  Rightmost = rrs;
	  UFirstWalk(gf,Rightmost,Level +1);
	  rrs = URIGHTSIBLING(gf,Rightmost);
	}

      Midpoint = (Leftmost->prelim + Rightmost->prelim)/2;

      if(ls)
	{
	  Node->prelim = ls->prelim + ss
	    +MeanNodeSize(gf,ls,Node);
	  Node->modifier = Node->prelim - Midpoint;
	  UApportion(gf,Node,Level);
	}
      else
	Node->prelim = Midpoint;
    }
}

Boolean
USecondWalk(GraphFrame *gf, PNODE Node, int Level, int Modsum,
	   int RootOrientation)
{
  Boolean Result=True;
  int xTemp, yTemp;
  PNODE rs = URIGHTSIBLING(gf,Node);
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
      if(CheckExtentsRange(gf,xTemp,yTemp))
	{
	  Node->loc.x = xTemp;
	  Node->loc.y = yTemp;
	  if(UHASCHILD(gf,Node))
	    {
	      /* Apply the Modifier value for this node to */
	      /* all its offspring */
	      Result = USecondWalk(gf,UFIRSTCHILD(gf,Node),Level+1, 
				  Modsum + Node->modifier,RootOrientation);
	    }
	    
	  if((Result == True) && (rs != NULL))
	    Result = USecondWalk(gf,rs, Level, Modsum,RootOrientation);
	      
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
UApportion(GraphFrame *gf,PNODE Node, int Level)
{
  PNODE Leftmost = UFIRSTCHILD(gf,Node);
  PNODE Neighbor = Leftmost->left_neighbor;
  PNODE AncestorLeftmost, AncestorNeighbor;
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
	  AncestorLeftmost = UPARENT(gf,AncestorLeftmost);
	  AncestorNeighbor = UPARENT(gf,AncestorNeighbor);
	  RightModsum += AncestorLeftmost->modifier;
	  LeftModsum += AncestorNeighbor->modifier;
	}
      /* Find the MoveDistance, and apply it to Node's subtree */
      /* Add appropriate portions to smaller interior subtrees */
      MoveDistance = (Neighbor->prelim + LeftModsum + 
		      scale_dim(gf->scale,SubtreeSeparation) +
		      MeanNodeSize(gf,Leftmost, Neighbor)) - 
	             (Leftmost->prelim + RightModsum);
      if(MoveDistance > 0)
	{
	  /* Count interior sibling subtrees in LeftSiblings */
	  PNODE TempPtr = Node;
	  int LeftSiblings = 0;
	  while((TempPtr != NULL) && (TempPtr != AncestorNeighbor))
	    {
	      LeftSiblings++;
	      TempPtr = ULEFTSIBLING(gf,TempPtr);
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
		  TempPtr = ULEFTSIBLING(gf,TempPtr);
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
      if(UISLEAF(gf,Leftmost))
	Leftmost = UGetLeftMost(gf,Node, 0, CompareDepth);
      else
	Leftmost = UFIRSTCHILD(gf,Leftmost);

      /* This is missing in the original algorithm */

      if(Leftmost)
	Neighbor = Leftmost->left_neighbor; 
      else
	Neighbor = NULL;
    }
}

PNODE 
UGetLeftMost(GraphFrame *gf,PNODE  Node, int Level, int Depth)
{
  if(Level >= Depth)
    return Node;
  else if (UISLEAF(gf,Node)) 
    return NULL;
  else
    {
      PNODE Rightmost = UFIRSTCHILD(gf,Node);
      PNODE Leftmost = UGetLeftMost(gf,Rightmost, Level + 1, Depth);
      PNODE rs = URIGHTSIBLING(gf,Rightmost);
      /* Do a postorder walk of the subtree below Node */
      while((Leftmost == NULL) && (rs != NULL))
	{
	  Rightmost = rs;
	  Leftmost = UGetLeftMost(gf,Rightmost, Level + 1, Depth);
	  rs = URIGHTSIBLING(gf,Rightmost);
	}
      return Leftmost;
    }
}





