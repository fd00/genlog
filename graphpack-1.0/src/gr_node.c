#include "gr_graph_fun.h"
#include "gr_node_p.h"
#include "gr_structure.h"
#include "alist.h"

char *
get_label_short_title(GraphFrame *gf,PNODE v);

char *
get_label_long_title(GraphFrame *gf,PNODE v);

/* 
   draw_vertex - Draw a vertex in the window and pixmap
   
         inputs  
                 widget - Drawing widget
		 gc - Graphics Context used to draw a vertex
		 x, y - Position of the vertex
		 side - size of the vertex
		 text - name of the vertex
		 weight - weight of the vertex
		 shape - shape of the vertex
		 gf - Graph Structure where the vertex is located.
         returns
                  none
         notes
	          If the 'draw' flag of the Graph is false, the graph is 
		  only drawn in the Pixmap and not in the window. 
		  If the 'show_vweight' flag of the Graph is true, the 
		  weight of the vertex is written in the vertex. 
     
*/
#ifdef XGR   
void
draw_vertex(
Widget	widget,
GC gc,
int x, 
int y, 
int side,
char	*text,char *weight, char shape, GraphFrame *gf)
{
	XTextItem	ti[1];
	Pixmap pixmap = gf->pixmap;

	/*if(color != GR_BG)
	  {
	    change_gc(widget, gf->pix_color, color);
	    gc = gf->pix_color;
	  }*/

	switch (shape)
	{
	  case CIRCULAR: /* circle */
	    draw_circular( widget, gc, x, y, side, gf);
	    break;
          case HOR_ELLIPSIS: /* horizontal ellipsis */
            draw_h_ellipsis( widget, gc, x, y, side, gf);
            break;
          case VER_ELLIPSIS: /* vertical ellipsis */
            draw_v_ellipsis( widget, gc,  x, y, side, gf );
            break;
	  case RHOMBUS: /* rhombus */
	    draw_rhombus(widget,gc,x,y,side,gf);
	    break;
	  case TRIANGLE: /* triangle */
	    draw_triangle(widget,gc,x,y,side,gf);
	    break;
	  case PENTAGON: /* pentagon */
	    draw_pentagon(widget,gc,x,y,side,gf);
	    break;
	  case HEXAGON: /* hexagon */
	    draw_hexagon(widget,gc,x,y,side,gf);
	    break;
	  case OCTAGON: /* octagon */
	    draw_octagon(widget,gc,x,y,side,gf);
	    break;
	  default: /* box */
	    draw_box( widget, gc, x, y, side, gf);
	    break;
	}

	/* and now write the text */

	if( text == NULL )
		return;
	
	ti[0].chars = text;
	ti[0].nchars = strlen( text );
	ti[0].delta = 0;
	ti[0].font = gf->font;

	if( strlen( text ) == 1 )
	  {
	    if(gf->draw)
	      XDrawText( XtDisplay( widget ), XtWindow( widget ),
			 gc, x-3, y+3, ti, 1);
	    if(gf->pdraw)
	      XDrawText( XtDisplay( widget ), pixmap,
			 gc, x-3, y+3, ti, 1);
	  }
	else
	  {
	    if(gf->draw)
	      XDrawText( XtDisplay( widget ), XtWindow( widget ),
			 gc, x-7, y+3, ti, 1);
	    if(gf->pdraw)
	      XDrawText( XtDisplay( widget ), pixmap,
			 gc, x-7, y+3, ti, 1);
	  }

	if(gf->show_vweight) 
	  if(weight)
	    {
	      int wx;
	      wx=x - ((strlen(weight)/2)*7);
	      
	      ti[0].chars = weight;
	      ti[0].nchars = strlen(weight );
	      ti[0].delta = 0;
	      ti[0].font = gf->font;
	      if(gf->draw)
		XDrawText( XtDisplay(widget), XtWindow(widget), gc,
			   wx-3, y - 13, ti, 1 );
	      if(gf->pdraw)
		XDrawText( XtDisplay(widget), pixmap, gc,
			   wx-3, y - 13, ti, 1 );
	    }
}

