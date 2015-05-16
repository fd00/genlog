#include "gr_drawing.h"
#include "gr_structure.h"
#include "gr_operations.h"
#include "gr_util.h"

/* compute_circular_positions - This function computes all positions of the 
                                nodes in circular way. The size of the Drawing
				Area can be resized depending of the number of
				nodes. 
         inputs
                 gf - Graph Structure
         returns
                  0 - No problem in computing circular positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  WINDOW_FULL - The window can not hold more nodes.
         side effects
                  The hash table of vertices changes completely. 
         notes
                 If a vertex can not be located in the circle, the vertex 
		 is placed in a empty location of the window.
*/

Dimension
get_circular_window_dim(GraphFrame *gf)
{
  Dimension w=GR_MWIDTH-100;

  int noc = gf->count_vertex;
  int nsize = gf->vsize;
  
  long per = nsize * noc;
  double rad = (double)per/ (double)2*PI;
  if(w < (Dimension)rad)
    {
      Dimension h;
      h = w = (Dimension) rad;
      increase_window_size(gf, &w, &h);
    }
  return w;
}
int
compute_circular_positions(GraphFrame *gf)
{
  double  th, inc;
  int     j;
  int     x_radius, y_radius, x_position, y_position;
  int mes = 0;

  Dimension x, y;

  LNode_type *ptr;
  int noc = gf->count_vertex;
  
  x = y = get_circular_window_dim(gf);
    
  /* get the size of the screen and calc the center */
  
  x_radius = (int)(x / 2 - gf->hsize * 5);
  y_radius = (int)(y / 2 - gf->vsize * 5);
  x_position = (int)(x / 2);
  y_position = (int)(y / 2);
  
  Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */

  inc = 2 * PI / noc;
  j=0;
  
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      Node_type *nptr;

      th = inc * -(j++);
      ov->loc.x  = cos(th) * x_radius + x_position;
      ov->loc.y = sin(th) * y_radius + y_position;
      nptr = select_nearest_vertex(&(ov->loc), gf); 
      if(nptr) /* try again */
	{
	  th = inc * -(j++);
	  ov->loc.x  = cos(th) * x_radius + x_position;
	  ov->loc.y = sin(th) * y_radius + y_position;
	  nptr = select_nearest_vertex(&(ov->loc), gf); 
	  if(nptr)
	    {
	      if(find_empty_spot(ov,gf))
		return WINDOW_FULL;
	    }
	}
      insert_v(ov,gf);
    }
  return mes;
}

/* compute_horizontal_positions - This function computes all positions of the 
                                  nodes in horizontal way. The size of the 
				  Drawing Area can be resized depending of the
				  number of nodes. 
         inputs  
                 gf - Graph Structure
         returns
                  0 - No problem in computing horizontal positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  WINDOW_FULL - The window can not hold more nodes.
         side effects
                  The hash table of vertices changes completely. 
         notes
                 If a vertex can not be located in a horizontal point, the 
		 vertex is placed in a empty location of the window.
*/
int
compute_horizontal_positions(GraphFrame *gf)
{
  int i,dx;
  int x_radius, y_radius, x_position, y_position;

  Dimension x=GR_MWIDTH-100, y = GR_MHEIGHT-100;

  LNode_type *ptr;
  int noc = gf->count_vertex;

  if(noc <= 1)
    return 0;

  x_radius = x / 2 - x / 10;
  y_radius = y / 2 - y / 10;
  x_position = x / 2;
  y_position = y / 2;

  dx = 2 * x_radius / (noc - 1); /* spacing of vertices */

  Delete_hash_table(gf->HV);  

  /* calc the positions of the vertices */
  i = 0;
  
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      Node_type *nptr;

      ov->loc.x = x_position - x_radius + (i++) * dx;

      if(i%2 == 0)
	ov->loc.y = y_position;
      else
	ov->loc.y = y_position - gf->hsize;

      nptr = select_nearest_vertex(&(ov->loc), gf); 
      if(nptr)
	{
	  if(find_empty_spot(ov,gf))
	    return WINDOW_FULL;
	}
      insert_v(ov,gf);
    }
  return 0;
}
/* compute_diagonal_positions - This function computes all positions of the 
                                  nodes in diagonal way. The size of the 
				  Drawing Area can be resized depending of the
				  number of nodes. 
         inputs  
                 gf - Graph Structure
         returns
                  0 - No problem in computing diagonal positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  WINDOW_FULL - The window can not hold more nodes.
         side effects
                  The hash table of vertices changes completely. 
         notes
                 If a vertex can not be located in a diagonal point, the 
		 vertex is placed in a empty location of the window.
*/
int
compute_diagonal_positions(GraphFrame *gf)
{
  int     i,dx,dy;
  int     x_radius, y_radius, x_position, y_position;

  Dimension x=GR_MWIDTH-100, y = GR_MHEIGHT-100;

  LNode_type *ptr;
  int noc = gf->count_vertex;

  if(noc <= 1)
    return 0;

  x_radius = x / 2 - x / 10;
  y_radius = y / 2 - y / 10;
  x_position = x / 2;
  y_position = y / 2;

  dx = 2 * x_radius / (noc - 1); /* spacing of vertices */
  dy = 2 * y_radius / (noc - 1);  /* ver spacing of vertices */
  Delete_hash_table(gf->HV);

  /* calc the positions of the vertices */
  i = 0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      Node_type *nptr;

      ov->loc.x = x_position - x_radius + i * dx;

      if(i%2 == 0)
	ov->loc.y = y_position + y_radius - i * dy; 
      else
	ov->loc.y = y_position + y_radius - i * dy - gf->hsize;

      i++;

      nptr = select_nearest_vertex(&(ov->loc), gf); 
      if(nptr)
	{
	  if(find_empty_spot(ov,gf))
	    return WINDOW_FULL;
	}
      insert_v(ov,gf);
    }
  return 0;
}
/* compute_random_positions - This function computes all positions of the 
                                  nodes in random way. The size of the 
				  Drawing Area can be resized depending of the
				  number of nodes. 
         inputs  
                 gf - Graph Structure
         returns
                  0 - No problem in computing random positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  WINDOW_FULL - The window can not hold more nodes.
         side effects
                  The hash table of vertices changes completely. 
         notes
                 If a vertex can not be located in a random point, the 
		 vertex is placed in a empty location of the window.
*/
int
compute_random_positions(GraphFrame *gf)
{

  Dimension x=GR_MWIDTH-100, y = GR_MHEIGHT-100;

  LNode_type *ptr;
  int noc = gf->count_vertex;

  if(noc <= 1)
    return 0;

  Delete_hash_table(gf->HV);

  /* calc the positions of the vertices */

  srandom(time(NULL));

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      Node_type *nptr;
      
      ov->loc.x = (int)(random()%x);
      ov->loc.y = (int)(random()%y);



      nptr = select_nearest_vertex(&(ov->loc), gf); 
      if(nptr)
	{
	  ov->loc.x = (int)(random()%x);
	  ov->loc.y = (int)(random()%y);

	  nptr = select_nearest_vertex(&(ov->loc), gf); 

	  if(nptr)
	    {
	      if(find_empty_spot(ov,gf))
		return WINDOW_FULL;
	    }
	}
      insert_v(ov,gf);
    }
  return 0;
}

