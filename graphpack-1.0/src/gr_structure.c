#include "gr_structure.h"
#include "gr_drawing.h"
#include "gr_operations.h"


/* save_list_selected - Insert the given vertex in the list of selected 
                        vertices
         inputs  
                 v - Vertex
		 gf - Graph Structure
         returns
                  none
*/

void
save_list_selected(struct pt *v,GraphFrame *gf)
{
  Insert_linked_list(gf->list_sel_vertex, create_new_node_llist((char *) v));
}
/* delete_list_selected - Delete Vertex from the list of selected Vertices.

         inputs  
                 v - Vertex
		 gf - Graph Structure
         returns
                  none 
*/
		 
void
delete_list_selected(struct pt *v,GraphFrame *gf)
{
  char * info = (char *) v;
  LNode_type * node = Search_Sequential_list(gf->list_sel_vertex, info);
  if(node != gf->list_sel_vertex)
    Delete_linked_list(node);
}
/* insert_hash_vertex_name - Insert name of the given vertex in the Hash
                             Table of Vertex names. 

         inputs  
                 gf - Graph Structure.
		 vertex_name - Name of the vertex
		 v - Vertex
         returns
                  none
*/

void
insert_hash_vertex_name(GraphFrame *gf, char *vertex_name, struct pt *v)
{
  Node_type *nptr;
  Item_type item;
  Key_type key = NULL;
  GRStrAllocCopy(key,vertex_name); 

  item.key = key;
  item.gr = (char *)v;
  nptr = create_new_node(item);
  Insert(gf->HVname, nptr);
}
/* change_names_edges - The name of the edges depends on the name of the
                        two extreme vertices. If one of these vertices changes
			the name, all the edges in and out from that vertex
			must change the name. This function changes the names
			of the edges that are in and out of the given vertex. 
			The new name of the vertex is also a parameter.
         inputs  
                 gf - Graph Structure
		 v - Vertex
		 label - New name of the vertex
         returns
                  none
*/

void
change_names_edges(GraphFrame *gf, struct pt *v, char * label)
{
  LNode_type *ei;
  char *tmp = v->label;

  if(gf->graph_dir == DIRECTED)
    {
      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  Node_type *nptr;
	  struct edge *e = (struct edge *)ei->next->info;
	  nptr = select_nearest_edge(e->from, e->to, gf);
	  if(nptr)
	    {
	      Delete_target(gf->Hedge, nptr->info.key);
	      v->label = label;
	      insert_e(e,gf);
	      v->label = tmp;
	    }
	}

      for(ei = v->ledges_out; ei->next != v->ledges_out; ei=ei->next)
	{
	  Node_type *nptr;
	  struct edge *e = (struct edge *)ei->next->info;
	  nptr = select_nearest_edge(e->from, e->to, gf);
	  if(nptr)
	    {
	      Delete_target(gf->Hedge, nptr->info.key);
	      v->label = label;
	      insert_e(e,gf);
	      v->label = tmp;
	    }
	}
    }
  if(gf->graph_dir == UNDIRECTED)
    {
      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  Node_type *nptr;
	  struct edge *e = (struct edge *)ei->next->info;
	  nptr = select_nearest_edge(e->to, e->from, gf);
	  Delete_target(gf->Hedge, nptr->info.key);
	  nptr = select_nearest_edge(e->from, e->to, gf);
	  Delete_target(gf->Hedge, nptr->info.key);
	  v->label = label;
	  insert_e(e,gf);
	  v->label = tmp;
	}
    }
}
/* delete_list_highlight - Delete vertex from the list of highlighted vertices.

         inputs  
                 v - Vertex
		 gf - Graph Structure.
         returns
                  none
*/

void
delete_list_highlight(struct pt *v,GraphFrame *gf)
{
  char * info = (char *) v;
  LNode_type * node = Search_Sequential_list(gf->list_high_vertex, info);
  if(node != gf->list_high_vertex)
    Delete_linked_list(node);
}
/* save_list_highlight - Insert vertex in the list of highlighted vertices.

         inputs  
                 v - Vertex
		 gf - Graph Structure.
         returns
                  none
*/
void
save_list_highlight(struct pt *v,GraphFrame *gf)
{
  LNode_type * node = create_new_node_llist((char *) v);
  Insert_linked_list(gf->list_high_vertex, node);
}


/* select_nearest_vertex - Find a vertex that is near to the given location.

         inputs  
                 loc - Location in the Drawing Area.
		 gf - Graph Structure.
         returns
                  Structure that contains the vertex info.
*/

Node_type *
select_nearest_vertex(struct pr_pos *loc, GraphFrame *gf)
{
  
  Key_type key;
  Node_type *nptr;

  key = calculate_key_pos(loc, gf->vsize, (int)gf->w);

  nptr = Retrieve(gf->HV,key);

  free(key); key = NULL;

  return nptr;
}
  
/* select_nearest_edge - Find a edge that is between the two given extreme
                         vertices.
         inputs  
                 v1, v2 - Extreme vertices
		 gf - Graph Structure
         returns
                  Structure that contains the edge info.
*/

Node_type *
select_nearest_edge(struct pt *v1, struct pt *v2, GraphFrame *gf)
{
  Key_type key;
  Node_type *nptr;

  key = calculate_key_edge(v1,v2);
  nptr = Retrieve(gf->Hedge,key);
  free(key); key = NULL;
  return nptr;
}  

/* insert_e - Insert edge structure in a Graph Structure. This information is
              saved in the Hash Table of Edges.
         inputs  
                 e - Edge
		 gf - Graph Structure
         returns
                  none 
	 notes
	         If the edge is undirected, this is inserted as edge in and
		 out of the vertices. 
*/

void
insert_e( struct edge *e,GraphFrame *gf )
{
  Node_type *nptr;
  Item_type item1, item2;
  Key_type key1,key2;

  if(gf->graph_dir == UNDIRECTED)
    {
      key1 = calculate_key_edge(e->from, e->to);
      item1.key = key1;
      item1.gr = (char *)e;
      nptr = create_new_node(item1);
      Insert(gf->Hedge, nptr);
      key2 = calculate_key_edge(e->to, e->from);
      item2.key = key2;
      item2.gr = (char *)e;
      nptr = create_new_node(item2);
      Insert(gf->Hedge, nptr);
    }
  else if (gf->graph_dir == DIRECTED)
    {
      key1 = calculate_key_edge(e->from, e->to);
      item1.key = key1;
      item1.gr = (char *)e;
      nptr = create_new_node(item1);
      Insert(gf->Hedge, nptr);
    }      
}

