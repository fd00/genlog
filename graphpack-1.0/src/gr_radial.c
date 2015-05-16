#include "gr_radial.h"
#include "gr_tree.h" 
#include "gr_util.h" 
#include "gr_operations.h"
#include "gr_bary.h"
#include "queue.h"

int 
compute_radial_positions(GraphFrame *gf, struct pt *sel)
{
  int radius;
  int size;
  double total,angle;
  LNode_type *ptr;
  LNode_type *num_node_level = init_linked_list();

  int max_level=0;

  if(!gf->no_class_edge) /* Classification of edges is done already */
  {
      if(!gf->drawhighlight)
	  compute_tree_graph(gf,sel);
      else
	  compute_tree_graph_highlight(gf, sel);
  }

  gf->rootv = sel; /* set root vertex */

  reset_mark_pick_vertices(gf);
  max_level = bfs_count_nodes(gf, sel, num_node_level);

  calculate_radius_level(gf,num_node_level);
  /*print_level_info(num_node_level);*/
  reset_mark_pick_vertices(gf);

  /*sel->loc.x = GR_MWIDTH/2;
  sel->loc.y = GR_MHEIGHT/2;*/

  sel->mark = True;

  if(gf->graph_dir == DIRECTED)
    size = count_not_marked(sel->ledges_out);
  else
    size = count_not_marked_undir(sel, sel->ledges_in);

  radius = (int)(2*gf->vsize *size )/ PI;

  if(radius < CONS_RADIUS*gf->vsize)
    radius = CONS_RADIUS *gf->vsize;

  total = (double)2*PI;
  
  angle = 2*PI/(double)size;

  if(gf->graph_dir == DIRECTED)
    {
      for(ptr = sel->ledges_out->back; ptr!= sel->ledges_out ; ptr=ptr->back)
	{
	  struct edge *e = (struct edge *) ptr->info;
	  struct pt *w = e->to;
	  
	  if((w->mark == False) && (e->class == TREE_CLASS))
	    {
	      sub_embed(gf, total, radius, w, (long)size, 
			sel->loc.x, sel->loc.y,num_node_level);
	      total += angle;
	    }
	}
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      for(ptr = sel->ledges_in->back; ptr!= sel->ledges_in ; ptr=ptr->back)
	{
	  struct edge *e = (struct edge *) ptr->info;
	  struct pt *w = e->to;
	  if(w == sel)
	    w = e->from;
	  
	  if((w->mark == False) && (e->class == TREE_CLASS))
	    {
	      sub_embed(gf, total, radius, w, (long)size,
			sel->loc.x, sel->loc.y,num_node_level);
	      total += angle;
	    }
	}
    }

  delete_level_info(num_node_level);
  Delete_all_list(num_node_level);
  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  return 0;
}


int
count_not_marked(LNode_type *ledges)
{
  LNode_type *ptr;
  int count = 0;
  for(ptr = ledges->back; ptr!= ledges ; ptr=ptr->back)  
    {
      struct edge *e = (struct edge *)ptr->info;
      struct pt *v = e->to;

      if((e->class == TREE_CLASS)&&(v->mark == False))
	count++;
    }
  return count;
}
int
count_not_marked_undir(struct pt *par, LNode_type *ledges)
{
  LNode_type *ptr;
  int count = 0;
  for(ptr = ledges->back; ptr!= ledges ; ptr=ptr->back)  
    {
      struct edge *e = (struct edge *)ptr->info;
      struct pt *v = e->to;
      if(v == par)
	v = e->from;

      if((e->class == TREE_CLASS)&&(v->mark == False))
	count++;
    }
  return count;
}