/* compute_pretty_positions - This function computes all positions of the 
                               nodes using the force algorithm. The size of 
			       the Drawing Area can be resized depending of the
			       number of nodes. 
         inputs  
                 gf - Graph Structure
         returns
                  0 - No problem in computing random positions of nodes
		  RESIZE_WINDOW - The Drawing Area was resized. 
		  MEMORY_MATRIX - There are not more memory.
		  WINDOW_FULL - The window can not hold more nodes.
		  GRAPH_NOT_CONNECTED - The graph is not connected and the 
		                        algorithm can not be computed.
         side effects
                  The hash table of vertices changes completely. 
         notes
	        All vertices are assumed that are in different positions.
*/
int 
compute_pretty_positions(GraphFrame *gf, Boolean initial)
{
  int noc = gf->count_vertex;
  int **l, **d = NULL;
  int big,i,j,L;
  int pm,idle,idle2=0;
  double **k;

  if(!initial && gf->drawhighlight)
      return 0; 

  if(initial && gf->drawhighlight)
      noc = count_llist(gf->list_high_vertex);

  d = floyd(gf,noc);

  if(!d)
    return MEMORY_MATRIX;

  if(!initial)
    {
      if(compute_circular_positions(gf) ==  WINDOW_FULL)
	{
	  delete_matrix_int(d, noc);
	  return WINDOW_FULL;
	}
    }
  
  /* find the distance between farthest pair of vertices */
  big = 0;
  for(j=0;j< noc;j++)
    for(i=0;i< j; i++)
      if(d[i][j] > big)
	big = d[i][j];

  if (big == INFINITY)
    {
      /*show_message
       	 "Graph is not connected. Pretty Drawing can not be applied");*/
      
      delete_matrix_int(d, noc);
     return GRAPH_NOT_CONNECTED;
    }


/*  if(noc <= MAX_NODES_WIN)
    L = (GR_MWIDTH - 300)/(big ? big:1);
  else
  L = (gf->w - 300)/(big ? big:1);*/

  { 
    int xbox, ybox, wbox, hbox;
    find_graph_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    L = (wbox > hbox ? wbox : hbox)/(big ? big:1);
  }

  if(initial && gf->drawhighlight)
  { 
    int xbox, ybox, wbox, hbox;
    find_highlighted_nodes_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    L = (wbox > hbox ? wbox : hbox)/(big ? big:1);
  }


  l = (int **)malloc(noc * sizeof(int *));
  if(!l) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    l[i]=(int *)malloc(noc*sizeof(int));
    if(!l[i]){
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }	
  }
  /* calculate original length of spring */
  
  for(i=0;i< noc; i++)
    for(j=0;j< noc; j++)
      l[i][j]= L*d[i][j];

  /* now find the strength of the springs */
  k = (double **)malloc(noc *sizeof(double *));
  if(!k) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    k[i]=(double *)malloc(noc*sizeof(double));
    if(!k[i]) {
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }
  }

  for(i = 0; i< noc; i++)
    for(j = 0; j< noc; j++)
      {
	if(d[i][j])
	  k[i][j]= 1.0/(d[i][j]*d[i][j]);
	else
	  k[i][j]=0;
      }
  /*  print_matrix(k,noc);*/
  while(delta_m(gf,k,l,(pm = maxm(gf,k,l,noc)),noc) > EPSILON && 
	idle2++ < noc*2)
    {
      idle = 0;
      while(delta_m(gf,k,l,pm,noc) > EPSILON && idle++ < noc*2)
	new_coord(pm,k,l,gf,noc);
    }

  delete_matrix_int(l, noc);
  delete_matrix_double(k,noc);
  delete_matrix_int(d,noc);

  Delete_hash_table(gf->HV);
  /* Insert vertices in the Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);

  /*  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(ov->loc.x < 0)
	ov->loc.x = 0;
      if(ov->loc.y < 0)
	ov->loc.y = 0;
      verify_insert_v(ov,gf);
    }
    */
  return 0;
}

