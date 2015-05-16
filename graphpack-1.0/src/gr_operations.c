#include "gr_operations.h"
#include "gr_structure.h"
#include "gr_graph_p.h"
#include "gr_window.h"
#include "gr_drawing.h"
#include "gr_util.h"
#include "gr_bary.h"
#include "gr_authub.h"
#include "gr_algorithms.h"

/* find_vertex - Find a vertex in the Graph that has a near given location. 

         inputs  
                 cgr - Graph Structure
		 loc - location of the vertex. 
         returns
                  Vertex
         side effects
                  none
*/

struct pt *
find_vertex(GraphFrame *cgr, struct pr_pos *loc)
{
  Node_type *nptr;
  nptr = select_nearest_vertex(loc,cgr);
  if(nptr == NULL)
    return NULL;
  else
    return ((struct pt *)(nptr->info.gr));
}

struct pt *
find_vertex_by_name(GraphFrame *cgr, char *vlabel)
{
  Node_type *nptr;
  nptr = Retrieve(cgr->HVname,(Key_type)vlabel);
  if(nptr == NULL)
    return NULL;
  else
    return ((struct pt *)(nptr->info.gr));
  
}

struct pt *
find_vertex_by_number(GraphFrame *cgr, int number)
{
  LNode_type *ptr;
  struct pt *v = NULL;
  for(ptr = cgr->list_vertices->back; ptr != cgr->list_vertices; 
      ptr= ptr->back)
    {
      struct pt *ov = (struct pt *)ptr->info;
      if(ov->num == number)
	{
	  v = ov;
	  break;
	}
    }
  return v;
}
/* find_edge - Find a edge in the Graph that has the two given extreme 
               vertices. 
         inputs  
                 cgr - Graph Structure
		 v1, v2 - Extreme vertices of the edge.
         returns
                  Edge
         side effects
                  none
*/

struct edge *
find_edge(GraphFrame *cgr,struct pt *v1, struct pt *v2)
{
  Node_type *nptr;
  nptr = select_nearest_edge(v1,v2,cgr);
  if(nptr == NULL)
    return NULL;
  else
    return ((struct edge*)(nptr->info.gr));
}


PEDGE
find_edge_by_name(GraphFrame *cgr,char *sname, char *tname)
{
    Key_type key = NULL;
    PEDGE e = NULL;
    if(!cgr || !sname || !tname)
	return NULL;
    key = get_key_string_edge(sname, tname);
    e = key ? get_element_hashed(cgr->Hedge,key) : NULL;
    return e;
}

/* adding_vertex - Add a vertex to the graph with the given name, weight and
                   location. 
         inputs  
                 gf - Graph Structure
		 name - Name of the vertex
		 weight - Weight of the vertex
		 loc - location of the vertex.

         returns
	          0 - if the vertex was added without problem.
		  POS_NODE - if a vertex is already in the given location
		  NAME_NODE - if a vertex has already the given name
		  MEMORY_NODE - if there is not more memory for a new vertex.

         side effects
	         none
         notes
                 The shape of the vertex is assigned from the shape variable
		 of the graph. 

*/

  
int
adding_vertex(GraphFrame *gf, char *name, char *weight, struct pr_pos *loc)
{
  PNODE v;
  Node_type *nptr;
  LNode_type *vptr;
  
  char *vertex_name;

  /* assuming it is the left mouse button */

  /* try to select a vertex nearby */
  nptr = select_nearest_vertex(loc, gf );

  /* if we did, return */
  if( nptr )
    {
      return POS_NODE;
    }

  /* get the name for it */
  add_get_vertex_name(&vertex_name,name,gf);	
  
  nptr = Retrieve(gf->HVname,(Key_type)vertex_name);
  if(nptr)
    {
      free(vertex_name); vertex_name=NULL;
      return NAME_NODE;
    }


  /* else create a new vertex */
  v = (PNODE) calloc(1, sizeof(struct pt) );
  if( v == NULL )
    {
      fprintf(stderr,"Out of Memory adding_vertex\n");
      abort();
    }

  v->loc.x = loc->x;
  v->loc.y = loc->y;
  v->select = False;
  v->group = False;
  v->num = gf->nvertex;
  v->counter = 0;
  v->highlight = False;
  v->shape = gf->shape;
  v->label = vertex_name;
  GRStrAllocCopy(v->weight,weight); 
  v->mark = False;
  v->level = 0;
  v->color = GR_BG;
  v->pick = False;
  v->modifier =0;
  v->prelim = 0;
  v->attrib = NULL;
  v->ledges_in = init_linked_list();
  v->ledges_out = init_linked_list();
  
  insert_hash_vertex_name(gf, vertex_name, v);
  /* insert it in the structure */
  insert_v(v,gf);
  
  (gf->count_vertex)++; /* count one more vertex */

  vptr = create_new_node_llist((char *)v);
  Insert_linked_list(gf->list_vertices,vptr); /*Insert new vertex in a list */

#ifdef XGR
  /* draw it on the screen */
  if (gf->pdraw) {
    char *vlabel = get_vertex_display_name(gf, v);
    GC gc = choose_gc(gf,v);
    draw_vertex( gf->drawing_a, gc, v->loc.x, v->loc.y,
		 gf->vsize, vlabel, v->weight, v->shape,gf);
    if(vlabel) free(vlabel); vlabel = NULL;
  }
#endif
  return 0;
}
/* add_get_vertex_name - If not name is given to the vertex, it is assigned
                         a number. 
         inputs  
                 vertex_name - New name for the vertex
		 src_name - Name of the vertex. 
		 gf - Graph Structure
         returns
                  none
         notes
                 The number assigned to the vertex is not the total number
		 of vertices. 

*/

void
add_get_vertex_name(char **vertex_name, char *src_name,GraphFrame *gf)
{
  (gf->nvertex)++;
  if(!src_name)
    {
      *vertex_name = (char*)malloc(MDIGITS);
      if(!(*vertex_name)) {
	  fprintf(stderr,"Out of Memory add_get_vertex_name\n");
	  abort();
      }
      sprintf(*vertex_name,"%d",gf->nvertex);
    }
  else {
/*
 * changed by Andrew Morrison - 11.0.02
 *		adding_vertex was dying right here ... so, i'm trying to fix that
      GRStrAllocCopy(*vertex_name,src_name); 
 *
 */
		*vertex_name=(char *)malloc(sizeof(char)*(strlen(src_name)+1));
		strcpy(*vertex_name,src_name);
	}
}
/* adding_edge - An edge is added to the graph. The edge is between the given
                 extreme vertices. A weight is assigned to the new edge.

         inputs  
                 gf - Graph Structure.
		 v1, v2 - Extreme Vertices of the new edge
		 weight - Weight of the edge
         returns
                  0 - if the edge was added to the graph.
		  MEMORY_EDGE - if there is not more memory for the edge.
         side effects
                  the new edge is drawn in the drawing area.
         notes
                 It assumes that there is not an edge between the given 
		 vertices

*/

int
adding_edge(GraphFrame *gf,struct pt *v1,struct pt *v2, char *weight)
{

  PEDGE e;  

  /* get space for the edge */
  e = (PEDGE) calloc( 1, sizeof(struct edge) );
  if( e == NULL )
    {
      fprintf(stderr,"Out of Memory adding_edge\n");
      abort();
    }
      
  /* fill it up */
  e->from = v1;
  e->to = v2;
  e->label = NULL;
  if(weight)
      GRStrAllocCopy(e->weight,weight);
  e->highlight = False;
  e->class = NO_EDGE_CLASS;
  e->lwidth = 0;
  e->color = GR_BG;
  e->attrib = '\0';

  /* insert it in the structure */
  insert_e( e,gf );

  /* draw the edge in the screen */
  if(gf->graph_dir == UNDIRECTED)
    {
      LNode_type *ptr;
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v1->ledges_in,ptr);
      /* printf("node %s", v1->label);
      printing_linked_list(v1->ledges_in);*/
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v2->ledges_in,ptr);
      /*printf("node %s", v2->label);
      printing_linked_list(v2->ledges_in);*/
