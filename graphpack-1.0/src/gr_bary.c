#include "gr_drawing.h"
#include "gr_bary.h"
#include "gr_structure.h"

/* compute_bary_positions - This function computes all positions of the 
                            nodes in baricentric way. It finds a cycle in
			    the graph of a given number. It places the cycle
			    in a circle and compute the rest of the nodes in
			    baricentric way inside of the circle. 
         inputs  
                 gf - Graph Structure
		 cycle_len - Length of the Cycle
         returns
                  0 - No problem in computing circular positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  WINDOW_FULL - The window can not hold more nodes.
		  NO_CYCLE - There is not cycle of cycle_len in the graph.
		  MEMORY_MATRIX - There is not memory for the matrix.
         side effects
                  The hash table of vertices changes completely. 
         notes
                 If a vertex can not be located in the circle, the vertex 
		 is placed in a empty location of the window. If two or
		 more vertices are in the same location, it finds a near 
		 spot and place all vertices in different positions.
*/
int 
compute_bary_positions(GraphFrame *gf, int cycle_len)
{
  int mes = 0;
  enumerate_vertices(gf);
  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  if(!gf->the_cycle)
    gf->the_cycle = init_linked_list();
  if(!gf->list_visited_vertex)
    gf->list_visited_vertex = init_linked_list();
  if(!gf->the_rest)
    gf->the_rest = init_linked_list();

  get_cycle(gf, cycle_len);
  /*printing_linked_list(gf->the_cycle);*/
  if(!is_empty_list(gf->the_cycle))
    {
      Delete_hash_table(gf->HV);
      
      circularize(gf,gf->the_cycle, cycle_len);
      get_rest(gf);
      /*printing_linked_list(gf->the_rest);*/
      if(!is_empty_list(gf->the_rest))
	mes = layout_rest(gf, gf->the_rest, gf->count_vertex-cycle_len);

    }
  else
    mes = NO_CYCLE;

  Delete_all_list(gf->the_cycle);
  Delete_all_list(gf->list_visited_vertex);
  Delete_all_list(gf->the_rest);
  gf->the_cycle = init_linked_list();
  gf->list_visited_vertex = init_linked_list();
  gf->the_rest = init_linked_list();
  return mes;
}
/* get_cycle - Get cycle of length "cycle_len" in the graph. It uses backtrack
               algorithm to find a cycle in the graph and it verifies that it
	       is length "cycle_len"

         inputs  
                 gf - Graph Structure
		 cycle_len - Length of the Cycle.
         returns
                  none
*/

void
get_cycle(GraphFrame *gf, int cycle_len)
{
  int i;
  struct pt *v;

  if(cycle_len > gf->count_vertex)
    return; 

  for(i = 1 ; i <= gf->count_vertex; i++)
    {
      v = get_vertex(i,gf);
      /*printf("first node %s \n", v->label);*/
      if(!v->mark)
	find_cycle(gf, v, 0, cycle_len);
      if(!is_empty_list(gf->the_cycle))
	break;
    }
}
/* find_cycle - This function verifies if the found cycle is the length 
                cycle_len. This function recursively visits all nodes of 
		the graph and marks a level in each node. 
         inputs  
                 gf - Graph Structure
		 v - Current visited Vertex
		 level - Level of the vertex in the graph.
		 cycle_len - Length of the desired cycle.
         returns
                  none
*/

void 
find_cycle(GraphFrame *gf, struct pt *v, int level, int cycle_len)
{
  LNode_type *poed;

  if(level > cycle_len)
    return;

  v->mark = True;
  v->pick = True;
  v->level = level;

  Insert_linked_list(gf->list_visited_vertex,create_new_node_llist((char*)v));

  /*printf(" level : %d , vertex %s \n", level, v->label);*/

  for(poed = v->ledges_out; poed->next != v->ledges_out; poed = poed->next)
    {
      struct edge *e = (struct edge *)poed->next->info;
      struct pt * tv = e->to;
      
      if(tv->pick)
	save_new_cycle(gf,tv,v,cycle_len);
      else if(!(tv->mark))
	find_cycle(gf,tv, level+1, cycle_len);
    }

  for(poed = v->ledges_in; poed->next != v->ledges_in; poed = poed->next)
    {
      struct edge *e = (struct edge *)poed->next->info;
      struct pt * tv = e->from;

      if(tv == v)
	tv = e->to;

      if(tv->pick)
	save_new_cycle(gf,tv,v,cycle_len);
      else if(!(tv->mark))
	find_cycle(gf,tv, level+1, cycle_len);
    }
  v->pick = False;
  v->mark = False;
  Delete_linked_list(Search_Sequential_list(gf->list_visited_vertex, 
					    (char *)v));
}
/* save_new_cycle - If a cycle is found of the given cycle_len, it saves all
                    vertices in a list. 
         inputs  
                 gf - Graph Structure
		 begin - Initial vertex
		 end - End Vertex
		 cycle_len - Length of the cycle
         returns
                  none 
*/