void
new_coord(int m, double **k, int **l, GraphFrame *gf,int noc)
{
  int i;
  double a, b, c, d, e, f, dx, dy, deltax, deltay;
  struct pt *v1, *v2;
  /*int noc = gf->count_vertex;*/
  
  a=b=c=d=e=f=0;
  v1 = get_vertex(m, gf);
  
  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  v2 = get_vertex(i,gf);
	  if(v1&&v2)
	    {
	      deltax = v1->loc.x-v2->loc.x;
	      deltay = v1->loc.y-v2->loc.y;
	      if(deltax&&deltay)
		{
		  a += k[m-1][i-1]*(1-(l[m-1][i-1]*deltay*deltay/
                       pow((deltax*deltax+deltay*deltay),1.5)));
		  b += k[m-1][i-1]*(l[m-1][i-1]*deltax*deltay/
				    pow((deltax*deltax+deltay*deltay),1.5));
		  c += k[m-1][i-1]*(l[m-1][i-1]*deltax*deltay/
				    pow((deltax*deltax+deltay*deltay),1.5));
		  d += k[m-1][i-1]*(1-(l[m-1][i-1]*deltax*deltax/
				    pow((deltax*deltax+deltay*deltay),1.5)));
		  e += k[m-1][i-1]*(deltax-(l[m-1][i-1]*deltax/
				       sqrt(deltax*deltax+deltay*deltay)));
		  f += k[m-1][i-1]*(deltay-(l[m-1][i-1]*deltay/
				       sqrt(deltax*deltax+deltay*deltay)));
		}
	    }
	}
    }
  dx = (-f/c + d*e/(b*c))/(1-a*d/(b*c));
  dy = (-e - a*dx)/b;

  v1->loc.x += dx;
  v1->loc.y += dy;

}
int
maxm(GraphFrame *gf,double **k, int **l,int noc)
{
  double big = 0, temp;
  int i, result = 1;
  /*int noc = gf->count_vertex;*/

  for(i=1;i<=noc;i++) {
    if((temp = delta_m(gf,k,l,i,noc))>big)
      {
	big = temp;
	result = i;
      }
  }
  return result;
}
double 
delta_m(GraphFrame *gf,double **k, int **l, int m,int noc)
{
    /*int noc = gf->count_vertex;*/
  double dx, dy;
  int i,tempx, tempy;
  struct pt *vm, *vi;

  dx = dy =  0.0;
  vm = get_vertex(m,gf);

  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  vi = get_vertex(i,gf);
	  if(vi && vm)
	    {
	      tempx = vm->loc.x - vi->loc.x;
	      tempy = vm->loc.y - vi->loc.y;
	      if(tempx && tempy)
		{
		  dx += k[m-1][i-1] * (tempx - (l[m-1][i-1] * tempx/ 
		       sqrt((double)(tempx *tempx + tempy*tempy))));
		  dy += k[m-1][i-1] * (tempy - (l[m-1][i-1] * tempy/ 
		       sqrt((double)(tempx *tempx + tempy*tempy))));
		}
	    }
	}
    }
  return(sqrt(dx*dx+dy*dy));
}
/* get_vertex - Get n-esimo vertex from the list of vertices.

         inputs  
                 n - number of vertex
		 gf - Graph Structure
         returns
                  Vertex
*/

struct pt *
get_vertex(int n, GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(v->prelim == n)
	return v;
    }
  return NULL;
}


int **
floyd(GraphFrame *gf,int noc)
{
  int **a;
  int i,j,k; 

  a = (int **)malloc(noc * sizeof(int *));
  if(!a) {
      fprintf(stderr,"Out of Memory floyd\n");
      abort();
  }
  for(i = 0; i< noc; i++) {
    a[i] = (int *)malloc(noc * sizeof(int));
    if(!a[i]) {
	fprintf(stderr,"Out of Memory floyd\n");
	abort();
    }
  }

  if(a)
  {
      if(!gf->drawhighlight)
	  create_adjacency_matrix(a,gf,noc);
      else
	  create_adjacency_matrix_highlight(a,gf,noc);

  }
  else
    {
      fprintf(stderr,"No memory for adjancecy matrix\n");
      return NULL;
    }

  /*  print_matrix(a, noc,noc);*/

  for(k = 1; k <=noc; k++)
    for(i = 1; i <=noc; i++)
      for(j=1; j <= noc; j++)
	if(a[i-1][k-1]+a[k-1][j-1] < a[i-1][j-1])
	  a[i-1][j-1] = a[i-1][k-1] + a[k-1][j-1];

  return a;
}
/* enumerate_vertices - This function enumerates all vertices of the graph.
                       I
         inputs  
                 gf - Graph Structure
         returns
                  none
*/

void 
enumerate_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 1;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	v->prelim = c++;
    }
}
/* create_adjacency_matrix - Creates an adjacency matrix from the Graph 
                             Structure.

         inputs  
                 a - Adjancecy Matrix
		 gf - Graph Structure
         returns
                  none
*/

void
create_adjacency_matrix(int **a, GraphFrame *gf,int noc)
{
  LNode_type *ptr;
/*  int noc = gf->count_vertex;*/
  int i, j;

  for(i = 0; i< noc ; i++)
    for(j = 0; j< noc ; j++)
      {
	if(i == j)
	  a[i][j]=0;
	else
	  a[i][j]=INFINITY;
      }

  enumerate_vertices(gf);
  
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	    {
	      struct edge *e = (struct edge *)ei->next->info;
	      struct pt *v1 = e->from;
	      struct pt *v2 = e->to;
	      a[v1->prelim-1][v2->prelim-1] = 1;
	      /*if(gf->graph_dir == UNDIRECTED)*/
	      a[v2->prelim-1][v1->prelim-1] = 1;
	    }
	}
    }
}


