#include "gr_algorithms.h"
#include "gr_tree.h"
#include "gr_radtree.h"
#include "gr_operations.h"
#include "queue.h"
#include "gr_bary.h"
#include "gr_util.h"

#define MAX_NODES 500

typedef struct short_dist {
  long dist;
  PEDGE pe;
  PNODE v;
  short mark;
} SHORTDIS;

typedef SHORTDIS *PDIS;


void
init_sdis(PDIS pdis,PNODE v);

PDIS
find_pdis_node(GraphFrame *gf, PNODE v);
void
initialize_single_source(GraphFrame *gf, PNODE s);
void 
relax(PNODE u, PNODE v, PEDGE e, GraphFrame *gf, long (*comp_weight)());

PDIS
extract_min(GraphFrame *gf, int *nmn);
void
dfs_to_shortest_path_tree(GraphFrame *gf,PNODE sel);
void
dfs_short(PNODE v);
void
dfs_short_undirected(PNODE v);

void
init_sdis(PDIS pdis,PNODE v)
{
  pdis->dist = INFINITY;
  pdis->pe = NULL;
  pdis->v = v;
  pdis->mark = 0;
}

void 
delete_short_distance_list(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lsdist)
    {
      for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
	{
	  free(ptr->info); ptr->info = NULL;
	}
      Delete_all_list(gf->lsdist);
      gf->lsdist = NULL;
    }
}

void 
print_short_distance_list(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lsdist)
    {
      printf("\n NEW LIST\n");
      for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
	{
	  PDIS pdis = (PDIS)ptr->info;
	  PNODE v = pdis->v;
	  if(pdis->pe)
	    printf("M:%d NODE %d %s : %ld PARENT %d\n", 
		   pdis->mark,v->num, v->label, 
		   pdis->dist,pdis->pe->from->num);
	  else
	    printf("M:%d NODE %d %s : %ld PARENT NULL\n", 
		   pdis->mark,v->num, v->label, 
		   pdis->dist);
	}
    }
}
PDIS
find_pdis_node(GraphFrame *gf, PNODE v)
{
  LNode_type *ptr;
  if(gf->lsdist)
    {
      for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
	{
	  PDIS pdis = (PDIS)ptr->info;
	  PNODE u = pdis->v;
	  if(u == v)
	    return pdis;
	}
    }
  return NULL;
}

PDIS
find_pdis_source(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lsdist)
    {
      for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
	{
	  PDIS pdis = (PDIS)ptr->info;

	  if(pdis->dist == 0)
	    return pdis;
	}
    }
  return NULL;
}

void
initialize_short_distance_list(GraphFrame *gf)
{
  LNode_type *ptr;

  if(gf->lsdist)
    {
      delete_short_distance_list(gf);
    }

  gf->lsdist = init_linked_list();

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; 
      ptr= ptr->back)
    {
      PNODE ov = (PNODE)ptr->info;
      PDIS pdis = (PDIS)calloc(1,sizeof(SHORTDIS));
      if(!pdis) {
	  fprintf(stderr,"Out of Memory initialize_short_distance_list\n");
	  abort();
      }
      init_sdis(pdis,ov);
      ov->modifier = (int)pdis;
      Insert_linked_list(gf->lsdist, create_new_node_llist((char *)pdis));
    }
}

void
initialize_single_source(GraphFrame *gf, PNODE s)
{
  PDIS spdis;
  initialize_short_distance_list(gf);
  spdis = (PDIS)s->modifier;
  if(spdis)
    spdis->dist = 0;
}

void 
relax(PNODE u, PNODE v, PEDGE e, GraphFrame *gf, long (*comp_weight)())
{
  PDIS updis = (PDIS)u->modifier;
  PDIS vpdis = (PDIS)v->modifier;

  long w = (*comp_weight)(e,gf);

  /*printf("RELAXING %d %d weight %ld, dist u %ld dist v : %ld\n",
    u->num,v->num,w,updis->dist,vpdis->dist);*/

  if(w <= 0)
    w = 1;

  if(vpdis->dist > updis->dist + w)
    {
      vpdis->dist = updis->dist + w;
      vpdis->pe = e;
    }
}