void
draw_octagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;
  XPoint points[9];
  int kside = side/2;
 
  points[0].x = x-side;
  points[0].y = y-kside;
  points[1].x = x-side;
  points[1].y = y+kside;
  points[2].x = x-kside;
  points[2].y = y+side;
  points[3].x = x+kside;
  points[3].y = y+side;
  points[4].x = x+side;
  points[4].y = y+kside;
  points[5].x = x+side;
  points[5].y = y-kside;
  points[6].x = x+kside;
  points[6].y = y-side;  
  points[7].x = x-kside;
  points[7].y = y-side;
  points[8].x = x-side;
  points[8].y = y-kside;

  /* draw the filled polygon */
  if(gf->draw)
    XFillPolygon( d, w,
		    gf->pix_erase, points,8,Complex,CoordModeOrigin);
  if(gf->pdraw)
    XFillPolygon( d, pixmap,
		  gf->pix_erase,  points,8,Complex,CoordModeOrigin);
  
  /* and now draw the border */
  if(gf->draw)
    XDrawLines( d, w, gc, points,9,CoordModeOrigin);
  if(gf->pdraw)
    XDrawLines( d, pixmap, gc, points,9,CoordModeOrigin);
}
void
draw_hexagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;
  XPoint points[7];
  int kside = side/2;
 
  points[0].x = x-side;
  points[0].y = y;
  points[1].x = x-kside;
  points[1].y = y+side;
  points[2].x = x+kside;
  points[2].y = y+side;
  points[3].x = x+side;
  points[3].y = y;
  points[4].x = x+kside;
  points[4].y = y-side;
  points[5].x = x-kside;
  points[5].y = y-side;
  points[6].x = x-side;
  points[6].y = y;

  /* draw the filled polygon */
  if(gf->draw)
    XFillPolygon( d, w,
		    gf->pix_erase, points,6,Complex,CoordModeOrigin);
  if(gf->pdraw)
    XFillPolygon( d, pixmap,
		  gf->pix_erase,  points,6,Complex,CoordModeOrigin);
  
  /* and now draw the border */
  if(gf->draw)
    XDrawLines( d, w, gc, points,7,CoordModeOrigin);
  if(gf->pdraw)
    XDrawLines( d, pixmap, gc, points,7,CoordModeOrigin);
}
void
draw_pentagon(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;
  XPoint points[6];
 
  points[0].x = x-side;
  points[0].y = y;
  points[1].x = x-side;
  points[1].y = y+side;
  points[2].x = x+side;
  points[2].y = y+side;
  points[3].x = x+side;
  points[3].y = y;
  points[4].x = x;
  points[4].y = y-side;
  points[5].x = x-side;
  points[5].y = y;

  /* draw the filled polygon */
  if(gf->draw)
    XFillPolygon( d, w,
		    gf->pix_erase, points,5,Complex,CoordModeOrigin);
  if(gf->pdraw)
    XFillPolygon( d, pixmap,
		  gf->pix_erase,  points,5,Complex,CoordModeOrigin);
  
  /* and now draw the border */
  if(gf->draw)
    XDrawLines( d, w, gc, points,6,CoordModeOrigin);
  if(gf->pdraw)
    XDrawLines( d, pixmap, gc, points,6,CoordModeOrigin);
}
void
draw_rhombus(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;
  XPoint points[5];
 
  points[0].x = x-side;
  points[0].y = y;
  points[1].x = x;
  points[1].y = y-side;
  points[2].x = x+side;
  points[2].y = y;
  points[3].x = x;
  points[3].y = y+side;
  points[4].x = x-side;
  points[4].y = y;

  /* draw the filled polygon */
  if(gf->draw)
    XFillPolygon( d, w,
		    gf->pix_erase, points,4,Complex,CoordModeOrigin);
  if(gf->pdraw)
    XFillPolygon( d, pixmap,
		  gf->pix_erase,  points,4,Complex,CoordModeOrigin);
  
  /* and now draw the border */
  if(gf->draw)
    XDrawLines( d, w, gc, points,5,CoordModeOrigin);
  if(gf->pdraw)
    XDrawLines( d, pixmap, gc, points,5,CoordModeOrigin);
}
void
draw_triangle(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;
  XPoint points[4];
 
  points[0].x = x;
  points[0].y = y-side;
  points[1].x = x-side;
  points[1].y = y+side;
  points[2].x = x+side;
  points[2].y = y+side;
  points[3].x = x;
  points[3].y = y-side;


  /* draw the filled polygon */
  if(gf->draw)
    XFillPolygon( d, w,
		    gf->pix_erase, points,3,Complex,CoordModeOrigin);
  if(gf->pdraw)
    XFillPolygon( d, pixmap,
		  gf->pix_erase,  points,3,Complex,CoordModeOrigin);
  
  /* and now draw the border */
  if(gf->draw)
    XDrawLines( d, w, gc, points,4,CoordModeOrigin);
  if(gf->pdraw)
    XDrawLines( d, pixmap, gc, points,4,CoordModeOrigin);
}
void
draw_box(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;

  /* draw the filled polygon */
  if(gf->draw)
    XFillRectangle( d, w,
		    gf->pix_erase, x-side, y-side, 2*side, 2*side );
  if(gf->pdraw)
    XFillRectangle( d, pixmap,
		    gf->pix_erase, x-side, y-side, 2*side, 2*side );
  
  /* and now draw the border */
  if(gf->draw)
    XDrawRectangle( d, w, gc, x-side, y-side, 2*side, 2*side );
  if(gf->pdraw)
    XDrawRectangle( d, pixmap, gc, x-side, y-side, 2*side, 2*side );
}