#ifdef XGR
      if(gf->pdraw){
	GC egc = choose_edge_gc(gf,e);
      if(gf->draw)
	XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
		   egc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );
      if(gf->pdraw)
	XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
		 egc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );

      if(gf->show_eweight ) 
	{
	  int x, y;
	  char *elabel = get_edge_display_name(gf,e);
	  x = (v1->loc.x + v2->loc.x ) / 2;
	  y = (v1->loc.y + v2->loc.y ) / 2;
	  draw_text( gf->drawing_a, x, y-5, egc,
		     gf->font,elabel, gf );
	  if(elabel) free(elabel); elabel = NULL;
	}
      }
#endif
    }
  else if(gf->graph_dir == DIRECTED)
    {
      LNode_type *ptr;
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v1->ledges_out,ptr);
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v2->ledges_in,ptr);
#ifdef XGR
      if(gf->pdraw){
	GC egc = choose_edge_gc(gf,e);
	char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
      /* draw the edge in the screen */
      draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y,
	       .4, 10, .4, 15, egc,elabel,gf);
      if(elabel) free(elabel); elabel = NULL;
      }
#endif
    }

#ifdef XGR
  if(gf->pdraw){ 
    char *v1label = get_vertex_display_name(gf,v1);
    char *v2label = get_vertex_display_name(gf,v2);
    GC gc1 = choose_gc(gf, v1);
    GC gc2 = choose_gc(gf, v2);
  /* and then draw the boxes at the end */
  if(v1->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, v1->loc.y,
		gf->vsize, v1label, v1->weight,v1->shape,gf);
  else
    draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,
		gf->vsize, v1label, v1->weight,v1->shape,gf);
  if(v2->highlight)
    draw_vertex(gf->drawing_a,gf->pix_high, v2->loc.x, v2->loc.y,
		gf->vsize,v2label, v2->weight,v2->shape,gf);
  else
    draw_vertex(gf->drawing_a,gc2, v2->loc.x, v2->loc.y,
		gf->vsize,v2label, v2->weight,v2->shape,gf);
  if(v1label) free(v1label); 
  if(v2label) free(v2label); v1label = NULL; v2label = NULL;
  }
#endif
  return 0;

}

/* move_vertex - Move given vertex to a new location. 

         inputs  
                 cgr - Graph Structure.
		 ov - Vertex to be moved
		 loc - New location
         returns
                  none
         notes
                 It assumes that there is not a vertex in the new location. 

*/


void
move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc)
{
  Node_type *nver = select_nearest_vertex(&(ov->loc),cgr);

  Key_type target = nver->info.key;
  
#ifdef XGR
  redraw_vertex_edges(cgr,cgr->pix_erase,ov,False);
  /*redraw_vertex_edges(cgr,cgr->gc,ov,False);*/
#endif

  ov->loc.x = loc.x;
  ov->loc.y = loc.y;
  
  Delete_target(cgr->HV,target);

  insert_v(ov, cgr);

#ifdef XGR  
  redraw_vertex_edges(cgr,cgr->gc,ov,True);
#endif

}

void
verify_move_vertex(GraphFrame *cgr,struct pt *ov, struct pr_pos loc)
{
  Node_type *nver = select_nearest_vertex(&(ov->loc),cgr);

  Key_type target = nver ? nver->info.key : NULL;
  
  if(!target)
      return;

#ifdef XGR
  redraw_vertex_edges(cgr,cgr->pix_erase,ov,False);
#endif

  ov->loc.x = loc.x;
  ov->loc.y = loc.y;
  
  Delete_target(cgr->HV,target);

  verify_insert_v(cgr,ov);

#ifdef XGR  
  redraw_vertex_edges(cgr,cgr->gc,ov,True);
#endif

}


struct pt *
get_first_vertex(GraphFrame *gf)
{
  return((struct pt *)(gf->list_vertices->back->info));
}

struct pt *
get_last_vertex(GraphFrame *gf)
{
  return((struct pt *)(gf->list_vertices->next->info));
}
/* zooming_graph - The graph changes of scale. All vertices are assigned 
                   a new position and new size. 

         inputs  
                 gf - Graph Structure
         returns
                  0 - If the graph is drawn without problems.
		  RESIZE_WINDOW - If the Drawing area and Pixmap were resized. 
         side effects
                  Drawing Area and Pixmap can be resized for this function
         notes
                 If the client receives the message RESIZE_WINDOW, it should
		 update the dimensions and pointers to the new pixmap and 
		 widget. 

*/

int
zooming_graph(GraphFrame *gf)
{
  int nsize = calculate_size(gf->scale);
  int osize = gf->vsize;
  LNode_type *ptr;
  int wlength = gf->w;
  int mes = 0;
  Delete_hash_table(gf->HV);

  gf->vsize = nsize; 
  gf->hsize = 1.2 * nsize;


  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;

      ov->loc.x = ((ov->loc.x) * nsize )/osize ;
      if(ov->loc.x > wlength)
	wlength = ov->loc.x;
      ov->loc.y = ((ov->loc.y) * nsize )/osize ;
      if(ov->loc.y > wlength)
	wlength = ov->loc.y;
    }

  if(wlength > gf->w)
    {
      Dimension x,y;
      if( wlength < 4*GR_WIDTH)
	  x = gf->w + wlength; 
      else
	  x = 4 * GR_WIDTH;
      y = x;
      increase_window_size(gf, &x, &y);
      mes = RESIZE_WINDOW;
    }

  insert_all_vertices_hash_table(gf);

#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
   return mes;
}

int
expanding_graph(GraphFrame *gf,int escale)
{
  LNode_type *ptr;
  int wlength = gf->w;
  int mes = 0;
  Delete_hash_table(gf->HV);

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;

      ov->loc.x = scale_dim(escale,ov->loc.x);
      if(ov->loc.x > wlength)
	wlength = ov->loc.x;
      ov->loc.y = scale_dim(escale,ov->loc.y);
      if(ov->loc.y > wlength)
	wlength = ov->loc.y;
    }

  if(wlength > gf->w)
    {
      Dimension x,y;
      if( wlength < 4*GR_WIDTH)
	  x = gf->w + wlength; 
      else
	  x = 4 * GR_WIDTH;
      y = x;
      increase_window_size(gf, &x, &y);
      mes = RESIZE_WINDOW;
    }

  insert_all_vertices_hash_table(gf);

#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
   return mes;
}

int
expanding_graph_without_moving(GraphFrame *gf, int escale,int xc, int yc)
{
  LNode_type *ptr;
  int wlength = gf->w;
  int mes = 0;
  /*int xd = scale_dim(escale,xc);
    int yd = scale_dim(escale,yc);*/

  Delete_hash_table(gf->HV);

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;

      if(gf->drawhighlight && !ov->highlight)
	  continue;
      
      ov->loc.x = scale_dim(escale,ov->loc.x);
      if(ov->loc.x > wlength)
	wlength = ov->loc.x;
      ov->loc.y = scale_dim(escale,ov->loc.y);
      if(ov->loc.y > wlength)
	wlength = ov->loc.y;

      verify_insert_v(gf,ov);
    }

#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
   return mes;
}

int
zooming_graph_without_moving(GraphFrame *gf, int xc, int yc)
{
  int nsize = calculate_size(gf->scale);
  int osize = gf->vsize;
  LNode_type *ptr;
  int wlength = gf->w;
  int mes = 0;
  int xc1 = (xc * nsize )/osize ;
  int yc1 = (yc * nsize )/osize ;
  int xd = xc - xc1;
  int yd = yc - yc1;

  Delete_hash_table(gf->HV);

  gf->vsize = nsize; 
  gf->hsize = 1.2 * nsize;


  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;

      ov->loc.x = ((ov->loc.x) * nsize )/osize + xd;
      if(ov->loc.x > wlength)
	wlength = ov->loc.x;
      ov->loc.y = ((ov->loc.y) * nsize )/osize + yd;
      if(ov->loc.y > wlength)
	wlength = ov->loc.y;

      verify_insert_v(gf,ov);
    }

#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
   return mes;
}

int
scaling_graph(GraphFrame *gf)
{
  int nsize = calculate_size(gf->scale);
  int mes = 0;

  gf->vsize = nsize; 
  gf->hsize = 1.2 * nsize;

  return mes;

}
/* This function calculates the new size of the vertex */
int
calculate_size(int scale)
{
  int size = BSIDE * ( scale + 10) / 10;
  return size;
}

int 
scale_dim(int scale, int x)
{
  int size = x * ( scale + 10) / 10;
  return size;
}