PDIS
extract_min(GraphFrame *gf, int *nmn)
{
  LNode_type *ptr;
  PDIS mpdis = NULL;
  int mind = INFINITY ;

  for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
    {
      PDIS pdis = (PDIS)ptr->info;
      if(pdis->mark == 1)
	continue;

      if(pdis->dist < mind)
	{
	  mind = pdis->dist;
	  mpdis = pdis;
	}
    }
  if(mpdis)
    {
      mpdis->mark = 1;
      (*nmn)++;
    }
  return mpdis;

}

void
Dijkstra(GraphFrame *gf, PNODE s,long (*comp_weight)())
{
  PDIS updis = NULL;
  int num_marked_nodes = 0;

  initialize_single_source(gf,s);
  reset_mark_pick_vertices(gf); /* For objects weight */
  if(gf->graph_dir == DIRECTED) {
  while(num_marked_nodes != gf->count_vertex)
    {
      PNODE u;
      LNode_type *ei;
      updis = extract_min(gf, &num_marked_nodes);
      if(!updis)
	return;
      u = updis->v;
      /*u->pick = True;*/ /* Vertex is black */
      for(ei = u->ledges_out->back; ei != u->ledges_out; ei = ei->back)
	{
	  PEDGE e = (PEDGE) ei->info;
	  PNODE v = e->to;
	  relax(u,v,e,gf,comp_weight);
	}
    }
  } else {
  while(num_marked_nodes != gf->count_vertex)
    {
      PNODE u;
      LNode_type *ei;
      updis = extract_min(gf, &num_marked_nodes);
      if(!updis)
	return;
      u = updis->v;
      /*u->pick = True;*/ /* Vertex is black */
      for(ei = u->ledges_in->back; ei != u->ledges_in; ei = ei->back)
	{
	  PEDGE e = (PEDGE) ei->info;
	  PNODE v = e->from;
	  if(v == u)
	      v = e->to;
	  relax(u,v,e,gf,comp_weight);
	}
    }
  }
  /*print_short_distance_list(gf);*/
}

long
select_shortest_path(GraphFrame *gf, PNODE s, PNODE t,long (*comp_weight)())
{
  PNODE parent = NULL;
  PDIS tpdis = NULL;
  PEDGE e = NULL;
  PDIS spdis = find_pdis_node(gf,s);
  Boolean flag = False;
  long tdist = 0;

  if(!spdis || (spdis && spdis->dist!=0))
    {
      /*printf("COMPUTING Dijkstra again\n");*/
      Dijkstra(gf, s,comp_weight);
      tpdis = (PDIS)t->modifier;
      flag = True;
    }
  else
    {
      /*printf("FINDING %d\n", t->num);*/
      tpdis = find_pdis_node(gf,t);
    }
  if(gf->graph_dir == DIRECTED) {
  if(tpdis->pe)
    {
      tdist = tpdis->dist;
      do {
	e = tpdis->pe;
	parent = e->from;
	select_edge_selecting_vertices(gf,e);
	if(flag)
	  tpdis = (PDIS)parent->modifier;
	else
	  tpdis = find_pdis_node(gf,parent);
      } while(parent != s);
      return tdist;
    }
  } else {
  PNODE ct = t;
  if(tpdis->pe)
    {
      tdist = tpdis->dist;
      do {
	e = tpdis->pe;
	parent = e->from;
	if(parent == ct)
	    parent = e->to;
	ct = parent;
	select_edge_selecting_vertices(gf,e);
	if(flag)
	  tpdis = (PDIS)parent->modifier;
	else
	  tpdis = find_pdis_node(gf,parent);
      } while(parent != s);
      return tdist;
    }
  }
  return -1;
}

void
print_shortest_path(PNODE t)
{
  do {
  PDIS tpdis = (PDIS)t->modifier;
  PEDGE pedge = tpdis->pe;
  PNODE parent;
  printf("\t-> %d %s\n", t->num, t->label);
  if(pedge)
      parent = pedge->from;
  else
    break;
  t = parent;
  } while(1);
}

long
get_value_shortest_path(PNODE t)
{
    PDIS tpdis = (PDIS)t->modifier;
    if(tpdis)
	return tpdis->dist;
    return -1;
}

long
get_number_edges_shortest_path(PNODE t)
{
  long c = -1;
  do {
  PDIS tpdis = (PDIS)t->modifier;
  PEDGE pedge = tpdis->pe;
  PNODE parent;
  /*printf("\t-> %d %s\n", t->num, t->label);*/
  c++;
  if(pedge)
      parent = pedge->from;
  else
    break;
  t = parent;
  } while(1);
  return c;
}