void
draw_circular(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;

  /* draw the filled polygon */
  if(gf->draw)
    XFillArc( d, w,
	      gf->pix_erase, x-side, y-side, 2*side, 2*side,0*64,360*64 );
  if(gf->pdraw)
    XFillArc( d, pixmap,
	      gf->pix_erase, x-side, y-side, 2*side, 2*side,0*64,360*64 );

  /* and now draw the border */
  if(gf->draw)
    XDrawArc(d, w, gc, x-side, y-side, 2*side, 2*side, 0*64, 360*64);
  if(gf->pdraw)
    XDrawArc( d, pixmap, gc, x-side, y-side, 2*side, 2*side ,0*64, 360*64);

}
void
draw_h_ellipsis(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;


  /* draw the filled polygon */
  if(gf->draw)
    XFillArc( d, w,
	      gf->pix_erase, x-side, y-(side/2), 2*side, side,0*64,360*64 );
  if(gf->pdraw)
    XFillArc( d, pixmap,
	      gf->pix_erase, x-side, y-(side/2), 2*side, side,0*64,360*64 );

  /* and now draw the border */
  if(gf->draw)
    XDrawArc(d, w, gc, x-side, y-(side/2), 2*side, side, 0*64, 360*64);
  if(gf->pdraw)
    XDrawArc( d, pixmap, gc, x-side, y-(side/2), 2*side, side ,0*64, 360*64);

}
void
draw_v_ellipsis(
Widget	widget,
GC gc,
int x, 
int y, 
int side,GraphFrame *gf)
{
  Display	*d = XtDisplay(widget);
  Window	w = XtWindow(widget);
  Pixmap pixmap = gf->pixmap;

  /* draw the filled polygon */
  if(gf->draw)
    XFillArc( d, w,
	      gf->pix_erase, x-(side/2), y-side, side, 2*side,0*64,360*64 );
  if(gf->pdraw)
    XFillArc( d, pixmap,
	      gf->pix_erase, x-(side/2), y-side, side, 2*side,0*64,360*64 );

  /* and now draw the border */
  if(gf->draw)
    XDrawArc(d, w, gc, x-(side/2), y-side, side, 2*side, 0*64, 360*64);
  if(gf->pdraw)
    XDrawArc( d, pixmap, gc, x-(side/2), y-side, side, 2*side ,0*64, 360*64);
  
}

/*
 * write some text in the screen
 */
void
draw_text(
Widget	widget,
int	x, 
int     y,
GC	op,
Font	font,
char	*text,
GraphFrame *gf)
{
	XTextItem	ti[1];

	if( text == NULL )
		return;
	
	ti[0].chars = text;
	ti[0].nchars = strlen( text );
	ti[0].delta = 0;
	ti[0].font = font;

	if(gf->draw)
	  XDrawText( XtDisplay(widget), XtWindow(widget),
		     op, x, y, ti, 1 );
	if(gf->pdraw)
	  XDrawText( XtDisplay(widget), gf->pixmap,
		     op, x, y, ti, 1 );
}
#endif
/* 
      select_vertex - Vertex is saved in the list of selected vertex and
                        it is redrawn as selected vertex. 

         inputs  
                 v - Selected vertex
		 gf - Graph Structure where the vertex is located. 
         returns
                  none
                  
*/

void    
select_vertex(struct pt *v,GraphFrame *gf)
{

  save_list_selected(v,gf);
#ifdef XGR
  {
    char *vlabel = get_vertex_display_name(gf, v);
    GC gc = choose_gc(gf, v);

  draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y,
	      gf->hsize, NULL,NULL,v->shape,gf);
  
  if(v->highlight)
    draw_vertex(gf->drawing_a,gf->pix_high, v->loc.x, v->loc.y,
		gf->vsize, vlabel, v->weight,v->shape,gf);
  else
    draw_vertex(gf->drawing_a,gc, v->loc.x, v->loc.y,
		gf->vsize, vlabel, v->weight,v->shape,gf);
  if (vlabel) free(vlabel); vlabel = NULL;
  }
#endif  
  v->select = True;

}
/* deselect_vertex - Vertex is removed from the selected list and it
                       is redrawn as normal vertex. 

         inputs  
                 v - Unselected Vertex
		 gf - Graph Structure where the vertex is located.
         returns
                  none
*/
void
deselect_vertex(struct pt *v,GraphFrame *gf)
{
#ifdef XGR
  char *vlabel = get_vertex_display_name(gf, v);
  /* changed pix_src_xor_pix_dst */
  draw_vertex( gf->drawing_a, gf->pix_erase,v->loc.x, v->loc.y,
	       gf->hsize,NULL, NULL, v->shape,gf);
	
  if(v->highlight)
    draw_vertex(gf->drawing_a,gf->pix_high, v->loc.x, v->loc.y,
		gf->vsize, vlabel, v->weight,v->shape,gf);
  else
    {
      GC gc = choose_gc(gf, v);
    draw_vertex(gf->drawing_a,gc, v->loc.x, v->loc.y,
		gf->vsize, vlabel, v->weight,v->shape,gf);
    }
   if(vlabel) free(vlabel); vlabel = NULL;
#endif
  
  v->select = False;
  delete_list_selected(v,gf);
}
/* get_string_shape_file - It converts the type of the shape of the vertex
                           in a string that describes the shape of the vertex
         inputs  
                 v - vertex
         returns
                  string of the shape of the vertex
*/
char *
get_string_shape_file(struct pt *v)
{
  char *str = (char *)malloc(25);
  if(!str) {
      fprintf(stderr,"Out of Memory get_string_shape_file\n");
      abort();
  }

  if(v->shape == BOX)
    strcpy(str,"box");
  else if(v->shape == CIRCULAR)
    strcpy(str,"circle");
  else if(v->shape == VER_ELLIPSIS)
    strcpy(str,"ver_ellipsis");
  else if(v->shape == HOR_ELLIPSIS)
    strcpy(str,"hor_ellipsis");
  else if(v->shape == RHOMBUS)
    strcpy(str,"rhombus");
  else if(v->shape == TRIANGLE)
    strcpy(str,"triangle");
  else if(v->shape == PENTAGON)
    strcpy(str,"pentagon");
  else if(v->shape == HEXAGON)
    strcpy(str,"hexagon");
  else if(v->shape == OCTAGON)
    strcpy(str,"octagon");

  return str;
}