void
highlight_selected_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_sel_vertex->next; ptr != gf->list_sel_vertex ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(!v->highlight)
	  highlight_vertex(v,gf);
    }
}

void
highlight_selected_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
	  PEDGE e = (PEDGE)ei->info;

	  if(e->select && !e->highlight) {
	      change_highlight_edge(e,True);
#ifdef XGR	      
	      highlighting_edge(gf,e,gf->pix_high);
#endif
	  }

      }
    }
}

void
dehighlight_all_edges(GraphFrame *gf)
{
  LNode_type *ptr = NULL;
  
  for(ptr= gf->list_vertices->next ; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
      {
	  PEDGE e = (PEDGE)ei->info;
	  change_highlight_edge(e,False);
      }
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif

}

void
dehighlight_all_nodes(GraphFrame *gf)
{
  LNode_type *ptr = NULL;
  
  Delete_all_list(gf->list_high_vertex);

  gf->list_high_vertex = init_linked_list();

  for(ptr= gf->list_vertices->next ; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE ov = (struct pt *)ptr->info;
      ov->highlight = False;
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif

}
/* select_all_nodes - Select all vertices in the Graph.

         inputs 
                 gf - Graph Structure
         returns
                  none
*/

void
select_all_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  
  Delete_all_list(gf->list_sel_vertex);

  gf->list_sel_vertex = init_linked_list();

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      ov->select = True;
      save_list_selected(ov,gf);
    }
#ifdef XGR
  if(gf->pdraw)
    Draw_all_vertices(gf);
#endif
}

/* deselect_all_nodes - Deselect all vertices in the Graph.

         inputs 
                 gf - Graph Structure
         returns
                  none
		  */

void
deselect_all_nodes(GraphFrame *gf)
{

  LNode_type *ptr;
  
  Delete_all_list(gf->list_sel_vertex);

  gf->list_sel_vertex = init_linked_list();

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      ov->select = False;
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
}

int
deselect_all_edges(GraphFrame *gf)
{  
  LNode_type *ptr;
  int c=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next
)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
        {
          struct edge *e = (struct edge *)ei->next->info;
          if(e->select)
            {
              c++;
              e->select = False;
              e->lwidth = 0;
            }
        }
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif

  return c;
}

long
get_count_edges(GraphFrame *gf)
{
  LNode_type *ptr = NULL;
  long c=0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	    /*struct edge *e = (struct edge *)ei->info;*/
	  c++;
	}
    }
  if(gf->graph_dir == UNDIRECTED)
      c/=2;
  return c;
}

long
get_count_selected_edges(GraphFrame *gf)
{
  LNode_type *ptr = NULL;
  long c=0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	    PEDGE e = (PEDGE)ei->info;
	    if(e->select)
		c++;
	}
    }

  if(gf->graph_dir == UNDIRECTED)
	c /= 2;
  return c;
}

void
reset_color_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  e->color = GR_BG;
	  e->class = NO_EDGE_CLASS;
	}
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
}


int
select_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h)
{
  LNode_type *ptr = NULL;
  int i=0;

  if(!gf || w <=0 || h <= 0)
      return 0;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE ov = (PNODE)ptr->info;

      if(is_vertex_in_region(ov,x,y,w,h))
	{
	  if(!ov->select)
	    select_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}

int
select_all_edges_in_region(GraphFrame *gf, int x, int y, int w, int h)
{
  LNode_type *ptr = NULL;
  int i=0;

  if(!gf || w <=0 || h <= 0)
      return 0;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;

      if(is_vertex_in_region(v,x,y,w,h))
	{
	    LNode_type *ei;
	    for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	    {
		PEDGE e = (PEDGE)ei->info;
		PNODE u = e->from;
		if(u == v)
		    u = e->to;
		if(is_vertex_in_region(u,x,y,w,h))
		{
		    if(!e->select)
		    {
			i++;
			changing_select_edge(gf,e, True);
		    }
		}
	    }
	}
    }
  return i;
}

int
select_only_edges_in_region(GraphFrame *gf, int x, int y, int w, int h)
{
  LNode_type *ptr = NULL;
  int i=0;

  if(!gf || w <=0 || h <= 0)
      return 0;
  if(get_count_selected_edges(gf) > 0)
      deselect_all_edges(gf);	

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;

      if(is_vertex_in_region(v,x,y,w,h))
	{
	    LNode_type *ei;
	    for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	    {
		PEDGE e = (PEDGE)ei->info;
		PNODE u = e->from;
		if(u == v)
		    u = e->to;
		if(is_vertex_in_region(u,x,y,w,h))
		{
		    if(!e->select)
		    {
			i++;
			changing_select_edge(gf,e, True);
		    }
		}
	    }
	}
    }
  return i;
}


int
deselect_all_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h)
{
  LNode_type *ptr = NULL;
  int i=0;

  if(!gf || w <=0 || h <= 0)
      return 0;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE ov = (PNODE)ptr->info;

      if(is_vertex_in_region(ov,x,y,w,h))
	{
	  if(ov->select)
	    deselect_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}

int
select_only_nodes_in_region(GraphFrame *gf, int x, int y, int w, int h)
{
  LNode_type *ptr = NULL;
  int i=0;

  if(!gf || w <=0 || h <= 0)
      return 0;

  if(count_llist(gf->list_sel_vertex) > 0)
      deselect_all_nodes(gf);	

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE ov = (PNODE)ptr->info;

      if(is_vertex_in_region(ov,x,y,w,h))
	{
	  if(!ov->select)
	    select_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}

int
select_single_component(GraphFrame *gf, PNODE v)
{
    int i = 0, j = 0;
    if(!gf || !v)
	return 0;

    if(!v->select)
	select_vertex(v,gf);

    i = count_llist(gf->list_sel_vertex);
    while(1) 
    {
	select_neighbors(gf);
	j = count_llist(gf->list_sel_vertex);
	if(i == j)
	    return i;
	i = j;
    };
}

int 
select_neighbors(GraphFrame *gf)
{
  LNode_type *ptr = gf->list_sel_vertex->next;
  int i=0;
  for(; ptr != gf->list_sel_vertex ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->info;

      i+=select_immediate_children(gf,ov);
      i+=select_immediate_parents(gf,ov);

    }
  return i;
}

int
select_all_nodes_in_degree(GraphFrame *gf,int indeg)
{
  LNode_type *ptr;
  int i=0;

  if (indeg < 0)
      return 0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(get_num_edges_in(ov)==indeg)
	{
	  if(!ov->select)
	    select_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}

int
select_all_nodes_out_degree(GraphFrame *gf,int outdeg)
{
  LNode_type *ptr;
  int i=0;

  if (outdeg < 0)
      return 0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(get_num_edges_out(ov)==outdeg)
	{
	  if(!ov->select)
	    select_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}

int
select_all_leaves(GraphFrame *gf)
{
  LNode_type *ptr;
  int i=0;

  if(gf->graph_dir == UNDIRECTED) {
      for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
	  ptr = ptr->next)
      {
	  PNODE ov = (PNODE) ptr->info;
	  if(count_llist(ov->ledges_in) < 2)
	  {
	      if(!ov->select)
		  select_vertex(ov,gf);
	      i++;
	  }
      }
      return i;
  }

  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->info;
      if(is_vertex_sink(ov))
      {
	  if(!ov->select)
	      select_vertex(ov,gf);
	  i++;
      }
    }


  return i;
}

int
select_all_inside_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE ov = (PNODE)ptr->info;
      if(!is_vertex_sink(ov))
	{
	  if(!ov->select)
	    select_vertex(ov,gf);
	  i++;
	}
    }
  return i;
}
int
select_all_sources(GraphFrame *gf)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(is_vertex_source(ov))
	{
	  i++;
	  if(!ov->select)
	    select_vertex(ov,gf);
	}
    }
  return i;
}

int
select_complement(GraphFrame *gf)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(!ov->select)
	{
	  i++;
	  select_vertex(ov,gf);
	}
      else
	deselect_vertex(ov,gf);
    }
  return i;
}

int
select_complement_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	  PEDGE e = (PEDGE)ei->info;
	  if(e->to != v)
	      continue;
	  if(e->select)
	      changing_select_edge(gf,e, False);
	  else
	  {
	      c++;
	      changing_select_edge(gf,e, True);
	  }
	}
    }
  return c;
}