void 
enumerate_vertices_highlight(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 1;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(!v)
	  continue;

      if(v->highlight)
	v->prelim = c++;
      else
	v->prelim = 0;	
    }
}
/* create_adjacency_matrix - Creates an adjacency matrix from the Graph 
                             Structure.

         inputs  
                 a - Adjancecy Matrix
		 gf - Graph Structure
         returns
                  none
*/

void
create_adjacency_matrix_highlight(int **a, GraphFrame *gf,int noc)
{
  LNode_type *ptr;
  int i, j;

  for(i = 0; i< noc ; i++)
    for(j = 0; j< noc ; j++)
      {
	if(i == j)
	  a[i][j]=0;
	else
	  a[i][j]=INFINITY;
      }

  enumerate_vertices_highlight(gf);
  
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(!v)
	  continue;

      if(v->highlight)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	    {
	      struct edge *e = (struct edge *)ei->next->info;
	      struct pt *v1 = e->from;
	      struct pt *v2 = e->to;

	      if(!v1->highlight || !v2->highlight)
		  continue;

	      a[v1->prelim-1][v2->prelim-1] = 1;
	      /*if(gf->graph_dir == UNDIRECTED)*/
	      a[v2->prelim-1][v1->prelim-1] = 1;
	    }
	}
    }
}

void
print_matrix(double **a, int noc, int moc)
{
  int i, j;
  for(i = 0; i< noc; i++)
    {
      for(j= 0 ; j< moc; j++)
	{
	  if(a[i][j]==INFINITY)
	    printf("e  ");
	  else
	    printf("%f  ",a[i][j]);
	}
      printf("\n");
    }
  printf("\n");
}

/* delete_matrix_int - Delete Matrix from memory

         inputs  
                 A - Matrix 
		 m - Dimension
*/

void 
delete_matrix_int(int **A, int m)
{
  int i;
  for(i=0;i<m; i++) {
    free(A[i]); A[i] = NULL;
  }
  free(A); A = NULL;
}
/* delete_matrix_double - Delete Matrix from memory

         inputs  
                 A - Matrix 
		 m - Dimension
*/
void 
delete_matrix_double(double **A, int m)
{
  int i;
  for(i=0;i<m; i++) {
    free(A[i]); A[i] = NULL;
  }
  free(A); A = NULL;
}
/* find_empty_spot - Find empty spot in the window to place the given vertex.

         inputs  
                 gf - Graph Structure
		 v - Vertex
         returns
                  True - if there are not empty location.
*/

Boolean
find_empty_spot(struct pt *v, GraphFrame *gf)
{

  Node_type *nptr;
  int x, y;
  int dsize = 2*gf->vsize;
  /*printf("Find empty spot\n");*/
  for(x = 0; x < gf->w; x+=dsize)
      for(y = 0; y < gf->h; y+=dsize)
	{
	  v->loc.x = x;
	  v->loc.y = y;
	  nptr = select_nearest_vertex(&(v->loc), gf);
	  if(!nptr)
	    return False;
	}
  return True;

}
/* find_near_spot - Find near spot from the actual location of the given 
                    vertex in the window. 
         inputs  
                 gf - Graph Structure
		 v - Vertex
         returns
                  True - if there are not empty location.
*/
Boolean
find_near_spot(struct pt *v, GraphFrame *gf)
{
    /*static int i=0;*/
  Node_type *nptr;
  int x = v->loc.x, y = v->loc.y;

  int dsize = 2*gf->vsize;
  /*printf("%d Find near spot\n",i++);*/
  for(; y <= gf->h; y+=dsize)
    for(; x <= gf->w; x+=dsize)
	{
	  v->loc.x = x;
	  v->loc.y = y;
	  nptr = select_nearest_vertex(&(v->loc), gf);
	  if(!nptr)
	    return False;
	}
  return find_empty_spot(v,gf);

}

int
compute_linear_positions(GraphFrame *gf)
{
  LNode_type *ptr;
  int noc = gf->count_vertex;
  int anode = 4 *gf->vsize *gf->hsize;
  int maxnodes = (gf->w * gf->h) / anode;
  /*long maximum_area = GR_MAX_LENGTH * GR_MAX_LENGTH;*/
  Dimension x, y;
  int incx=gf->hsize, incy =gf->vsize;
  int cn = 0;
  if(noc <= 1)
    return 0;

  if(noc > maxnodes)
    {
      long ca = noc * anode;

      /*if (ca > maximum_area)
	return -1;*/

      x = y = (Dimension)(sqrt(ca) + 2*gf->vsize);

      printf("NEW DIMENSIONS x = %d\n",x);

      increase_window_size(gf, &x, &y);
    }
  
  Delete_hash_table(gf->HV);

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;

      ov->loc.x = incx;
      ov->loc.y = incy;

      incx += 2*gf->hsize;
      if(incx > gf->w)
	{
	  incx = 0;
	  incy += 2*gf->vsize;
	}

      
      if(verify_insert_v(gf,ov))
	return WINDOW_FULL;
      cn++;
      if(cn%100 == 0)
	fprintf(stderr,"%d nodes has been placed \n", cn);
    }

  return 0;
}