void
pick_objects_in_path(PNODE v)
{
  LNode_type *ptr, *leout = get_edges_out(v);
  for(ptr = leout->next; ptr != leout ; ptr = ptr->next)
    {
      PEDGE ei = (PEDGE)ptr->info;
      if(get_edge_attribute(ei) == 'z')
	{
	  PNODE vobj = ei->to;
	  vobj->pick = True;
	  /*printf("\t Picking %s\n", vobj->label);*/
	}
    }
}

void
apply_function_to_shortest_path(PNODE t, void (*funct)())
{
  do {
  PDIS tpdis = (PDIS)t->modifier;
  PEDGE pedge = tpdis->pe;
  PNODE parent;
  /*printf("\t-> %d %s\n", t->num, t->label);*/
  (*funct)(t);
  if(pedge)
      parent = pedge->from;
  else
    break;
  t = parent;
  } while(1);
}

long
select_shortest_path_to(GraphFrame *gf, PNODE t,long (*comp_weight)())
{
  PDIS spdis = find_pdis_source(gf);
  
  if(!spdis)
    return -2;

  return select_shortest_path(gf, spdis->v, t,comp_weight);
}

void
classify_tree_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lsdist)
    {
      for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
	{
	  PDIS pdis = (PDIS)ptr->info;
	  PEDGE e = pdis->pe;
	  if(e)
	    e->class = TREE_CLASS;
	}
    }
}

void
dfs_to_shortest_path_tree(GraphFrame *gf,PNODE sel)
{
  classify_tree_edges(gf);
  if(gf->graph_dir == DIRECTED)
      dfs_short(sel);
  else
      dfs_short_undirected(sel);
}

void
dfs_short(PNODE v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;

      if(e->class == TREE_CLASS)
	{
	  if( tv->mark == False)
	    {
	      dfs_short(tv);
	    }
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else 
	e->class = BACK_CLASS;
    }
  v->pick = True;

}

void
dfs_short_undirected(PNODE v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in; poed->back != v->ledges_in; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      if(tv == v)
	  tv = e->from;

      if(e->class == TREE_CLASS)
	{
	  if( tv->mark == False)
	    {
		tv->context = v; /* Parent node */
	      dfs_short_undirected(tv);
	    }
	}
      else if (tv->pick == True)
	e->class = FORWARD_CLASS;
      else 
	e->class = BACK_CLASS;
    }
  v->pick = True;

}

int
compute_tree_from_shortest_path(GraphFrame *gf, PNODE sel,
				long (*comp_weight)())
{
  Dijkstra(gf, sel,comp_weight);

  reset_mark_pick_vertices(gf);
  reset_edges(gf);

  gf->rootv = sel; /* set root vertex */

  dfs_to_shortest_path_tree(gf,sel);

  return 0;
}


int
draw_radial_tree_from_shortest_path(GraphFrame *gf, PNODE sel,
				    long (*comp_weight)())
{
  Dijkstra(gf, sel,comp_weight);

  if(!gf->level_zero)
    gf->level_zero = init_linked_list();
  reset_mark_pick_vertices(gf);
  reset_edges(gf);

  gf->rootv = sel; /* set root vertex */

  dfs_to_shortest_path_tree(gf,sel);

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

  Delete_all_list(gf->level_zero);
  gf->level_zero = NULL;
  return 0;
}