int
select_all_documents(GraphFrame *gf)
{
  LNode_type *ptr;
  int i = 0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  PEDGE e = (PEDGE)ei->next->info;
	  if(e->attrib=='c' || e->attrib == 'n' || e->attrib == 'u' 
	     || e->attrib == 'p' || e->attrib == 'i' ||
	     e->attrib == 't')
	    {
	      select_edge_selecting_vertices(gf, e);
	      /*if(e->to && e->from)
		printf("%c %d-%d\n",e->attrib,e->from->num,e->to->num);
	      else
	      printf("error\n");*/
	      i++;
	    }
	}
    }
  return i;
}
void
dehigh_all_nodes(GraphFrame *gf)
{

  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(ov->highlight)
	dehighlight_vertex(ov,gf);
    }
#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
}

int 
delete_nodes_from_to(GraphFrame *gf, int fromno, int tono)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_vertices->next; ptr!=gf->list_vertices; ptr=ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->info;
      if(ov->num >= fromno && ov->num <=tono)
	{
	  deleting_vertex(gf, ov);
	  i++;
	}
    }
  return i;
}

int
delete_selected_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_sel_vertex->next; ptr!=gf->list_sel_vertex; ptr=ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->info;
      deleting_vertex(gf, ov);
      i++;
    }
  Delete_all_list(gf->list_sel_vertex);
  gf->list_sel_vertex = init_linked_list();
  return i;
}
/* deleting_vertex - Delete a given vertex from the Graph.

         inputs  
                 cgr - Graph Structure 
		 v - Vertex to be deleted.
         returns
                  none
         side effects
                  If a vertex is deleted, all edges in and out of the vertex
		  are also deleted. 
*/

void 
deleting_vertex(GraphFrame *cgr,struct pt *v)
{
  Node_type *nptr;
  nptr = select_nearest_vertex(&(v->loc), cgr );
  delete_edges_in_out_vertex(cgr,v);
  delete_v(cgr,nptr,v);
}
/* delete_edges_in_out_vertex - Delete all edges that are in and out of the
                                given vertex.

         inputs  
                 gf - Graph Structure
		 v - Vertex
         returns
                  none
         side effects
                  If an edge is deleted, the list of the edges in or out of 
		  the vertex of the other extreme should be updated.
*/

void
delete_edges_in_out_vertex(GraphFrame *gf,struct pt *v)
{
#ifdef XGR
  GC gc = gf->pix_erase;
#endif

  if(gf->graph_dir == UNDIRECTED)
    {
      LNode_type *ne;
      for(ne = v->ledges_in; ne->next != v->ledges_in; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v2 = e->from;
	  if(v2 == v)
	    v2 = e->to;
#ifdef XGR
	  XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
		     gc, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );
	  XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
		     gc, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );


	  if(gf->show_eweight ) {
	    int x, y;
	    char *elabel = get_edge_display_name(gf,e);
	    x = (v->loc.x + v2->loc.x ) / 2;
	    y = (v->loc.y + v2->loc.y ) / 2;
	    draw_text( gf->drawing_a, x, y-5, gc,
		       gf->font,elabel, gf );
	    if(elabel) free(elabel); elabel = NULL;
	  }
	  {
	    char *v2label = get_vertex_display_name(gf, v2);
	    GC gc2 = choose_gc(gf, v2);
	  if(v2->select)
	    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,
			gf->hsize, NULL,NULL,v2->shape,gf);
	  if(v2->highlight)
	    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y, 
		       gf->vsize, v2label, v2->weight, v2->shape,gf);
	  else
	    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,gf->vsize, 
			v2label, v2->weight, v2->shape,gf);
	  if(v2label) free(v2label); v2label = NULL;
	  }
#endif
	  update_list_edges(v2->ledges_in,e);

	  delete_e(gf,e);
	}
    }
  else if(gf->graph_dir == DIRECTED)
    {
      LNode_type *ne;
      
      for(ne = v->ledges_in; ne->next != v->ledges_in; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v1 = e->from;
#ifdef XGR
	char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
	  /* draw the edge in the screen */
	  draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v->loc.x, v->loc.y,
		  .4, 10, .4, 15, gc, elabel,gf);
	  if(elabel) free(elabel); elabel = NULL;
	  {
	    char *v1label = get_vertex_display_name(gf, v1);
	    GC gc1 = choose_gc(gf,v1);
	  if(v1->select)
	    draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,
			gf->hsize, NULL,NULL,v1->shape,gf);

	  if(v1->highlight)
	    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, v1->loc.y, 
		       gf->vsize, v1label, v1->weight, v1->shape,gf);
	  else
	    draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,gf->vsize, 
			v1label, v1->weight, v1->shape,gf);
	  if(v1label) free(v1label); v1label = NULL;
	  }
#endif
	  update_list_edges(v1->ledges_out,e);

	  delete_e(gf,e);
	}

      for(ne = v->ledges_out; ne->next != v->ledges_out; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v2 = e->to;
#ifdef XGR
	char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
	  /* draw the edge in the screen */
	  draw_arc(gf->drawing_a, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y,
		  .4, 10, .4, 15, gc,elabel,gf);
	  {
	    char *v2label = get_vertex_display_name(gf, v2);
	    GC gc2 = choose_gc(gf, v2);
	  if(v2->select)
	    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,
			gf->hsize, NULL,NULL,v2->shape,gf);
	  if(v2->highlight)
	    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y, 
			gf->vsize, v2label, v2->weight,v2->shape,gf);
	  else
	    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,gf->vsize, 
			v2label, v2->weight,v2->shape,gf);
	  if(v2label) free(v2label); v2label = NULL;
	  }
#endif
	  update_list_edges(v2->ledges_in,e);

	  delete_e(gf,e);
	}

    }

  if(v->select)
    {
#ifdef XGR
      draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y,
		  gf->hsize, NULL,NULL,v->shape,gf);
#endif
      update_list(gf->list_sel_vertex,(char *)v);
    }
#ifdef XGR
  {
    char *vlabel = get_vertex_display_name(gf, v);
  draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y,gf->vsize, 
	      vlabel, v->weight, v->shape,gf);
    if(vlabel) free(vlabel); vlabel = NULL;
  }
#endif
}
/* deleting_edge - Delete an edge from the graph. 

         inputs  
                 gf - Graph Structure
		 e - Edge
         returns
                  none
         side effects
	          The list of the edges in and out of the extreme vertices
		  of the edge should be updated. 
                  
*/


void
deleting_edge(GraphFrame *gf, struct edge *e)
{
  struct pt *v1 = e->to;
  struct pt *v2 = e->from;

#ifdef XGR
  draw_edge(gf, e, gf->pix_erase);
  if(gf->pdraw) {
    char *v1label = get_vertex_display_name(gf, v1);
    char *v2label = get_vertex_display_name(gf, v2);
    GC gc1 = choose_gc(gf,v1);
    GC gc2 = choose_gc(gf,v2);
    if(v1->select)
      draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,
		gf->hsize, NULL,NULL,v1->shape,gf);
  if(v1->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, v1->loc.y, gf->vsize, 
		v1label, v1->weight,v1->shape,gf);
  else
    draw_vertex(gf->drawing_a,gc1, v1->loc.x, v1->loc.y,gf->vsize, 
		v1label, v1->weight,v1->shape,gf);
  

  if(v2->select)
    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,
		gf->hsize, NULL,NULL,v2->shape,gf);
  if(v2->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y, gf->vsize, 
		v2label, v2->weight, v2->shape,gf);
  else
    draw_vertex(gf->drawing_a, gc2, v2->loc.x, v2->loc.y,gf->vsize, 
		v2label, v2->weight, v2->shape,gf);
  if(v1label) free(v1label); 
  if(v2label) free(v2label); v1label = NULL; v2label = NULL;
  }
#endif
  
  delete_e(gf, e);

  if(gf->graph_dir == DIRECTED)
    {
      update_list_edges(v1->ledges_in, e);
      update_list_edges(v2->ledges_out, e);
    }
  else  if(gf->graph_dir == UNDIRECTED)
    {
      update_list_edges(v1->ledges_in, e);
      update_list_edges(v2->ledges_in, e);
    }
}