void
sub_embed(GraphFrame *gf, double total, int r, struct pt *v, 
	  long precount, int x, int y,LNode_type *num_node_level)
{
  int radius;
  int size;
  long totalcount;
  int tr;
  double temp,angle,bangle;
  LNode_type *ptr;

  position_vertex_radial(gf, v, r, total,x, y);

  v->mark = True;

  if(total > 2*PI) total-=2*PI;

  if(gf->graph_dir == DIRECTED)
    size = count_not_marked(v->ledges_out);
  else
    size = count_not_marked_undir(v, v->ledges_in);

  if(size == 0)
    return;

  totalcount = size *precount;
  /*printf("v = %d , totalcount %ld, precount %ld\n", v->num, totalcount,precount);*/

  tr = find_optimal_radius(num_node_level, v->level+1);

  radius = CONS_RADIUS*gf->vsize;
  /*printf("radius1  %d ", r);*/
  if(r+radius > tr)
    r+=radius;
  else
    r= tr;

  bangle = (double)(gf->vsize)/(double)r;

  /*printf("radius2  %d \n", r);*/
  angle = 2*PI/(double)totalcount;
  if (angle < bangle)
    angle = bangle;

  temp = ((double)(size -1) *angle)/2;
  
  if(temp <= angle ) 
    angle-=temp;
  else
    angle = 2*PI -temp;

  if(gf->graph_dir == DIRECTED)
    {
      for(ptr = v->ledges_out->back; ptr!= v->ledges_out ; ptr=ptr->back)
	{
	  struct edge *e = (struct edge *) ptr->info;
	  struct pt *w = e->to;
	  if((w->mark == False) && (e->class == TREE_CLASS))
	    {
	      sub_embed(gf,total, r, w, totalcount,x, y, num_node_level);
	      total +=angle;
	    }
	}
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      for(ptr = v->ledges_in->back; ptr!= v->ledges_in ; ptr=ptr->back)
	{
	  struct edge *e = (struct edge *) ptr->info;
	  struct pt *w = e->to;
	  if(w == v)
	    w = e->from;

	  if((w->mark == False) && (e->class == TREE_CLASS))
	    {
	      sub_embed(gf,total, r, w, totalcount,x,y,num_node_level);
	      total +=angle;
	    }
	}
    }

}

void
position_vertex_radial(GraphFrame *gf, struct pt *v, int r, double total,
		       int x, int y)
{
  /*printf("v: %d, r = %d\n", v->num, r);*/
  v->loc.x = (int)(cos(total)*r) + x;
  v->loc.y = (int)(sin(total)*r) + y;
}