void
copy_vertex_info(PNODE v, PNODE ov)
{
  if(!ov || !v)
      return;

  v->group = ov->group;
  v->shape = ov->shape;
  v->color = ov->color;
  v->counter = ov->counter;
  if(ov->attrib)
    v->attrib = copy_assoc_list(ov->attrib);
  GRStrAllocCopy(v->weight,ov->weight);
}
/* copy_vertex - Creates a new vertex that is a copy of other vertex

         inputs  
                 ov - vertex to be copied
         returns
                  new vertex
         notes
                 returns NULL if there is not more memory. 

*/
struct pt * 
copy_vertex(struct pt * ov)
{
  struct pt *v;

  v = (struct pt *) calloc( 1,sizeof(struct pt) );
  if( v == NULL ) {
      fprintf(stderr,"Out of Memory copy_vertex\n");
      abort();
  }
  v->loc.x = ov->loc.x;
  v->loc.y = ov->loc.y;
  v->select = False;
  v->highlight = False;
  v->group = ov->group;
  v->shape = ov->shape;
  GRStrAllocCopy(v->label,ov->label);
  v->color = ov->color;
  v->num = ov->num;
  v->counter = ov->counter;
  v->prelim = 0;
  v->modifier = 0;
  v->attrib = NULL;
  if(ov->attrib)
    v->attrib = copy_assoc_list(ov->attrib);
  GRStrAllocCopy(v->weight,ov->weight);
  v->ledges_in = init_linked_list();
  v->ledges_out = init_linked_list();
  return(v);
}

/* highlight_vertex - Highlights a vertex with a different color (red) and 
                 save it to hightlight list of vertices
         inputs  
                 v - vertex to be highlighted
		 gf - Graph Structure where vertex is located.
         returns
                  none
*/
void    
highlight_vertex(struct pt *v,GraphFrame *gf)
{
  save_list_highlight(v,gf);
#ifdef XGR
  {
  char *vlabel = get_vertex_display_name(gf, v);

  draw_vertex(gf->drawing_a, gf->pix_high, v->loc.x, v->loc.y,
	      gf->vsize, vlabel, v->weight,v->shape,gf);
  if(vlabel) free(vlabel); vlabel = NULL;
  }  
#endif
  v->highlight = True;
}

/* dehighlight_vertex - Dehighlights a vertex drawing it as a normal vertex and
                 remove it from hightlight list of vertices
         inputs  
                 v - vertex to be dehighlighted
		 gf - Graph Structure where vertex is located.
         returns
                  none
*/

void
dehighlight_vertex(struct pt *v,GraphFrame *gf)
{
#ifdef XGR
  char *vlabel = get_vertex_display_name(gf, v);
  GC gc = choose_gc(gf, v);
  draw_vertex(gf->drawing_a,gc, v->loc.x, v->loc.y,
	      gf->vsize, vlabel, v->weight,v->shape,gf);
  if (vlabel) free(vlabel);  vlabel = NULL;
#endif
  v->highlight = False;
  delete_list_highlight(v,gf);

}
/* create_node_memory - Creates a regular vertex with default information. 

         inputs  
                 nname - String of the name of the vertex
         returns
                  new vertex structure
         notes
                 Returns NULL if there is not more memory
*/
struct pt *
create_node_memory(char *nname)
{
  /* else create a new vertex */
  struct pt *v = (struct pt *) calloc( 1, sizeof(struct pt) );
  if( v == NULL ) {
      fprintf(stderr,"Out of Memory create_node_memory\n");
      abort();
  }
  v->loc.x = 0;
  v->loc.y = 0;
  v->select = False;
  v->group = False;
  v->highlight = False;
  v->shape = BOX;
  GRStrAllocCopy(v->label,nname);
  v->weight = NULL;
  v->mark = False;
  v->pick = False;
  v->level = 0;
  v->num =0;
  v->counter = 0;
  v->color = GR_BG;
  v->prelim =0;
  v->modifier =0;
  v->attrib = NULL;
  v->ledges_in = init_linked_list();
  v->ledges_out = init_linked_list();
  return v;
}
/* delete_vertex_info - Destructor of the memory allocated for the vertex

         inputs  
                 v - Vertex to be deleted
         returns
                  none
*/
void
delete_vertex_info(struct pt *v)
{
    if(v->label) {
	free(v->label); v->label = NULL; }
    if(v->weight) {
	free(v->weight); v->weight = NULL; }

  if(v->attrib) { /* Deleting Attributes of node */
      delete_assoc_list(v->attrib); v->attrib = NULL; }

  Delete_all_list(v->ledges_in); v->ledges_in = NULL;
  Delete_all_list(v->ledges_out); v->ledges_out = NULL;
}
/* is_vertex_selected - Verify if the vertex is selected

         inputs  
                 v - vertex
         returns
	         Boolean value 

*/
Boolean
is_vertex_selected(struct pt *v)
{
  return (v->select);
}
/* is_vertex_highlighted - Verify if the vertex is highlighted

         inputs  
                 v - vertex
         returns
	         Boolean value 

*/
Boolean
is_vertex_highlighted(struct pt *v)
{
  return (v->highlight);
}
/* is_selected - Verify if the vertex is in the list of 
                 selected vertices
         inputs  
                 v - vertex
		 gf - Graph structure where the vertex is located. 
         returns
	         Boolean value 

*/
Boolean
is_selected(struct pt *v,GraphFrame *gf)
{
  LNode_type *nptr;
  nptr = Search_Sequential_list(gf->list_sel_vertex,(char *)v);
  if(nptr->info == (char *)v)
    return True;
  else
    return False;
  
}
/* get_shape - Get type of the shape of vertex 

         inputs  
                 v - vertex
         returns
	         type of the shape of the vertex

*/
char
get_shape(struct pt *v)
{
  return (v->shape);
}