/* ------------------------------------------------------------- */
int
layer_maxm(GraphFrame *gf,double **k, int **l,int noc);

double 
layer_delta_m(GraphFrame *gf,double **k, int **l, int m,int noc);

void
layer_new_coord(int m, double **k, int **l, GraphFrame *gf,int noc);

int 
compute_layered_pretty_positions(GraphFrame *gf)
{
  int noc = gf->count_vertex;
  int **l, **d = NULL;
  int big,i,j,L;
  int pm,idle,idle2=0;
  double **k;

  if(gf->drawhighlight)
      noc = count_llist(gf->list_high_vertex);

  d = floyd(gf,noc);

  if(!d)
    return MEMORY_MATRIX;

  /* find the distance between farthest pair of vertices */
  big = 0;
  for(j=0;j< noc;j++)
    for(i=0;i< j; i++)
      if(d[i][j] > big)
	big = d[i][j];

  if (big == INFINITY)
    {
      /*show_message
       	 "Graph is not connected. Pretty Drawing can not be applied");*/
      
      delete_matrix_int(d, noc);
     return GRAPH_NOT_CONNECTED;
    }


/*  if(noc <= MAX_NODES_WIN)
    L = (GR_MWIDTH - 300)/(big ? big:1);
  else
  L = (gf->w - 300)/(big ? big:1);*/

  { int xbox, ybox, wbox, hbox;
    find_graph_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    L = (wbox > hbox ? wbox : hbox)/(big ? big:1);
  }


  l = (int **)malloc(noc * sizeof(int *));
  if(!l) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    l[i]=(int *)malloc(noc*sizeof(int));
    if(!l[i]){
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }	
  }
  /* calculate original length of spring */
  
  for(i=0;i< noc; i++)
    for(j=0;j< noc; j++)
      l[i][j]= L*d[i][j];

  /* now find the strength of the springs */
  k = (double **)malloc(noc *sizeof(double *));
  if(!k) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    k[i]=(double *)malloc(noc*sizeof(double));
    if(!k[i]) {
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }
  }

  for(i = 0; i< noc; i++)
    for(j = 0; j< noc; j++)
      {
	if(d[i][j])
	  k[i][j]= 1.0/(d[i][j]*d[i][j]);
	else
	  k[i][j]=0;
      }
  /*  print_matrix(k,noc);*/
/*  while(layer_delta_m(gf,k,l,(pm = layer_maxm(gf,k,l,noc)),noc) > EPSILON && 
	idle2++ < noc*2)
    {
      idle = 0;
      while(layer_delta_m(gf,k,l,pm,noc) > EPSILON && idle++ < noc*2)
	layer_new_coord(pm,k,l,gf,noc);
	}*/

  while(layer_delta_m(gf,k,l,(pm = layer_maxm(gf,k,l,noc)),noc) > EPSILON)
    {
	if(idle2++ > noc*2) {
	    break; 
	}
      idle = 0;
      while(layer_delta_m(gf,k,l,pm,noc) > EPSILON) {
	  if(idle++ > noc*2) {
	    break; 
	  }
	layer_new_coord(pm,k,l,gf,noc);
      }
    }

  delete_matrix_int(l, noc);
  delete_matrix_double(k,noc);
  delete_matrix_int(d,noc);

  Delete_hash_table(gf->HV);
  /* Insert vertices in the Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);

  return 0;
}

int
layer_maxm(GraphFrame *gf,double **k, int **l,int noc)
{
  double big = 0, temp;
  int i, result = 1;
  /*int noc = gf->count_vertex;*/

  for(i=1;i<=noc;i++) {
    if((temp = layer_delta_m(gf,k,l,i,noc))>big)
      {
	big = temp;
	result = i;
      }
  }
  return result;
}

double 
layer_delta_m(GraphFrame *gf,double **k, int **l, int m,int noc)
{
  double dx;
  int i,tempx,tempy;
  struct pt *vm, *vi;

  dx =  0.0;
  vm = get_vertex(m,gf);

  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  vi = get_vertex(i,gf);
	  if(vi && vm)
	    {
	      tempx = vm->loc.x - vi->loc.x;
	      tempy = vm->loc.y - vi->loc.y;
	      if(tempx && tempy)
		{
		  dx += k[m-1][i-1] * (tempx - (l[m-1][i-1] * tempx/ 
		       sqrt((double)(tempx *tempx + tempy*tempy))));
		}
	    }
	}
    }
  return(dx > 0 ? dx : -dx);
}