int 
select_biggest_shortest_path(GraphFrame *gf)
{
  PDIS max_dis = NULL ;
  LNode_type *ptr;
  PNODE t;
  PDIS tpdis;
  int i =0;

  if(!gf->lsdist)
    return 0;

  /*ptr = gf->lsdist->back;
  if(ptr!= gf->lsdist)
  max_dis = (PDIS)ptr->info;*/

  for(ptr= gf->lsdist->back; ptr!=gf->lsdist ; ptr = ptr->back)
  {
      PDIS pdis = (PDIS)ptr->info;
      if(pdis->dist != INFINITY)
      {
	  max_dis = pdis;
	  break;
      }
  }

  if(!max_dis)
    return 0;

  for(ptr = gf->lsdist->back; ptr != gf->lsdist; ptr = ptr->back)
    {  
      PDIS pdis = (PDIS)ptr->info;
      /*printf("pdis dist %ld \n", pdis->dist);*/

      if(pdis->dist == INFINITY)
	  continue;

      if(pdis->dist > max_dis->dist)
	max_dis = pdis;
    }
  
  if(!max_dis)
    return 0;

  t = max_dis->v;
  tpdis = max_dis;
  if(gf->graph_dir == DIRECTED) {
  do {
    PEDGE pedge = tpdis->pe;
    PNODE parent;

    if(pedge)
      {
	i++;
	select_edge_selecting_vertices(gf, pedge);
	parent = pedge->from;
      }
    else
      break;
    t = parent;
    tpdis = find_pdis_node(gf,t);
  } while(1); 
  } else {
  do {
    PEDGE pedge = tpdis->pe;
    PNODE parent;

    if(pedge)
      {
	i++;
	select_edge_selecting_vertices(gf, pedge);
	parent = pedge->from;
	if(parent == t)
	    parent = pedge->to;
      }
    else
      break;
    t = parent;
    tpdis = find_pdis_node(gf,t);
  } while(1); 
  }
  return i;
}

/*****************************************************************/

PNODE
find_content_vertex(GraphFrame *gf)
{
    LNode_type *ptr = NULL;
    LNode_type *lver = gf->list_vertices;
    PNODE vmax = NULL;
    int maxd = 0;
    PNODE v = NULL;
    int c = 0;

    for(ptr = lver->back; ptr != lver;ptr= ptr->back) 
    {
	v = (PNODE)ptr->info;
	c = get_num_edges_in(v) + get_num_edges_out(v);	
	if(c > maxd)
	{
	    vmax = v;
	    maxd = c;
	}
    }
    return vmax;
}

PNODE
get_parent(PNODE s)
{
    PNODE u = NULL;
    LNode_type *ei = s->ledges_in->next;
    for(;ei != s->ledges_in; ei=ei->next)
    {
	PEDGE e = (PEDGE)ei->info;
	u = e->from;
	if(e->class == TREE_CLASS)
	    return u;
    }
    return NULL;
}

int
is_descendant(PNODE s,PNODE t,PNODE root)
{
    PNODE p = NULL;
    int maxlevel = 100;
    do {
	p = get_parent(s);
	/*printf("Parent %d Child %d\n",p->num, s->num);*/
	if(p == t)
	    return 1;
	maxlevel--;
	s = p;
    } while(p && p != root && maxlevel);
    return 0;
}

int
is_final(PNODE v, PNODE root, PNODE content,GraphFrame *gf)
{ 
    int c = 0;
    LNode_type *ei = NULL;
    PEDGE et = NULL;
    for(ei = v->ledges_out->next; ei != v->ledges_out; ei=ei->next)
    {
	PEDGE e = (PEDGE) ei->info;
	PNODE u = e->to;	    
	if(u == content || u == root)
	    continue;
	et = find_edge_by_name(gf,u->label,v->label);
	if(et)
	    c++;
    }
    /*printf("V %d has %d edges\n",v->num,c);*/
    return (c==1);
}

int
is_double_connected(GraphFrame *gf,PNODE t,PNODE r)
{
    PEDGE es, et;
    es = find_edge_by_name(gf,t->label,r->label);
    et = find_edge_by_name(gf,r->label,t->label);
    return (es && et);
}

int 
is_stop_vertex(PNODE v, LNode_type *lsnodes)
{
    LNode_type *ptr = NULL;
    if(!lsnodes || !v)
	return 0;
    for(ptr = lsnodes->back; ptr != lsnodes;ptr= ptr->back) 
    {
	PNODE u = (PNODE)ptr->info;
	if(u == v)
	    return 1;
    }
    return 0;
}

