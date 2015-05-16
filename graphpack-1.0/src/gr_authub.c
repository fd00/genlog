#include "gr_authub.h"
#include "gr_operations.h"
#include "gr_util.h"

#define ANODE 0
#define AEDGE 1

typedef struct ahub {
  double x;
  double y;
  void * ele;
} AHUB;

typedef AHUB *PHUB;

#define EPSIL 0.00001
#define EPSIL2 0.000001
#define EPSIL3 0

#define NORM_1 1
#define NORM_INF 2

void
add_y_values_of_parent(PHUB hv);
void
init_ahub(PHUB hv,void *ele);
void
add_x_values_of_children(PHUB hv);
void
assign_big_auth(GraphFrame *gf,PHUB *A);
void
assign_big_hub(GraphFrame *gf,PHUB *A);
int
is_auth_lower(PHUB u, PHUB v);
int 
is_auth_bigger(PHUB u, PHUB v);
int 
is_hub_bigger(PHUB u, PHUB v);
int 
is_hub_bigger(PHUB u, PHUB v);

void 
delete_author_hub_list(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lauthub)
    {
      for(ptr = gf->lauthub->back; ptr != gf->lauthub; ptr = ptr->back)
	{
	  free(ptr->info); ptr->info = NULL;
	}
      Delete_all_list(gf->lauthub);
      gf->lauthub = NULL;
    }
}

void
init_ahub(PHUB hv,void * ele)
{
  hv->x = 1.0;
  hv->y = 1.0;
  hv->ele = ele;
}

void
initialize_author_hub_list(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lauthub)
    {
      delete_author_hub_list(gf);
    }

  gf->lauthub = init_linked_list();

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; 
      ptr= ptr->back)
    {
      struct pt *ov = (struct pt *)ptr->info;
      PHUB hv = NULL;

      ov->modifier = 0;

      /* Do not compute Kleinberg's algorithm in highlight vertices */
      if(ov->highlight)
	  continue;

      hv = (PHUB)malloc(sizeof(AHUB));
      if(!hv) {
	  fprintf(stderr,"Out of Memory initialize_author_hub_list \n");
	  abort();
      }
      init_ahub(hv,(void *)ov);
      ov->modifier = (int)hv;
      Insert_linked_list(gf->lauthub, create_new_node_llist((char *)hv));
    }
}

void
add_y_values_of_parent(PHUB hv)
{
  PNODE v  = (PNODE)hv->ele;
  LNode_type *ne;

  if(is_empty_list(v->ledges_in))
    return;

  hv->x = 0.0;
  for(ne = v->ledges_in->next; ne != v->ledges_in; ne = ne->next)
    {
      PEDGE e = (PEDGE)ne->info;
      PNODE u = e->from;
      PHUB hu = (PHUB)u->modifier;
      hv->x += hu ? hu->y : 0;
    }
}


void
apply_I_operation(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each vertex */
      add_y_values_of_parent(hv);
    }
}

void
add_x_values_of_children(PHUB hv)
{
  PNODE v  = (PNODE)hv->ele;
  LNode_type *ne;

  if(is_empty_list(v->ledges_out))
    return;

  hv->y = 0.0;
  for(ne = v->ledges_out->next; ne != v->ledges_out; ne = ne->next)
    {
      PEDGE e = (PEDGE)ne->info;
      PNODE u = e->to;
      PHUB hu = (PHUB)u->modifier;
      hv->y += hu ? hu->x : 0;
    }
}


void
apply_O_operation(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each vertex */
      add_x_values_of_children(hv);
    }
}

void
normalize3_x_and_y(GraphFrame *gf)
{
  LNode_type *ptr;
  double sum2x=0.0,sum2y=0.0;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      if (sum2x <  hv->x) sum2x=hv->x;
      if (sum2y <  hv->y) sum2y=hv->y;
    }

  if(sum2x == 0 || sum2y == 0)
    return;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      hv->x = hv->x / sum2x;
      hv->y = hv->y / sum2y;
    }
}