/* reset_graph_info - Deletes the  information in the graph structure.

         inputs  
                 gf - Graph Structure
         returns
                  none
         side effects
                  none
*/

void 
reset_graph_info(GraphFrame *gf)
{
  delete_graph_resources(gf);
  gf->shape = BOX;
  gf->vsize = BSIDE;
  gf->hsize = HSIDE;
  gf->scale = 0;
  gf->nvertex = 0;
  gf->count_vertex = 0;
  gf->nvgroup = 0;
  gf->graph_dir = NONE;
  gf->nver = NULL;
  gf->cgv = NULL;
  gf->dmode = DIS_NUM;
  initialize_graph_hash_tables(gf);
  gf->list_vertices = init_linked_list();
  gf->list_sel_vertex = init_linked_list();
  gf->list_high_vertex = init_linked_list();
  gf->lobjects = init_linked_list();
  gf->attrib_name = init_linked_list();

}

/* clear_graph_area  - The drawing area is erased. The graph structure is 
                       not modified.
         inputs  
                 gf - Graph Structure
         returns
                  none
*/
#ifdef XGR
void
clear_graph_area(GraphFrame *gf)
{
  GC gc = gf->pix_erase;
  Widget drawing_a = gf->drawing_a;
  Display *dpy = XtDisplay(drawing_a);
  Pixmap pixmap = gf->pixmap;
  
  /* to clear a pixmap, reverse foreground and background */
  /*XSetForeground (dpy, gc, resbg.pixel);*/

  /* ...and fill rectangle the size of the pixmap */
  if(gf->pdraw)
    {
      XFillRectangle (dpy, pixmap, gc, 0, 0, gf->w, gf->h);
  /* don't foreget to reset */
  /*XSetForeground (dpy, gc, resfg.pixel);*/

  /* Note: we don't have to use WIDTH and HEIGHT--we could pull the
   * exposed area out of the event structure, but only if the reason
   * was XmCR_EXPOSE... make it simple for the demo; optimize as needed.
   */

      XCopyArea (dpy, pixmap, XtWindow(drawing_a), gc,
		 0, 0, gf->w, gf->h, 0, 0);
    }

}
#endif
/* delete_edges - Delete all edges from the Graph. The edges are accessed 
                  thru the Hashtable of Edges.
         inputs  
                 H - Hashtable of the edges.
         returns
                  none

*/

void 
delete_edges(Hashtable_type H)
{
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(H[i] != NULL)
	{
	  Node_type * loc = H[i];
	  for(;loc!=NULL;loc=loc->next)
	    {
	      struct edge *e = (struct edge *) (loc->info.gr);
	      delete_edge_info(e);
	      free(e); e = NULL;
	    }
	}
    }
}
/* delete_graph_resources - Delete Lists and HashTables of Vertices and Edges
                            of the Graph Structure.

         inputs  
                 gf - Graph Structure. 
         returns
                  none
*/

void
delete_graph_resources(GraphFrame *gf)
{
  delete_vertices(gf->list_vertices);
  delete_edges(gf->Hedge);

  if(gf->list_vertices)
    {
      Delete_all_list(gf->list_vertices);
      gf->list_vertices =NULL;
    }

  if(gf->list_sel_vertex)
    {
      Delete_all_list(gf->list_sel_vertex);
      gf->list_sel_vertex = NULL;
    }
  if(gf->list_high_vertex)
    {
      Delete_all_list(gf->list_high_vertex);
      gf->list_high_vertex = NULL;
    }

  if(gf->list_visited_vertex)
    {
      Delete_all_list(gf->list_visited_vertex);
      gf->list_visited_vertex = NULL;
    }
  if(gf->attrib_name)
    {
      Delete_all_list(gf->attrib_name);
      gf->attrib_name = NULL;
    }
  if(gf->the_cycle)
    {
      Delete_all_list(gf->the_cycle);
      gf->the_cycle = NULL;
    }
  if(gf->the_rest)
    {
      Delete_all_list(gf->the_rest);
      gf->the_rest = NULL;
    }
  if(gf->level_zero)
    {
      Delete_all_list(gf->level_zero);
      gf->level_zero = NULL;
    }
      
  if(gf->lauthub)
    {
      delete_author_hub_list(gf);
      gf->lauthub = NULL;
    }
  if(gf->lrank)
    {
      Delete_all_list(gf->lrank);
      gf->lrank = NULL;
    }
  if(gf->lsdist)
    {
      delete_short_distance_list(gf);
      gf->lsdist = NULL;
    }

  if(gf->lobjects)
    {
      Delete_all_list(gf->lobjects);
      gf->lobjects = NULL;
    }

  Delete_hash_table(gf->HV);
  Delete_hash_table(gf->HVname);
  Delete_hash_table(gf->Hedge);
}
/* delete_vertices - Delete all vertices from the Graph. The vertices are 
                     accessed thru the Hashtable of Vertices.
         inputs  
                 H - Hashtable of the vertices.
         returns
                  none

*/

void 
delete_vertices2(Hashtable_type H)
{
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(H[i] != NULL)
	{
	  Node_type * loc = H[i];
	  for(;loc!=NULL;loc=loc->next)
	    {
	      struct pt *v = (struct pt *) (loc->info.gr);
	      delete_vertex_info(v);
	      free(v); v = NULL;
	    }
	}
    }
}
void 
delete_vertices(LNode_type *lver)
{
    LNode_type *ptr = NULL;
    if(!lver)
	return;
    for(ptr = lver->next ;ptr!=lver; ptr=ptr->next)
    {
	PNODE v = (PNODE)ptr->info;
	delete_vertex_info(v);
	free(v); v = NULL;
    }
}

void
resize_window_size(GraphFrame *gf,Dimension x, Dimension y)
{
  if(!gf->pdraw)
      return;

#ifdef XGR
  XtVaSetValues(gf->drawing_a, XmNwidth, x, XmNheight, y, NULL);
  XFreePixmap(XtDisplay (gf->drawing_a),gf->pixmap);
  gf->pixmap = XCreatePixmap (XtDisplay (gf->drawing_a),
			       RootWindowOfScreen (XtScreen (gf->drawing_a)), 
		        x, y, DefaultDepthOfScreen (XtScreen (gf->drawing_a)));
  gf->wp = x;
  gf->hp = y;

  if(gf->wp > gf->w || gf->hp > gf->h)
    {
      gf->w = gf->wp;
      gf->h = gf->hp;
      Delete_hash_table(gf->HV); /* Delete Hash Table of Vertices */
      insert_all_vertices_hash_table(gf);
    }
#endif
}

int 
increase_window_size(GraphFrame *gf, Dimension *X, Dimension *Y)
{
  int mes = 0;
  Dimension x = *X, y = *Y;

  if(!gf->pdraw)
    {
      if(gf->w < x || gf->h < y)
	{
	  *X = gf->w =x; *Y = gf->h = y;
	}
      return 0;
    }

  if(gf->w < x || gf->h < y)
    {
      gf->w =x; gf->h = y;
      mes = RESIZE_WINDOW;
    }
  *X = gf->w;
  *Y = gf->h;
  return mes;
}
int 
decrease_window_size(GraphFrame *gf, Dimension *X, Dimension *Y)
{
  int mes = 0;
  Dimension x = *X, y = *Y;

  if(!gf->pdraw)
    {
      if(gf->w > x)
	{
	  *X = gf->w =x; *Y = gf->h = y;
	}
      return 0;
    }

  if(gf->w > x)
    {
      gf->w =x; gf->h = y;
      mes = RESIZE_WINDOW;
    }
  *X = gf->w;
  *Y = gf->h;
  return mes;
}

int
move_graph_hor(GraphFrame *gf, int x)
{
  int xc = x + 4*gf->vsize;
  int mes =0;
  LNode_type *ptr;


  Delete_hash_table(gf->HV);
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      v->loc.x +=xc;
      if(v->mark == True)
	verify_insert_v(gf,v);
    }

  return mes;
}

int
move_graph_ver(GraphFrame *gf, int y)
{
  int yc = y + 4*gf->vsize;
  int mes =0;
  LNode_type *ptr;
  Delete_hash_table(gf->HV);
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      v->loc.y +=yc;
      if(v->mark == True)
	verify_insert_v(gf,v);
    }
  return mes;
}