void
save_new_cycle(GraphFrame *gf,struct pt *begin, struct pt *end, int cycle_len)
{
  LNode_type *bptr, *eptr,*tptr;

  /*printf("begin %s, level %d, end %s, level  %d\n", 
	 begin->label, begin->level,end->label, end->level);*/

  if( (end->level - begin->level + 1) != cycle_len 
      || ! is_empty_list(gf->the_cycle))
    return;
  
  bptr = Search_Sequential_list(gf->list_visited_vertex, (char *)begin);
  eptr = Search_Sequential_list(gf->list_visited_vertex, (char *)end);
  
  /*printf ("found cycle\n");*/

  for(tptr = bptr ; tptr != eptr->back; tptr = tptr->back)
    {
      struct pt *v = (struct pt *)tptr->info;
      /*printf ("vcicle : %s ", v->label);*/
      Insert_linked_list(gf->the_cycle,create_new_node_llist((char*)v));
    }
  /*printf("\n");*/
}

/* reset_mark_pick_vertices - Reset all mark and pick flags in the graph.
                       I
         inputs  
                 gf - Graph Structure
         returns
                  none
*/
void
reset_mark_pick_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  v->mark = False;
	  v->pick = False;
	}
    }
}

void
reset_mark_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  v->mark = False;
	}
    }
}

void
reset_level_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	  v->level = -1;
    }
}
/* circularize - This function computes the circular positions of the nodes
                 in the cycle.

         inputs  
                 gf - Graph Structure
		 the_cycle - List of the vertices in the cycle
		 node_cnt - Number of nodes in the cycle
         returns
                  0 - No problem computing circular positions.
		  WINDOW_FULL - There is not empty locations in the window.
         side effects
                  The Hash table of the vertices changes completely.
*/

int
circularize(GraphFrame *gf, LNode_type *the_cycle, int node_cnt)
{
  Dimension X, Y;
  double pX, pY,rX, rY;
  double theta = 0.0;
  double delta = 2 * PI / node_cnt;
  LNode_type *ptr;
  int mes;

  mes = calculate_window_size(gf, &X, &Y);

  rX =  (double)(X / 2 - X/10);
  rY =  (double)(Y / 2 - Y/10);
  
  pX = (double)(X / 2);
  pY = (double)(Y / 2);


  for(ptr= the_cycle; ptr->back != the_cycle ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      Node_type *nver;

      v->loc.x = (int)(rX * cos(theta) + pX); 
      v->loc.y = (int)(rY * sin(theta) + pY);

      nver = select_nearest_vertex(&(v->loc),gf);
      if(nver)
	{
	  theta += delta;
	  v->loc.x = (int)(rX * cos(theta) + pX);
	  v->loc.y = (int)(rY * sin(theta) + pY);
	  nver = select_nearest_vertex(&(v->loc),gf);
	  if(nver)
	    {
	      if(find_empty_spot(v,gf))
		return WINDOW_FULL;
	    }
	}

      insert_v(v, gf);

      theta += delta;
      
      v->pick = True; /* Identify vertices that are in the_cycle */
    }
  return 0;
}
/* calculate_window_size - Calculates the size of the window depending on
                           the number of the vertices in the graph.

         inputs  
                 gf - Graph Structure
		 X , Y  - Dimensions of the Window
         returns
	         0 - no changes
		 RESIZE_WINDOW - Window changes size.
*/