/* calculate_key_edge - The key for the Hashtable of the edges is the name
                        of the edge. The name of a edge depends on the 
			combination of the name of the two extreme vertices
			of the edge.

         inputs  
                 v1, v2 - Extreme vertices of the edge
         returns
                  The key of the edge 
*/
Key_type 
get_key_string_edge(char *sname, char *tname)
{
  int nlv1 = sname ? strlen(sname) : 0;

  int nlv2 = tname ? strlen(tname) : 0;
  int sum = nlv1 + nlv2;

  Key_type key = NULL;

  if(sum <=0)
      return NULL;

  key = (Key_type)malloc(sum+2);
  if(key)
      /*sprintf(key,"%s_%s",sname,tname);*/
      sprintf(key,"%s|%s",sname,tname);
  else {
      fprintf(stderr,"Out of Memory calculate_key_edge");
      abort();
  }
  return key;
}

Key_type
calculate_key_edge(struct pt *v1, struct pt *v2)
{
  char *lv1 = v1->label;
  int nlv1 = strlen(lv1);

  char *lv2 = v2->label;
  int nlv2 = strlen(lv2);

  Key_type key = (Key_type)malloc(nlv1+nlv2+2);

  if(key)
      sprintf(key,"%s|%s",lv1,lv2);
  else {
      fprintf(stderr,"Out of Memory calculate_key_edge");
      abort();
  }
  return key;
}
/* calculate_key_pos - The key for the Hashtable of the vertices depends on
                       the position of the vertex in the Drawing Area. 
		       The formula for the key is :
		          Xpos * Width + Ypos. 
		       where Xpos, Ypos are the coordinates of the vertex and
		       Width is the width of the Drawing Area.

         inputs  
                 loc - Position of the vertex
		 side - size of the side of the vertex
		 rel_width - Width of the drawing Area.
         returns
                  Key of the vertex
	 notes
                 The Drawing Area is considered as a grid, where the distance
		 between two points is the side of a vertex. 

*/

Key_type
calculate_key_pos(struct pr_pos	*loc, int side, int rel_width)
{
  Key_type key;
  char str[20];
  int kpos;
  int xn, yn;
  int wn = rel_width / side;
  
  update_pos_grid(loc,side);

  xn = loc->x / side;
  yn = loc->y / side;
  
  kpos = xn * (wn + 1) + yn;

  sprintf(str,"%d",kpos);
  
  key = (Key_type)malloc(strlen(str)+1);

  if(!key) {
      fprintf(stderr,"Out of Memory calculate_key_pos\n");
      abort();
  }

  Strcpy(key,str);
  
  return key;
}

/* update_pos_grid - This function calculates the relative position of the
                     vertex in the grid of the Drawing Area.
         inputs  
                 loc - Location of the vertex
		 side - size of the side of the vertex
         returns
                  Location of the vertex is updated. 
*/

void
update_pos_grid(struct pr_pos *loc, int side)
{
  int nx,ny,dside;
  
  if(loc->x < 0 || loc->y < 0)
    return;

  dside = 2 * side;
  nx = (int)(loc->x) / dside;
  ny = (int)(loc->y) / dside;

  loc->x = side * ( nx + nx + 1);
  loc->y = side * ( ny + ny + 1);
  
}
/* insert_v - Insert a vertex structure in the Graph Structure. The vertex is
              saved in the Hash Table of the Vertices.

         inputs  
                 elem - Vertex
		 gf - Graph Structure
         returns
                  none
*/
void
insert_v(struct	pt *elem,GraphFrame *gf)
{
  Node_type * nptr;
  Item_type item;
  Key_type key;
  
  key = calculate_key_pos(&(elem->loc), gf->vsize, gf->w);
  item.key = key;
  item.gr = (char *)elem;
  
  nptr = create_new_node(item);
  Insert(gf->HV,nptr);

  /*printing_hash_table(HV);*/
  
}
/* update_list_edges - Delete edge from the list of the Edges.

         inputs  
                 ledges - List of Edges
		 e - Edge
         returns
                  none
*/

void
update_list_edges(LNode_type *ledges, struct edge *e)
{
  LNode_type *ptr =  Search_Sequential_list(ledges, (char *)e);
  if(ptr)
    Delete_linked_list(ptr);
}
/* update_list - Delete element from a List

         inputs  
                 lhead - Head of the List
		 elem - Element of the List
         returns
                  none
*/

void
update_list(LNode_type *lhead, char * elem)
{
  LNode_type *ptr =  Search_Sequential_list(lhead, elem);
  if(ptr)
    Delete_linked_list(ptr);
}

/* delete_v - Delete Vertex from the Graph Structure. The Vertex is deleted
              from the Hash Table of Vertices and the name of the vertex from
	      the Hash Table of the name of the vertices.

         inputs  
                 gf - Graph Structure
		 nptr - Structure that saves the vertex info
		 v - Vertex to be deleted
         returns
                  none
*/

void
delete_v(GraphFrame *gf,Node_type *nptr,struct pt *v)
{
  Delete_target(gf->HV, nptr->info.key);
  Delete_target(gf->HVname, v->label);
  update_list(gf->list_vertices, (char *) v);
  (gf->count_vertex)--;
  delete_vertex_info(v);
  free(v); 
  v = NULL;
}

/* delete_e - Delete edge from the Graph Structure. The edge is deleted from
              the Hash Table of Edges.
         inputs  
                 gf - Graph Structure
		 e - Edge
         returns
                  none
*/
 
void
delete_e(GraphFrame *gf,struct edge *e)
{
  Node_type *nptr = select_nearest_edge(e->from, e->to, gf);


  Delete_target(gf->Hedge, nptr->info.key);
  if(gf->graph_dir == UNDIRECTED)
    {
      nptr = select_nearest_edge(e->to, e->from, gf);
      Delete_target(gf->Hedge, nptr->info.key);
    }

  delete_edge_info(e);
  free(e);
  e = NULL;
}  

/**************************************************************************/
#define MAX_CLUSTER_NODES 10000