int
inv_double_bfs(GraphFrame *gf, PNODE root, LNode_type *lsnodes)
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
      PNODE u = (PNODE)head_queue(Q);
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_in->back; poed != u->ledges_in; 
	      poed = poed->back)
	    {
	      PEDGE e = (PEDGE)poed->info;
	      PNODE tv = e->from;
	      PEDGE et = NULL;

	      if(is_stop_vertex(tv,lsnodes))
		  continue;

	      et = find_edge_by_name(gf,u->label,tv->label);
	      if(!et)
	      {
		  e->class = FORWARD_CLASS;
		  continue;
	      }
	      if(!(tv->mark || tv->pick)) /* Is vertex white? */
		{
		  tv->mark = True; /* Vertex is gray */
		  tv->level = u->level + 1;
		  e->class = BACK_CLASS;
		  et->class = TREE_CLASS;
		  Enqueue(Q,(char *)tv);
		}
	      else if(tv->mark) /* no idea yet */
		{
		    /*if(tv->pick)
		      e->class = BACK_CLASS;
		  else
		  e->class = FORWARD_CLASS;*/
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
bfs_rest(GraphFrame *gf, PNODE root)
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
      PNODE u = (PNODE)head_queue(Q);

      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_out->back; poed != u->ledges_out; 
	      poed = poed->back)
	    {
	      PEDGE e = (PEDGE)poed->info;
	      PNODE tv = e->to;

	      if(e->class == TREE_CLASS)
	      {
		  tv->mark = True;
		  tv->level = u->level + 1;
		  Enqueue(Q,(char *)tv);
	      }
	      else if(!(tv->mark || tv->pick))
	      {
		  if(get_parent(tv)== NULL)
		  {
		      e->class = TREE_CLASS;
		      tv->mark = True;
		      tv->level = u->level + 1;
		      Enqueue(Q,(char *)tv);
		  }
		  else
		      e->class = FORWARD_CLASS;
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
get_number_inside_nodes(GraphFrame *gf)
{
    LNode_type *lver = gf->list_vertices;
    LNode_type *ptr = NULL;
    int c = 0;
    for(ptr = lver->back; ptr != lver;ptr= ptr->back) 
    {
	PNODE v = (PNODE)ptr->info;
	if(!is_vertex_sink(v))	
	    c++;
    }
    return c;
}

LNode_type *
get_list_stop_nodes(GraphFrame *gf)
{
    LNode_type *lsnodes = init_linked_list();
    int n = get_number_inside_nodes(gf);
    LNode_type *lver = gf->list_vertices;
    LNode_type *ptr = NULL;

    n = (int)(0.9 * (float)n);
    for(ptr = lver->back; ptr != lver;ptr= ptr->back) 
    {
	PNODE v = (PNODE)ptr->info;
	if(count_llist(v->ledges_in) > n)
	{
	    /*printf("STOP NODE %d %s\n", v->num, v->label);*/
	    Insert_linked_list(lsnodes,create_new_node_llist((char *)v));
	}
    }
    return lsnodes;
}

int
draw_hierarchical_radial_tree_graph(GraphFrame *gf, PNODE sel)
{
  LNode_type *hlist = NULL;
  
  if(!gf->level_zero)
    gf->level_zero = init_linked_list();
  reset_mark_pick_vertices(gf);
  reset_edges(gf);

  hlist = classify_hierarchical_edges(gf,sel);

  if(hlist) {
      Delete_all_list(hlist);
      hlist = NULL;
  }

  PositionTree(gf,sel,NORTH);

  compute_new_radial_positions(gf, sel);

  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);

  Delete_all_list(gf->level_zero);
  gf->level_zero = NULL;
  return 0;
}

/*****************************************************************/

int
get_siblings_of_child(PNODE v, PNODE *a, PNODE *b, PNODE *c, int *la, 
		      int *lb,GraphFrame *gf,
		      PNODE r, PNODE p, LNode_type *lsnodes)
{
  LNode_type *Q = init_queue();
  int ca = 0;
  int cb = 0;
  int error = 0;
  int endnode = 0;
  /*int max;*/
  reset_mark_vertices(gf);

  v->mark = True; /* root is Gray */
  Enqueue(Q,(char *)v);
  *a = NULL;
  *b = NULL;

  while(!is_queue_empty(Q))
    {
      LNode_type *poed;
      PNODE u = (PNODE)head_queue(Q);
      /*printf("Visiting U %d\n", u->num);*/
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = u->ledges_in->back; poed != u->ledges_in; 
	      poed = poed->back)
	    {
	      PEDGE e = (PEDGE)poed->info;
	      PNODE tv = e->from;
	      PEDGE et = NULL;

	      if(tv == r)
		  continue;
	      if(tv == v)
		  continue;
	      if(is_stop_vertex(tv,lsnodes))
		  continue;
	      et = find_edge_by_name(gf,u->label,tv->label);
	      if(!et)
		  continue;

	      if(tv->level > 0 && tv->level <= r->level)
	      {
		  /*printf(" END CHILD  V %d, %d - %d\n",v->num, u->num, tv->num);*/
		  endnode = 1;
		  continue;
	      }

	      if(is_double_connected(gf,tv,r))
	      {
		  if(!*b)
		  {
		      cb++;
		      *b = tv;
		  } 
		  else 
		  {
		      if(tv == *b)
			  cb++;	
		      else if(!*a)
		      {
			  ca++;
			  *a = tv;
		      }
		      else if(tv == *a)
			  ca++;
		      else
			  error = 1;
  		  }

		  /*printf("One extreme v %d a %d\n", v->num, tv->num);*/
		  continue;
	      }

	      if(!tv->mark) /* Is vertex white? */
		{
		  tv->mark = True; /* Vertex is gray */
		  Enqueue(Q,(char *)tv);
		}
	      else if(tv->mark) 
		{
		    /*if(tv->pick)
		        e->class = BACK_CLASS;
        	      else
	                e->class = FORWARD_CLASS;*/
		}
	    }
	}
      else
	return -1;

      Dequeue(Q);
      /*u->pick = True;*/ /* Vertex u is Black */
    }
  Delete_linked_list(Q);
  /*printf("Error %d A %d cA %d B %d cB %d\n", error, *a ? (*a)->num : -1,
    ca, *b ? (*b)->num : -1, cb);*/
  {
      PNODE t = NULL;
      *la = 0;
      *lb = 0;
      if(cb > 1)
      {
	  if(ca == 1)
	  {
	      t = *a;
	      *a = *b;	
	      *b = t;
	      *la = 1;
	      *lb = 2;
	  }
	  else if(ca == 0)
	  {
	      *a = *b;
	      *b = NULL;
	      *la = 1;
	      *lb = -1;
	  }
	  else if(ca > 1)
	      error = 1;
      } 
      else if(cb == 1)
      {
	  if(ca > 1)
	  {
	      *la = 1; 
	      *lb = 2;
	  }
	  else if(ca == 0)
	      *la = -1;

	  if(endnode == 1)
	  {
	      *a = *b;
	      *b = NULL;
	      *la = 1;
	      *lb = -1;
	  }

      }

      /*else if(cb == 0)
	error = 1;*/
  }
  
  /*printf("EXTREMES Error %d A %d B %d \n", error, *a ? (*a)->num : -1, 
   *b ? (*b)->num : -1);*/
  return error;

}

