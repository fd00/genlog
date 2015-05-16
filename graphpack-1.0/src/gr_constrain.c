#include "gr_drawing.h"
#include "gr_bary.h"
#include "gr_structure.h"
#include "gr_constrain.h"
#include "gr_parser.h"
#include "gr_operations.h"
#include "queue.h"

static int error;
static int t;

int 
compute_geometry_positions(GraphFrame *gf, struct pt *v, char *sconst)
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

  t= 0;
  set_constraints_nodes(gf,ncons,n);

  if(error < 0)
    {
      delete_constrains_vertex(gf);
      delete_constrains_array(ncons,n);
      return error;
    }

  dehigh_all_nodes(gf);
  /*enumerate_vertices(gf);*/
  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  Delete_hash_table(gf->HV);

  error = layout_graph_constrains(gf,v);

  delete_constrains_vertex(gf);
  delete_constrains_array(ncons,n);

  return error;
}

int 
layout_graph_constrains(GraphFrame *gf,struct pt *v)
{
  LNode_type *Q;
  
  Q = init_queue();
  
  error = find_first_face(gf,Q,v);

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

void
report_nodes_constraints(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(!is_const_ok(v))
	{
	  print_vertex_const(v);
	  highlight_vertex(v,gf);
	}
    }  
}
void
print_vertex_const(struct pt *v)
{
  LNode_type *ptr;
  fprintf(stderr,"Constraints of vertex %s : ",v->label);
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      fprintf(stderr,"%s ",tv->label);
    }
  fprintf(stderr,"\n");
}

Boolean
is_const_ok(struct pt *v)
{
  int i;
  double ang[4];
  LNode_type *ptr;
  for(i=0;i<4;i++)
    ang[i]=-1;

  i=0;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      if(tv->mark)
	  ang[i++]=cal_angle(v,tv);
    }
  if((i==1)|| (i==2))
     return True;
  if(i==3)
    return (clock_wise(ang[0],ang[1],ang[2]));
  else if(i == 4)
    return(clock_wise_4(ang[0],ang[1],ang[2],ang[3]));
  else
    return False;
}
void
place_rest_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      struct pt *par,*v1,*v2;
      double ang, ang2, ang3;
      int d = CON_UNIT*gf->vsize;
      if(v->mark == False)
	{
	  par = (struct pt *)(v->lver_con->next->info);
	  /*printf("parent %s, child %s\n", par->label, v->label);*/
	  v1 = next_vertex_const(v,par);
	  v2 = back_vertex_const(v,par);
	  /*printf("v1 %s, v2 %s \n", v1->label, v2->label);*/
	  ang2 = cal_angle(par,v1);
	  ang3 = cal_angle(par,v2);
	  ang = average_angle(ang3,ang2);
	  /*printf("ang2 %f, ang3 %f, ang %f\n",ang2* 57.29578, ang3* 57.29578
		 , ang* 57.29578);*/

	  v->loc.x = par->loc.x + (int)(d*cos(ang));
	  v->loc.y = par->loc.y - (int)(d*sin(ang));
	  /*v->mark = True;
	  insert_v(v,gf);*/
	  verify_insert_v(gf,v);
	}
    }
}

int
find_first_face(GraphFrame *gf, LNode_type *Q,struct pt *v)
{
  LNode_type *ptr;
  LNode_type *troute = init_linked_list();
  Boolean found = False;

  if(v)
    {
      if(find_route_4(gf,v,troute))
	{
	  found = True;
	  place_route(gf,troute);
	  save_edges_queue(gf,troute,Q);
	  /*print_queue(gf,Q);*/
	}
      else if(find_route_3(gf,v,troute))
	{
	  found = True;
	  place_route(gf,troute);
	  save_edges_queue(gf,troute,Q);
	}
    }
  if(!found)
    {
      for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; 
	  ptr = ptr->back)
	{
	  struct pt *v = (struct pt *)ptr->back->info;
	  if(find_route_4(gf,v,troute))
	    {
	      found = True;
	      place_route(gf,troute);
	      save_edges_queue(gf,troute,Q);
	      /*print_queue(gf,Q);*/
	      break;
	    }
	}
    }
  if(!found)
    {
      for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; 
	  ptr = ptr->back)
	{
	  struct pt *v = (struct pt *)ptr->back->info;
	  if(find_route_3(gf,v,troute))
	    {
	      found = True;
	      place_route(gf,troute);
	      save_edges_queue(gf,troute,Q);
	      break;
	    }
	}
    }

  if(!found)
    {
      Delete_linked_list(troute);
      return NO_FACE;
    }
  Delete_linked_list(troute);
  return NO_ERROR;
}

void 
queue_loop(GraphFrame *gf,LNode_type *Q)
{
  while(!is_queue_empty(Q))
    {
      /* remove from queue edges with unv = 0 */
      /* Is queue empty ? */
      Edge_con * ec = (Edge_con *)Dequeue(Q);
      /* place  vertices */
      place_vertices_face(gf,ec,Q);
      /*printf("\nFirst Node Queue:\n");
      print_ec(ec);*/
      free(ec);
      if(error < 0)
	return;
      /*print_queue(gf,Q);*/
    }
}
void
place_vertices_face(GraphFrame *gf, Edge_con * ec, LNode_type *Q)
{
  if((ec->lface == 2)&&(ec->unv == 1))
    place_triangle(gf,ec,Q);
  else if((ec->lface == 3)&&(ec->unv == 2))
    place_square(gf,ec,Q);
  else if((ec->lface == 3)&&(ec->unv == 1))
    place_rhombus(gf,ec,Q);
  else if((ec->lface > 3)&&(ec->unv > 1))
    {
      /*printf("BIG face %d\n", ec->lface);*/
      if(!t)
	try_place_two_vertices(gf, ec, Q);
      t = 1;
    }
}