double *
generate_zero_vector(int n)
{
    double *e = calloc(n,sizeof(double));
    if(!e) {
      fprintf(stderr,"Out of Memory generate_zero_vector\n");
      abort();
    }
    return e;
}
char *
generate_byte_zero_vector(int n)
{
    char *e = calloc(n,sizeof(char));
    if(!e) {
      fprintf(stderr,"Out of Memory generate_zero_vector\n");
      abort();
    }
    return e;
}
void
delete_vector(void *p)
{
    if(p) {
	free(p); p = NULL;
    }
}

void
print_byte_matrix(char **a, int m, int n)
{
  int i, j;
  for(i = 0; i< m; i++)
    {
      for(j= 0 ; j< n; j++)
	{
	    printf("%d  ",(int)a[i][j]);
	}
      printf("\n");
    }
  printf("\n");
}

void
delete_byte_matrix(char **a, int m, int n)
{
  int i;
  for(i = 0; i< m; i++)
    {
	free(a[i]); a[i]=NULL;
    }
  free(a); a = NULL;
}

char **
get_byte_matrix(int m, int n)
{
  char **a = NULL;
  int i = 0;
  a = (char **)calloc(m , sizeof(char *));
  if(!a) {
      fprintf(stderr,"Out of Memory get_short_matrix\n");
      abort();
  }
  for(i = 0; i< m; i++) {
    a[i] = (char *)calloc(n , sizeof(char));
    if(!a[i]) {
	fprintf(stderr,"Out of Memory get_short_matrix\n");
	abort();
    }
  }
  return a;
}
void
print_double_matrix(double **a, int m, int n)
{
  int i, j;
  for(i = 1; i<= m; i++)
    {
      for(j= 1 ; j<= n; j++)
	{
	    printf("%f  ",a[i][j]);
	}
      printf("\n");
    }
  printf("\n");
}
void
print_double_vector(double *a, int n)
{
    int i;
    printf("[");
    for(i=1;i<=n;i++)
	printf("%f ", a[i]);
    printf("]\n\n");
}

void
delete_double_matrix(double **a, int m, int n)
{
  int i;
  for(i = 0; i< m; i++)
    {
	free(a[i]); a[i]=NULL;
    }
  free(a); a = NULL;
}

double **
get_double_matrix(int m, int n)
{
  double **a = NULL;
  int i = 0;
  a = (double **)calloc(m , sizeof(double *));
  if(!a) {
      fprintf(stderr,"Out of Memory get_double_matrix\n");
      abort();
  }
  for(i = 0; i< m; i++) {
    a[i] = (double *)calloc(n , sizeof(double));
    if(!a[i]) {
	fprintf(stderr,"Out of Memory get_double_matrix\n");
	abort();
    }
  }
  return a;
}

double **
copy_byte_matrix(char ** a, int m, int n)
{
    double ** b = get_double_matrix(m+1,n+1);
    int i, j;
    for(i=1; i<= m ; i++)
	for(j=1; j<= n ; j++)
	    b[i][j] = (double)a[i-1][j-1];
    return b;
}

double **
copy_double_matrix(double ** a, int m, int n)
{
    double ** b = get_double_matrix(m,n);
    int i, j;
    for(i=0; i< m ; i++)
	for(j=0; j< n ; j++)
	    b[i][j] = a[i][j];
    return b;
}

double *
copy_double_vector(char *a, int n)
{
    double *v = generate_zero_vector(n+1);
    int i;
    for(i=1; i<=n; i++)
	v[i]=(double)a[i-1];
    return v;
}

int
enumerate_inside_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 1;
  for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->info;

      if(v) {
	  if(c == MAX_CLUSTER_NODES)
	  {
	     v->prelim = 0;
	     continue;
	  }
	  if(is_vertex_sink(v))
	      v->prelim = 0;
	  else 
	      v->prelim = c++;
      }
    }
  return c-1;
}

int
enumerate_vertices_by_shape(GraphFrame *gf,char shape)
{
  LNode_type *ptr;
  int c = 1;
  for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->info;

      if(v) {
	  if(c == MAX_CLUSTER_NODES)
	  {
	     v->prelim = 0;
	     continue;
	  }
	  if(get_shape(v)!=shape)
	      v->prelim = 0;
	  else 
	      v->prelim = c++;
      }
    }
  return c-1;
}

/* get_adjacency_matrix - Creates an adjacency matrix from the Graph 
                             Structure.

         inputs  
		 gf - Graph Structure
         returns
                 adjacency matrix
*/
char **
get_reduced_adjacency_matrix(GraphFrame *gf, int *n)
{
  LNode_type *ptr = NULL;
  int noc = gf->count_vertex;

  char **a = NULL ; 
  *n = 0;

  if(noc <= 1 )
    return NULL;

  printf("before enumerate %d %d\n", noc, MAX_CLUSTER_NODES);

  noc = enumerate_vertices_by_shape(gf,BOX);

  if(noc == 0)
      noc = enumerate_inside_vertices(gf);

  printf("after enumerate %d %d\n", noc, MAX_CLUSTER_NODES);
  if(noc > MAX_CLUSTER_NODES)
      return NULL;

  *n = noc;
  a = get_byte_matrix(noc,noc);

  for(ptr= gf->list_vertices->next ; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei = NULL;
      if(!v)
	 continue;
      if(v->prelim <=0)
	  continue;
      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
	  PEDGE e = (PEDGE)ei->info;
	  PNODE u = e->from;
	  if(u->prelim <=0)
	      continue;
	  a[u->prelim-1][v->prelim-1] = 1;
	  if(gf->graph_dir == UNDIRECTED)
	      a[v->prelim-1][u->prelim-1] = 1;
      }
    }
  return a;
}
/* get_adjacency_matrix - Creates an adjacency matrix from the Graph 
                             Structure.

         inputs  
		 gf - Graph Structure
         returns
                 adjacency matrix
*/
char **
get_adjacency_matrix(GraphFrame *gf)
{
  LNode_type *ptr = NULL;
  int noc = gf->count_vertex;

  char **a = NULL ; 

  if(noc <= 1 )
    return NULL;

  a = get_byte_matrix(noc,noc);

  enumerate_vertices(gf);

  for(ptr= gf->list_vertices->next ; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei = NULL;
      if(!v)
	 continue;
      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
	  PEDGE e = (PEDGE)ei->info;
	  PNODE u = e->from;
	  a[u->prelim-1][v->prelim-1] = 1;
	  if(gf->graph_dir == UNDIRECTED)
	      a[v->prelim-1][u->prelim-1] = 1;
      }
    }
  return a;
}