int
bfs(GraphFrame *gf, struct pt *root)
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
	  for(poed = u->ledges_out; poed->back != u->ledges_out; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->back->info;
	      struct pt * tv = e->to;
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
      else if(gf->graph_dir == UNDIRECTED)
	{
	  for(poed = u->ledges_in; poed->back != u->ledges_in; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->back->info;
	      struct pt * tv = e->to;
	      if(tv == u)
		tv = e->from;

	      if(!(tv->mark || tv->pick)) /* Is vertex white? */
		{
		  tv->mark = True; /* Vertex is gray */
		  tv->level = u->level + 1;
		  e->class = TREE_CLASS;
		  tv->context = u; /* Parent for tree drawing */
		  Enqueue(Q,(char *)tv);
		}
	      else if(tv->mark)
		{
		  if(tv->pick)
		    {
		      if(e->class ==NO_EDGE_CLASS)
			  e->class = BACK_CLASS;
		    }
		  else
		    {
		      if(e->class ==NO_EDGE_CLASS)
			  e->class = FORWARD_CLASS;
		    }
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
bfs_highlight(GraphFrame *gf, struct pt *root)
{
  /* White - no mark, no pick
     Gray  - mark , no pick
     Black - mark, pick */
  LNode_type *Q = NULL;
  int max;

  if(!root->highlight)
      return 0;

  Q = init_queue();
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
	  for(poed = u->ledges_out; poed->back != u->ledges_out; 
	      poed = poed->back)
	    {
	      struct edge *e = (struct edge *)poed->back->info;
	      struct pt * tv = e->to;

	      if(!tv->highlight)
		  continue;

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


void
reset_edges(GraphFrame *gf)
{
  LNode_type *poed;
  LNode_type *ptr;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices; ptr = ptr->next)
    {
      struct pt *v = (struct pt *) ptr->info;
      for(poed = v->ledges_in->next; poed != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->info;
	  e->class = NO_EDGE_CLASS;
	  e->touch = False; 
	}
    }
}

int
bfs_count_nodes(GraphFrame *gf, struct pt *sel,LNode_type * num_node_level)
{
  LNode_type *Q = init_queue();
  int max;

  sel->mark = True; /* root is Gray */
  sel->level = 0;
  Enqueue(Q,(char *)sel);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);
      int cchild = 0;
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_out; poed->next != u->ledges_out; 
	      poed = poed->next)
	    {
	      struct edge *e = (struct edge *)poed->next->info;
	      struct pt * tv = e->to;
	      if(e->class == TREE_CLASS)
		{
		  if(!(tv->mark || tv->pick)) /* Is vertex white? */
		    {
		      cchild++;
		      tv->mark = True; /* Vertex is gray */
		      tv->level = u->level + 1;
		      Enqueue(Q,(char *)tv);
		    }
		}
	    }

	  if(cchild > 0)
	    add_num_child(num_node_level, cchild, u->level+1);
	}
      else if(gf->graph_dir == UNDIRECTED)
	{
	  for(poed = u->ledges_in; poed->next != u->ledges_in; 
	      poed = poed->next)
	    {
	      struct edge *e = (struct edge *)poed->next->info;
	      struct pt * tv = e->to;
	      if(tv == u)
		tv = e->from;
	      if(e->class == TREE_CLASS)
		{
		  if(!(tv->mark || tv->pick)) /* Is vertex white? */
		    {
		      cchild++;
		      tv->mark = True; /* Vertex is gray */
		      tv->level = u->level + 1;
		      Enqueue(Q,(char *)tv);
		    }
		}
	    }
	  if(cchild > 0)
	    add_num_child(num_node_level, cchild, u->level+1);
	}

      Dequeue(Q);
      u->pick = True; /* Vertex u is Black */
      max = u->level;
    }
  Delete_linked_list(Q);
  return max;
}


void
add_num_child(LNode_type *num_node_level, int cchild, int level)
{
  LNode_type *ptr;
  Boolean check = False;

  for(ptr = num_node_level->back; ptr != num_node_level; ptr = ptr->back)
    {
      LEVEL_INFO *linfo = (LEVEL_INFO *)ptr->info;
      if(linfo->level == level)
	{
	  check = True;
	  linfo->num_nodes += cchild;
	  if(cchild > linfo->max_node)
	    linfo->max_node = cchild;
	  break;
	}
    }
  if(check == False)
    {
      LEVEL_INFO *linfo = (LEVEL_INFO *) malloc(sizeof(LEVEL_INFO));
      if(!linfo) {
	  fprintf(stderr,"Out of Memory add_num_child\n");
	  abort();
      }
      linfo->num_nodes = cchild;
      linfo->max_node = cchild;
      linfo->level = level;
      Insert_linked_list(num_node_level,create_new_node_llist((char*)linfo));
    }
}

void
print_level_info(LNode_type *num_node_level)
{
  LNode_type *ptr;
  /*printf("\nLEVEL OF GRAPH\n");*/
  for(ptr = num_node_level->back; ptr != num_node_level; ptr = ptr->back)
    {
      /*LEVEL_INFO *linfo = (LEVEL_INFO *)ptr->info;
      printf("Level: %d, Max Node %d, Num Node %d , Radius %d\n", 
	     linfo->level, linfo->max_node, linfo->num_nodes, linfo->radius);*/
    }
}

void
calculate_radius_level(GraphFrame *gf, LNode_type *num_node_level)
{
  LNode_type *ptr;
  for(ptr = num_node_level->back; ptr != num_node_level; ptr = ptr->back)
    {
      LEVEL_INFO *linfo = (LEVEL_INFO *)ptr->info;
      LEVEL_INFO *linfob = (LEVEL_INFO *)ptr->next->info;
      /*long max=0;*/
      long avg=1;

      if(linfob)
	{
	  /*max = linfo->max_node * linfob->num_nodes;*/
	  avg = (long)((double)linfo->num_nodes/(double)linfob->num_nodes);
	  avg = avg * linfob->num_nodes;
	}
      else
	avg = linfo->num_nodes;

      /*if(linfo->num_nodes > max)
	max = linfo->num_nodes;*/

      linfo->radius = (int)(2*gf->vsize *avg)/PI;
    }
}

int
find_optimal_radius(LNode_type *num_node_level, int level)
{
  LNode_type *ptr;
  for(ptr = num_node_level->back; ptr != num_node_level; ptr = ptr->back)
    {
      LEVEL_INFO *linfo = (LEVEL_INFO *)ptr->info;
      if(linfo->level == level)
	  return linfo->radius;
    }
  return 0;
}




void
delete_level_info(LNode_type *num_node_level)
{
  LNode_type *ptr;
  for(ptr = num_node_level->back; ptr != num_node_level; ptr = ptr->back)
    {
      LEVEL_INFO *linfo = (LEVEL_INFO *)ptr->info;
      free(linfo); linfo = NULL;
      ptr->info = NULL;
    }
}


int
complete_bfs_directed(GraphFrame *gf, struct pt *root)
{
  /* White - no mark, no pick
     Gray  - mark , no pick
     Black - mark, pick */
  LNode_type *Q = init_queue();

  reset_mark_pick_vertices(gf);

  root->mark = True; /* root is Gray */

  Enqueue(Q,(char *)root);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);

      for(poed = u->ledges_out->back; poed != u->ledges_out; 
	  poed = poed->back)
	{
	  struct edge *e = (struct edge *)poed->info;
	  struct pt * tv = e->to;

	  if(e->class != TREE_CLASS)
	    continue;

	  if(!(tv->mark || tv->pick)) /* Is vertex white? */
	    {
	      tv->mark = True; /* Vertex is gray */
	      Enqueue(Q,(char *)tv);
	    }
	}
      Dequeue(Q);
    }

  root->pick = True; /* root is Gray */

  Enqueue(Q,(char *)root);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);

      for(poed = u->ledges_out->back; poed != u->ledges_out; 
	  poed = poed->back)
	{
	  struct edge *e = (struct edge *)poed->info;
	  struct pt * tv = e->to;

	  if(e->class == TREE_CLASS)
	    {
	      if(tv->mark == False)
		tv->mark = True;
	      if(!tv->pick)
		{
		  tv->pick = True; /* Vertex is gray */
		  Enqueue(Q,(char *)tv);
		}
	    }
	  else if(e->class == NO_EDGE_CLASS)
	    {
	      if(tv->mark == False)
		{
		  e->class = TREE_CLASS;
		  tv->mark = True;
		  if(!tv->pick)
		    {
		      tv->pick = True;
		      Enqueue(Q,(char *)tv);
		    }
		}
	      else
		{
		  e->class = BACK_CLASS;
		}
	    }
	}
      Dequeue(Q);
    }

  Delete_linked_list(Q);
  return 1;
}

void
print_no_pick_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr=gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(v->pick == False)
	printf("Node %d %s was not picked\n", v->num, v->label);
    }
  printf("ALL NODES were picked\n");
}

Boolean
there_are_no_pick_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr=gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(v->pick == False)
	return True;
    }
  return False;
}

int
bfs_first_variation(GraphFrame *gf, struct pt *root)
{
  /* White - no mark, no pick
     Gray  - mark , no pick
     Black - mark, pick */
  LNode_type *Q = init_queue();
  int max;

  if(gf->graph_dir != DIRECTED)
    return -1;

  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  root->mark = True; /* root is Gray */
  root->level = 0;
  Enqueue(Q,(char *)root);

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      struct pt *u = (struct pt *)head_queue(Q);

      for(poed = u->ledges_out; poed->back != u->ledges_out; 
	  poed = poed->back)
	{
	  struct edge *e = (struct edge *)poed->back->info;
	  struct pt * tv = e->to;

	  if(u->shape == PENTAGON)
	    {
	      e->class = BACK_CLASS;
	      continue;
	    }

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
      Dequeue(Q);
      u->pick = True; /* Vertex u is Black */
      max = u->level;
    }
  Delete_linked_list(Q);
  print_no_pick_nodes(gf);
  return max;
}