/* set_vertex_shape - Set type of the shape of vertex 

         inputs  
                 v - vertex
		 shape - shape of vertex

*/
void
set_vertex_shape(struct pt *v,char shape)
{
  v->shape = shape;
}
/* get_color - Get type of the color of vertex 

         inputs  
                 v - vertex
         returns
	         type of the color of the vertex

*/
short
get_color(struct pt *v)
{
  return (v->color);
}
/* set_vertex_color- Set type of the color of vertex

         inputs  
                 v - vertex
		 color - color of vertex

*/
void
set_vertex_color(struct pt *v,short color)
{
  v->color = color;
}
/* get_counter - Get type of the color of vertex 

         inputs  
                 v - vertex
         returns
	         counter number

*/
int
get_counter(struct pt *v)
{
  return (v->counter);
}
/* set_vertex_counter- Set counter of vertex

         inputs  
                 v - vertex
		 counter - new initial counter number

*/
void
set_vertex_counter(struct pt *v,int counter)
{
  v->counter = counter;
}

void
increment_counter_vertex(struct pt *v)
{
  v->counter++;
}
/* get_shape_vertex_number - Get number of the shape of vertex 

         inputs  
                 v - vertex
         returns
	         Number of the shape of the vertex

*/
int
get_shape_vertex_number(struct pt *v)
{
  if(v->shape == BOX)
    return 0;
  if(v->shape == CIRCULAR)
    return 1;
  if(v->shape == VER_ELLIPSIS)
    return 2;
  if(v->shape == HOR_ELLIPSIS)
    return 3;
  if(v->shape == RHOMBUS)
    return 4;
  if(v->shape == TRIANGLE)
    return 5;
  if(v->shape == PENTAGON)
    return 6;
  if(v->shape == HEXAGON)
    return 7;
  if(v->shape == OCTAGON)
    return 8;

  return 0;
}
/* get_shape_number - Given the string of the shape of the vertex, convert it
                      to type of the shape of the vertex
         inputs  
                 nshape - String of the shape of the vertex
         returns
	         type of the shape of the vertex.
	 notes
	         If the string is not recognized a regular BOX type is returned
*/
char
get_shape_number(char *nshape)
{
  if(!strcmp(nshape,"circle"))
    return CIRCULAR;
  if(!strcmp(nshape,"box"))
    return BOX;
  if(!strcmp(nshape,"ver_ellipsis"))
    return VER_ELLIPSIS;
  if(!strcmp(nshape,"hor_ellipsis"))
    return HOR_ELLIPSIS;
  if(!strcmp(nshape,"rhombus"))
    return RHOMBUS;
  if(!strcmp(nshape,"triangle"))
    return TRIANGLE;
  if(!strcmp(nshape,"pentagon"))
    return PENTAGON;
  if(!strcmp(nshape,"hexagon"))
    return HEXAGON;
  if(!strcmp(nshape,"octagon"))
    return OCTAGON;

  fprintf(stderr, "Error in shape name : %s\n",nshape);
  /*error = NSHAPE_ERROR;*/
  return BOX;
}
/* change_label_vertex - New name is assigned to the vertex, the names of 
                         the edges is updated and the Hash table of the 
			 names of the vertices is updated. 

         inputs  
	         gf - Graph Structure where the vertex is located
		 v - vertex
		 label - new name of the vertex
         returns
                  False, if the label is already asigned to a different vertex.
         side effects
                  Vertex is erased with the old  name. Vertex should be 
		  redrawn after this function is called. 

*/

Boolean
change_label_vertex(GraphFrame *gf, struct pt *v, char * label)
{
  Node_type *nptr = Retrieve(gf->HVname,(Key_type)label);

  if(nptr)
    return False;

  if(gf->graph_dir != NONE)
    change_names_edges(gf, v, label);

  Delete_target(gf->HVname,(Key_type)v->label);
#ifdef XGR
  {
    char *vlabel = get_vertex_display_name(gf, v);
  draw_vertex(gf->drawing_a, gf->pix_erase, v->loc.x, 
		      v->loc.y, gf->vsize, vlabel, v->weight, v->shape,gf);
  if(vlabel) free(vlabel); vlabel = NULL;
  }
#endif

  if(v->label) free(v->label); v->label = NULL;
  GRStrAllocCopy(v->label,label); 	

  insert_hash_vertex_name(gf,label,v);

  return True;
}
/* change_weight_vertex - Change the weight of the vertex
                       I
         inputs  
                 gf - Graph Structure 
		 v - Vertex
		 weight - new weight of the vertex
         returns
                  none
         side effects
                  none
*/