PNODE
find_node_by_index(GraphFrame *cgr, int index)  
{
  LNode_type *ptr;
  for(ptr = cgr->list_vertices->back; ptr != cgr->list_vertices; 
      ptr= ptr->back)
    {
      PNODE ov = (PNODE)ptr->info;
      if(ov->prelim == index)
	{
	    return ov;
	}
    }
  return NULL;
}

char **
transpose_byte_matrix(char **a, int m, int n)
{
  char **at = get_byte_matrix(n,m);
  int i,j;
  for(i = 0; i< m; i++)
    {
      for(j= 0 ; j< n; j++)
      {
	  at[i][j] = a[j][i];
      }
    }
  return at;
}
char **
multiply_byte_matrix(char **a, int m, int n, char **b, int p, int q)
{
  char **c = get_byte_matrix(m,q);
  int i,j,k;

  if(n != p)
      return NULL;

  for(i = 0 ; i < m ; i++)
  {
    for(j = 0 ; j < q ; j++)
    {
      c[i][j]=0;
      for(k = 0; k<n ; k++)
      {
	c[i][j] += a[i][k]*b[k][j];  
      }
    }
  }
  return c;
}
double **
transpose_double_matrix(double **a, int m, int n)
{
  double **at = get_double_matrix(n+1,m+1);
  int i,j;
  for(i = 1; i<= n; i++)
    {
      for(j= 1 ; j<= m; j++)
      {
	  at[i][j] = a[j][i];
      }
    }
  return at;
}
double **
multiply_double_matrix(double **a, int m, int n, double **b, int p, int q)
{
  double **c = get_double_matrix(m+1,q+1);
  int i,j,k;

  if(n != p)
      return NULL;

  for(i = 1 ; i <= m ; i++)
  {
    for(j = 1 ; j <= q ; j++)
    {
      c[i][j]=0;
      for(k = 1; k<=n ; k++)
      {
	c[i][j] += a[i][k]*b[k][j];  
      }
    }
  }
  return c;
}
double **
multiply_double_matrix_byte_matrix(double **a, int m, int n, char **b, 
				    int p, int q)
{
  double **c = get_double_matrix(m+1,q+1);
  int i,j,k;

  if(n != p)
      return NULL;

  for(i = 1 ; i <= m ; i++)
  {
    for(j = 1 ; j <= q ; j++)
    {
      c[i][j]=0;
      for(k = 1; k<=n ; k++)
      {
	c[i][j] += a[i][k]*(double)b[k-1][j-1];  
      }
    }
  }
  return c;
}
double *
multiply_double_vector_matrix(double *b, double **a, int m, int n)
{
  double *c = generate_zero_vector(n+1);
  int i,j;

  for(j = 1 ; j <= n ; j++)
  {
    c[j]=0.0;
    for(i = 1 ; i <= m ; i++)
    {
	c[j] += b[i]*a[i][j];
    }
  }
  return c;
}
double *
multiply_abs_double_vector_matrix(double *b, double **a, int m, int n)
{
  double *c = generate_zero_vector(n+1);
  int i,j;

  for(j = 1 ; j <= n ; j++)
  {
    c[j]=0.0;
    for(i = 1 ; i <= m ; i++)
    {
	c[j] += fabs(b[i]*a[i][j]);
    }
  }
  return c;
}

double *
multiply_double_matrix_vector(double **a, int m, int n, double *b)
{
  double *c = generate_zero_vector(m+1);
  int i,j;

  for(i = 1 ; i <= m ; i++)
  {
    c[i]=0.0;
    for(j = 1 ; j <= n ; j++)
    {
	c[i] += a[i][j]*b[j];  
    }
  }
  return c;
}


/**********************************************************************
      JACOBI from "Numerical recipes in C: The Art of Scientific Computing"

      Computes all eigenvalues and eigenvectors of a real symmetric matrix
      a. On output, elements of a above the diagonal are destroyed. 
      d returns the eigenvalues of a. v is a matrix whose columns contain,
      on output, the normalized eigenvectors of a. nrot returns the number
      of Jacobi rotations that were required.
**********************************************************************/

#define ROTATE(a,i,j,k,l) g=a[i][j]; h=a[k][l]; a[i][j]=g-s*(h+g*tau);\
    a[k][l]=h+s*(g-h*tau);

#define NUM_SWEEPS 4
#define NUM_CLUSTERS 10
#define MIN_WEIGHT 0.5

int
compute_jacobi(double **a, int n, double d[], double **v, int *nrot)
{
  int j, iq, ip,i;
  double tresh, theta, tau, t, sm, s,h, g,c,*b, *z;
  b = generate_zero_vector(n+1);
  z = generate_zero_vector(n+1);
  for(ip = 1; ip<=n; ip++)
  {
      for(iq=1; iq<=n; iq++) v[ip][iq]=0.0;
      v[ip][ip]=1.0;
  }
  for(ip = 1; ip<=n; ip++)
  {
      b[ip]=d[ip]=a[ip][ip];
      z[ip]=0.0;
  }
  *nrot = 0;
  for(i=1;i<=50;i++) 
  {
      sm = 0.0;
      for(ip=1;ip<=n-1;ip++)
      {
	  for(iq=ip+1;iq<=n;iq++)
	      sm += fabs(a[ip][iq]);
      }
      if(sm == 0.0) {
	  delete_vector((void *)z);
	  delete_vector((void *)b);
	  return 0;
      }
      if (i < 4)
	  tresh = 0.2*sm/(n*n);
      else
	  tresh = 0.0;
      for(ip = 1; ip<=n-1; ip++) 
      {
	  for(iq=ip+1; iq <=n; iq++)
	  {
	      g = 100.0*fabs(a[ip][iq]);
	      if(i > NUM_SWEEPS && (fabs(d[ip])+g == fabs(d[ip]))
		 && (fabs(d[iq])+g == fabs(d[iq])))
	      {
		  a[ip][iq]=0.0;
	      }
	      else if(fabs(a[ip][iq]) > tresh) 
	      {
		  h = d[iq]-d[ip];
		  if(fabs(h)+g == fabs(h))
		  {
		      t = (a[ip][iq])/h;
		  }
		  else
		  {
		      theta = 0.5*h/(a[ip][iq]);
		      t = 1.0/(fabs(theta)+sqrt(1.0+theta*theta));
		      if(theta < 0.0) t=-t;
		  }
		  c = 1.0/sqrt(1+t*t);
		  s = t*c;
		  tau = s/(1.0+c);
		  h = t*a[ip][iq];
		  z[ip]-=h;
		  z[iq]+=h;
		  d[ip]-=h;
		  d[iq]+=h;
		  a[ip][iq]=0.0;
		  for(j=1;j<=ip-1; j++) 
		  {
		      ROTATE(a,j,ip,j,iq)
		   }
		  for(j=ip+1;j<=iq-1;j++) 
		  {
		      ROTATE(a,ip,j,j,iq)
		   }
		  for(j=iq+1;j<=n;j++)
		  {
		      ROTATE(a,ip,j,iq,j)
		   }
		  for(j=1;j<=n;j++)
		  {
		      ROTATE(v,j,ip,j,iq)
		  }
		  ++(*nrot);
	      }
	  }
      }
      for(ip=1; ip<=n;ip++)
      {
	  b[ip]+=z[ip];
	  d[ip]=b[ip];
	  z[ip]=0.0;
      }
  }
  fprintf(stderr,"Too many iterations in routine jacobi\n");
  return 1;
}