void
try_place_two_vertices(GraphFrame *gf, Edge_con * ec, LNode_type *Q)
{
  struct pt *v3, *v4;
  struct pt *v1 = ec->e->from;
  struct pt *v2 = ec->e->to;
  double ang;
  double r;

  Boolean found = False;

  v3 = next_vertex_const(v1,v2);
  if(v3->mark == False)
    {
      v4 = next_vertex_const(v2,v3);
      if(v4->mark == False)
	{
	  ec->cw = 0;
	  found = True;
	}
    }
  else
    {  
      v3 = back_vertex_const(v1,v2);
      if(v3->mark == False)
	{
	  v4 = back_vertex_const(v2,v3);
	  if(v4->mark == False)
	    {
	      ec->cw = 1;
	      found = True;
	    }
	}
    }
	
  if(!found)
    {
      v1 = ec->e->to;
      v2 = ec->e->from;
      v3 = next_vertex_const(v1,v2);
      if(v3->mark == False)
	{
	  v4 = next_vertex_const(v2,v3);
	  if(v4->mark == False)
	    {
	      ec->cw = 0;
	      found = True;
	    }
	}
      else
	{  
	  v3 = back_vertex_const(v1,v2);
	  if(v3->mark == False)
	    {
	      v4 = back_vertex_const(v2,v3);
	      if(v4->mark == False)
		{
		  ec->cw = 1;
		  found = True;
		}
	    }
	}
    }
  if(!found)
    return;

  r =
    (double)(CON_UNIT*gf->vsize)/distance(v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y);
  /*printf("PLACING %s and %s\n", v3->label,v4->label);*/
  ang = cal_angle(v1,v2);
  /*printf("v3 : %s, v4 %s, angle %f \n", v3->label, v4->label,ang* 57.29578);*/

  if(ec->cw)
    {
      v4->loc.x = v1->loc.x -(int)(r *(v2->loc.y - v1->loc.y));
      v4->loc.y = v1->loc.y +(int)(r *(v2->loc.x - v1->loc.x));
      v3->loc.x = v2->loc.x -(int)(r *(v2->loc.y - v1->loc.y));
      v3->loc.y = v2->loc.y +(int)(r *(v2->loc.x - v1->loc.x));
    }
  else
    {
      v4->loc.x = v1->loc.x +(int)(r *(v2->loc.y - v1->loc.y));
      v4->loc.y = v1->loc.y -(int)(r *(v2->loc.x - v1->loc.x));
      v3->loc.x = v2->loc.x +(int)(r *(v2->loc.y - v1->loc.y));
      v3->loc.y = v2->loc.y -(int)(r *(v2->loc.x - v1->loc.x));
    }

  /*v3->mark = True;
  insert_v(v3,gf);
  v4->mark = True;
  insert_v(v4,gf);*/
  verify_insert_v(gf,v3);
  verify_insert_v(gf,v4);

  if(recompute_faces_queue(gf,Q))
    reorder_queue(gf,Q);

  /* Insert new edges in priority Queue */
  {
    struct edge *e1 = find_edge(gf,v2,v3);
    struct edge *e2 = find_edge(gf,v3,v4);
    struct edge *e3 = find_edge(gf,v4,v1);
    Edge_con *nec;
    if(e1)
      {
	nec = (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e1);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
    if(e2)
      {
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e2);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
    if(e3)
      {
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e3);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
  }
}

void
place_triangle(GraphFrame *gf, Edge_con * ec, LNode_type *Q)
{
  struct pt *v = find_unplaced_vertex(gf,ec);
  struct pt *v1 = ec->e->from;
  struct pt *v2 = ec->e->to;
  int dire;

  /*printf("placing %s\n", v->label);*/

  dire = compute_triangle(v,v1,v2,ec->cw,CON_UNIT*(gf->vsize));

  satisfy_const(gf,v1,v2,v,dire);

  /*v->mark = True;
  insert_v(v,gf);*/

  if(find_vertex(gf,&(v->loc)))
    {
      dire = compute_triangle(v,v1,v2,ec->cw,(CON_UNIT/2)*(gf->vsize));
      satisfy_const(gf,v1,v2,v,dire);
    }

  verify_insert_v(gf,v);

  if(recompute_faces_queue(gf,Q))
    reorder_queue(gf,Q);

  /* Insert new edges in priority Queue */
  {
    struct edge *e1 = find_edge(gf,v,v1);
    struct edge *e2 = find_edge(gf,v,v2);
    Edge_con *nec;
    if(e1)
      {
	nec = (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e1);
	Enqueue_prior(Q,nec);
      }
    if(e2)
      {
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e2);
	Enqueue_prior(Q,nec);
      }
  }
}

