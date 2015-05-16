#include "gr_level.h"
#include "gr_bary.h"
#include "gr_structure.h"
#include "gr_operations.h"

static LNode_type ** levels;
static int maxlevel;

int
compute_level_positions(GraphFrame *gf)
{
  int max_level;
  int mes = 0;

  if(gf->count_vertex <= 1)
    return mes;

  Delete_hash_table(gf->HV);
  max_level = Assign_level_nodes(gf);
  make_levels(gf);
  mes = place_nodes(gf);
  delete_levels(levels);
  return mes;
}
void
delete_levels( LNode_type ** levels)
{
  int i;
  for(i = 0 ; i <= maxlevel ; i++)
  {
    Delete_all_list(levels[i]);
    levels[i]=NULL;
  }
  free(levels); levels = NULL;
}
void
initial_order_nodes(struct pt * v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in; poed->next != v->ledges_in; poed = poed->next)
    {
      struct edge *e = (struct edge *)poed->next->info;
      struct pt * tv = e->from;
      if( tv->mark == False)
	initial_order_nodes(tv);
    }
   Insert_linked_list(levels[v->level],create_new_node_llist((char*)v));
}

void 
make_levels(GraphFrame *gf)
{
  struct pt *maxlevelnode = NULL;
  LNode_type *ptr;
  int i;

  maxlevel = -1;

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(maxlevel < v->level)
	{
	  maxlevel  = v->level;
	  maxlevelnode = v;
	}
    }
  
  levels = (LNode_type **)malloc(sizeof(LNode_type *)*(maxlevel+1));
  if(!levels) {
      fprintf(stderr,"Out of Memory make_levels\n");
      abort();
  }
  for( i = 0 ; i <=  maxlevel ; i++)
    levels[i] = init_linked_list();
  
  reset_mark_pick_vertices(gf);
  
  initial_order_nodes(maxlevelnode);
  
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;  
      if (v->mark == False)
	initial_order_nodes(v);
    }
  
}

int 
place_level(GraphFrame *gf,LNode_type *nodes,double L, double y, int levelcnt)
{
  double xstep;
  int mes = 0,i = 0;
  LNode_type *ptr;
  mes = calculate_width_level(gf, &L, &xstep,levelcnt);
  if(mes < 0)
    return mes;

  for(ptr= nodes; ptr->back != nodes ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;  
      v->loc.x = xstep * (i+1);
      v->loc.y = y;
      insert_v(v, gf);
      i++;
    }
  return mes;
}


int
place_nodes(GraphFrame *gf)
{
  int i;
  double L,ystep;
  double y = 0.0;
  Dimension X, Y;
  int mes = 0;
  mes = calculate_window_size(gf, &X, &Y);
  L = (double )X;
  mes = calculate_hight_level(gf, &L,&ystep);
  if(mes < 0)
    return mes;

  for(i=0; i<= maxlevel ; i++)
    {
      LNode_type *nodes = levels[i];
      mes = place_level(gf,nodes, L, y, count_llist(nodes));
      if (mes < 0)
	return mes;
      y += ystep;
    }
  return mes;
}

int
calculate_width_level(GraphFrame *gf, double *L,  double *xstep,int levelcnt)
{
  Dimension X, Y;
  int mes =0,i=0;

  *xstep = (*L)/(levelcnt+1);
  X = (Dimension)*L; 
  Y = (Dimension)*L;

  while((*xstep)< (double)(2*gf->vsize))
    {
      X+= X; Y+= Y;
      *L = (double)Y;
      *xstep = (*L)/(levelcnt+1);
      if(i++ == 3)
	return ERROR_WIDTH_LEVEL_WINDOW;
    }    
  mes = increase_window_size(gf,&X,&Y);
  return mes;
}
int
calculate_hight_level(GraphFrame *gf, double *L,  double *ystep)
{
  Dimension X, Y;
  int mes =0,i=0;

  *ystep = (*L)/(maxlevel+1);
  X = (Dimension)*L; 
  Y = (Dimension)*L;

  while((*ystep)< (double)(2*gf->vsize))
    {
      X+= X; Y+= Y;
      *L = (double)X;
      *ystep = (*L)/(maxlevel+1);
      if(i++ == 3)
	return ERROR_HIGHT_LEVEL_WINDOW;
    }    
  mes = increase_window_size(gf,&X,&Y);
  return mes;
}

void 
topo_sort(struct pt *v, LNode_type *topoSortedNodes)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in; poed->next != v->ledges_in; poed = poed->next)
    {
      struct edge *e = (struct edge *)poed->next->info;
      struct pt * tv = e->from;
      if( tv->mark == False)
	topo_sort(tv,topoSortedNodes);
    }
   Insert_linked_list(topoSortedNodes,create_new_node_llist((char*)v));
}

int
Assign_level_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  LNode_type *topoSortedNodes = init_linked_list();
  int max_level = -1;

  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v->mark == False)
	topo_sort(v, topoSortedNodes);
    }
  
  for(ptr = topoSortedNodes; ptr->back != topoSortedNodes ; ptr = ptr->back)
    {
      int level = 0;
      struct pt *v = (struct pt *)ptr->back->info;
      LNode_type *poed;
      for(poed = v->ledges_in; poed->next != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->next->info;
	  struct pt * tv = e->from;
	  if(tv->level > level)
	    level = tv->level;
	}
      v->level = level+1;
      if(level + 1 > max_level)
	max_level = level+1;
      /*printf("Vertex %s, level %d\n", v->label, v->level);*/
    }
  Delete_all_list(topoSortedNodes);
  return max_level;
}