void
normalize2_x_and_y(GraphFrame *gf)
{
  LNode_type *ptr;
  double sum2x=0.0,sum2y=0.0;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      sum2x += hv->x;
      sum2y += hv->y;
    }

  if(sum2x == 0 || sum2y == 0)
    return;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      hv->x = hv->x / sum2x;
      hv->y = hv->y / sum2y;
    }
}
void
normalize_x_and_y(GraphFrame *gf)
{
  LNode_type *ptr;
  double sum2x=0.0,sum2y=0.0;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      sum2x += hv->x * hv->x;
      sum2y += hv->y * hv->y;
    }

  sum2x = sqrt(sum2x);
  sum2y = sqrt(sum2y);

  if(sum2x == 0 || sum2y == 0)
    return;

  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each element */
      hv->x = hv->x / sum2x;
      hv->y = hv->y / sum2y;
    }

}

void 
print_values_hub(GraphFrame *gf)
{
  LNode_type *ptr;
  printf("NEW VALUES OF HUBS\n");
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each vertex */
      printf("v=%d, x= %f, y =%f\n", ((PNODE)(hv->ele))->num, hv->x, hv->y);
    }
}  

void
compute_author_hub(GraphFrame *gf, int maxiter, int sel)
{
  int i;

  gf->alg_node_edge = ANODE;

  initialize_author_hub_list(gf);

  for(i = 0; i < maxiter ; i++)
    {
      /*print_values_hub(gf);*/
      apply_I_operation(gf);
      apply_O_operation(gf);

      if(sel == NORM_1)
	normalize2_x_and_y(gf);
      else if(sel == NORM_INF)
	normalize3_x_and_y(gf);
      else
	normalize_x_and_y(gf);
    }
}

int
how_many_elements_are_big_auth(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lauthub->next; ptr!= gf->lauthub ; ptr = ptr->next)
    {
      PHUB hv = (PHUB)ptr->info;
      if(hv->x > EPSIL3)
	c++;
    }
  return c;
}

int
how_many_elements_are_big_hub(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lauthub->next; ptr!= gf->lauthub ; ptr = ptr->next)
    {
      PHUB hv = (PHUB)ptr->info;
      if(hv->y > EPSIL3)
	c++;
    }
  return c;
}

void
assign_big_auth(GraphFrame *gf,PHUB *A)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lauthub->next; ptr!= gf->lauthub ; ptr = ptr->next)
    {
      PHUB hv = (PHUB)ptr->info;

      if(hv->x > EPSIL3)
	A[c++]=hv;
    }

}

void
assign_big_hub(GraphFrame *gf,PHUB *A)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lauthub->next; ptr!= gf->lauthub ; ptr = ptr->next)
    {
      PHUB hv = (PHUB)ptr->info;

      if(hv->y > EPSIL3)
	A[c++]=hv;
    }
}

int
is_auth_lower(PHUB u, PHUB v)
{
  return(u->x < v->x);
}

int 
is_auth_bigger(PHUB u, PHUB v)
{
  return(u->x > v->x);
}

int
is_hub_lower(PHUB u, PHUB v)
{
  return(u->y < v->y);
}

int 
is_hub_bigger(PHUB u, PHUB v)
{
  return(u->y > v->y);
}

int
select_big_auth(GraphFrame *gf, int mah)
{
  int c = 0;
  PHUB *A = NULL;

  if(gf->alg_node_edge!= ANODE)
      return -1;

  c = gf->lauthub ? how_many_elements_are_big_auth(gf) : 0;

  if(!c) 
  {
      fprintf(stderr,"No authorities\n");
      return 0;
  }

  A = c ? (PHUB *)calloc(c,sizeof(PHUB)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_big_auth\n");
      abort();
  }

  srand(time(NULL));

  assign_big_auth(gf,A);

  printf("SELECTED NODES\n");
  if(mah < c)
    {
      int i;
      for(i = 1; i<=mah ; i++)
	{
	  PHUB hv = (PHUB)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_auth_lower,is_auth_bigger);
	  PNODE v = (PNODE)hv->ele;
	  
	  if(!v->select)
	    select_vertex(v, gf);
	  if(v)
	    printf("%f,%f %d %s\n", hv->x, hv->y, v->num,v->label);
	}
      free(A); A = NULL;
      return mah;
    }
  else
    {
      int i;
      for(i = 0; i< c ; i++)
	{
	  PHUB hv = A[i];
	  PNODE v = (PNODE)hv->ele;
	  if(!v->select)
	    select_vertex(v, gf);
	}
      free(A); A = NULL;
      return c;
    }
}