/*void
layer_new_coord(int m, double **k, int **l, GraphFrame *gf,int noc)
{
  int i;
  double a, b, c, d, e, f, dx, deltax, deltay;
  struct pt *v1, *v2;
  
  a=b=c=d=e=f=0;
  v1 = get_vertex(m, gf);
  
  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  v2 = get_vertex(i,gf);
	  if(v1&&v2)
	    {
	      deltax = v1->loc.x-v2->loc.x;
	      deltay = v1->loc.y-v2->loc.y;
	      if(deltax&&deltay)
		{
		  a += k[m-1][i-1]*(1-(l[m-1][i-1]*deltay*deltay/
                       pow((deltax*deltax+deltay*deltay),1.5)));
		  b += k[m-1][i-1]*(l[m-1][i-1]*deltax*deltay/
				    pow((deltax*deltax+deltay*deltay),1.5));
		  c += k[m-1][i-1]*(l[m-1][i-1]*deltax*deltay/
				    pow((deltax*deltax+deltay*deltay),1.5));
		  d += k[m-1][i-1]*(1-(l[m-1][i-1]*deltax*deltax/
				    pow((deltax*deltax+deltay*deltay),1.5)));
		  e += k[m-1][i-1]*(deltax-(l[m-1][i-1]*deltax/
				       sqrt(deltax*deltax+deltay*deltay)));
		  f += k[m-1][i-1]*(deltay-(l[m-1][i-1]*deltay/
				       sqrt(deltax*deltax+deltay*deltay)));
		}
	    }
	}
    }
  dx = (-f/c + d*e/(b*c))/(1-a*d/(b*c));

  v1->loc.x += dx;

}*/
void
layer_new_coord(int m, double **k, int **l, GraphFrame *gf,int noc)
{
  int i;
  double a, b, c, d, e, f, dx, deltax, deltay;
  struct pt *v1, *v2;
  
  a=b=c=d=e=f=0;
  v1 = get_vertex(m, gf);
  
  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  v2 = get_vertex(i,gf);
	  if(v1&&v2)
	    {
	      deltax = v1->loc.x-v2->loc.x;
	      deltay = v1->loc.y-v2->loc.y;
	      if(deltax&&deltay)
		{
		  a += k[m-1][i-1]*(1-(l[m-1][i-1]*deltay*deltay/
                       pow((deltax*deltax+deltay*deltay),1.5)));
		  e += k[m-1][i-1]*(deltax-(l[m-1][i-1]*deltax/
				       sqrt(deltax*deltax+deltay*deltay)));
		}
	    }
	}
    }

  dx = -e/a;

  v1->loc.x += dx;

}

/* ------------------------------------------------------------- */


/* ------------------------------------------------------------- */

int
radial_maxm(GraphFrame *gf,double **k, int **l,double *R, double *T, int noc);

double 
radial_delta_m(GraphFrame *gf,double **k, int **l, double *R, double *T, 
	       int m,int noc);

int
compute_radius_theta(GraphFrame *gf, double *R, double *T,int noc);

void
radial_new_coord(int m, double **k, int **l, double *R, double *T, GraphFrame *gf,int noc);

int 
compute_radial_pretty_positions(GraphFrame *gf)
{
  int noc = gf->count_vertex;
  int **l, **d = NULL;
  int big,i,j,L;
  int pm,idle,idle2=0;
  double **k;
  double *R;
  double *T;
  PNODE r = gf->rootv;

  if(!r) 
      return 0;

  if(gf->drawhighlight)
      noc = count_llist(gf->list_high_vertex);

  d = floyd(gf,noc);

  if(!d)
    return MEMORY_MATRIX;

  /* find the distance between farthest pair of vertices */
  big = 0;
  for(j=0;j< noc;j++)
    for(i=0;i< j; i++)
      if(d[i][j] > big)
	big = d[i][j];

  if (big == INFINITY)
    {
      /*show_message
       	 "Graph is not connected. Pretty Drawing can not be applied");*/
      
      delete_matrix_int(d, noc);
     return GRAPH_NOT_CONNECTED;
    }

  { int xbox, ybox, wbox, hbox;
    find_graph_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    L = (wbox > hbox ? wbox : hbox)/(big ? big:1);
  }


  l = (int **)malloc(noc * sizeof(int *));
  if(!l) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    l[i]=(int *)malloc(noc*sizeof(int));
    if(!l[i]){
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }	
  }
  /* calculate original length of spring */
  
  for(i=0;i< noc; i++)
    for(j=0;j< noc; j++)
      l[i][j]= L*d[i][j];

  /* now find the strength of the springs */
  k = (double **)malloc(noc *sizeof(double *));
  if(!k) {
      fprintf(stderr,"Out of Memory compute_pretty_positions\n");
      abort();
  }
  for(i=0;i<noc;i++)
  {
    k[i]=(double *)malloc(noc*sizeof(double));
    if(!k[i]) {
	fprintf(stderr,"Out of Memory compute_pretty_positions\n");
	abort();
    }
  }

  for(i = 0; i< noc; i++)
    for(j = 0; j< noc; j++)
      {
	if(d[i][j])
	  k[i][j]= 1.0/(d[i][j]*d[i][j]);
	else
	  k[i][j]=0;
      }

  R = (double *)calloc(noc,sizeof(double));
  if(!R) {
      fprintf(stderr,"Out of Memory compute_radial_pretty_positions\n");
      abort();
  }

  T = (double *)calloc(noc,sizeof(double));
  if(!T) {
      fprintf(stderr,"Out of Memory compute_radial_pretty_positions\n");
      abort();
  }

  compute_radius_theta(gf,R,T,noc);

  while(radial_delta_m(gf,k,l,R,T,(pm = radial_maxm(gf,k,l,R,T,noc)),noc) > EPSILON)
    {
	if(idle2++ > noc*2) {
	    /*printf(" 1: WRONG PARTICLE %d\n",pm);*/
	    break; 
	}
      idle = 0;
      while(radial_delta_m(gf,k,l,R,T,pm,noc) > EPSILON) {
	  if(idle++ > noc*2) {
	      /*printf("2 : WRONG PARTICLE %d\n",pm);*/
	    break; 
	  }
	radial_new_coord(pm,k,l,R,T,gf,noc);
      }
    }
  delete_matrix_int(l, noc);
  delete_matrix_double(k,noc);
  delete_matrix_int(d,noc);
  if (R) free(R);
  if (T) free(T);

  Delete_hash_table(gf->HV);
  /* Insert vertices in the Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);

  return 0;
}

int
compute_radius_theta(GraphFrame *gf, double *R, double *T,int noc)
{
  int i;

  PNODE vi = NULL;   
  PNODE r = gf->rootv;
  double tempx, tempy;

  if(!r)
      return -1;
/*  printf("R %d \n", r->num);*/
  for(i=1;i<=noc;i++)
  {
      vi = get_vertex(i,gf);
      if(vi && vi != r ) {
	  tempx = (double)(vi->loc.x - r->loc.x);
	  tempy = -(double)(vi->loc.y - r->loc.y);
	  R[i-1] = sqrt(tempx *tempx + tempy *tempy);
	  T[i-1] = atan2(tempy,tempx);
	  if (T[i-1] < 0.0)
	      T[i-1] += 2 * PI;
	  /*fprintf(stderr,"V %d i %d R %f T %f\n", vi->num, i, R[i-1], T[i-1]);*/
      }
      else
      {
	  R[i-1] = 0.0;	
	  T[i-1] = 0.0;
      }
  }
  return 0;
}