int
move_graph_lin(GraphFrame *gf, int x,int y)
{
  LNode_type *ptr;
  Delete_hash_table(gf->HV);
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      v->loc.y +=y;
      v->loc.x +=x;
      verify_insert_v(gf,v);
    }
  return 0;
}

int
move_selected_nodes_lin(GraphFrame *gf, int x,int y)
{
  LNode_type *ptr;
  Delete_hash_table(gf->HV);
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v->select)
      {
	  v->loc.y +=y;
	  v->loc.x +=x;
      }
      verify_insert_v(gf,v);
    }
  return 0;
}

int
move_neg_graph(GraphFrame *gf)
{
  int minx=gf->w,miny=gf->h, maxx=0, maxy=0;
  int mes=0;

  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v->loc.x < minx)
	minx = v->loc.x;
      if(v->loc.y < miny)
	miny = v->loc.y;
    }

  if(minx < 0)
    {
      /*printf("moving horizontal %d\n", minx);*/
      mes = move_graph_hor(gf,-minx);
    }
  if(miny < 0)
    {
      /*printf("moving vertical %d\n", miny);*/
      mes = move_graph_ver(gf,-miny);
    }
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v->loc.x > maxx)
	maxx = v->loc.x;
      if(v->loc.y > maxy)
	maxy = v->loc.y;
    }

  if((maxx > gf->w) || (maxy > gf->h))
    {
      Dimension X, Y;
      X = GR_MAX_LENGTH/2; Y = X;
      if((maxx > X)||(maxy > Y))
	{
	  int maxd = maxx > maxy ? maxx : maxy;

	  if(maxd > GR_MAX_LENGTH)
	    {
	      X = GR_MAX_LENGTH; Y = X;
	    }
	  else
	    {
	      X = maxd+20; Y = X;
	    }
	}
      mes = increase_window_size(gf,&X,&Y);
    }
  return mes;
}


void
insert_all_vertices_hash_table(GraphFrame *gf)
{  
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      verify_insert_v(gf,v);
    }
}

int
verify_insert_v(GraphFrame *gf, struct pt *v)
{
  struct pt *vt;
  vt = find_vertex(gf,&(v->loc));
  if(vt)
    {
	/*printf("VERTEX TOGETHER %d %s ,%d %s :", vt->num, vt->label,v->num, v->label);
      printf("locations %d,%d - %d,%d\n", vt->loc.x, vt->loc.y, v->loc.x, v->loc.y);*/
      if(find_near_spot(v,gf))
	return WINDOW_FULL;
    }
  v->mark = True;
  insert_v(v,gf);
  return 0;
}

int
get_template_from_name(char *t, char *s)
{
  int i, e=0;
  int l = strlen(s);
  for(i = l; i >= 0 ; i--)
    {
      if(s[i]=='/')
	{
	  e = i;
	  break;
	}
    }
  if(e != 0)
    {
      strncpy(t,s,e+1);
      t[e+1] = '\0';
      strcat(t,"*");
      return 0;
    }
  return -1;
}

int 
select_tree_by_template(GraphFrame *gf, struct pt *v)
{
  int i=1;
  
  char *tmpl = malloc(strlen(v->label)+4); 
  if(!tmpl) {
      fprintf(stderr,"Out of Memory select_tree_by_template\n");
      abort();
  }

  get_template_from_name(tmpl, v->label);

  reset_mark_pick_vertices(gf);

  if(v->select != True)
    select_vertex(v, gf);

  if(gf->graph_dir == DIRECTED)
    {
      i = dfs_visit_tree_by_template(gf, v, tmpl);
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      i = dfs_undirected_visit_tree(gf, v);
    }
  free(tmpl); tmpl = NULL;
  return i;
}

int
dfs_visit_tree_by_template(GraphFrame *gf, struct pt *v, char *tmpl)
{
  LNode_type *poed;
  int i = 1;
  int n =0;
  v->mark = True;

  if(is_vertex_object(v))
    i = 0;

  i += select_leaves_children(gf,v);
    
  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      char * resp = GRStrMatch(tmpl, tv->label);

      if(tv->ledges_out->back != tv->ledges_out) /* no leaf node */
	{
	  if(!resp)
	    {
	      return -1; /* Don't choose this tree by template */
	    }
	}
	
      if( tv->mark == False && e->class == TREE_CLASS && resp)
	{
	  if(tv->select != True)
	    select_vertex(tv, gf);
	  i += select_leaves_children(gf,tv);
	  n = dfs_visit_tree_by_template(gf, tv,tmpl);

	  if(n == -1)
	    return -1;
	  else
	    i += n;
	}
    }
  v->pick = True;
  return i;
}

int
select_leaves_children(GraphFrame *gf, struct pt *v)
{
  LNode_type *ptr;
  int i = 0;
  for(ptr = v->ledges_out->back; ptr != v->ledges_out; ptr= ptr->back)
    {
      struct edge *e = (struct edge *)ptr->info;
      struct pt *u = e->to;
      if(u->select == True)
	continue;
      if(u->ledges_out->back == u->ledges_out) /* leaf node */
	{
	  if(!is_vertex_object(u))
	    i++;
	  /*printf("LEAF from %d  is %d %s\n", v->num, u->num, u->label);*/
	  select_vertex(u, gf);
	}
    }
  return i;
}

int
select_nodes_by_string_template(GraphFrame *gf, char *tmpl)
{
  LNode_type * ptr;
  int i=0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices; 
      ptr = ptr->next)   
    {
      struct pt *v = (struct pt *)(ptr->info);
      char * resp = GRStrMatch(tmpl, v->label);
      if(resp)
	{
	  if(v->select != True)
	    select_vertex(v, gf);
	  i++;
	}
    }
  return i;
}

int
select_immediate_children(GraphFrame *gf, struct pt *v)
{
  LNode_type *ptr;
  int i=1;

  if(v->select != True)
    select_vertex(v, gf);

  if(gf->graph_dir == DIRECTED)
    {
      for(ptr = v->ledges_out->back; ptr!= v->ledges_out; ptr = ptr->back)
	{
	  struct edge *e = (struct edge *)ptr->info;
	  struct pt *u = e->to;
	  
	  if(u->select != True)
	    select_vertex(u, gf);
	  i++;
	}
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      for(ptr = v->ledges_in->back; ptr!= v->ledges_in; ptr = ptr->back)
	{
	  struct edge *e = (struct edge *)ptr->info;
	  struct pt *u = e->to;
	  if(u == v)
	    u = e->from;

	  if(u->select != True)
	    select_vertex(u, gf);
	  i++;
	}
    }
  return i;
}

int
select_immediate_parents(GraphFrame *gf, struct pt *v)
{
  LNode_type *ptr;
  int i=1;

  if(v->select != True)
    select_vertex(v, gf);

  if(gf->graph_dir == DIRECTED)
    {
      for(ptr = v->ledges_in->back; ptr!= v->ledges_in; ptr = ptr->back)
	{
	  struct edge *e = (struct edge *)ptr->info;
	  struct pt *u = e->from;
	  
	  if(u->select != True)
	    select_vertex(u, gf);
	  i++;
	}
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      for(ptr = v->ledges_in->back; ptr!= v->ledges_in; ptr = ptr->back)
	{
	  struct edge *e = (struct edge *)ptr->info;
	  struct pt *u = e->to;
	  if(u == v)
	    u = e->from;

	  if(u->select != True)
	    select_vertex(u, gf);
	  i++;
	}
    }
  return i;
}


int
select_tree(GraphFrame *gf, struct pt *v)
{
  int i=1;

  reset_mark_pick_vertices(gf);

  if(v->select != True)
    select_vertex(v, gf);

  if(gf->graph_dir == DIRECTED)
    {
      i = dfs_visit_tree(gf, v);
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      i = dfs_undirected_visit_tree(gf, v);
    }
  return i;
}

int
dfs_visit_tree(GraphFrame *gf, struct pt *v)
{
  LNode_type *poed;
  int i = 1;

  v->mark = True;

  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      if( tv->mark == False && e->class == TREE_CLASS)
	{
	  if(tv->select != True)
	    select_vertex(tv, gf);
	  i += dfs_visit_tree(gf, tv);
	}
    }
  v->pick = True;
  return i;
}