int
select_big_hub(GraphFrame *gf, int mah)
{
  int c = 0;
  PHUB *A = NULL;

  if(gf->alg_node_edge!= ANODE)
      return 0;

  c = gf->lauthub ? how_many_elements_are_big_hub(gf) : 0;

  if(!c) 
  {
      fprintf(stderr,"No Hubs\n");
      return 0;
  }

  A = c ? (PHUB *)calloc(c,sizeof(PHUB)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_big_hub\n");
      abort();
  }

  srand(time(NULL));

  assign_big_hub(gf,A);

  printf("SELECTED NODES\n");
  if(mah < c)
    {
      int i;
      for(i = 1; i<=mah ; i++)
	{
	  PHUB hv = (PHUB)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_hub_lower,is_hub_bigger);
	  PNODE v = (PNODE)hv->ele;
	  
	  if(!v->select)
	    select_vertex(v, gf);
	  if(v)
	    printf("%f,%f %d %s\n", hv->x,hv->y,v->num,v->label);
	}
      free(A); A = NULL;
      return mah;
    }
  else
    {
      int i;
      for(i = 0; i< c ; i++)
	{
	  PHUB hv = A[i];
	  PNODE v = (PNODE)hv->ele;
	  if(!v->select)
	    select_vertex(v, gf);
	}
      free(A); A = NULL;
      return c;
    }
}

/************************************************************************/

void
add_y_values_of_parent_edges(PHUB hv);

void
initialize_author_hub_list_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lauthub)
    {
      delete_author_hub_list(gf);
    }

  gf->lauthub = init_linked_list();

  untouch_edges(gf);

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; ptr= ptr->back)
    {
      struct pt *v = (struct pt *)ptr->info;

      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in->back; ei != v->ledges_in; ei=ei->back)
	    {
	      PEDGE e = (struct edge *)ei->info;
	      if(e && e->touch == False)
		{
		  PHUB hv = (PHUB)malloc(sizeof(AHUB));
		  if(!hv) {
		      fprintf(stderr,"Out of Memory initialize_author_hub_list \n");
		      abort();
		  }	
		  init_ahub(hv,(void *)e);
		  e->context = (void *)hv;
		  Insert_linked_list(gf->lauthub, 
				     create_new_node_llist((char *)hv));

		  e->touch = True;
		}
	    }
	}
    }
}


void
add_y_values_of_parent_edges(PHUB hv)
{
  PEDGE e  = (PEDGE)hv->ele;
  LNode_type *ne;
  PNODE v = NULL;
  if(!e)
      return;

  v = e->from;

  if(is_empty_list(v->ledges_in))
    return;

  hv->x = 0.0;
  for(ne = v->ledges_in->next; ne != v->ledges_in; ne = ne->next)
    {
      PEDGE ei = (PEDGE)ne->info;
      PHUB hu = (PHUB)ei->context;
      hv->x += hu->y;
    }
}

void
apply_I_operation_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each edge */
      add_y_values_of_parent_edges(hv);
    }
}


void
add_x_values_of_children_edges(PHUB hv)
{
  PEDGE e  = (PEDGE)hv->ele;
  PNODE v;
  LNode_type *ne;
  if(!e)
      return;
  v = e->to;

  if(is_empty_list(v->ledges_out))
    return;

  hv->y = 0.0;
  for(ne = v->ledges_out->next; ne != v->ledges_out; ne = ne->next)
    {
      PEDGE ei = (PEDGE)ne->info;
      PHUB hu = (PHUB)ei->context;
      hv->y += hu->x;
    }
}



void
apply_O_operation_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each edge */
      add_x_values_of_children_edges(hv);
    }
}

void 
print_values_hub_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  printf("NEW VALUES OF HUBS\n");
  for(ptr = gf->lauthub->back; ptr != gf->lauthub; 
      ptr= ptr->back)
    {
      PHUB hv = (PHUB)ptr->info; /* For each edge */
      PEDGE e = (PEDGE)hv->ele;
      printf("u=%d, v=%d, x= %f, y =%f\n", e->from->num, e->to->num, 
	     hv->x, hv->y);
    }
}  