int is_index_lower(double *p, double *q)
{
    return(*p < *q);
}

int is_index_bigger(double *p, double *q)
{
    return(*p > *q);
}
double **
get_pointer_vector(double *v, int n)
{
    double **r;
    int i;
    r = (double **)calloc(n , sizeof(double *));
    if(!r) {
	fprintf(stderr,"Out of Memory get_pointer_vector\n");
	abort();
    }
    for(i = 0; i < n ; i++)
    {
	r[i] = &v[i];
    }
    return r;
}
double
dmodule(double *a, int n)
{
  int i = 0;
  double sum = 0.0;
  for(i=1; i<=n;i++)
      sum+= a[i]*a[i];
  return (sqrt(sum));
}

void
add_vector_double(double *a, double *b, int n)
{
  int i = 0;
  for(i=1; i<=n;i++)
  {
    a[i] += b[i];
  }
}

void
clear_vector_double(double *a, int n)
{
  int i = 0;
  for(i=1; i<=n;i++)
  {
    a[i] = 0.0;
  }
}
void
normalize_vector(double *Q, int n)
{
  double nor = dmodule(Q,n);
  int i = 0; 
  if (nor <=0)
      return;
  for(i = 1; i<= n; i++)	
  {
    Q[i] = Q[i]/nor;
  }
}

double *
compute_qs(double **S,int s,int n,double **v,int inx)
{
    double *p = v[inx]; /* eigenvector */
    double *sp = multiply_double_vector_matrix(p,S,s,n);
    double nor = dmodule(sp,n);
    int i=0;
    if(nor > 0)
	for(i=1;i<= n ; i++)
	    sp[i]=sp[i]/nor;
    return sp;
}

void
combine_highest_clusters(double **w,int k,int n)
{
  double wmax = 0;
  int jmax = 0;
  int i,j,l;
  double *maxs = NULL;
  int *jmaxs = NULL;
  maxs = (double *)calloc(k+1, sizeof(double));
  if(!maxs) {
      fprintf(stderr,"Out of Memory combine_highest_clusters\n");
      abort();
  }
  jmaxs = (int *)calloc(k+1, sizeof(int));
  if(!jmaxs) {
      fprintf(stderr,"Out of Memory combine_highest_clusters\n");
      abort();
  }
  for(i=1; i <= k; i++)
  {
    wmax = 0;
    jmax = 0;
    for(j=1;j<=n;j++)
    {
      if(w[i][j] > wmax)
      {
	wmax = w[i][j];
	jmax = j;
      }
    }
    maxs[i] = wmax;
    jmaxs[i] = jmax;
    printf("Maximum of Cluster %d is %d:%f\n", i,jmax,wmax);
  }
  for(i=1; i <= k-1; i++)
  {
    if(jmaxs[i] == 0)
       continue;
    for(l=i+1; l<=k; l++)
    {
      if(jmaxs[i] == jmaxs[l])
      {
	printf("Combining Clusters %d %d\n", jmaxs[i], jmaxs[l]);
	add_vector_double(w[i],w[l],n);
	clear_vector_double(w[l],n);
	normalize_vector(w[i],n);
	jmaxs[l] = 0;
      }
    }
  }
  free(jmaxs); jmaxs = NULL;
  free(maxs); maxs = NULL;
}
void
eliminate_weights(double **w,int k,int n)
{
    double wmax = 0;
    int i,j;
    int imax;
    for(j=1 ; j<= n; j++)
    {
	wmax = w[1][j];
	imax = 1;
	w[1][j]=0.0;
	for(i=2;i<=k;i++)
	{
	    if(w[i][j] > wmax)
	    {
		wmax = w[i][j];
		imax = i;
	    }
	    w[i][j]=0.0;
	}
	w[imax][j]=wmax;
    }
}
void
eliminate_lowest_values(double **w,int k,int n)
{
  int i,j;
  double mlog = 0;
  double slog = 0;
  int nc = 0;
  for(i=1;i<=k;i++)
  {
    slog = 0;
    nc = 0;
    for(j=1 ; j<= n; j++)
    {
      if(w[i][j] > 0)
      {
	  /*slog += log(w[i][j]);*/
	slog += w[i][j]*w[i][j];
	nc++;
      }
    }
    if(nc == 0)
	continue;
    mlog = slog/n;
    /*mlog = exp(mlog);*/
    printf("MEDIAN log %f of %d values\n", mlog,n);
    nc = 0;
    for(j=1 ; j<= n; j++)
    {
      if(w[i][j] < mlog && w[i][j] > 0)
      {
	  w[i][j] = 0;
	  nc++;
      }
    }
    printf("%d values deleted\n",nc);
  }
}

void
erase_column(double **w, int k, int n, int index)
{
  int i, j;
  for(j=1 ; j<=n; j++)
  {
    if(w[index][j] > 0)
    {
	for(i = index+1 ; i<= k; i++)
	    /*for(i = index-1 ; i>= 1; i--)*/
	w[i][j] = 0;
    }
  }
}

void
compute_cluster_component(double **w,int k,int n, GraphFrame *gf)
{
  int i,j;
  int nc;
  gf->draw = False;
  for(i=1 ; i<=k; i++)
      /*for(i=k ; i>=1; i--)*/
  {
    deselect_all_nodes(gf);

    for(j=1; j<=n; j++)
    {
      if(w[i][j]>0)
      {
	PNODE v = find_node_by_index(gf,j);

	if(!is_vertex_selected(v))
	    select_vertex(v,gf);
	
      }
    }	

    nc = select_max_component_of_selected_vertices(gf);

    for(j=1; j<=n; j++)
    {
      if(w[i][j]>0)
      {
	PNODE v = find_node_by_index(gf,j);
	if(!is_vertex_selected(v))
	    w[i][j] = 0;
      }	
    }
    erase_column(w,k,n,i);
  }
  deselect_all_nodes(gf);
  gf->draw = True;

}