int 
dfs_undirected_visit_tree(GraphFrame *gf, struct pt *v)
{
  LNode_type *poed;
  int i = 1;

  v->mark = True;

  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      if(tv == v)
	tv = e->from;

      if( tv->mark == False && e->class == TREE_CLASS)
	{
	  if(tv->select != True)
	    select_vertex(tv, gf);
	  i += dfs_undirected_visit_tree(gf, tv);
	}
    }
  v->pick = True;
  return i;
}


int
select_tree_levels(GraphFrame *gf, struct pt *v, int levels)
{
  int i=1;

  reset_mark_pick_vertices(gf);

  if(v->select != True)
    select_vertex(v, gf);

  if(gf->graph_dir == DIRECTED)
    {
      i = dfs_visit_tree_levels(gf, v,levels);
    }
  else if(gf->graph_dir == UNDIRECTED)
    {
      i = dfs_undirected_visit_tree_levels(gf, v,levels);
    }
  return i;
}

int
dfs_visit_tree_levels(GraphFrame *gf, struct pt *v,int levels)
{
  LNode_type *poed;
  int i = 1;

  v->mark = True;

  if(!levels)
    return 0;

  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      if( tv->mark == False && e->class == TREE_CLASS)
	{
	  if(tv->select != True)
	    select_vertex(tv, gf);
	  i += dfs_visit_tree_levels(gf, tv,levels-1);
	}
    }
  v->pick = True;
  return i;
}

int 
dfs_undirected_visit_tree_levels(GraphFrame *gf, struct pt *v,int levels)
{
  LNode_type *poed;
  int i = 1;

  v->mark = True;

  if(!levels)
    return 0;

  for(poed = v->ledges_in->back; poed != v->ledges_in; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      if(tv == v)
	tv = e->from;

      if( tv->mark == False && e->class == TREE_CLASS)
	{
	  if(tv->select != True)
	    select_vertex(tv, gf);
	  i += dfs_undirected_visit_tree_levels(gf, tv,levels-1);
	}
    }
  v->pick = True;
  return i;
}

struct pt *
choose_best_tree(GraphFrame *gf, struct pt *v, int gmax)
{
  LNode_type *poed;
  struct pt *bv=NULL;
  int max=gmax;
  
  printf("NEW TREES %d %s\n",v->num, v->label);
  for(poed = v->ledges_out->back; poed != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      int icount = 0;
      if(tv==v)	
	  continue;
      icount = select_tree(gf,tv)-count_objects_tree(gf,tv);
      /*printf("TREES %d:%s %d nodes\n", tv->num, tv->label, icount);*/
      if(icount > max)
	{
	  max = icount;
	  bv = tv;
	}
    }

   if(bv)
    printf("Selection was made %d:%s %d nodes\n", bv->num, bv->label, max);
  else
    printf("No Selection was made\n");

  deselect_all_nodes(gf);
  return bv;
}

int
count_objects_tree(GraphFrame *gf, PNODE v)
{
  int i =0;

  if(gf->lobjects->next  == gf->lobjects) /* list is empty */
    return 0;

  reset_mark_pick_vertices(gf);
  if(gf->graph_dir == DIRECTED)
    {
      i = dfs_visit_tree_counting_objects(gf, v);
    }  
  return i;
}

int
dfs_visit_tree_counting_objects(GraphFrame *gf, struct pt *v)
{
  LNode_type *poed;
  int i = 0;

  v->mark = True;

  if(is_vertex_object(v))
     i=1;

  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;
      if( tv->mark == False && e->class == TREE_CLASS)
	{
	  i += dfs_visit_tree_counting_objects(gf, tv);
	}
    }
  v->pick = True;
  return i;
}


struct pt *
choose_best_tree_by_template(GraphFrame *gf, struct pt *v, int gmax)
{
  LNode_type *poed;
  struct pt *bv=NULL;
  int max=gmax;
  
  /*printf("NEW TREES %d %s\n",v->num, v->label);*/
  for(poed = v->ledges_out->back; poed != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      int icount = 1;

      if(tv->ledges_out->back != tv->ledges_out) /* not leaf */
	icount = select_tree_by_template(gf,tv);

      /*printf("TREES %d:%s %d nodes\n", tv->num, tv->label, icount);*/
      if(icount > max)
	{
	  max = icount;
	  bv = tv;
	}
    }

  /*  if(bv)
    printf("Selection was made %d:%s %d nodes\n", bv->num, bv->label, max);
  else
    printf("No Selection was made\n");*/

  deselect_all_nodes(gf);
  return bv;
}


struct pt *
choose_first_tree_by_template(GraphFrame *gf, struct pt *v, int gmax)
{
  LNode_type *poed;
  struct pt *bv=NULL;
  int ic=0;
  
  printf("NEW TREES %d %s\n",v->num, v->label);
  for(poed = v->ledges_out->back; poed != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->info;
      struct pt * tv = e->to;
      int icount =1;

      if(tv->ledges_out->back != tv->ledges_out) /* leaf */
	icount = select_tree_by_template(gf,tv);

      /*printf("TREES %d:%s %d nodes\n", tv->num, tv->label, icount);*/
      if(icount > gmax)
        {
          bv = tv;
	  ic = icount;
	  break;
        }
    }

  if(bv)
    printf("Selection was made %d:%s %d nodes\n", bv->num, bv->label, ic);
  else
    printf("No Selection was made\n");

  deselect_all_nodes(gf);
  return bv;
}              


LNode_type *
linear_dfs_visit_tree(GraphFrame *gf, struct pt *v, int *count, char shape)
{
  LNode_type *lin_list_nodes = init_linked_list();

  Insert_linked_list(lin_list_nodes, create_new_node_llist((char *)v));
  
  *count = rec_linear_dfs_visit_tree(gf,v, lin_list_nodes,shape);

  return lin_list_nodes;
}

int
rec_linear_dfs_visit_tree(GraphFrame *gf, struct pt *v, LNode_type *lnodes, 
			  char shape)
{
  LNode_type *poed;
  int i = 1;

  for(poed = v->ledges_out; poed->back != v->ledges_out; poed = poed->back)
    {
      struct edge *e = (struct edge *)poed->back->info;
      struct pt * tv = e->to;

      if(e->class == TREE_CLASS)
	{
	  if(tv->shape == shape)
	      Insert_linked_list(lnodes,create_new_node_llist((char *)tv));
	  i += rec_linear_dfs_visit_tree(gf, tv,lnodes,shape);

	}
    }
  return i;
}

int
select_edge_by_class(GraphFrame *gf, short class)
{
  LNode_type *ptr = NULL;
  int c = 0;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	    PEDGE e = (PEDGE)ei->info;
	    if(e->class == class)
	    {
		c++;
		changing_select_edge(gf,e,True);
	    }
	}
    }
  return c;
}

int
select_shape_vertices(GraphFrame *gf, char shape)
{
  LNode_type *ptr;
  int count =0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(ov->shape == shape)
	{
	  count++;
	  if(ov->select == False)
	    {
	      ov->select = True;
	      save_list_selected(ov,gf);
	    }
	}
    }
#ifdef XGR
  if(gf->pdraw && count)
    Draw_all_vertices(gf);
#endif
  return count;
}

void
reset_counter_nodes(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      ov->counter = 0;
    }
}

void
reset_weight_edges(GraphFrame *gf)
{
  LNode_type *ptr;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  change_weight_edge(e, "0");
	}
    }
}
int
how_many_nodes_are_visited(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(ov->counter)
	c++;
    }
  return c;
}

void
assign_visited_nodes(GraphFrame *gf,PNODE *A)
{
  LNode_type *ptr;
  int i=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(ov->counter)
	A[i++]= ov;
    }
}

int
how_many_edges_are_visited(GraphFrame *gf)
{
  LNode_type *ptr;
  int c=0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  int ewi = e->weight ? atoi(e->weight) : 0;
	  if(ewi)
	    c++;
	}
    }
  return c;
}

void
assign_visited_edges(GraphFrame *gf,PEDGE *A)
{
  LNode_type *ptr;
  int i = 0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  int ewi = e->weight ? atoi(e->weight) : 0;
	  if(ewi)
	    A[i++] = e;
	}
    }
}