void
compute_author_hub_edges(GraphFrame *gf, int maxiter, int sel)
{
  int i;
  gf->alg_node_edge = AEDGE;
  initialize_author_hub_list_edges(gf);

  for(i = 0; i < maxiter ; i++)
    {
	/*print_values_hub_edges(gf);*/
      apply_I_operation_edges(gf);
      apply_O_operation_edges(gf);

      if(sel == NORM_1)
	normalize2_x_and_y(gf);
      else if(sel == NORM_INF)
	normalize3_x_and_y(gf);
      else
	normalize_x_and_y(gf);
    }
}

int
select_big_auth_edges(GraphFrame *gf, int mah)
{
  int c = 0; 

  PHUB *A = NULL;

  if(gf->alg_node_edge!= AEDGE)
      return -1;

  c = gf->lauthub ? how_many_elements_are_big_auth(gf) : 0;
  A = c ? (PHUB *)calloc(c,sizeof(PHUB)) : NULL;
  if(!A) {
      fprintf(stderr,"Out of Memory select_big_auth\n");
      abort();
  }

  if(!c)
    return 0;

  srand(time(NULL));

  assign_big_auth(gf,A);

  printf("SELECTED EDGES\n");
  if(mah < c)
    {
      int i;
      for(i = 1; i<=mah ; i++)
	{
	  PHUB hv = (PHUB)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_auth_lower,is_auth_bigger);
	  PEDGE e = (PEDGE)hv->ele;
	  
	  if(!e->select)
	      select_edge_selecting_vertices(gf,e);
	  printf("%f,%f %d-%d %s-%s\n", hv->x, hv->y, e->from->num,e->to->num,
		 e->from->label,e->to->label);
	}
      free(A); A = NULL;
      return mah;
    }
  else
    {
      int i;
      for(i = 0; i< c ; i++)
	{
	  PHUB hv = A[i];
	  PEDGE e = (PEDGE)hv->ele;
	  if(!e->select)
	    select_edge_selecting_vertices(gf,e);
	  printf("%f,%f %d-%d %s-%s\n", hv->x, hv->y, e->from->num,e->to->num,
		 e->from->label,e->to->label);
	}
      free(A); A = NULL;
      return c;
    }
}

int
select_big_hub_edges(GraphFrame *gf, int mah)
{
  int c = 0;

  PHUB *A = NULL;

  if(gf->alg_node_edge!= AEDGE)
      return 0;

  c = gf->lauthub ? how_many_elements_are_big_hub(gf) : 0;

  A = c ? (PHUB *)calloc(c,sizeof(PHUB)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_big_auth\n");
      abort();
  }

  if(!c)
    return 0;

  srand(time(NULL));

  assign_big_hub(gf,A);

  printf("SELECTED EDGES\n");
  if(mah < c)
    {
      int i;
      for(i = 1; i<=mah ; i++)
	{
	  PHUB hv = (PHUB)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_hub_lower,is_hub_bigger);
	  PEDGE e = (PEDGE)hv->ele;
	  
	  if(!e->select)
	    select_edge_selecting_vertices(gf,e);

	  printf("%f,%f %d-%d %s-%s\n", hv->x, hv->y, e->from->num,e->to->num,
		 e->from->label,e->to->label);
	}
      free(A); A = NULL;
      return mah;
    }
  else
    {
      int i;
      for(i = 0; i< c ; i++)
	{
	  PHUB hv = A[i];
	  PEDGE e = (PEDGE)hv->ele;
	  if(!e->select)
	      select_edge_selecting_vertices(gf,e);
	  printf("%f,%f %d-%d %s-%s\n", hv->x, hv->y, e->from->num,e->to->num,
		 e->from->label,e->to->label);
	}
      free(A); A = NULL;
      return c;
    }
}

int
is_in_array(void **A,int n, void *p)
{
  int i;
  for(i = 0; i < n; i++)
  {
    if(A[i] == NULL)
       return 0;
    if(A[i] == p)
       return 1;
  }
  return 0;
}