int 
find_end_link(int con[MAX_NODES][5],int max)
{
    int i = 0;
    for(i = 0 ; i < max; i++)
    {
	if(con[i][2]==1 && con[i][4]==-1)
	    return i;
    }
    return -1;
}

int 
find_first_single_link(int con[MAX_NODES][5],int max)
{
    int i = 0;
    for(i = 0 ; i < max; i++)
    {
	if(con[i][2]==-1 && con[i][4]==-1)
	    continue;
	if(con[i][2]==-1 || con[i][4]==-1)
	    return i;
    }
    return -1;
}

int 
find_good_link(int con[MAX_NODES][5],int max)
{
    int i = 0;
    for(i = 0 ; i < max; i++)
    {
	if(con[i][2]==1 && con[i][4]==2)
	    return i;
    }
    return -1;
}

void
change_rows(int con[MAX_NODES][5],int i, int j)
{
    int t;
    int k = 0;
    for(k = 0; k< 5; k++)
    {
	t = con[i][k];
	con[i][k] = con[j][k];
	con[j][k] = t;
    }
}

int
find_previous_sibling(int con[MAX_NODES][5],int max, int b)
{
    int i = 0;
    for(i = 0 ; i < max; i++)
    {
	if(con[i][0]==b)
	    return i;
    }
    return -1;
}

int
traverse_links(int con[MAX_NODES][5],int max, int j)
{
    int b = con[j][1];
    int i = j;
    int c = 0;
    while(1) 
    {
	c = con[i][0];
	i = find_previous_sibling(con,max,b);
	if(i == -1)
	    break;
	if(con[i][2]==-1 || con[i][4]==-1)
	    return i;
	if(con[i][1] == c)
	    b = con[i][3];
	else if(con[i][3] == c)
	    b = con[i][1];
	else
	    break;
    };
    return -1;
}