int
radial_maxm(GraphFrame *gf,double **k, int **l,double *R, double *T, int noc)
{
  double big = 0, temp;
  int i, result = 1;

  for(i=1;i<=noc;i++) {
    if((temp = radial_delta_m(gf,k,l,R,T,i,noc))>big)
      {
	big = temp;
	result = i;
      }
  }
  /*printf("M %d MAXM  %f\n", result, big);*/
  return result;
}

/*double 
radial_delta_m(GraphFrame *gf,double **k, int **l, double *R, double *T, int m,int noc)
{
  double dx, dy,tempx,tempy;
  int i;

  PNODE vm, vi,r = gf->rootv;

  dx = dy =  0.0;
  vm = get_vertex(m,gf);

  if(!vm || vm == r)
      return 0.0;

  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  vi = get_vertex(i,gf);
	  if(vi && vi != r)
	    {
	      tempx = R[m-1]*cos(T[m-1]) - R[i-1]*cos(T[i-1]);
	      tempy = R[m-1]*sin(T[m-1]) - R[i-1]*sin(T[i-1]);

	      if(tempx && tempy)
		{
		  dx += k[m-1][i-1] * (tempx - (l[m-1][i-1] * tempx/ 
		       sqrt(tempx *tempx + tempy*tempy)));
		  dy += k[m-1][i-1] * (tempy - (l[m-1][i-1] * tempy/ 
		       sqrt(tempx *tempx + tempy*tempy)));
		}
	    }
	}
    }
  return(sqrt(dx*dx+dy*dy));
}*/

double 
radial_delta_m(GraphFrame *gf,double **k, int **l, double *R, double *T, 
	       int m,int noc)
{
  int i;
  PNODE vm, vi,r = gf->rootv;

  double a, b, c, d, de_dt=0, dt;

  dt =  0.0;
  vm = get_vertex(m,gf);

  if(!vm || vm == r)
      return 0;

  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  vi = get_vertex(i,gf);
	  if(vi && vi!=r)
	    {
		a = R[m-1]*R[i-1]*sin(T[m-1]-T[i-1]);
		b = R[m-1]*R[m-1] + R[i-1]*R[i-1] 
		    - 2 * R[m-1] *R[i-1]*cos(T[m-1]-T[i-1]);
		c = R[m-1]*R[i-1]*cos(T[m-1]-T[i-1]);
		d = 1 - (l[m-1][i-1]/sqrt(b));
		de_dt += k[m-1][i-1] * a * d;
	    }
	}
    }
  /*dt = de_dt/R[m-1];*/
  dt = de_dt;

  return (dt > 0 ? dt : -dt);
}

void
radial_new_coord(int m, double **k, int **l, double *R, double *T, 
		 GraphFrame *gf,int noc)
{
  int i;
  double a, b, c, d, de_dt=0, de2_dt2=0,dx,dy,dt;
  PNODE vm, vi, r = gf->rootv;
  
  a=b=c=d=0;
  vm = get_vertex(m, gf);
/*  printf("VM %d \n", vm->num);*/
  if(!vm || vm == r)
      return;
  
  for(i=1;i<=noc;i++)
    {
      if(i!=m)
	{
	  vi = get_vertex(i,gf);
	  if(vi && vi != r)
	    {
		a = R[m-1]*R[i-1]*sin(T[m-1]-T[i-1]);
		b = R[m-1]*R[m-1] + R[i-1]*R[i-1] 
		    - 2 * R[m-1] *R[i-1]*cos(T[m-1]-T[i-1]);
		c = R[m-1]*R[i-1]*cos(T[m-1]-T[i-1]);
		d = 1 - (l[m-1][i-1]/sqrt(b));
		de_dt += k[m-1][i-1] * a * d;
		de2_dt2 += k[m-1][i-1] * ( (l[m-1][i-1] * a * a)/pow(b,1.5) +
					   c * d );
	    }
	}
    }
  dt = -de_dt/de2_dt2;
  /*printf("DE %f DE2 %f DT %f\n",de_dt, de2_dt2, dt);*/


  dx = R[m-1] * cos(T[m-1]+dt) - R[m-1] * cos(T[m-1]);
  dy = - R[m-1] * sin(T[m-1]+dt) + R[m-1] * sin(T[m-1]);

  T[m-1] = T[m-1] + dt;

/*  printf(" T %f DT %f\n", T[m-1], dt);*/

  while(T[m-1] > 2*PI)
      T[m-1]-= 2*PI;

  while(T[m-1] < 0)
      T[m-1] += 2*PI;

/*  printf("NEW T %f DT %f\n", T[m-1], dt);*/


  vm->loc.x += (int)dx;
  vm->loc.y += (int)dy;

/*  printf("DX %f DY %f X %d Y %d\n",dx,dy,vm->loc.x,vm->loc.y);*/
}