int
select_big_nodes_from_hub_edges(GraphFrame *gf, int mah)
{
  int c = 0;
  int iv = 0;
  int i = 0;

  PHUB *A = NULL;

  PNODE *Vmax = NULL;

  if(gf->alg_node_edge!= AEDGE)
      return 0;

  c = gf->lauthub ? how_many_elements_are_big_hub(gf) : 0;

  A = c ? (PHUB *)calloc(c,sizeof(PHUB)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_big_nodes_from_hub_edges\n");
      abort();
  }

  Vmax = mah > 0 ? (PNODE *)calloc(mah+1,sizeof(PNODE)) : NULL;

  if(!Vmax) {
      fprintf(stderr,"Out of Memory select_big_nodes_from_hub_edges\n");
      abort();
  }

  if(!c)
    return 0;

  srand(time(NULL));

  assign_big_hub(gf,A);

  printf("Sorting hub EDGES\n");

  randomized_quicksort((GPTR *)A,1,c-1,is_hub_lower,is_hub_bigger);

  iv = 0;

  for(i = 1; i<=c-1 ; i++)
  {
    PHUB hv = A[i];
    PEDGE e = (PEDGE)hv->ele;
    PNODE v = e->to;
    if(!is_in_array((void **)Vmax,mah, (void *)v))
    {
	Vmax[iv++] = v;
	if(!v->select)
	  select_vertex(v, gf);
    }
    if(iv == mah)
	break;
  }

  for(i = 0; i< iv; i++)
  {
    printf("Vmax %d: %s\n", Vmax[i]->num, Vmax[i]->label);
  }

  free(A); A = NULL;
  free(Vmax); Vmax = NULL;

  return iv;
}

void
assign_biggest_degree(GraphFrame *gf, PNODE *A)
{
  int c = 0;
  LNode_type *ptr;

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; 
      ptr= ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      A[c++] = v;
    }
}

int
select_biggest_in_degree(GraphFrame *gf, int nbigin)
{
  int c = 0;
  PNODE *A = NULL;

  c = count_llist(gf->list_vertices);

  A = c ? (PNODE *)calloc(c,sizeof(PNODE)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_biggest_in_degree\n");
      abort();
  }

  if(!c)
    return 0;

  srand(time(NULL));

  assign_biggest_degree(gf,A);

  printf("SELECTED NODES\n");
  if(nbigin < c)
  {
    int i;
    for(i = 1; i<=nbigin ; i++)
    {
	PNODE v = (PNODE)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_vertex_num_edges_in_lower,
					   is_vertex_num_edges_in_bigger);
	if(!v->select)
	  select_vertex(v, gf);
	if(v)
	  printf("%d %d %s\n",get_num_edges_in(v),v->num,v->label);
    }
    free(A); A = NULL;
    return nbigin;
  }
  else
  {
    int i;
    for(i = 0; i< c ; i++)
    {
      PNODE v = A[i];
      if(!v->select)
	  select_vertex(v, gf);
    }	
    free(A); A = NULL;
    return c;
  }
}

int
select_biggest_out_degree(GraphFrame *gf, int nbigout)
{
  int c = 0;
  PNODE *A = NULL;

  c = count_llist(gf->list_vertices);

  A = c ? (PNODE *)calloc(c,sizeof(PNODE)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_biggest_in_degree\n");
      abort();
  }

  if(!c)
    return 0;

  srand(time(NULL));

  assign_biggest_degree(gf,A);

  printf("SELECTED NODES\n");
  if(nbigout < c)
  {
    int i;
    for(i = 1; i<=nbigout ; i++)
    {
	PNODE v = (PNODE)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_vertex_num_edges_out_lower,
					   is_vertex_num_edges_out_bigger);
	if(!v->select)
	  select_vertex(v, gf);
	if(v)
	  printf("%d %d %s\n",get_num_edges_out(v),v->num,v->label);
    }
    free(A); A = NULL;
    return nbigout;
  }
  else
  {
    int i;
    for(i = 0; i< c ; i++)
    {
      PNODE v = A[i];
      if(!v->select)
	  select_vertex(v, gf);
    }	
    free(A); A = NULL;
    return c;
  }
}