int
find_start_link(int con[MAX_NODES][5],int max,int j)
{
    int i = 0;
    for(i = 0 ; i < max; i++)
    {
	if(i == j)
	    continue;
	if(con[i][2]==-1 && con[i][4]==-1)
	    continue;
	if(con[i][2]==-1 || con[i][4]==-1)
	    return i;
    }
    return -1;
}

int 
order_children(int con[MAX_NODES][5],int max)
{
    int i = 0;
    int error = 0;
    /*printf("Check end link\n");*/
    i = find_end_link(con,max);
    if(i >= 0)
    {
	/*printf("Check start link\n");*/
	i = find_start_link(con,max,i);
    }
    else
    {
	/*printf("Check good link\n");*/
	i = find_good_link(con,max);
	if(i >= 0) 
	{
	    /*printf("Check traverse link\n");*/
	    i = traverse_links(con,max,i);
	}
	else
	{
	    /*printf("Check first single link\n");*/
	    i = find_first_single_link(con,max);
	}
    }

    if(i == -1)
    {
	return 1;
    }
    else
    {
	change_rows(con,i,0);
	return 0;
    }

    return error;
}

int
get_first_child(PNODE root, PNODE parent, GraphFrame *gf,LNode_type *lsnodes, 
		PNODE *next)
{
    LNode_type *ptr = NULL;
    int error = 0;
    PNODE a=NULL,b=NULL,c=NULL;
    int la = 0;
    int lb = 0;
    int con[MAX_NODES][5];
    int i = 0;
    for(i = 0; i< MAX_NODES; i++)
    {
	con[i][0] = 0;
	con[i][1] = 0;
	con[i][2] = 0;
	con[i][3] = 0;
	con[i][4] = 0;
    }
    i = 0;
    for(ptr = root->ledges_in->next; ptr != root->ledges_in; ptr = ptr->next)
    {	
	PEDGE e = (PEDGE)ptr->info;
	PNODE tv = e->from;
	PEDGE et = NULL;	
	/*if(tv == parent)
	  continue;*/
	if(tv->level > 0)
	    continue;
	if(is_stop_vertex(tv,lsnodes))
	    continue;
	et = find_edge_by_name(gf,root->label,tv->label);
	if(!et)
	    continue;
	/*printf("\nFINDING SIBLINGS of V %d\n",tv->num);*/
	error = get_siblings_of_child(tv,&a, &b, &c, &la, &lb,gf,
				      root,parent,lsnodes);
	tv->level = root->level + 1;
	et->class = TREE_CLASS;
	e->class = BACK_CLASS;
	/*printf("SIBLINGS A %d LA %d, B%d LB %d\n", a ? a->num: -1, la, 
	  b ? b->num: -1, lb);*/
	con[i][0] = tv->num;
	con[i][1] = a ? a->num : -1;
	con[i][2] = la;
	con[i][3] = b ? b->num : -1;
	con[i][4] = lb;
	i++;
	if(i >= MAX_NODES)
	{
	    error = 1;
	    break;
	}
    }
    if(i > 1)
    {
	order_children(con,i);
	if(con[0][2] == -1 && con[0][4] == 0)
	{
	    /*printf("First Child %d\n", con[0][0]);*/
	    *next = find_vertex_by_number(gf,con[0][0]);
	}
	else
	{
	    /*printf("NO First Child\n");*/
	    /*error = 1;*/
	}
    }
    else if(i == 1)
    {
	*next = find_vertex_by_number(gf,con[0][0]);
	error = 0;
    }
    else
	*next = NULL;
    return error;
}

PNODE
get_next_sibling(PNODE v,GraphFrame *gf,LNode_type *lsnodes)
{
    LNode_type *ptr = NULL;
    PNODE next = NULL;
    int c = 0;
    /*printf("TRYING NEXT for %d\n", v->num);*/
    for(ptr = v->ledges_in->next; ptr != v->ledges_in; ptr = ptr->next)
    {	
	PEDGE e = (PEDGE)ptr->info;
	PNODE tv = e->from;
	PEDGE et = NULL;	
	if(tv->pick)
	    continue;
	if(is_stop_vertex(tv,lsnodes))
	    continue;
	et = find_edge_by_name(gf,v->label,tv->label);
	if(!et)
	    continue;
	next = tv;
	/*printf("POSSIBLE NEXT %d\n", tv->num);*/
	c++;
    }    
    if(c > 1)
	return NULL;
    return next;
}