void
compute_cluster_tree(double **w,int k,int n, GraphFrame *gf)
{
  int i,j;
  int nc = 0, nvs = 0;
  double wmax = 0;
  PNODE vroot = NULL;
  int sdraw = gf->draw;
  int pdraw = gf->pdraw;
   
  gf->draw = False;
  gf->pdraw = False;


  for(i=1 ; i<=k; i++)
  {
    deselect_all_nodes(gf);
    wmax = 0;
    vroot = NULL;
    for(j=1; j<=n; j++)
    {
      if(w[i][j]!=0)
      {
	PNODE v = find_node_by_index(gf,j);

	if(!is_vertex_selected(v))
	    select_vertex(v,gf);
	if(wmax < w[i][j])
	{
	  wmax = w[i][j];
	  vroot = v;
	}
      }
    }	

    if(!vroot)
	continue;

    printf("ROOT of Cluster %d:%f is %d %s\n", i,wmax, vroot->num, vroot->label);

    nvs = count_llist(get_list_selected_vertices(gf));

    nc = select_tree_of_selected_vertices(gf,vroot);

    if(nc == nvs)
	continue;

    for(j=1; j<=n; j++)
    {
      if(w[i][j]!=0)
      {
	PNODE v = find_node_by_index(gf,j);
	if(!is_vertex_selected(v))
	{
	    /*printf("Deleting value (%d, %d) , %f\n", i,j,w[i][j]);*/
	    w[i][j] = 0;
	}
      }	
    }
    normalize_vector(w[i],n);
    printf("Normalizing cluster %d\n", i);
  }
  deselect_all_nodes(gf);
  gf->draw = sdraw;
  gf->pdraw = pdraw;
}

void 
print_clusters(double **w, int k, int n, GraphFrame *gf)
{
    int i,j;
    for(i=1;i<=k;i++)
    {
	printf("%d CLUSTER\n",i);
	for(j=1;j<=n;j++)
	{
	    if(w[i][j]!=0)
	    {
		PNODE v = find_node_by_index(gf,j);
		if(v)
		    printf("w %f V %d %s\n",w[i][j],v->num, v->label);
	    }
	}
    }

}

int
assign_clusters(double **w, int k, int n, GraphFrame *gf)
{
    int i,j;
    int nc = 0;
    reset_node_context(gf);
    for(i=1;i<=k;i++)
    {
      for(j=1;j<=n;j++)
      {
	if(w[i][j]>0)
	{
	  nc++;
	  break;
	}
      }
      if(w[i][j] > 0)
      {
	for(;j<=n;j++)
	{
	  if(w[i][j]>0)
	  {	
	    PNODE v = find_node_by_index(gf,j);
	    if(v)
	      v->context = (void *)nc;
	  }
	}
      }	
    }
    return nc;
}

int 
frobenius_norm(char **a, int m, int n)
{
    int fn = 0;
    int i, j;
    for(i = 0; i< n; i++)
	for(j=0; j< n; j++)
	    fn += (int)(a[i][j]*a[i][j]);
    return fn;
}

double * 
probability_vector(char **a, int m, int n)
{
    double *pi = generate_zero_vector(m+1);
    int fn = frobenius_norm(a,m,n);
    int i,j;
    printf("FNORM %d \n", fn);
    for(i=1; i<=n;i++)
    {	
	int imods = 0;
	for(j = 0; j< m; j++)
	    imods += (int)(a[j][i-1] * a[j][i-1]);

	pi[i] = (double)imods/ (double)fn;
    }
    return pi;
}
double *
copy_double_column(char **a,int m, int n, int i)
{	
    double *c = generate_zero_vector(m+1);
    int j;
    for(j = 0; j < m ; j++)
    {
	c[j+1]  = (double)a[j][i];
    }
    return c;
}

/*
  1. Generate a pair of uniform random numbers, u1, u2 in the interval (0,1)
  2. Using u1 locate a point p on the horizontal axis as
            p = A + (B - A)* u1
  3. Using u2 locate a point q on the vertical axis as q = C * u2;
  4. if q > f(p) reject the pair and go to step 1. otherwise, accept p
     as the value of a sample from the desired distribution
*/

int
select_random_index(double *pi, int m, char *sel, double maxp)
{
  double u1,u2,q;
  int p = 0;
  while(1) {
      u1 = drand48();
      u2 = drand48();
      p = 1 + (int)(m - 1)*u1;
      q = maxp * u2;
      if(q <= pi[p])
      {
	  if(!sel[p])
	  {
	    sel[p] = 1;
	    return p;
	  } 
	  else
	  {
	    int j;
	    for(j = p+1; j != p ; j++)
	    {
	       if(j > m) j = 1;
	       if(!sel[j])
	       {  
		 sel[j] = 1;
		 return j;
	       }
	    }
	    return 0;
	  }
      }
  }
}

double
get_upper_limit(double *pi,int m)
{
  int i;
  double maxp = pi[1];

  for(i = 2; i<= m; i++)
  {
      if(pi[i] > maxp)
	  maxp = pi[i];
  }
  return maxp;
}

double **
select_random_matrix(char **a, int m, int n, double *pi, int s)
{
  double **S = NULL;
  char *sel = generate_byte_zero_vector(m+1);
  int i,j;
  double maxp;

  srand48(time(NULL));

  S = (double **)calloc(s+1, sizeof(double *));
  
  maxp = get_upper_limit(pi,m);

  printf("upper limit is %f\n", maxp);

  for (i = 1; i<=s ; i++)
  {
    int index = select_random_index(pi,m,sel,maxp);

    /*printf("COLUMN chosen %d \n", index);*/

    if(index == 0)
    {
	free(sel); sel = NULL;
	return NULL;
    }

    S[i] = copy_double_column(a,m,n,index-1);

    for(j = 1; j<= m; j++)
	S[i][j] = S[i][j]/sqrt(s * pi[index]);
  }
  printf("Selection is finished\n");
  free(sel); sel = NULL;
  return S;
}

