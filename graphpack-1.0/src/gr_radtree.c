#include "gr_tree.h"
#include "gr_radtree.h"
#include "gr_operations.h"
#include "queue.h"
#include "gr_radtree.h"
#include "gr_util.h"

void
get_circle_coordinates(int r, struct pt *v,struct pt *c, int *a, int *b, int maxp);


int
compute_radial_tree_positions(GraphFrame *gf, struct pt *sel)
{
  if(!gf->level_zero)
    gf->level_zero = init_linked_list();

  /* gf->drawhighlight = True; */

  if(!gf->no_class_edge) /* Classification of edges is done already */
  {
      if(!gf->drawhighlight)
	  compute_tree_graph(gf, sel);
      else
	  compute_tree_graph_highlight(gf, sel);
  }

  gf->rootv = sel; /* set root vertex */

  if(gf->graph_dir == UNDIRECTED) 
  {
      sel->context = NULL; /* parent of root is NULL */
      UPositionTree(gf,sel,NORTH);
  }
  else
      PositionTree(gf,sel,NORTH);

  compute_new_radial_positions(gf, sel);

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
compute_new_radial_positions(GraphFrame *gf, struct pt *root)
{
  int i,max;
  int ls = scale_dim(gf->scale, LevelSeparation);
  int ss = scale_dim(gf->scale, SubtreeSeparation);
  int pmax = 0;

  LNode_type *Q = init_queue();
  int *perimeters,*radius;
  PNODE *lnodes;

  perimeters = (int *)calloc(sizeof(int),MAX_LEVEL);
  if(!perimeters) {
      fprintf(stderr,"Out of Memory compute_new_radial_positions\n");
      abort();
  }
  radius = (int *)calloc(sizeof(int),MAX_LEVEL);
  if(!radius) {
      fprintf(stderr,"Out of Memory compute_new_radial_positions\n");
      abort();
  }
  lnodes = (PNODE *)calloc(sizeof(PNODE), MAX_LEVEL);
  if(!lnodes) {
      fprintf(stderr,"Out of Memory compute_new_radial_positions\n");
      abort();
  }
  root->level = 0;
  
  Enqueue(Q,(char *)root);
  
  /*printf("NUMBER\tLEVEL\tX\tY\tLEFT NUM\n");*/

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);
      /*struct pt *p =PARENT(gf,u);*/

      lnodes[u->level]=u;
      if(u->left_neighbor)
	{
	  perimeters[u->level]+=(int)abs(u->left_neighbor->loc.x - u->loc.x);

	  lnodes[u->level]=u;
	}

      /*if(p)
	  u->prelim = u->loc.x - p->loc.x;*/
      
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_out; poed->back != u->ledges_out; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->back->info;
	      struct pt * tv = e->to;

	      if(e->class == TREE_CLASS)
		{
		  tv->level = u->level + 1;
		  Enqueue(Q,(char *)tv);
		}
	    }
	}
      else if(gf->graph_dir == UNDIRECTED)
	{
	  for(poed = u->ledges_in; poed->back != u->ledges_in; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->back->info;
	      struct pt * tv = e->to;
	      if(tv == u)
		  tv = e->from;

	      if(e->class == TREE_CLASS && tv->context == u)
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
      int lr = (lnodes[i])->loc.x - root->loc.x; /* right side */
      int ll = perimeters[i] - lr; /* left side */
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
      printf(" fnode %d\n", lnodes[i] ? lnodes[i]->num : -1);*/

      while(cnode!= NULL)
	{
	  int a, b;
	  get_circle_coordinates(radius[i], cnode, root,&a, &b,maxp);
	  cnode->loc.x = root->loc.x +a;
	  cnode->loc.y = root->loc.y +b;
	  cnode = cnode->left_neighbor;
	}
    }
  
  free(perimeters); perimeters = NULL;
  free(radius); radius = NULL;
  Delete_linked_list(Q); Q = NULL;
  return;
}

void
get_circle_coordinates(int r, struct pt *v, struct pt *c, int *a, int *b, 
		       int maxp)
{
  double tn = (double)(v->loc.x - c->loc.x) * PI / (double)maxp;
  *a = (int)((double)r*sin(tn));
  *b = (int)((double)r*cos(tn));
}