int
compute_triangle(struct pt *v, struct pt *v1, struct pt *v2,int cw,int d)
{
  int x, y;

  x = v2->loc.x - v1->loc.x;
  y = -v2->loc.y + v1->loc.y;
  
  if(v1->loc.y == v2->loc.y) /* ang = 0 or ang = PI */
    {
      int l = (2*(cw) - 1) * d;

      v->loc.x = v2->loc.x;

      if(v2->loc.x > v1->loc.x)
	  v->loc.y = v2->loc.y + l;
      else
	  v->loc.y = v2->loc.y - l;
	  
    }
  else if(v1->loc.x == v2->loc.x) /* ang = PI/2 or ang =3*PI/2 */
    {
      int l = (2*cw - 1) * d;

      v->loc.y = v2->loc.y;

      if(v2->loc.y > v1->loc.y)
	  v->loc.x = v2->loc.x - l;
      else
	  v->loc.x = v2->loc.x + l;
      
    }
  else
    {
      double ang = cal_angle(v1,v2);
      if(cw)
	{
	  if((ang >= PI/2 && ang <= PI) || 
	     (ang >= 3*PI/2 && ang <= 2*PI))
	    {
	      v->loc.x = v1->loc.x;
	      v->loc.y = v1->loc.y - y;
	    }
	  else
	    {
	      v->loc.x = v1->loc.x + x;
	      v->loc.y = v1->loc.y;
	    }
	  
	}
      else 
	{
	  if((ang >= PI/2 && ang <= PI) || 
	     (ang >= 3*PI/2 && ang <= 2*PI))
	    {
	      v->loc.x = v1->loc.x +  x;
	      v->loc.y = v1->loc.y;
	    }
	  else
	    {
	      v->loc.x = v1->loc.x;
	      v->loc.y = v1->loc.y - y;
	    }
	}
    }

  if((v->loc.x >= v1->loc.x)&&(v->loc.x >= v2->loc.x))
    {
      if((v->loc.y >= v1->loc.y)&&(v->loc.y >= v2->loc.y))
	return SEAST;
      else
	return NEAST;
    }
  else
    {
      if((v->loc.y >= v1->loc.y)&&(v->loc.y >= v2->loc.y))
	return SWEST;
      else
	return NWEST;
    }
}

void
place_square(GraphFrame *gf, Edge_con * ec, LNode_type *Q)
{
  struct pt *v3, *v4;
  struct pt *v1 = ec->e->from;
  struct pt *v2 = ec->e->to;
  /*double ang;*/

  find_two_unplaced_vertex(gf,ec,&v3,&v4);

  if(v3->mark == True || v4->mark == True)
    return;

  /*printf("placing %s and %s\n", v3->label,v4->label);*/
  /*ang = cal_angle(v1,v2);*/
  /*printf("v3 : %s, v4 %s, angle %f \n", v3->label, v4->label,ang* 57.29578);*/
  compute_square(v1,v2,v3,v4,ec->cw,CON_UNIT*(gf->vsize));
  /*v3->mark = True;
  insert_v(v3,gf);
  v4->mark = True;
  insert_v(v4,gf);*/

  if(find_vertex(gf,&(v3->loc)) || find_vertex(gf,&(v4->loc)))
    compute_square(v1,v2,v3,v4,ec->cw,(CON_UNIT/2)*(gf->vsize));

  verify_insert_v(gf,v3);
  verify_insert_v(gf,v4);

  if(recompute_faces_queue(gf,Q))
    reorder_queue(gf,Q);

  /* Insert new edges in priority Queue */
  {
    struct edge *e1 = find_edge(gf,v2,v3);
    struct edge *e2 = find_edge(gf,v3,v4);
    struct edge *e3 = find_edge(gf,v4,v1);
    Edge_con *nec;
    if(e1)
      {
	nec = (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e1);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
    if(e2)
      {
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e2);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
    if(e3)
      {
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e3);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
  }
}

void
compute_square(struct pt *v1, struct pt *v2, struct pt *v3, struct pt *v4, 
	       int cw,int d)
{
  double r = 
    (double)(d)/distance(v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y);

  if(cw)
    {
      v4->loc.x = v1->loc.x -(int)(r *(v2->loc.y - v1->loc.y));
      v4->loc.y = v1->loc.y +(int)(r *(v2->loc.x - v1->loc.x));
      v3->loc.x = v2->loc.x -(int)(r *(v2->loc.y - v1->loc.y));
      v3->loc.y = v2->loc.y +(int)(r *(v2->loc.x - v1->loc.x));
    }
  else
    {
      v4->loc.x = v1->loc.x +(int)(r *(v2->loc.y - v1->loc.y));
      v4->loc.y = v1->loc.y -(int)(r *(v2->loc.x - v1->loc.x));
      v3->loc.x = v2->loc.x +(int)(r *(v2->loc.y - v1->loc.y));
      v3->loc.y = v2->loc.y -(int)(r *(v2->loc.x - v1->loc.x));
    }
}

void
place_rhombus(GraphFrame *gf, Edge_con * ec, LNode_type *Q)
{
  struct pt *v3, *v4;
  struct pt *v1 = ec->e->from;
  struct pt *v2 = ec->e->to;

  find_two_unplaced_vertex(gf,ec,&v3,&v4);
  /*printf("placing rombus v1: %s, v2 : %s\n", v1->label,v2->label);*/

  if(v3->mark == False)
    {
      int dire = compute_triangle(v3,v4,v2,ec->cw,CON_UNIT*(gf->vsize));
      /*printf("placing %s\n", v3->label);*/
      satisfy_const_rh(gf,v1,v2,v4,v3,dire);

      if(find_vertex(gf,&(v3->loc)))
	{
	  dire = compute_triangle(v3,v4,v2,ec->cw,(CON_UNIT/2)*(gf->vsize));
	  satisfy_const_rh(gf,v1,v2,v4,v3,dire);
	}
      /*v3->mark = True;
      insert_v(v3,gf);*/
      verify_insert_v(gf,v3);

      if(recompute_faces_queue(gf,Q))
	reorder_queue(gf,Q);

      /* Insert new edges in priority Queue */
      {
	struct edge *e1 = find_edge(gf,v2,v3);
	struct edge *e2 = find_edge(gf,v3,v4);
	Edge_con *nec = (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e1);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e2);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }

    }
  else if(v4->mark == False)
    {
      int dire = compute_triangle(v4,v1,v3,ec->cw,CON_UNIT*(gf->vsize));

      /*printf("placing %s\n", v4->label);*/
      satisfy_const_rh(gf,v2,v1,v3,v4,dire);

      if(find_vertex(gf,&(v4->loc)))
	{
	  dire = compute_triangle(v4,v1,v3,ec->cw,(CON_UNIT/2)*(gf->vsize));
	  satisfy_const_rh(gf,v2,v1,v3,v4,dire);
	}
      /*v4->mark = True;
      insert_v(v4,gf);*/
      verify_insert_v(gf,v4);

      if(recompute_faces_queue(gf,Q))
	reorder_queue(gf,Q);

      /* Insert new edges in priority Queue */
      {
	struct edge *e1 = find_edge(gf,v3,v4);
	struct edge *e2 = find_edge(gf,v4,v1);
	Edge_con *nec = (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e1);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
	nec= (Edge_con *)malloc(sizeof(Edge_con));
	set_edge_con(gf,nec,e2);
	if(nec->unv > 0)
	  Enqueue_prior(Q,nec);
	else
	  free(nec);
      }
    }
}