double **
select_random_matrix_max(char **a, int m, int n, double *pi, int s)
{
  double **ppi = get_pointer_vector(pi,m+1);
  double **S = NULL;
  int i,j;
  S = (double **)calloc(s+1, sizeof(double *));
  for (i = 1; i<=s ; i++)
  {
    double *ip = (double *)randomized_select((GPTR *)ppi,i,m,1,
					     is_index_lower,is_index_bigger);
    int index = (int)(ip - pi);

    /*printf("COLUMN chosen %d \n", index);*/

    S[i] = copy_double_column(a,m,n,index-1);

    for(j = 1; j<= m; j++)
	S[i][j] = S[i][j]/sqrt(s * pi[index]);
  }
  free(ppi); ppi = NULL;
  return S;
}
/* a (adjacency matrix ) (m x n)
   k (number of clusters)
   */
double **
get_random_selected(char **a, int m, int n,int s)
{
  double *pi = probability_vector(a, m, n);
  double **S = NULL; 
  printf("PI vector = \n");
  /*print_double_vector(pi,m);*/
  S = select_random_matrix(a, m, n, pi, s);
  free(pi); pi = NULL;
  return S;
}

double *
split_negatives(double *QI,int s)
{
  int i = 0;
  double *NQ = generate_zero_vector(s+1);
  for (i = 1; i <= s; i++)
  {
    if(QI[i] < 0) 
    {
      NQ[i] = -QI[i];
      QI[i] = 0;
    }	

  }	
  return NQ;
}
int 
split_negative_clusters(double **Q, int n, int s, int k)
{
  int i=0,j = 0,in = k+1;
  for(i=1;i<=k;i++)
  {
    double *QI = Q[i];
    for(j = 1;j <=s; j++)
    {
      if(QI[j] < 0) 
      {
	Q[in] = split_negatives(QI,s);
	normalize_vector(Q[in],s);
	normalize_vector(QI,s);
	in++;
      }
    }	
  }
  return in-1;
}

int
compute_clusters(GraphFrame *gf,int k)
{
    char **A = NULL;
    double **S = NULL;
    double **ST = NULL;
    double **STS = NULL;
    double *E = NULL;
    double **PE = NULL;
    double **V = NULL;
    double **VT = NULL;
    double **Q = NULL;
    int s = 0;
    int i = 0;
    int nrot=0;
    double eps = 0.5;
    double delt = 0.5;
    int noc = 0;
    int nk = 0;
    int rk = 0;

    A = get_reduced_adjacency_matrix(gf,&noc);

    if(!A)
    {
      printf("Graph is too big: %d nodes\n", noc);
      return 0;
    }

    if(k < 0) k = NUM_CLUSTERS;
    if(k > noc) k = noc;

    s = (int) ((4.0  * k)/ (eps *eps *delt));
    if(s > noc) s = noc;
    printf("s chosen are %d from %d columns and %d clusters\n", s, noc, k);
    printf("A = (%d, %d)\n",noc,noc);
    /*print_byte_matrix(A, noc,noc);*/
    /*S = get_random_selected(A, noc, noc,s);*/

    if(s != noc) 
    {
	S = get_random_selected(A, noc, noc,s);
	ST = transpose_double_matrix(S,s,noc);
    }
    else
    {
	ST = copy_byte_matrix(A, noc,noc);
	S = transpose_double_matrix(ST,noc,noc);
    }
    printf("S = (%d, %d)\n",s,noc);
    /*print_double_matrix(S, s,noc);*/
    printf("ST = (%d,%d)\n",noc,s);
    /*print_double_matrix(ST, noc,s);*/
    STS = multiply_double_matrix(S,s,noc, ST, noc,s);
    printf("STS = (%d,%d)\n",s,s);
    /*print_double_matrix(STS, s,s);*/
    delete_double_matrix(ST,s,noc);
    E = generate_zero_vector(s+1);
    V = get_double_matrix(s+1,s+1);

    printf("START JACOBI\n");

    if(compute_jacobi(STS,s,E,V,&nrot))
    {
      delete_byte_matrix(A,noc,noc);
      delete_double_matrix(STS,s,s);
      delete_double_matrix(S,s,noc);
      delete_double_matrix(V,s,s);
      free(E); E = NULL;
      return 0;
    }
    
    printf("FINISHED JACOBI\n");
    printf("E = \n");
    /*print_double_vector(E,s);*/
    /*printf("STS = \n");
    print_double_matrix(STS,s,s);
    printf("V = \n");
    print_double_matrix(V,s,s);*/
    printf("nrot %d\n", nrot);

    VT = transpose_double_matrix(V,s,s);
    PE = get_pointer_vector(E,s+1);

    Q = (double **)calloc(2*k+1, sizeof(double *));
    if(!Q) {
	fprintf(stderr,"Out of Memory compute_clusters\n");
	abort();
    }

    for(i=1; i <= k; i++)
    {
	double *gp;
	PNODE uv;
	double *QI = NULL;
	int pi = 0;
	gp = (double *)randomized_select((GPTR *)PE,i,s,1,
					 is_index_lower,is_index_bigger);
	pi = (int)(gp-E);
	printf("eigenvalue %d = (%d,%f)\n", i,pi, *gp);
	uv = find_node_by_index(gf,pi);
	if(uv)
	    printf("NODE is %d %s\n", uv->num, uv->label);
	printf("EIGENVECTOR\n");
	/*print_double_vector(VT[pi],s); */
	printf("CLUSTER\n");
	QI = compute_qs(S,s,noc,VT,pi);
	/*print_double_vector(QI,noc); */
	Q[i]=QI;
    }
    printf("CLUSTER MATRIX (%d,%d)\n",k,noc);
    /*print_double_matrix(Q,k,noc);*/

    nk = split_negative_clusters(Q,2*k+1,noc,k);
    /*nk = k;*/

    printf("NEW CLUSTER MATRIX (%d,%d)\n",nk,noc);
    /*print_double_matrix(Q,nk,noc);*/

    /*eliminate_weights(Q,nk,noc);*/
    /*eliminate_lowest_values(Q,nk,noc);*/
    /*compute_cluster_component(Q,nk,noc,gf);*/
    combine_highest_clusters(Q,nk,noc);
    compute_cluster_tree(Q,nk,noc,gf);
    eliminate_weights(Q,nk,noc);
    compute_cluster_tree(Q,nk,noc,gf);
    rk = assign_clusters(Q,nk,noc,gf);
    printf("Total Cluster %d\n", rk);
    printf("CLUSTER MATRIX UNIQUE(%d,%d)\n",nk,noc);
    /*print_double_matrix(Q,nk,noc);*/
    print_clusters(Q,nk,noc,gf);

    set_num_clusters(gf,rk);

    delete_byte_matrix(A,noc,noc);
    delete_double_matrix(Q,nk,noc);
    delete_double_matrix(STS,s,s);
    delete_double_matrix(S,s,noc);
    delete_double_matrix(V,s,s);
    delete_double_matrix(VT,s,s);
    free(E); E = NULL;
    free(PE); PE = NULL;
    return rk;
}