void
change_weight_vertex(GraphFrame *gf, struct pt *v, char * weight)
{
    if(v->weight) {
    free(v->weight); v->weight = NULL;
    }
    GRStrAllocCopy(v->weight,weight); 
}
/* change_selection_vertex - Change Selection of the vertex. If the vertex 
                             is selected, it is saved on the list of the
			     selected vertices. If the vertex is unselected
			     it is deleted from the list of selected vertex.

         inputs  
	          gf - Graph structure
		  v - Vertex
		  sel - Select or unselect the vertex. 
         returns
                  none
         side effects
	          If the vertex is selected, it is redrawn with double line.
*/

void
change_selection_vertex(GraphFrame *gf, struct pt *v, Boolean sel)
{
#ifdef XGR
  if(v->select)
    draw_vertex(gf->drawing_a, gf->pix_erase, v->loc.x, v->loc.y,
		gf->hsize, NULL,NULL,v->shape,gf);
#endif

  if(sel)
    save_list_selected(v,gf);
  else
    delete_list_selected(v,gf);
  v->select = sel;
#ifdef XGR  
  if(v->select)
    {
      GC gc = choose_gc(gf,v);
      draw_vertex(gf->drawing_a, gc,v->loc.x, v->loc.y,
		  gf->hsize, NULL,NULL,v->shape,gf);
    }
#endif
}
/* change_highlight_vertex - Change Highlight of the vertex. If the vertex 
                             is highlighted, it is saved on the list of the
			     highlighted vertices. If the vertex is not
			     highlighted it is deleted from the list of 
			     highlighted vertex.

         inputs  
	          gf - Graph structure
		  v - Vertex
		  high - Highlight or not the vertex. 
         returns
                  none
         side effects
	          If the vertex is highlighted, it is redrawn with different
		  color (red).
*/
void
change_highlight_vertex(GraphFrame *gf, struct pt *v, Boolean high)
{
#ifdef XGR
  char *vlabel = get_vertex_display_name(gf, v);
  if(v->highlight)
    draw_vertex(gf->drawing_a, gf->pix_erase,v->loc.x, 
		v->loc.y, gf->vsize, vlabel, v->weight, v->shape,gf); 
  if(vlabel) free(vlabel); vlabel = NULL;
#endif
  if(high)
    save_list_highlight(v,gf);
  else
    delete_list_highlight(v,gf);

  v->highlight = high;

}
/* change_shape_vertex  - Change shape of the vertex. 

         inputs  
                 gf - Graph Structure
		 v - Vertex
		 shape - New shape of the vertex
         returns
                  none
         side effects
                  The vertex is erased from the window. The vertex should 
		  be redrawn after this function is called. 

*/

void
change_shape_vertex(GraphFrame *gf, struct pt *v, char shape)
{
#ifdef XGR
  char *vlabel = get_vertex_display_name(gf, v);
  if(v->select)
    draw_vertex(gf->drawing_a, gf->pix_erase, v->loc.x, v->loc.y,
		gf->hsize, NULL,NULL,v->shape,gf);
  draw_vertex(gf->drawing_a, gf->pix_erase, v->loc.x, 
	      v->loc.y, gf->vsize, vlabel, v->weight, v->shape,gf);
  if(vlabel) free(vlabel); vlabel = NULL;
#endif
  v->shape = shape;

#ifdef XGR
  if(v->select)
    {
      GC gc = choose_gc(gf,v);
	draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y,
		    gf->hsize, NULL,NULL,v->shape,gf);
    }
#endif
}
/* get_string_shape - Get the name of the shape of the vertex.
                       I
         inputs  
                 v - Vertex
         returns
                  Name of the shape of the vertex
         side effects
                  none
*/

char *
get_string_shape(struct pt *v)
{
  char *str = (char *)malloc(25);
  if(!str) {
      fprintf(stderr,"Out of Memory get_string_shape\n");
      abort();
  }

  if(v->shape == BOX)
    strcpy(str,"Box");
  else if(v->shape == CIRCULAR)
    strcpy(str,"Circle");
  else if(v->shape == VER_ELLIPSIS)
    strcpy(str,"Vert. Ellypsis");
  else if(v->shape == HOR_ELLIPSIS)
    strcpy(str,"Hor. Ellypsis");
  else if(v->shape == RHOMBUS)
    strcpy(str,"Rhombus");
  else if(v->shape == TRIANGLE)
    strcpy(str,"Triangle");
  else if(v->shape == PENTAGON)
    strcpy(str,"Pentagon");
  else if(v->shape == HEXAGON)
    strcpy(str,"Hexagon");
  else if(v->shape == OCTAGON)
    strcpy(str,"Octagon");

  return str;
}
/* get_label_vertex - Get Name of the vertex.

         inputs  
                 v - Vertex
         returns
                  Name of the vertex
         side effects
                  none

*/

char *
get_label_vertex(struct pt *v)
{
  return (v->label);
}
/* get_weight_vertex - Get Weight of the vertex.

         inputs  
                 v - Vertex
         returns
                  Weight of the vertex
         side effects
                  none

*/

char *
get_weight_vertex(struct pt *v)
{
  return (v->weight);
}

void
set_vertex_position(struct pt *v, int x, int y)
{
  v->loc.x = x;
  v->loc.y = y;
}
int
get_x(struct pt *v)
{
  return v->loc.x;
}
int
get_y(struct pt *v)
{
  return v->loc.y;
}