Boolean
recompute_faces_queue(GraphFrame *gf,LNode_type *Q)
{
  LNode_type *ptr;
  Boolean change=False;
  for(ptr= Q; ptr->back != Q ; ptr = ptr->back)
    {
      Edge_con *ec = (Edge_con *)ptr->back->info;
      ec->touch = False;
      if(recompute_unplaced_vertex(gf,ec))
	change = True;
    }
  return change;
}
void
reorder_queue(GraphFrame *gf,LNode_type *Q)
{
  LNode_type *ptr;
  LNode_type *ltouch = init_linked_list();
  for(ptr= Q; ptr->back != Q ; ptr = ptr->back)
    {
      Edge_con *ec = (Edge_con *)ptr->back->info;
      if(ec->touch)
	{
	  Delete_linked_list(ptr->back);
	  Insert_linked_list(ltouch,create_new_node_llist((char *)ec));
	}
    }
  for(ptr=ltouch; ptr->back !=ltouch; ptr=ptr->back)
    {
      Edge_con *ec = (Edge_con *)ptr->back->info;
      if(ec->unv > 0)
	{
	  ec->touch = False;
	  Enqueue_prior(Q,ec);
	}
    }
  Delete_all_list(ltouch);
}
void
save_edges_queue(GraphFrame *gf, LNode_type *troute, LNode_type *Q)
{
  LNode_type *ptr;
  for(ptr= troute; ptr->back != troute ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      struct pt *v2;
      struct edge *e;
      Edge_con *ec = (Edge_con *)malloc(sizeof(Edge_con));
      if(ptr->back->back == troute)
	v2 = (struct pt *)troute->back->info;
      else
	v2 = (struct pt *)ptr->back->back->info;
      e = find_edge(gf,v,v2);
      set_edge_con(gf,ec,e);
      if(ec->unv > 0)
	Enqueue_prior(Q,ec);
      else
	free(ec);
    }
}

void
set_edge_con(GraphFrame *gf,Edge_con *ec, struct edge *e)
{
  ec->e = e;
  compute_faces_vertex(gf,ec,e);
}

Boolean
recompute_unplaced_vertex(GraphFrame *gf, Edge_con *ec)
{
  struct edge *e = ec->e;
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  int uv;
  struct pt *par, *vch, *nv;

  if(!ec->cw) /*going clock wise */
    {
      uv = 0; 
      par = v1;
      vch = v2;
      nv = next_vertex_const(par,vch);
      while(nv != v1)
	{
	  if(nv->mark == False)
	    uv++;
	  par = vch;
	  vch = nv;
	  nv = next_vertex_const(par,vch);
	}
    }
  else
    {
      uv = 0; 
      par = v1;
      vch = v2;
      nv = back_vertex_const(par,vch);
      while(nv != v1)
	{
	  if(nv->mark == False)
	    uv++;
	  par = vch;
	  vch = nv;
	  nv = back_vertex_const(par,vch);
	}
    }
  if(uv!= ec->unv)
    {
      ec->unv = uv;
      ec->touch = True;
      return True;
    }
  return False;
}