/***************************************************************************/

void 
dfs_connected_component_undirected(PNODE v, int nclique)
{
  LNode_type *poed;
  v->mark = True;
  v->context = (void *)nclique;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->from;

      if(tv == v)
	  tv = e->to;

      if( tv->mark == False)
	{
	  dfs_connected_component_undirected(tv,nclique);
	}
    }
  v->pick = True;
}

void 
dfs_connected_component_directed(PNODE v, int nclique)
{
  LNode_type *poed;
  v->mark = True;
  v->context = (void *)nclique;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->from;

      if(tv == v)
	  continue;

      if( tv->mark == False)
	{
	  dfs_connected_component_directed(tv,nclique);
	}
    }
  for(poed = v->ledges_out->back; poed != v->ledges_out; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->to;

      if(tv == v)
	  continue;

      if( tv->mark == False)
	{
	  dfs_connected_component_directed(tv,nclique);
	}
    }
  v->pick = True;
}

int
compute_connected_components(GraphFrame *gf)
{
  int nc = 1;
  LNode_type *ptr;
  reset_mark_pick_vertices(gf);
  if(gf->graph_dir == DIRECTED) {
      for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; 
	  ptr = ptr->back)
      {
	  PNODE v = (PNODE)ptr->info;
	  if(!v->pick)
	      dfs_connected_component_directed(v, nc++);
	  
      }
  } else {
      for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; 
	  ptr = ptr->back)
      {
	  PNODE v = (PNODE)ptr->info;
	  if(!v->pick)
	      dfs_connected_component_undirected(v, nc++);
      }
  }
  printf("%d connected components computed \n",nc-1);
  set_num_components(gf,nc-1);
  return nc-1;
}

void 
dfs_sel_connected_component_undirected(PNODE v, int nclique)
{
  LNode_type *poed;
  v->mark = True;
  v->context = (void *)nclique;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->from;

      if(tv == v || !(tv->select))
	  continue;

      if( tv->mark == False)
	{
	  dfs_sel_connected_component_undirected(tv,nclique);
	}
    }
  for(poed = v->ledges_out->back; poed != v->ledges_out; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->to;

      if(tv == v || !(tv->select))
	  continue;

      if( tv->mark == False)
	{
	  dfs_sel_connected_component_undirected(tv,nclique);
	}
    }
  v->pick = True;
}

int
compute_sel_connected_components(GraphFrame *gf)
{
  int nc = 1;
  LNode_type *ptr;
  reset_mark_pick_vertices(gf);
  for(ptr= gf->list_sel_vertex->back; ptr != gf->list_sel_vertex ; 
      ptr = ptr->back)
  {
      PNODE v = (PNODE)ptr->info;
      if(!v->pick)
	  dfs_sel_connected_component_undirected(v, nc++);
  }
  printf("%d connected components computed on selected vertices \n",nc-1);
  /*set_num_components(gf,nc-1);*/
  return nc-1;
}

int
select_max_component_of_selected_vertices(GraphFrame *gf)
{
  LNode_type *lsel = get_list_selected_vertices(gf);
  int nc = compute_sel_connected_components(gf);
  int *nvercomp = NULL;
  LNode_type *ptr = NULL;  
  int i,imax, maxv;

  if(nc == 1)
      return (count_llist(lsel));

  nvercomp = (int *)calloc(nc, sizeof(int));
  if(!nvercomp) {
    fprintf(stderr,"Out of Memory select_max_component_of_selected_vertices\n");
    abort();
  }
  
  for(ptr = lsel->back; ptr!= lsel ; ptr= ptr->back)
  {
    PNODE v = (PNODE)ptr->info;
    int icomp = (int)get_node_context(v);
    if(icomp > 0)
	nvercomp[icomp-1]++;
    else
	fprintf(stderr,"Error computing connected components\n");
  }

  /* select the maximum component */
  maxv = nvercomp[0];
  imax = 0;
  for(i=1; i< nc ; i++)
  {
    if(nvercomp[i] > maxv)
    {
      imax = i;
      maxv = nvercomp[i];
    }
  }
  imax++;
  printf("Maximum component is %d=%d of %d components\n", imax,maxv, nc);
  free(nvercomp); nvercomp = NULL;
  if(maxv == 1)
      imax = 0;
  /* deselect vertices that are not in the maximum component */
  for(ptr = lsel->back; ptr!= lsel ; ptr= ptr->back)
  {
    PNODE v = (PNODE)ptr->info;
    int ncomp = (int)get_node_context(v);
    if(ncomp != imax)
    {
      if(is_vertex_selected(v)) 
      {
	  /*printf("Deselecting %d %s\n", v->num, v->label);*/
	deselect_vertex(v,gf);
      }
    }
  }
  return (count_llist(lsel));
}

void 
dfs_sel_undirected(PNODE v)
{
  LNode_type *poed;
  v->mark = True;
  
  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      PEDGE e = (PEDGE)poed->info;
      PNODE tv = e->from;

      if(tv == v || !(tv->select))
	  continue;

      if( tv->mark == False)
	{
	  dfs_sel_undirected(tv);
	}
    }
  v->pick = True;
}

int
select_tree_of_selected_vertices(GraphFrame *gf, PNODE root)
{
  LNode_type *lsel = get_list_selected_vertices(gf);
  int nc = count_llist(lsel);
  LNode_type *ptr = NULL;  
  reset_mark_pick_vertices(gf);
  dfs_sel_undirected(root);
  for(ptr = lsel->back; ptr!= lsel ; ptr= ptr->back)
  {
    PNODE v = (PNODE)ptr->info;
    if(v->mark == False)
    {
      if(is_vertex_selected(v)) 
      {
	deselect_vertex(v,gf);
      }
    }
  }
  nc = count_llist(lsel);
  return(nc);
}