int
calculate_window_size(GraphFrame *gf, Dimension *X, Dimension *Y)
{
  int noc = gf->count_vertex;
  int mes = 0;
  Dimension x=GR_MWIDTH-100, y=GR_MHEIGHT-100;

  if(!gf->pdraw)
    return mes;

  if((noc > MAX_NODES_WIN) && (noc < MAX_NODES_FULL_WIN))
    {
      x = gf->w;
      y = gf->h;
    }
  else if((noc >= MAX_NODES_FULL_WIN) && (noc < MAX_NODES_DOUBLE_WIN))
    {
      x = 2*GR_WIDTH; y = 2*GR_HEIGHT;
      gf->w =x; gf->h = y;
      mes = RESIZE_WINDOW;
    }
  else if((noc >= MAX_NODES_DOUBLE_WIN))
    {      
      x = 4*GR_WIDTH; y = 4*GR_HEIGHT;
      gf->w =x; gf->h = y;
      mes = RESIZE_WINDOW;
    }
  *X=x; *Y = y;

  return mes;
}
/* get_rest -  This function gets the rest of the vertices in the graph after
               the cycle is found. 
         inputs  
                 gf - Graph Structure
         returns
                  none
         notes
                 The rest of the vertices is saved in the list the_rest.

*/

void 
get_rest(GraphFrame *gf)
{
  LNode_type *ptr;
  int pos =0;

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
     
      if(!v->pick)
	{
	  Insert_linked_list(gf->the_rest,create_new_node_llist((char*)v));
	  v->level = pos;
	  ++pos;
	}
      else
	v->level = -1; 
    }
}
/* layout_rest - Computes the position of the rest of the vertices in the 
                 graph. It computes them in a barycentric way inside of the
		 circle of the vertices belong to the cycle. 
         inputs  
                 gf - Graph Structure.
		 the_rest - List of the rest of the vertices. 
		 rest_cnt - Number of vertices in the_rest list.
         returns
                  0 - No problem in placing the rest of the vertices.
		  MEMORY_MATRIX - No memory for the matrix.
		  WINDOW_FULL - There are not empty locations in the window.
*/

int
layout_rest(GraphFrame *gf, LNode_type *the_rest, int rest_cnt)
{
  int i,j;
  double **M = (double **)malloc(rest_cnt * sizeof(double *));
  LNode_type *ptr;

  if(!M) {
    fprintf(stderr,"Out of Memory layout_rest\n");
    abort();
  }

  for(i=0;i<rest_cnt;i++)
    {
      M[i]=(double *)malloc((rest_cnt+2)*sizeof(double));
      if(!M[i]) {
	  fprintf(stderr,"Out of Memory layout_rest\n");
	  abort();
      }
    }
  
  for(i = 0; i< rest_cnt; i++)
    {
      for(j=0;j< rest_cnt+2; j++)
	M[i][j]=0.0;
    }

  /* Fill in the matrix with -1.0 if i,j are adjacent, 0.0 otherwise (default)
   */
  
  ptr = gf->the_rest;
  for(i=0; i< rest_cnt; i++)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      int degree = 0;
      LNode_type *poed;

      for(poed = v->ledges_out; poed->next != v->ledges_out; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->next->info;
	  struct pt * tv = e->to;
	  int col = tv->level;
	  ++degree;
	  if(col == -1) continue;
	  M[i][col] = -1.0;

	}
      for(poed = v->ledges_in; poed->next != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->next->info;
	  struct pt * tv = e->from;
	  int col;

	  if(tv == v)
	    tv = e->to;
	  ++degree;
	  col = tv->level;
	  if(col == -1) continue;
	  M[i][col] = -1.0;

	}
      M[i][i] = degree ;
      ptr = ptr->back;
    }
  /* Fill in the x and y columns with the sum of adjacent nodes'x and y's. */
  ptr = gf->the_rest;
  for(i=0; i < rest_cnt; i++)
    {
      struct pt *u = (struct pt *)ptr->back->info;
      double x = 0.0, y = 0.0;
      LNode_type *poed;
      for(poed = u->ledges_in; poed->next != u->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->next->info;
	  struct pt *v = (struct pt *)e->from;
	  if(u == v)
	    v = e->to;

	  if(v->level == -1)
	    {
	      x += (double)(v->loc.x);
	      y += (double)(v->loc.y);
	    }
	}
      for(poed = u->ledges_out; poed->next != u->ledges_out; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->next->info;
	  struct pt *v = (struct pt *)e->to;
	  if(v->level == -1)
	    {
	      x += (double)(v->loc.x);
	      y += (double)(v->loc.y);
	    }
	}
      M[i][rest_cnt] = x;
      M[i][rest_cnt+1] = y;
      ptr = ptr->back;
    }
  /*print_matrix(M,rest_cnt,rest_cnt+2);*/
  solve_gauss_jordan(M, rest_cnt, rest_cnt+2);
  /*print_matrix(M,rest_cnt,rest_cnt+2);*/

  i = 0;
  for(ptr = gf->the_rest; ptr->back != gf->the_rest ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      Node_type *nver;

      v->loc.x = (int)(M[i][rest_cnt]);
      v->loc.y = (int)(M[i][rest_cnt + 1]);

      nver = select_nearest_vertex(&(v->loc), gf);
      if(nver)
	{
	  if(find_near_spot(v,gf))
	    return WINDOW_FULL;
	}
	  
      /*printf(" %d:%d, %d\n", i , v->loc.x, v->loc.y);*/
      i++;

      insert_v(v, gf);
    }
  delete_matrix_double(M,rest_cnt);
  return 0;
}
/* solve_gauss_jordan - Solve the equation system using Gauss - Jordan.

         inputs  
                 M - matrix
		 n, m - dimensions of the Matrix
         returns
                  none
*/