void
compute_faces_vertex(GraphFrame *gf, Edge_con *ec, struct edge *e)
{
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  int lf1,lf2,uv2,uv1;
  struct pt *par, *vch, *nv;
  Boolean noface1 = False, noface2 = False;

  ec->lface = -1;
  ec->unv = -1;

  /* going clock wise from v1 */
  uv1 = 0; lf1 = 1;
  par = v1;
  vch = v2;
  nv = next_vertex_const(par,vch);
  while(nv != v1)
    {
      if(nv->mark == False)
	uv1++;
      lf1++;
      par = vch;
      vch = nv;
      nv = next_vertex_const(par,vch);
      if(nv == NULL)
	{
	  noface1 = True;
	  break;
	}
    }

  /* going counter clock wise from v1 */
  uv2 = 0; lf2 = 1;
  par = v1;
  vch = v2;
  nv = back_vertex_const(par,vch);
  while(nv != v1)
    {
      if(nv->mark == False)
	uv2++;
      lf2++;
      par = vch;
      vch = nv;
      nv = back_vertex_const(par,vch);
      if(nv == NULL)
	{
	  noface2 = True;
	  break;
	}
    }
  if((noface1 == False) &&(noface2 ==False))
    {
      if((uv1 != 0)&&(uv2 != 0))
	{
	  /*printf("choosing between %d, %d\n", lf1,lf2);*/
	  if(lf1 < lf2)
	    {
	      ec->lface = lf1;
	      ec->unv = uv1;
	      ec->cw = False;
	    }
	  else
	    {
	      ec->lface = lf2;
	      ec->unv = uv2;
	      ec->cw = True;
	    }
	}
      else
	{
	  if(uv1!=0)
	    {
	      ec->lface = lf1;
	      ec->unv = uv1;
	      ec->cw = False;
	    }
	  else if(uv2!=0)
	    {
	      ec->lface = lf2;
	      ec->unv = uv2;
	      ec->cw = True;
	    }
	}
    }
  else
    {
      if(noface1 == False)
	{
	  if(uv1!=0)
	    {
	      ec->lface = lf1;
	      ec->unv = uv1;
	      ec->cw = False;
	    }
	}
      else if(noface2 == False)
	{
	  if(uv1!=0)
	    {
	      ec->lface = lf1;
	      ec->unv = uv1;
	      ec->cw = False;
	    }
	}
    }
}
void
compute_faces_vertex2(GraphFrame *gf, Edge_con *ec, struct edge *e)
{
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  int lf,uv;
  struct pt *par, *vch, *nv;
  Boolean noface = False;
  /* going clock wise from v1 */
  uv = 0; lf = 1;
  par = v1;
  vch = v2;
  nv = next_vertex_const(par,vch);
  while(nv != v1)
    {
      if(nv->mark == False)
	uv++;
      lf++;
      par = vch;
      vch = nv;
      nv = next_vertex_const(par,vch);
      if(nv == NULL)
	{
	  noface = True;
	  break;
	}
    }
  if(noface == False)
    {
      if(uv != 0)
	{
	  ec->lface = lf;
	  ec->unv = uv;
	  ec->cw = False;
	  return;
	}
    }

  /* going counter clock wise from v1 */
  uv = 0; lf = 1;
  par = v1;
  vch = v2;
  nv = back_vertex_const(par,vch);
  while(nv != v1)
    {
      if(nv->mark == False)
	uv++;
      lf++;
      par = vch;
      vch = nv;
      nv = back_vertex_const(par,vch);
      if(nv == NULL)
	{
	  noface = True;
	  break;
	}
    }
  if(noface == False)
    {
      if(uv != 0)
	{
	  ec->lface = lf;
	  ec->unv = uv;
	  ec->cw = True;
	  return;
	}
    }
  ec->lface = -1;
  ec->unv = -1;
}

void
place_route(GraphFrame *gf,LNode_type *troute)
{
  LNode_type *ptr=troute;
  struct pt *v,*v2;
  int d = CON_UNIT*gf->vsize;
  v = (struct pt *)ptr->back->info;
  v->loc.x = GR_MWIDTH/2;
  v->loc.y = GR_MHEIGHT/2;

  /*v->mark = True;
  insert_v(v,gf);*/
  verify_insert_v(gf,v);
  ptr = ptr->back;
  v2 = (struct pt *)ptr->back->info;
  v2->loc.x = v->loc.x+d;
  v2->loc.y = v->loc.y;
  /*v2->mark = True;
  insert_v(v2,gf);*/
  verify_insert_v(gf,v2);
  ptr = ptr->back;
  v = (struct pt *)ptr->back->info;
  v->loc.x = v2->loc.x;
  v->loc.y = v2->loc.y-d;
  /*v->mark = True;
  insert_v(v,gf);*/
  verify_insert_v(gf,v);
  ptr = ptr->back;
  v2 = (struct pt *)ptr->back->info;

  if(v2 != NULL)
    {
      v2->loc.x = v->loc.x-d;
      v2->loc.y = v->loc.y;
      /*v2->mark = True;
      insert_v(v2,gf);*/
      verify_insert_v(gf,v2);
    }
}

Boolean
find_route_4(GraphFrame *gf,struct pt *v, LNode_type *troute)
{
  LNode_type *ptr;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      struct pt *atv[4];
      atv[0] = v;
      atv[1] = tv;
      atv[2] = next_vertex_const(atv[0],atv[1]);
      atv[3] = next_vertex_const(atv[1],atv[2]);
      if(atv[3] == atv[0])
	continue;
      if(next_vertex_const(atv[2],atv[3])==atv[0])
	{
	  int i;
	  /*printf("face ");*/
	  for(i = 0; i < 4; i++)
	    {
	      Insert_linked_list(troute,create_new_node_llist((char *)atv[i]));
	      /*printf("%s ",atv[i]->label);*/
	    }
	  /*printf("\n");*/
	  return True;
	}
    }
  return False;
}
Boolean
find_route_3(GraphFrame *gf,struct pt *v, LNode_type *troute)
{
  LNode_type *ptr;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      struct pt *ntv =next_vertex_const(v,tv);

      if(next_vertex_const(tv,ntv)==v)
	{
	  /*printf("face ");*/
	  Insert_linked_list(troute,create_new_node_llist((char *)v));
	  Insert_linked_list(troute,create_new_node_llist((char *)tv));
	  Insert_linked_list(troute,create_new_node_llist((char *)ntv));
	  /*printf("%s %s %s\n",v->label, tv->label, ntv->label);*/
	  return True;
	}
    }
  return False;
}