int
gvisit_tree(PNODE root, PNODE parent, GraphFrame *gf,LNode_type *lsnodes, 
	    PNODE *next)
{
    PNODE vnext = NULL;
    int error = 0;
    error = get_first_child(root,parent,gf,lsnodes,&vnext);
    /*printf("error %d V %d FIRST CHILD %d\n", error,root->num, 
      vnext ? vnext->num : -1);*/
    if(error)
    {
	*next = NULL;
	return error;
    }
    if(vnext)
    {
	*next = vnext;
	return 0;
    }
    else
    {
	PNODE b = get_next_sibling(root,gf,lsnodes);	
	if(b)
	{
	    *next = b;
	    return 0;
	}
	else
	{
	    *next = NULL;
	    return 1;
	}
    }
    return error;
}

LNode_type *
new_inv_double_bfs(GraphFrame *gf, PNODE root, LNode_type *lsnodes, int *error)
{
    PNODE next = root;
    PNODE v = NULL;
    LNode_type *hlist = NULL;
    
    hlist = init_linked_list();
    reset_level_vertices(gf);
    root->level = 1;
    do {
	v = next;
	v->pick = 1;
	*error = gvisit_tree(v, NULL, gf,lsnodes, &next);
	Insert_linked_list(hlist, create_new_node_llist((char *)v));
	/*printf("NEXT %d ERROR %d\n", next ? next->num : -1,*error);*/
    } while (next);
    return hlist;
}

LNode_type *
classify_hierarchical_edges(GraphFrame *gf, PNODE sel)
{
    LNode_type *lsnodes = NULL;
    int maxlevel = 0;
    int error = 0;
    LNode_type *hlist = NULL;

    if(!gf || !sel)
	return NULL;
	
    lsnodes = get_list_stop_nodes(gf);

    if(is_stop_vertex(sel,lsnodes) || is_empty_list(lsnodes))
    {
      Delete_all_list(lsnodes);
      lsnodes = NULL;
    }
    
    hlist = new_inv_double_bfs(gf,sel,lsnodes,&error);

    maxlevel = bfs_rest(gf,sel);

    /*get_hamiltonean_cycle(gf,sel,lsnodes);*/

    if(lsnodes) {
	Delete_all_list(lsnodes);
	lsnodes = NULL;
    }
    return hlist;
}

/*****************************************************************/


double
compute_cluster_coefficient(GraphFrame *gf)
{
    LNode_type *ptr = NULL;
    LNode_type *lver = gf->list_vertices;
    double *CC = NULL;
    double sum = 0.0;
    double mean = 0.0;
    int i = 0;
    int noc = gf->count_vertex;
    int npoints = 0;
    int nedges = 0;
    int nconections = 0;

    deselect_all_nodes(gf);
    CC = (double *)calloc(noc+1,sizeof(double));
    if(!CC) {
	fprintf(stderr,"Out of Memory compute_cluster_coefficient\n");
	abort();
    }
    for(ptr = lver->next ; ptr != lver; ptr = ptr->next)
    {
	PNODE v = (PNODE)ptr->info;
	select_vertex(v,gf);
	select_neighbors(gf);
	deselect_vertex(v,gf);
	npoints = count_llist(gf->list_sel_vertex);
	nedges = number_of_edges_selected_vertices(gf);
	if(gf->graph_dir == DIRECTED)
	    nconections = nedges/2;
	else
	    nconections = nedges;
	if(nconections > 0)
	    CC[i++]=(double)nconections/
		(double)((npoints * (npoints-1))/2.0);
	else	
	    CC[i++]=0;
	/*printf("Vertex %d edges %d points %d CC %f\n", v->num, nedges,
	  npoints, CC[i-1]);*/
	deselect_all_nodes(gf);
    }
    sum  = 0.0;
    for(i = 0 ; i < noc ; i++)
	sum+=CC[i];
    mean = sum/(double)noc;
    /*printf("mean %f\n", mean);*/
    free(CC); CC = NULL;
    return mean;
}