void
solve_gauss_jordan(double **M, int n, int m)
{
  int i,j,k;
  double factor;
  
  for(i=0;i< n-1; i++)
    {
      double fac = M[i][i];
      for(j = 0; j < m ; j++)
	M[i][j] /= fac;
      for (j = i+1; j < n; j++)
	{
	  fac = M[j][i];
	  for(k = i; k< m; k++)
	    M[j][k] = M[j][k] - fac * M[i][k];
	}
    }
  factor = M[n-1][n-1];
  for(j = 0; j < m; j++)
    M[n-1][j] /= factor;
  
  /* We solve for the position of the verties form the upper 
     triangular matrix */
  for(i = n-1; i > 0; --i)
    {
      for(j = 0; j < i; j++)
	{
	  double fac = M[j][i];
	  for(k = i-1; k < m; k++)
	    M[j][k] = M[j][k] - fac * M[i][k];
	}
    }
  for(i = 0; i< n ; i++)
    {
      M[i][n] = M[i][n] / M[i][i];
      M[i][n+1] = M[i][n+1] / M[i][i];
    }
}
/*
 */
int 
compute_sel_bary_positions(GraphFrame *gf)
{
  int mes = 0;
  int cycle_len = count_llist(gf->list_sel_vertex);
  enumerate_vertices(gf);
  reset_mark_pick_vertices(gf);
  reset_level_vertices(gf);

  if(!gf->the_cycle)
    gf->the_cycle = init_linked_list();
  if(!gf->list_visited_vertex)
    gf->list_visited_vertex = init_linked_list();
  if(!gf->the_rest)
    gf->the_rest = init_linked_list();

  get_sel_cycle(gf);
  /*printing_linked_list(gf->the_cycle);*/
  if(!is_empty_list(gf->the_cycle))
    {
      Delete_hash_table(gf->HV);
      
      circularize(gf,gf->the_cycle, cycle_len);
      get_rest(gf);
      /*printing_linked_list(gf->the_rest);*/
      if(!is_empty_list(gf->the_rest))
	mes = layout_rest(gf, gf->the_rest, gf->count_vertex-cycle_len);

    }
  else
    mes = NO_CYCLE;

  Delete_all_list(gf->the_cycle);
  Delete_all_list(gf->list_visited_vertex);
  Delete_all_list(gf->the_rest);
  gf->the_cycle = init_linked_list();
  gf->list_visited_vertex = init_linked_list();
  gf->the_rest = init_linked_list();
  return mes;
}
/* get_cycle - Get cycle of length "cycle_len" in the graph. It uses backtrack
               algorithm to find a cycle in the graph and it verifies that it
	       is length "cycle_len"

         inputs  
                 gf - Graph Structure
		 cycle_len - Length of the Cycle.
         returns
                  none
*/

void
get_sel_cycle(GraphFrame *gf)
{
    LNode_type *ptr = NULL, *lsel = gf->list_sel_vertex;
    for(ptr = lsel->next; ptr != lsel ; ptr = ptr->next) {
	PNODE v = (PNODE)ptr->info;
	Insert_linked_list(gf->the_cycle,create_new_node_llist((char*)v));
    }
}
