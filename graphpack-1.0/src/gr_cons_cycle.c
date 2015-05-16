#include "gr_cons_cycle.h"
#include "gr_constrain.h"
#include "gr_parser.h"
#include "gr_operations.h"
#include "gr_bary.h"
#include "queue.h"


static int error;

int
compute_geometry_positions_cycle(GraphFrame *gf, struct pt *v, struct pt *v2,
			   char *sconst)
{
  int i,n, **ncons;  
  error = NO_ERROR;
  n = gf->count_vertex;

  if (n == 0)
    {
      fprintf(stderr,"NO GRAPH\n");
      return NO_GRAPH;
    }

  ncons = (int **)malloc(sizeof(int *)*(n+1));
  for(i = 0; i< n; i ++)
    ncons[i] = (int *)malloc(sizeof(int)*5);

  initialize_const(ncons,n);

  error = parse_constraints(sconst,ncons,n);

  if(error < 0)
    {
      delete_constrains_array(ncons,n);
      return error;
    }
  
  set_constraints_nodes(gf,ncons,n);

  if(error < 0)
    {
      delete_constrains_vertex(gf);
      delete_constrains_array(ncons,n);
      return error;
    }

  dehigh_all_nodes(gf);
  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  Delete_hash_table(gf->HV);

  error = layout_graph_cons_cycle(gf,v,v2);

  delete_constrains_vertex(gf);
  delete_constrains_array(ncons,n);

  return error;
}

int 
layout_graph_cons_cycle(GraphFrame *gf,struct pt *v,struct pt *v2)
{
  LNode_type *Q;
  
  Q = init_queue();
  
  error = find_first_face_cycle(gf,Q,v,v2);

  if(error < 0)
    {
      Delete_all_list(Q);
      return error;
    }

  queue_loop(gf,Q);

  if(error < 0)
    {
      Delete_all_list(Q);
      return error;
    }

  place_rest_vertices(gf);

  error = move_neg_graph(gf);

  report_nodes_constraints(gf);
  
  Delete_all_list(Q);
  return error;
}

int
find_first_face_cycle(GraphFrame *gf, LNode_type *Q,struct pt *v,struct pt *v2)
{
  LNode_type *troute = init_linked_list();
  if(v&&v2)
    {
      if(find_face_cycle(gf,v,v2,troute))
	{
	  place_route_cycle(gf,troute);
	  save_edges_queue(gf,troute,Q);
	  return NO_ERROR;
	}
    }
  return NO_FACE;
}

Boolean
find_face_cycle(GraphFrame *gf,struct pt *v,struct pt *v2,LNode_type *troute)
{
  struct pt *v3, *v4 = NULL;
  v3 = next_vertex_const(v,v2);
  Insert_linked_list(troute,create_new_node_llist((char *)v));  
  Insert_linked_list(troute,create_new_node_llist((char *)v2));
  if(v3)
    {
      Insert_linked_list(troute,create_new_node_llist((char *)v3));
      v4 = next_vertex_const(v2,v3);
      while(v4!= NULL && v4!= v)
	{
	  /* verify that node is not in troute */
	  Insert_linked_list(troute,create_new_node_llist((char *)v4));
	  v2 = v3;
	  v3 = v4;
	  v4 = next_vertex_const(v2,v3);
	}
      if(v4 == NULL)
	{
	  Delete_all_list(troute);
	  return False;
	}
    }
  else
    {
      Delete_all_list(troute);
      return False;
    }
  return True;
}

void
place_route_cycle(GraphFrame *gf, LNode_type *troute)
{
  int n = count_llist(troute);
  int d = CON_UNIT*gf->vsize;
  int l = (n > 4) ? (int)ceil((double)(n - 4)/4) : 0;
  LNode_type *ptr;
  int X, Y;
  double pX, pY,rX, rY;
  double theta = 0.0;
  double delta = 2 * PI / n;  

  if(n <= 2)
    return;
  if(n <=4)
    {
      place_route(gf,troute);
      return;
    }
  
  X = Y = (l+1)*2*d;


  rX =  (double)(X / 2 - X/10);
  rY =  (double)(Y / 2 - Y/10);
  
  pX = (double)(X / 2);
  pY = (double)(Y / 2);


  for(ptr=troute; ptr->next !=troute; ptr=ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      v->loc.x = (int)(rX * cos(theta) + pX); 
      v->loc.y = (int)(rY * sin(theta) + pY);
      verify_insert_v(gf,v);

      theta += delta;
    }
}