int
parse_constraints(char *sconst,int **ncons,int n)
{
  Boolean endfile = False;
  char line[SIZELINE];
  int j = 0, i = 0;
  
  while (!endfile)
    {
      endfile = read_text_line(sconst,line,&i);  
      if(line[0] == '\0')
	continue;

      if(j > n)
	{
	  fprintf(stderr,"Line %d :Error in constraint file\n",j);
	  error =  ERROR_CONST_FILE;
	  return ERROR_CONST_FILE;
	}
      error = parse_line_constr(line, ncons[j++]);
      if(error != NO_ERROR)
	break;
    } 
  if(j!=n)
    {
      fprintf(stderr,"Line %d: Error in constraint file\n",j);
      error = ERROR_CONST_FILE;
      return ERROR_CONST_FILE;
    }
  return 0;
}

int
parse_line_constr(char *line, int *ncons)
{
  char *guion = "-";
  char *comma = ",";
  char *p;
  int i = 0;

  if(!ncons)
    return ERROR_CONST_FILE;

  p = strtok(line,guion);
  ncons[i++] = atoi(p);
  while(p != NULL)
    {
      p = strtok(NULL,comma);
      if((p != NULL)&&(i < 5))
	ncons[i++] = atoi(p);
    }
  /*for(i = 0 ; i< 5; i++)
    printf("%d ",ncons[i]);
  printf("\n");*/
  return 0;
}

void
set_constraints_nodes(GraphFrame *gf, int **ncons, int n)
{
  LNode_type *ptr;
  int i;

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      struct pt *tv;
      int *lcons = find_line_constr(v,ncons,n);
      char vlabel[80];
      struct edge *e;
      v->lver_con = init_linked_list();
      for(i = 1; i<= 4 ; i++)
	{
	  if(lcons[i] <= 0)
	    continue;
	  sprintf(vlabel,"%d",lcons[i]);
	  tv = find_vertex_by_name(gf,vlabel);
	  e = find_edge(gf,v,tv);
	  if(!e)
	    {
	      fprintf(stderr,"Vertex %s: Error in constraint file\n",v->label);
	      error = ERROR_CONST_FILE;
	    }
	  if(tv)
	    Insert_linked_list(v->lver_con,create_new_node_llist((char *)tv));
	  else
	    {
	      fprintf(stderr,"Vertex %s: Error in constraint file\n",v->label);
	      error = ERROR_CONST_FILE;
	    }
	}
    }
  /*
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      LNode_type *ptr2;
      printf("node %s :",v->label);
      for(ptr2= v->lver_con; ptr2->back != v->lver_con ; ptr2 = ptr2->back)
	{
	  struct pt *tv = (struct pt *)ptr2->back->info;
	  printf("%s ", tv->label);
	}
      printf("\n");
    }
    */
}

int *
find_line_constr(struct pt *v, int **ncons, int n)
{
  int i, info = atoi(v->label); /*this is not right */
  for(i=0;i< n ; i++)
    if(ncons[i][0] == info)
      return ncons[i];
  return NULL;
}

struct pt *
next_vertex_const(struct pt *par, struct pt *v)
{ 
  LNode_type *ptr;
  struct pt *vd = NULL;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      if(par == tv)
	{
	  LNode_type *tptr = ptr->back;
	  if(tptr->back == v->lver_con)
	    vd = (struct pt *)tptr->back->back->info;
	  else
	    vd = (struct pt *)tptr->back->info;

	  if(vd != par)
	    return vd;
	  else
	    return NULL;
	}
    }
  return NULL;
}
struct pt *
back_vertex_const(struct pt *par, struct pt *v)
{ 
  LNode_type *ptr;
  struct pt *vd = NULL;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      if(par == tv)
	{
	  LNode_type *tptr = ptr->back;
	  if(tptr->next == v->lver_con)
	    vd = (struct pt *)tptr->next->next->info;
	  else
	    vd = (struct pt *)tptr->next->info;

	  if(vd!= par)
	    return vd;
	  else
	    return NULL;
	}
    }
  return NULL;
}
void
print_queue(GraphFrame *gf, LNode_type *Q)
{
  LNode_type *ptr;
  printf("Queue : \n");
  for(ptr= Q; ptr->back != Q ; ptr = ptr->back)
    {
      Edge_con *ec = (Edge_con *)ptr->back->info;
      print_ec(ec);
    }
}
void 
print_ec(Edge_con *ec)
{
  struct edge *e = ec->e;
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  printf("Edge %s - %s\n", v1->label, v2->label);
  printf("Faces : %d , UnVer : %d , CW : %d\n", ec->lface, ec->unv,ec->cw);
}
void
Enqueue_prior(LNode_type *Q, Edge_con *ec)
{
  LNode_type *ptr,*t;
  LNode_type *nnode;
  for(ptr= Q; ptr->back != Q ; ptr = ptr->back)
    {
      Edge_con *tec = (Edge_con *)ptr->back->info;
      if(ec->unv < tec->unv)
	{
	  nnode = create_new_node_llist((char *)ec);
	  t = ptr->back;
	  nnode->next = t->next;
	  t->next = nnode;
	  nnode->next->back = nnode;
	  nnode->back = t;
	  return;
	}
    }  
  Enqueue(Q,(char *)ec);
}