int
select_most_visited_edges(GraphFrame *gf, int mv)
{
  int c = how_many_edges_are_visited(gf);

  PEDGE *A = c ? (PEDGE *)calloc(c,sizeof(PEDGE)) : NULL; 

  if(!c)
    return 0;

  if(!A) {
      fprintf(stderr,"Out of Memory select_most_visited_edges\n");
      abort();
  }


  srand(time(NULL));

  assign_visited_edges(gf,A);

  printf("SELECTED EDGES\n");

  if(mv < c)
    {
      int i;
      for(i = 1; i<=mv ; i++)
	{
	  PEDGE e = (PEDGE)randomized_select((GPTR *)A,i-1,c-1,1,
					     is_weight_lower,is_weight_bigger);

	  select_edge_selecting_vertices(gf, e);
	  
	  if(e->to && e->from)
	    printf("%s %d-%d\n",e->weight,e->from->num,e->to->num);
	  else
	    printf("error\n");
	}
      free(A); A = NULL;
      return mv;
    }
  else
    {
      int i;
      for(i = 0; i< c ; i++)
	{
	  PEDGE e = A[i];
	  select_edge_selecting_vertices(gf, e);
	}
      free(A); A = NULL;
      return c;
    }
}

/* Clusters must be computed before using this function. The number of the
cluster is saved in the context variable of the vertex */

int
select_i_cluster_vertices(GraphFrame *gf, int i)
{
  int c = 0;
  LNode_type *ptr;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
  {
    PNODE v = (PNODE)ptr->info;
    int cluster = (int)v->context;
    if(cluster == i) 
    {	  
      if(!v->select)
	 select_vertex(v, gf);
      c++;
    }
  }
  return c;
}
/* Cliques must be computed before using this function. The number of the
cliques is saved in the context variable of the vertex */

int
select_i_connected_component_vertices(GraphFrame *gf, int i)
{
  int c = 0;
  LNode_type *ptr;

  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
  {
    PNODE v = (PNODE)ptr->info;
    int component = (int)v->context;
    if(component == i) 
    {	  
      if(!v->select)
	 select_vertex(v, gf);
      c++;
    }
  }
  return c;
}
int
select_most_visited_vertices(GraphFrame *gf, int mv)
{
  int c = how_many_nodes_are_visited(gf);

  PNODE *A = c ? (PNODE *)calloc(c,sizeof(PNODE)) : NULL;

  if(!c)
    return 0;

  if(!A) {
      fprintf(stderr,"Out of Memory select_most_visited_vertices\n");
      abort();
  }

  srand(time(NULL));

  assign_visited_nodes(gf,A);

  printf("SELECTED NODES\n");
  if(mv < c)
    {
      int i;
      for(i = 1; i<=mv ; i++)
	{
	  PNODE v = (PNODE)randomized_select((GPTR *)A,i-1,c-1,1,
					   is_counter_lower,is_counter_bigger);
	  
	  if(!v->select)
	    select_vertex(v, gf);
	  if(v)
	    printf("%d %s\n", v->counter, v->label);
	}
      free(A); A = NULL;
      return mv;
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
select_objects(GraphFrame *gf)
{
  LNode_type *ptr;
  int i = 0;
  if(!gf->lobjects)
    return 0;
  for(ptr = gf->lobjects->next; ptr != gf->lobjects ; ptr= ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      i++;
      if(!v->select)
	select_vertex(v, gf);
    }
  return i;
}
void
swap(GPTR *a, GPTR *b)
{
  GPTR t = *b;
  *b = *a;
  *a = t;
}

int
partition(GPTR *A, int p, int r, int (*isl)(), int (*isb)())
{
  GPTR x = A[p];
  int i = p - 1;
  int j = r + 1;

  while(1)
    {
      do {
	j--;
      } while((*isl)(A[j],x));
      do {
	i++;
      } while((*isb)(A[i],x));

      if(i < j)
	swap(&(A[i]), &(A[j]));
      else
	return j;
    }
}

int
randomized_partition(GPTR *A, int p, int r,int (*isl)(), int (*isb)())
{
  int i = rand()%(r-p)+p;
  swap(&(A[p]), &(A[i]));
  return partition(A,p,r,isl,isb);
}

void 
randomized_quicksort(GPTR *A, int p, int r,int (*isl)(), int (*isb)())
{
  if(p < r)
    {
      int q = randomized_partition(A, p, r,isl,isb);
      randomized_quicksort(A,p,q,isl,isb);
      randomized_quicksort(A,q+1,r,isl,isb);
    }
}

GPTR
randomized_select(GPTR *A, int p, int r, int i,int (*isl)(), int (*isb)())
{
  int q, k;

  if(p == r)
    return A[p];

  q = randomized_partition(A,p,r,isl,isb);
  k = q-p+1;
  if(i <= k)
    return randomized_select(A,p,q,i,isl,isb);
  else
    return randomized_select(A,q+1,r,i-k,isl,isb);
}

void
reset_node_context(GraphFrame *gf)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      v->context = NULL;
    }
}

int 
number_of_edges_selected_vertices(GraphFrame *gf)
{
    LNode_type *ptr = NULL;
    LNode_type *lsver = gf->list_sel_vertex;
    int csv = 0;
    if(gf->graph_dir == DIRECTED)
    {
	for(ptr = lsver->next ; ptr != lsver; ptr = ptr->next)
	{
	    PNODE v = (PNODE)ptr->info;
	    LNode_type *ei;
	    for(ei = v->ledges_in->back; ei != v->ledges_in; ei = ei->back)
	    {
		PEDGE e = (PEDGE) ei->info;
		PNODE u = e->from;
		if(u->select)
		    csv++;
	    }
	}
    } 
    else if(gf->graph_dir == UNDIRECTED)
    {
	for(ptr = lsver->next ; ptr != lsver; ptr = ptr->next)
	{
	    PNODE v = (PNODE)ptr->info;
	    LNode_type *ei;
	    for(ei = v->ledges_in->back; ei != v->ledges_in; ei = ei->back)
	    {
		PEDGE e = (PEDGE) ei->info;
		PNODE u = e->from;
		if(u==v)
		    u = e->to;
		if(u->select)
		    csv++;
	    }
	}	
	csv = csv/2;
    }
    return csv;
}

int
select_double_linked_nodes_and_edges(GraphFrame *gf)
{
    int c = 0;
    LNode_type *ptr;
  for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; ptr = ptr->next)
  {
      PNODE v = (PNODE)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	    PEDGE e = (PEDGE)ei->info;
	    PNODE u = (PNODE)e->from;
	    PEDGE eb = find_edge_by_name(gf,v->label,u->label);
	    if(eb && eb != e)
	    {
		c+=2;
		changing_select_edge(gf,e,True);
		changing_select_edge(gf,eb,True);
	    }
	}
  }
  return c;
}

int 
rotate_graph(GraphFrame *gf, int degs)
{
    int xbox=0, ybox=0, wbox=0, hbox=0;
    int xc = 0 , yc = 0;
    double rads = 0.0, r = 0.0 , t= 0.0 ;
    LNode_type *ptr = NULL;

    if(!gf || !degs)
	return 0;

    rads = ((double)degs * PI) / 180.0;
    while(rads > 2*PI)
	rads-= 2*PI;
    while(rads < 0)
	rads += 2*PI;

    Delete_hash_table(gf->HV);

    if(gf->drawhighlight)
	find_highlighted_nodes_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    else
	find_graph_bounding_box(gf,&xbox, &ybox, &wbox, &hbox);
    
    xc = xbox + wbox / 2 ;
    yc = ybox + hbox / 2 ;

    for(ptr= gf->list_vertices->next; ptr != gf->list_vertices ; 
	ptr = ptr->next)
    {
	PNODE v = (PNODE)ptr->info;
	double dx = (double)(v->loc.x - xc);
	double dy = (double)(v->loc.y - yc);

	if(gf->drawhighlight && !v->highlight) {
	    verify_insert_v(gf,v);
	    continue;
	}

	r = sqrt(dx*dx+dy*dy);
	t = atan2(-dy,dx);
	t += rads;
	while(t > 2*PI)
	    t-= 2*PI;
	while(t < 0)
	    t += 2*PI;
	v->loc.x = xc + (int) (r * cos(t));
	v->loc.y = yc - (int) (r * sin(t));

	verify_insert_v(gf,v);
    }

#ifdef XGR
  if(gf->pdraw)
    refresh_graph_window(gf);
#endif
  return 0;
}