int
get_vertex_number(struct pt *v)
{
  return v->num;
}
/* redraw_vertex - Redraw vertex. If the vertex is highlighted the vertex is
                   redrawn with different color. 
         inputs  
                 gf - Graph Structure
		 v - Vertex
         returns
                  none
         side effects
                  none
         notes
	         If the vertex is selected, the vertex should be redrawn 
		 as selected after this function is called. 
*/
#ifdef XGR
void
redraw_vertex(GraphFrame *gf, struct pt *v)
{
  char *vlabel = get_vertex_display_name(gf, v);
  if(v->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v->loc.x, 
		v->loc.y, gf->vsize, vlabel, v->weight, v->shape,gf);      
  else
    {
      GC gc = choose_gc(gf,v);
      draw_vertex(gf->drawing_a,gc, v->loc.x, v->loc.y, 
		  gf->vsize, vlabel, v->weight, v->shape,gf);
    }
  if(vlabel) free(vlabel); vlabel = NULL;
}
#endif

char *
get_label_short_name(GraphFrame *gf,PNODE v)
{
    char *label = NULL;
    char *ptr = NULL;
    char *slabel = NULL;
    label = v->label;

    if(label[0]=='\0')
	return NULL;
    if(label[1]!='\0') 
    {
	ptr = &(label[strlen(label)-2]);
    
	while(ptr!=label)
	{
	    if(*ptr=='/')
		break;
	    ptr--;
	}
    }
    GRStrAllocCopy(slabel,ptr);
    return slabel;
}

char *
get_vertex_display_name(GraphFrame *gf, struct pt *v)
{
  char *name= NULL;

  if(gf->drawhighlight && !v->highlight)
      return NULL;

  if(gf->dmode == DIS_NO_LABEL)
      return GRStrAllocCopy(name,"");

  if(gf->dmode == DIS_NUM)
    {
      name = malloc(MDIGITS);
      if(!name) {
	  fprintf(stderr,"Out of Memory get_vertex_display_name\n");
	  abort();
      }
      sprintf(name,"%d",v->num);
    }
  else if(gf->dmode == DIS_LABEL)
      GRStrAllocCopy(name,v->label); 
  else if(gf->dmode == DIS_SHORT_LABEL)
    {
	name = get_label_short_name(gf,v);
    }
  else if(gf->dmode == DIS_SHORT_TITLE)
  {
      name = get_label_short_title(gf,v);
  }
  else if(gf->dmode == DIS_TITLE)
  {
      name = get_label_long_title(gf,v);
  }

  return name;
}


struct pt *
create_node_in_graph_name(GraphFrame *gf, char *name)
{
  struct pt *v = create_node_memory(name);

  (gf->count_vertex)++;
  (gf->nvertex)++;
  v->num = gf->nvertex;

  Insert_linked_list(gf->list_vertices,create_new_node_llist((char *)v)); 
  insert_hash_vertex_name(gf,v->label,v);
  return v;
}

int
get_num_attribute(struct pt *v)
{
  int numa = 0;
  if(v->attrib)
      numa = count_llist(v->attrib);
  return numa;
}

const char *
get_attribute(struct pt *v, const char *aname)
{
  if(v->attrib)
    return find_value_assoc_list(v->attrib,aname);
  return NULL;
}


String
get_color_string(short node_col)
{
  char *str = NULL;
  switch(node_col)
    {
    case GR_GREEN:
      GRStrAllocCopy(str,"Green");
      return str;
    case GR_NAVY:
      GRStrAllocCopy(str,"Navy");
      return str;
    case GR_YELLOW:
      GRStrAllocCopy(str,"Yellow");
      return str;
    case GR_MAGENTA:
      GRStrAllocCopy(str,"Magenta");
      return str;
    case GR_CYAN:
      GRStrAllocCopy(str,"Cyan");
      return str;
    case GR_GRAY:
      GRStrAllocCopy(str,"Gray");
      return str;
    case GR_ORANGE:
      GRStrAllocCopy(str,"Orange");
      return str;
    case GR_BLACK:
      GRStrAllocCopy(str,"Black");
      return str;
    case GR_MAROON:
      GRStrAllocCopy(str,"Maroon");
      return str;
    case GR_WHITE:
      GRStrAllocCopy(str,"White");
      return str;
    case GR_BLUE:
      GRStrAllocCopy(str,"Blue");
      return str;
    case GR_PINK:
      GRStrAllocCopy(str,"Pink");
      return str;
    case GR_BROWN:
      GRStrAllocCopy(str,"Brown");
      return str;
    case GR_GREY:
      GRStrAllocCopy(str,"Grey");
      return str;
    case GR_LIMEGREEN:
      GRStrAllocCopy(str,"LimeGreen");
      return str;
    case GR_TURQUOISE:
      GRStrAllocCopy(str,"Turquoise");
      return str;
    case GR_VIOLET:
      GRStrAllocCopy(str,"Violet");
      return str;
    case GR_WHEAT:
      GRStrAllocCopy(str,"Wheat");
      return str;
    case GR_PURPLE:
      GRStrAllocCopy(str,"Purple");
      return str;
    case GR_BEIGE:
      GRStrAllocCopy(str,"Beige");
      return str;
    case GR_RED:
      GRStrAllocCopy(str,"Red");
      return str;

    }
  return NULL;
}
#ifdef XGR
void
change_gc(Widget w, GC gc, short node_col)
{
  String color = get_color_string(node_col);
  Display *dpy = XtDisplay(w);
  Colormap cmap = DefaultColormapOfScreen(XtScreen(w));
  XColor col, unused;

  if(!color)
      return;

  if(!XAllocNamedColor(dpy, cmap, color, &col, &unused))
    {
      fprintf(stderr, "Can't alloc %s\n", color);
    }
  XSetForeground(dpy, gc, col.pixel);
  free(color); color = NULL;
}