struct pt *
find_unplaced_vertex(GraphFrame *gf, Edge_con * ec)
{
  struct edge *e = ec->e;
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  if(ec->cw == False)
    return next_vertex_const(v1,v2);
  else
    return back_vertex_const(v1,v2);
}

void
find_two_unplaced_vertex(GraphFrame *gf, Edge_con * ec,struct pt **v3,
			 struct pt **v4)
{
  struct edge *e = ec->e;
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;

  if(ec->cw == False)
    {
      *v3 = next_vertex_const(v1,v2);
      *v4 = next_vertex_const(v2,*v3);
    }
  else
    {
      *v3 = back_vertex_const(v1,v2);
      *v4 = back_vertex_const(v2,*v3);
    }
}
/*
void
swap_nodes(LNode_type *p,LNode_type *s)
{
  LNode_type *t;
  t = p->back;
  t->next = s;
  s->back = t;
  
  t = s->next;
  s->next = p;
  p->next = t;
  t->back = p;
  p->back = s;
}
*/

double
distance(double x1, double y1, double x2, double y2)
{
  return(sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
}

double
cal_angle(struct pt *v,struct pt *r)
{
  double ang;
  double y = r->loc.y-v->loc.y;
  double x = r->loc.x-v->loc.x;

  /*ang = atan2(r->loc.y-v->loc.y, r->loc.x-v->loc.x);*/
  ang = atan2(y,x);
  /*printf("y = %f, x = %f\n",y, x);*/
  /*return (ang * 57.29578);*/
  if(ang <0)
    return -ang;

  ang = 2*PI -ang;

  if(ang >= 2*PI)
    return (2*PI - ang);

  return ang;
}

void
initialize_const(int **ncons, int n)
{
  int i,j;
  for(i=0;i<n;i++)
    for(j=0;j<=5;j++)
      ncons[i][j]=0;
}

void
satisfy_const(GraphFrame *gf, struct pt *v1, struct pt *v2,struct pt *v,
	      int dire)
{
  double ang[4];
  LNode_type *ptr;
  struct pt *v5=NULL, *v6=NULL;
  int i;
  int minx, miny, maxx, maxy;
 
  for(i=0;i<4;i++)
    ang[i]=-1;
  i=0;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      if(tv->mark)
	  ang[i++]=cal_angle(v,tv);
    }

  v5 = next_vertex_const(v2,v);
  if(v5 == v1)
    v5 = back_vertex_const(v2,v);
  v6 = next_vertex_const(v1,v);
  if(v6 == v2)
    v6 = back_vertex_const(v1,v);

  if(i<=2)
    return;
  if(i==3)
    {
      if(!clock_wise(ang[0],ang[1],ang[2]))
	{
	  int d = CON_UNIT*gf->vsize;

	  /*printf("angles are: %f, %f, %f\n",ang[0]* 57.29578,ang[1]* 57.29578,ang[2]* 57.29578);
	  printf("BREAKING CONSTRAIN TRIANGLE 3: %s\n",v->label);*/

	  find_bounding_box(v1,v2,v5,v6, &minx, &miny, &maxx, &maxy);

	  /*printf("BREAKING CONSTRAIN 3: %s\n",v->label);*/

	  switch(dire)
	    {
	    case NEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = miny - d;
	      break;
	    case SEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = maxy + d;
	      break;
	    case NWEST:
	      v->loc.x = minx - d;
	      v->loc.y = miny - d;
	      break;
	    case SWEST:
	      v->loc.x = minx - d;
	      v->loc.y = maxy + d;
	      break;
	    }
	}
    }
  else if(i==4)
    {
      if(!clock_wise_4(ang[0],ang[1],ang[2],ang[3]))
	{
	  int d = CON_UNIT*gf->vsize;

	  /*printf("BREAKING CONSTRAIN TRIANGLE 4: %s\n",v->label);
	  printf("angles are: %f, %f, %f,%f\n",ang[0]* 57.29578,ang[1]* 57.29578,ang[2]* 57.29578,ang[3]* 57.29578);*/

	  find_bounding_box(v1,v2,v5,v6, &minx, &miny, &maxx, &maxy);

	  /*printf("BREAKING CONSTRAIN 3: %s\n",v->label);*/

	  switch(dire)
	    {
	    case NEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = miny - d;
	      break;
	    case SEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = maxy + d;
	      break;
	    case NWEST:
	      v->loc.x = minx - d;
	      v->loc.y = miny - d;
	      break;
	    case SWEST:
	      v->loc.x = minx - d;
	      v->loc.y = maxy + d;
	      break;
	    }
	}
    }
   
}

void
satisfy_const_rh(GraphFrame *gf, struct pt *v1, struct pt *v2, 
		 struct pt *v3,struct pt *v,int dire)
{
  double ang[4];
  LNode_type *ptr;
  struct pt *v5=NULL, *v6=NULL;
  int i;
  int minx, miny, maxx, maxy;
  
  for(i=0;i<4;i++)
    ang[i]=-1;
  i=0;
  for(ptr= v->lver_con; ptr->back != v->lver_con ; ptr = ptr->back)
    {
      struct pt *tv = (struct pt *)ptr->back->info;
      if(tv->mark)
	  ang[i++]=cal_angle(v,tv);
    }
  v5 = next_vertex_const(v2,v);
  if(v5 == v3)
    v5 = back_vertex_const(v2,v);
  v6 = next_vertex_const(v3,v);
  if(v6 == v2)
    v6 = back_vertex_const(v3,v);