/* ------------------------------------------------------------- */
#define MIN_TEMP 0
#define LOWER_TEMP 1.0

double temperature(double t, int n) {
    return ((n/2.0)/ (1 + exp(t/8-5)));
}
double fa (double x,double k) {
    return ((x * x)/ k);
}   

double fr (double x,double k) {
    return ((k * k)/ x);
}   

int 
compute_force_directed_positions(GraphFrame *gf)
{
    double area = 0.0;
    double Temp = 0.0;
    double k = 0.0;
    double Fa=0.0, Fr=0.0;
    int noc = gf->count_vertex;
    int i = 0;
    int iterations = 500; /* Iterations */
    double time = 0.0;
    Boolean gd = gf->drawhighlight;
    int xbox=0, ybox=0, wbox=0, hbox=0;

    if(!gd){ 
	find_graph_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    } else {
	noc = find_highlighted_nodes_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    }
    area = (double)wbox * (double)hbox; /* Area of the frame */

    if (area == 0)
	return 0;

  k = sqrt(area/noc);
/*  printf("Area %f K %f\n", area, k);*/
  for (i=1; i< iterations ; i++)
  {
      /* Compute repulsive forces */
      LNode_type *ptr, *ptr2;	
      for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
	  ptr = ptr->next)
      {
	  PNODE v = (PNODE)ptr->info;
	  if(gd && !v->highlight)
	      continue;
	  v->prelim = 0;
	  v->modifier = 0;
	  for(ptr2= gf->list_vertices->next; ptr2 != gf->list_vertices ; 
	      ptr2 = ptr2->next)
	  {
	      PNODE u = (PNODE)ptr2->info;
	      if(gd && !u->highlight)
		  continue;
	      if(u!=v)
	      {
		  double dx = (double)(v->loc.x - u->loc.x);
		  double dy = (double)(v->loc.y - u->loc.y);
		  double delta = sqrt(dx*dx+dy*dy);
		  if(delta < 0.00001 || delta >  3 * k)
		    continue;
		  Fr = fr(delta,k);
		  v->prelim+= (int)((dx * Fr)/delta);
		  v->modifier+= (int)((dy * Fr)/delta);
/*		  printf("1:V %d  U %d DX %d DY %d dx %f dy %f delta %f Fr %f\n", v->num, u->num, v->prelim, v->modifier,dx,dy,delta,Fr);*/
	      }
	  }
      }

      untouch_edges(gf);

      /* Compute attractive forces */
      for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
	  ptr = ptr->next)
      {
	  PNODE v = (PNODE)ptr->info;
	  LNode_type *ei;
	  if(gd && !v->highlight)
	      continue;
	  for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	  {
	    PEDGE e = (PEDGE)ei->info;
	    PEDGE et = NULL;
	    PNODE u = e->from;
	    double delta = 0.0, dx=0.0, dy=0.0;
	    if(e->touch == True)
		continue;
	    e->touch = True;
	    if(gf->graph_dir == UNDIRECTED)
	    {
		if(u == v)
		    u = e->to;
	    } else {
		et = find_edge_by_name(gf,v->label,u->label);
		if(et) et->touch = True;
	    }
	    if(u == v)
		continue;

	    if(gd && !u->highlight)
		continue;

	    dx = (double) (v->loc.x - u->loc.x);
	    dy = (double) (v->loc.y - u->loc.y);
	    delta = sqrt(dx*dx+dy*dy);
	    Fa = fa(delta,k);
/*	    printf("dx %f dy %f delta %f Fa %f\n",dx,dy,delta, Fa);*/
	    /*if(delta > 3 * k)
	      continue;*/
	    if(delta < 0.00001)
		continue;
	    v->prelim -= (int)((dx * Fa)/delta);
	    v->modifier -= (int)((dy * Fa)/delta);
	    u->prelim += (int)((dx * Fa)/delta);
	    u->modifier += (int)((dy * Fa)/delta);

/*	    printf("2:V %d  DX %d DY %d delta %f Fa %f\n", v->num, v->prelim, v->modifier,delta, Fa);
	    printf("2:U %d  DX %d DY %d\n", u->num, u->prelim, u->modifier);*/
	  }
      }
      Temp = temperature(time,noc) + MIN_TEMP;
      /*printf("Temperature %f time %f \n", Temp, time);*/
      /* Compute new positions */
      for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
	  ptr = ptr->next)
      {
	  PNODE v = (PNODE)ptr->info;
	  double delta=0.0, mind = 0.0;
	  if(gd && !v->highlight)
	      continue;
	  delta = sqrt((double)v->prelim *(double)v->prelim + 
			      (double)v->modifier *(double)v->modifier);
	  mind = delta < Temp ?  delta : Temp ;
	  /*printf("V %d X %d Y %d DX %d DY %d delta %f \n", v->num, v->loc.x,
	    v->loc.y, v->prelim, v->modifier,delta);*/
	  if(mind > 0 && delta > 0) {
	      v->loc.x += (int)((double)(v->prelim * mind)/delta);
	      v->loc.y += (int)((double)(v->modifier * mind)/ delta);
	  }
	  /*v->loc.x += v->prelim;
	    v->loc.y += v->modifier;*/
      }
      if(Temp < LOWER_TEMP)
	  break;
      time += 2.0;
  }

  Delete_hash_table(gf->HV);
  /* Insert vertices in the Hash Table of Vertices */
  insert_all_vertices_hash_table(gf);
  return 0;
}