GC 
choose_gc(GraphFrame *gf, struct pt *v)
{
  GC gc = gf->gc;
  if(v->color != GR_BG)
    {
      change_gc(gf->drawing_a, gf->pix_color, v->color);
      gc = gf->pix_color;
    }
  return gc;
}
#endif

void
copy_attributes(struct pt *v, struct pt *ov)
{
  if(ov->attrib)
    v->attrib = copy_assoc_list(ov->attrib);
}

LNode_type *
get_edges_in(struct pt *v)
{
  return v->ledges_in;
}

LNode_type *
get_edges_out(struct pt *v)
{
  return v->ledges_out;
}

int
get_num_edges_in(PNODE v)
{
    return v ? count_llist(v->ledges_in) : 0;
}


int
get_num_edges_out(PNODE v)
{
    return v ? count_llist(v->ledges_out) : 0;
}


void *
get_node_context(struct pt *v)
{
    return v->context;
}

void change_attribute(struct pt *v, const char *att_name, const char *att_value) {
/*
 *	This function added by Andrew Morrison (11.30.02) 
 *
 *	input:	A node, an attribute name, and a value
 *	output:	node v's attribute name set to value
 */
	if(att_name && v) {
		if(!v->attrib)
			add_attribute(v,att_name,att_value);
		else {
			remove_object_assoc_list(v->attrib,att_name);
			add_attribute(v,att_name,att_value);
		}
	}
	
}
void
add_attribute(struct pt *v, const char *att_name, const char *att_value)
{
  if(att_name && v)
    {
      if(!v->attrib)
	v->attrib = init_assoc_list();
      add_object_assoc_list(v->attrib,att_name,att_value);
    }
}

void
add_copy_attribute(struct pt *v, const char *att_name, const char *att_value)
{
  char *avalue = NULL;
  if(att_name && v && att_value)
    {
      if(!v->attrib)
	v->attrib = init_assoc_list();
      GRStrAllocCopy(avalue,att_value);
      add_object_assoc_list(v->attrib,att_name,avalue);
    }
}


LNode_type *
get_attribute_list(PNODE v)
{
    if(!v)
	return NULL;
    else
	return v->attrib;
}

struct pt *
get_first_parent(struct pt *Node)
{  
  LNode_type *ptr;
  if(!Node)
    return NULL;

  for(ptr = Node->ledges_in->next ; ptr != Node->ledges_in ; ptr= ptr->next)
    {
      struct edge *e = (struct edge *)(ptr->info);
      if(e)
	return (e->from);
    }
  return NULL;
}


int
is_counter_lower(PNODE u, PNODE v)
{
  return (u->counter < v->counter);
}

int
is_counter_bigger(PNODE u,PNODE v)
{
  return(u->counter > v->counter);
}

int
is_vertex_weight_lower(PNODE u, PNODE v)
{
  int uweight = atoi(u->weight);
  int vweight = atoi(v->weight);
  return (uweight < vweight);
}

int
is_vertex_weight_bigger(PNODE u,PNODE v)
{
  int uweight = atoi(u->weight);
  int vweight = atoi(v->weight);
  return (uweight > vweight);
}

int
is_vertex_num_edges_in_lower(PNODE u, PNODE v)
{
  int nuei = count_llist(u->ledges_in);
  int nvei = count_llist(v->ledges_in);
  return (nuei < nvei);
}

int
is_vertex_num_edges_in_bigger(PNODE u,PNODE v)
{
  int nuei = count_llist(u->ledges_in);
  int nvei = count_llist(v->ledges_in);
  return (nuei > nvei);
}

int
is_vertex_num_edges_out_lower(PNODE u, PNODE v)
{
  int nueo = count_llist(u->ledges_out);
  int nveo = count_llist(v->ledges_out);
  return (nueo < nveo);
}

int
is_vertex_num_edges_out_bigger(PNODE u,PNODE v)
{
  int nueo = count_llist(u->ledges_out);
  int nveo = count_llist(v->ledges_out);
  return (nueo > nveo);
}

int
is_vertex_in_region(PNODE v, int x, int y, int w, int h)
{
    if(!v)
	return 0;
    if(v->loc.x > x && v->loc.x < (x + w) && 
       v->loc.y > y && v->loc.y < (y +h))
	return 1;
    return 0;
}

int
is_vertex_label_lower(PNODE u, PNODE v)
{
  return (strcmp(u->label,v->label) > 0);
}

int
is_vertex_label_bigger(PNODE u,PNODE v)
{
  return (strcmp(u->label,v->label) < 0);
}

int 
is_vertex_sink(PNODE v)
{
  return is_empty_list(v->ledges_out);
}
int 
is_vertex_source(PNODE v)
{
  return is_empty_list(v->ledges_in);
}

int 
is_vertex_group(PNODE v)
{
  if(v->shape == PENTAGON || v->shape == HEXAGON)
    return 1;
  return 0;
}

int 
is_vertex_object(PNODE v)
{
  PEDGE ep = (PEDGE)(v->ledges_in->next->info);

  if (ep && ep->attrib == 'z')
    return 1;
  else
    return 0;
}
  

