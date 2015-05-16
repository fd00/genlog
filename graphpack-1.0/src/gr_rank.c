#include "gr_rank.h"

#define RANK_D 0.8

#define EPSIL3 0
 
void
initialize_rank_list(GraphFrame *gf)
{
    LNode_type *ptr;
    if(gf->lrank)
    {
	Delete_all_list(gf->lrank);
    }
    gf->lrank = init_linked_list();

    srand(time(NULL));
    if(gf->graph_dir == UNDIRECTED) {
    
    for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; 
      ptr= ptr->back)
    {
	PNODE v = (PNODE)ptr->info;
	v->modifier = 0;
	if(get_num_edges_in(v) > 1)
	{
	    v->modifier = rand();
	    Insert_linked_list(gf->lrank, create_new_node_llist((char *)v));
	}
    }

    } else {

    for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; 
      ptr= ptr->back)
    {
	PNODE v = (PNODE)ptr->info;
	v->modifier = 0;
	if(get_num_edges_out(v) > 0)
	{
	    v->modifier = rand();
	    Insert_linked_list(gf->lrank, create_new_node_llist((char *)v));
	}
    }
    }
}



int 
get_number_out_links_u(PNODE u)
{
    int n = 0;
    LNode_type *eo;    
    for(eo = u->ledges_in->next; eo != u->ledges_in; eo=eo->next)
      {
	  PEDGE e = (PEDGE)eo->info;

	  PNODE v = (PNODE)e->to;
	  if(v == u) v = e->from;

	  if(v->modifier > 0)
	      n++;
      }
    return n;
}

double 
sum_rank_u(PNODE v)
{
    LNode_type *ei;    
    double s = 0.0;
    double r = 0.0;
    int n = 0;
    for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
	  PEDGE e = (PEDGE)ei->info;
	  PNODE u = (PNODE) e->from;

	  if(u == v) u = e->to;
	      
	  if(u->modifier > 0)
	  {
	      n = get_number_out_links_u(u);
	      r = (double)(u->modifier) / (double) RAND_MAX;
	      if(!n)
		  break;
	      s += r / n ;
	  }	
      }
    return s;
}

int 
get_number_out_links(PNODE u)
{
    int n = 0;
    LNode_type *eo;    
    for(eo = u->ledges_out->next; eo != u->ledges_out; eo=eo->next)
      {
          PEDGE e = (PEDGE)eo->info;
          PNODE v = (PNODE)e->to;
          if(v->modifier > 0)
              n++;
      }
    return n;
}

double 
sum_rank(PNODE v)
{
    LNode_type *ei;    
    double s = 0.0;
    double r = 0.0;
    int n = 0;
    for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
          PEDGE e = (PEDGE)ei->info;
          PNODE u = (PNODE) e->from;
          if(u->modifier > 0)
          {
              n = get_number_out_links(u);
              r = (double)(u->modifier) / (double) RAND_MAX;
              if(!n)
                  break;
              s += r / n ;
          }
      }
    return s;
}

void
apply_R_operation(GraphFrame *gf)
{
    LNode_type *ptr;
    double C = 0;
    int m = count_llist(gf->lrank);

    if (!m)
	return;

    C = (1 - RANK_D) / (double)m ;


    if(gf->graph_dir == UNDIRECTED) {

    for(ptr = gf->lrank->back; ptr != gf->lrank; 
	ptr= ptr->back)
    {
	PNODE v = (PNODE)ptr->info;
	double r = 0.0;
	r = RANK_D * sum_rank_u(v) + C;
	v->modifier = (int) (r * RAND_MAX);
    }	
	

    } else {

    for(ptr = gf->lrank->back; ptr != gf->lrank; 
	ptr= ptr->back)
    {
	PNODE v = (PNODE)ptr->info;
	double r = 0.0;
	r = RANK_D * sum_rank(v) + C;
	v->modifier = (int) (r * RAND_MAX);
    }

    }
}

void 
print_values_rank(GraphFrame *gf)
{
    LNode_type *ptr;
    printf("\n\n");
    for(ptr = gf->lrank->back; ptr != gf->lrank; 
	ptr= ptr->back)
    {
	PNODE v = (PNODE)ptr->info;
	double r = (double)v->modifier / (double)RAND_MAX;
	printf("v=%d, r= %f\n", v->num,r);
    }
}

void
compute_rank(GraphFrame *gf, int maxiter)
{
    int i;

    initialize_rank_list(gf);

    for(i = 0; i < maxiter ; i++)
    {
	/*print_values_rank(gf);*/
	apply_R_operation(gf);
    }
}

int
how_many_elements_are_big_rank(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lrank->next; ptr!= gf->lrank ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      double r = (double)(v->modifier) / (double) RAND_MAX;
      if(r > EPSIL3)
	c++;
    }
  return c;
}

void
assign_big_rank(GraphFrame *gf,PNODE *A)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->lrank->next; ptr!= gf->lrank ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      double r = (double)(v->modifier) / (double) RAND_MAX;
      if(r > EPSIL3)
	A[c++]=v;
    }
}

int
is_rank_lower(PNODE u, PNODE v)
{
  return(u->modifier < v->modifier);
}

int 
is_rank_bigger(PNODE u, PNODE v)
{
  return(u->modifier > v->modifier);
}

int
select_big_rank(GraphFrame *gf, int mah)
{
  int c = 0;
  PNODE *A = NULL;

  c = gf->lrank ? how_many_elements_are_big_rank(gf) : 0;

  if(!c) 
  {
      fprintf(stderr,"No Ranking\n");
      return 0;
  }

  A = c ? (PNODE *)calloc(c,sizeof(PNODE)) : NULL;

  if(!A) {
      fprintf(stderr,"Out of Memory select_big_rank\n");
      abort();
  }

  srand(time(NULL));

  assign_big_rank(gf,A);

  printf("SELECTED NODES\n");
  if(mah < c)
    {
      int i;	
      double r=0.0;
      for(i = 1; i<=mah ; i++)
	{
	  PNODE v = (PNODE)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_rank_lower,is_rank_bigger);
	  if(!v->select)
	    select_vertex(v, gf);
	  if(v)
	  {
	    r = (double)(v->modifier) / (double) RAND_MAX;
	    printf("%f %d %s\n", r, v->num,v->label);
	  }
	}
      free(A); A = NULL;
      return mah;
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