  /*printf("V5 %s, V6 %s\n", v5->label, v6->label);*/
  
  if(i<=2)
    return;
  if(i==3)
    {
      if(!clock_wise(ang[0],ang[1],ang[2]))
	{
	  int d = CON_UNIT*gf->vsize;
	  
	  find_bounding_box(v2,v3,v5,v6, &minx, &miny, &maxx, &maxy);

	  /*printf("BREAKING CONSTRAIN 3: %s\n",v->label);*/

	  switch(dire)
	    {
	    case NEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = miny - d;
	      break;
	    case SEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = maxy + d;
	      break;
	    case NWEST:
	      v->loc.x = minx - d;
	      v->loc.y = miny - d;
	      break;
	    case SWEST:
	      v->loc.x = minx - d;
	      v->loc.y = maxy + d;
	      break;

	    }
	}
    }
  else if(i==4)
    {
      /*printf("angles are: %f, %f, %f,%f\n",ang[0]* 57.29578,ang[1]* 57.29578,ang[2]* 57.29578,ang[3]* 57.29578);*/
      if(!clock_wise_4(ang[0],ang[1],ang[2],ang[3]))
	{
	  int d = CON_UNIT*gf->vsize;

	  find_bounding_box(v2,v3,v5,v6, &minx, &miny, &maxx, &maxy);

	  /*printf("BREAKING CONSTRAIN 4: %s\n",v->label);*/
	  switch(dire)
	    {
	    case NEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = miny - d;
	      break;
	    case SEAST:
	      v->loc.x = maxx + d;
	      v->loc.y = maxy + d;
	      break;
	    case NWEST:
	      v->loc.x = minx - d;
	      v->loc.y = miny - d;
	      break;
	    case SWEST:
	      v->loc.x = minx - d;
	      v->loc.y = maxy + d;
	      break;
	    }
	}
    }
}
Boolean
clock_wise(double a, double b,double c)
{
  double cmpa = 2*PI -a;
  a = 2*PI;
  b = ang_sup(b+cmpa);
  c = ang_sup(c+cmpa);
  if(a > b && b > c)
    return True;
  return False;
}
Boolean
cc_clock_wise(double a, double b,double c)
{
  double cmpa = -a;
  a = 0;
  b = ang_sup(b+cmpa);
  c = ang_sup(c+cmpa);
  if(a < b && b < c)
    return True;
  return False;
}

Boolean
clock_wise_4(double a, double b,double c,double d)
{
  double cmpa = 2*PI -a;
  a = 2*PI;
  b = ang_sup(b+cmpa);
  c = ang_sup(c+cmpa);
  d = ang_sup(d+cmpa);
  if(a > b && b > c && c > d)
    return True;
  return False;
}

Boolean
cc_clock_wise_4(double a, double b,double c,double d)
{
  double cmpa = -a;
  a = 0;
  b = ang_sup(b+cmpa);
  c = ang_sup(c+cmpa);
  d = ang_sup(d+cmpa);
  if(a < b && b < c && c < d)
    return True;
  return False;
}

double
ang_sup(double angle)
{
  if(angle> 2*PI)
    return (angle - 2*PI);
  if(angle < 0)
    return (2*PI + angle);
  return angle;
}

Boolean
is_increased(double ang)
{
  if(ang==PI/2)
    return True;
  if(ang == 3*PI/2)
    return False;
  if(ang > 0 && ang < PI/2)
    return True;
  if(ang > PI && ang < 3*PI/2)
    return True;
  return False;
}

double
min_d(double a, double b)
{
  if(a < b)
    return a;
  return b;
}

double
max_d(double a, double b)
{
  if(a > b)
    return a;
  return b;
}

void
find_bounding_box(struct pt *v1, struct pt *v2,struct pt *v3,struct pt *v4,
		  int *minx, int *miny, int *maxx, int *maxy)
{
  *minx = v1->loc.x;
  *miny = v1->loc.y;
  *maxx = v1->loc.x;
  *maxy = v1->loc.y;

  if(v2->loc.x > *maxx)
    *maxx = v2->loc.x;
  if(v2->loc.x < *minx)
    *minx = v2->loc.x;
  if(v2->loc.y > *maxy)
    *maxy = v2->loc.y;
  if(v2->loc.y < *miny)
    *miny = v2->loc.y;

  if(v3->mark == True)
    {
      if(v3->loc.x > *maxx)
	*maxx = v3->loc.x;
      if(v3->loc.x < *minx)
	*minx = v3->loc.x;
      if(v3->loc.y > *maxy)
	*maxy = v3->loc.y;
      if(v3->loc.y < *miny)
	*miny = v3->loc.y;
    }
  if(v4->mark == True)
    {
      if(v4->loc.x > *maxx)
	*maxx = v4->loc.x;
      if(v4->loc.x < *minx)
	*minx = v4->loc.x;
      if(v4->loc.y > *maxy)
	*maxy = v4->loc.y;
      if(v4->loc.y < *miny)
	*miny = v4->loc.y;
    }
}

double
average_angle(double a, double b)
{
  double c = a + fabs(b-a)/2;
  if(clock_wise(a,c,b))
    return c;
  return ang_sup(c+PI);
}

void
delete_constrains_vertex(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v->lver_con)
	Delete_all_list(v->lver_con);
      v->lver_con = NULL;
    }
}

void
delete_constrains_array(int **ncons,int n)
{
  int i;
  for(i = 0; i< n; i++)
      free(ncons[i]);
  free(ncons);
}
