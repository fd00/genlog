#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

char *
get_label_edge_short_name(GraphFrame *gf,PEDGE e);

/* draw_arc - Draw an arc between two vertices (Directed Edges). The direction
              of the edge is shown as an arrow. 
         inputs  
	        widget - Widget of the Graph. 
		x1, y1, x2, y2 - Points of the vertices
		gf - Graph Structure
		weight - weight of the edge
		op - Graphic context to be used for the arc. 
         returns
                  none
         side effects
                  none
*/
#ifdef XGR
void
draw_arc(
Widget	widget,
int	x1, int y1, int x2, int y2,
double	theta,
int	n,
double	tharrow,
int	larrow,
GC	op,
char *	weight,
GraphFrame *gf)
{
  Display *dis = XtDisplay( widget );
  Window w = XtWindow( widget );
  Pixmap pixmap = gf->pixmap;
  
  double lx1 = (long)x1;
  double ly1 = -(long)y1;
  double lx2 = (long)x2;
  double ly2 = -(long)y2;
  
  double lxm = (lx1 + lx2)/2;
  double lym = (ly1 + ly2)/2;
  
  double t1 = atan2(ly2-ly1, lx2-lx1);
  double st1 = sin(t1);
  double ct1 = cos(t1);
  double alpha = 0.04;
  double ls = sqrt((lx2-lx1)*(lx2-lx1)+(ly2-ly1)*(ly2-ly1));
  double m = alpha *ls;
  
  double ly3 = lym + m * ct1;
  double lx3 = lxm - m * st1;

  double c = 2*(lx3 * (ly2-ly1) + ly3 *(lx1 -lx2) + lx2 *ly1 - ly2 *lx1);

  double a = ((lx1*lx1 + ly1*ly1) * (ly3-ly2) +
	    (lx2*lx2 + ly2*ly2) * (ly1-ly3) +
	    (lx3*lx3 + ly3*ly3) * (ly2-ly1)) / c;
  
  double b = ((lx1*lx1 +ly1*ly1) *( lx2 - lx3) +
	    (lx2*lx2 +ly2*ly2) *( lx3 - lx1) +
	    (lx3*lx3 +ly3*ly3) *( lx1 - lx2)) / c;

  double r = sqrt((ly1 - b)* (ly1 - b) + (lx1 - a)*(lx1 - a));
  
  int x = (int)(a - r);
  int y = (int)(- b - r);
  
  double t3 = atan2(ly1-b, lx1-a);
  double t4 = atan2(ly2-b, lx2-a);

  int angle1 = (int)(t3 * 3666.9299); /* 180 * 64 / PI */
  int angle2 = (int)(t4 * 3666.9299); /* 180 * 64 / PI */
  unsigned int dia = (unsigned int)2*r;
  int angled = angle1 - angle2;
  if(angled < 0)
    angled += 23040;
  /*printf("angle1 %d, angle2 %d, dia %d, angled %d\n", angle1, angle2, dia, angled);*/

  if(dia < MAX_UN_INT)
    {
      if(gf->draw)
	XDrawArc(dis, w, op, x, y, dia,dia, angle2, angled);
      if(gf->pdraw)
	XDrawArc(dis, pixmap, op, x, y,dia,dia, angle2, angled);
    }
  else
    {
      if(x1 < MAX_S_INT && x1 > MIN_S_INT &&
	 x2 < MAX_S_INT && x2 > MIN_S_INT &&
	 y1 < MAX_S_INT && y1 > MIN_S_INT &&
	 y2 < MAX_S_INT && y2 > MIN_S_INT)
	{
	  if(gf->draw)
	    XDrawLine(dis, w, op, x1,y1,x2,y2);
	  if(gf->pdraw)
	    XDrawLine(dis, pixmap, op, x1,y1,x2,y2);
	  lx3 = lxm;
	  ly3 = lym;
	}
      else
	{
	  /* Code must be written afterwards */
	}

    }
  /* Drawing arrow */
#define HARROW 8
   {
    double ha = (double)(HARROW * (gf->scale + 10) / 10); /* scale_dim */
    double mha = ha/3;
    double lx4 = lx3 - ha * ct1;
    double ly4 = ly3 - ha * st1;
    double lx3a = lx4 + mha * st1; /*cos(t1 - 90);*/
    double ly3a = ly4 - mha * ct1; /*sin(t1 - 90);*/
    double lx3b = lx4 - mha * st1; /*cos(t1 + 90);*/
    double ly3b = ly4 + mha * ct1; /*sin(t1 + 90);*/

    XPoint points[3];
    
    points[0].x = (int) lx3b;
    points[0].y = -(int) ly3b;
    points[1].x = (int) lx3;
    points[1].y = -(int) ly3;
    points[2].x = (int) lx3a;
    points[2].y = -(int) ly3a;
    
    if(gf->draw)
      XDrawLines( dis, w, op, points,3,CoordModeOrigin);
    if(gf->pdraw)
      XDrawLines( dis, pixmap, op,  points,3,CoordModeOrigin);

     if( weight )
       draw_text( widget, lx3, -ly3 + (int)(ha*2), op, gf->font, weight,gf );
    
   }

  XFlush(dis);
}

void
draw_arc3(
Widget	widget,
int	x1, int y1, int x2, int y2,
double	theta,
int	n,
double	tharrow,
int	larrow,
GC	op,
char *	weight,
GraphFrame *gf)
{
  Display *dis = XtDisplay( widget );
  Window w = XtWindow( widget );
  Pixmap pixmap = gf->pixmap;

  double a = ((double)(y2*y2 - y1*y1))/(double)(x1*x1*y2*y2-x2*x2*y1*y1);
  double b = ((double)(x2*x2 - x1*x1))/(double)(-x1*x1*y2*y2+x2*x2*y1*y1);
  a = 2/sqrt(a);
  b = 2/sqrt(b);
  {
  double t1 = - atan2((double)y2,(double)x2);
  double t2 = - atan2((double)y1,(double)x1);
  double td = t1-t2;
  int angle1 = (int)(t1 * 3666.9299); /* 180 * 64 / PI */
  int angle2 = (int)(td * 3666.9299); /* 180 * 64 / PI */
  int x = (int)(a/2);
  int y = (int)(b/2);
  int wi = (int)a;
  int he = (int)b;
  printf("x1 = %d, y1 = %d, x2 = %d, y2 = %d\n", x1,y1,x2,y2);
  printf("x = %d, y = %d, wi = %d, he = %d, t1=%d, td=%d\n", x,y,wi,he,angle1/64 , angle2 /64);
  if(gf->draw)
    XDrawArc(dis, w, op, x, y, wi,he, angle1, angle2);
  if(gf->pdraw)
    XDrawArc(dis, pixmap,op, x, y, wi,he, angle1, angle2);
  }
}
void
draw_arc2(
Widget	widget,
int	x1, int y1, int x2, int y2,
double	theta,
int	n,
double	tharrow,
int	larrow,
GC	op,
char *	weight,
GraphFrame *gf)
{
	int	dx, dy, d;
	double	phi, psi, th;
	int	p1x, p1y, p2x, p2y;
	double	l, len;
	int	i = n;
	Display	*dis = XtDisplay( widget );
	Window	w = XtWindow( widget );
	Pixmap pixmap = gf->pixmap;

	/* theta is the total angle that the arc traverses */
	
	theta = 0.07; /* this is temporary value */
	larrow = 5;

	/* find the distance between the two points */
	dx = x2 - x1;
	dy = y2 - y1;
	d = (int) sqrt( (double)(dx*dx + dy*dy) );

	/* if same points, don't draw anything.  Return */
	if( d == 0 )
		return;
	
	/* phi is the angle of vector p1-p2 wrt to the positive x-axis */
	phi = acos( (double)dx / d );
	if( dy < 0 )
		phi = -phi;
	
	/* arc has to return to same angle theta.  The arc has to cover
	 * 2*theta angle in n steps.  - because it's going the other direction
	 */
	th = -2*theta/n;

	/* psi is the initial angle of the arc.  It is the angle of the vector
	 * minus some weird angle calculation (it's strange, but it works!)
	 */
	psi = phi - th * (n-1)/2;

	/* initialize the first point of the st. lines drawn */
	p1x = x1;
	p1y = y1;
	/* here we cheat.  The length of each segment of the arc is not the
	 * same.  We divide the vector into equal segments so as the 
	 * projections of the segments will be the same.  l is not the length
	 * of each one of those projections
	 */
	l = d / n;

	/* we need to draw the direction arrown at the middle */
	n /= 2;
	while( i-- ) {
		/* fint the actual length of the segment */
		len = l / cos( phi - psi );

		/* calculate the second point */
		p2x = p1x + cos( psi ) * len;
		p2y = p1y + sin( psi ) * len;

		/* draw the vector */
		if(gf->draw)
		  XDrawLine( dis, w, op, p1x, p1y, p2x, p2y );
		if(gf->pdraw)
		  XDrawLine( dis, pixmap, op, p1x, p1y, p2x, p2y );

		/* if we are in the middle, draw the arrow */
		if( i == n ) {
		        if(gf->draw)
			  {
			  XDrawLine( dis, w, op, p2x, p2y,
				 (int)(p2x + cos(psi - PI+tharrow) * larrow),
				(int)(p2y + sin(psi - PI+tharrow) * larrow) );
			  XDrawLine( dis, w, op, p2x, p2y,
				(int)(p2x + cos(psi + PI-tharrow) * larrow),
				(int)(p2y + sin(psi + PI-tharrow) * larrow) );
			  }
			if(gf->pdraw)
			  {
			    XDrawLine( dis, pixmap,op, p2x, p2y,
				       (int)(p2x + cos(psi - PI+tharrow) * larrow),
				       (int)(p2y + sin(psi - PI+tharrow) * larrow) );
			    XDrawLine( dis, pixmap, op, p2x, p2y,
				       (int)(p2x + cos(psi + PI-tharrow) * larrow),
				       (int)(p2y + sin(psi + PI-tharrow) * larrow) );
			  }
			if( weight ) {
				draw_text( widget, p2x, p2y - 10,
					op, gf->font, 
					   weight,gf );
			}
		}

		/* set the first point to be the second */
		p1x = p2x;
		p1y = p2y;

		/* decrement the angle */
		psi += th;
	}
}
#endif
/* draw_edge - Draw an edge between two vertices. 

         inputs  
                 gf - Graph Structure
		 e - Edge to be drawn
		 gc - Graphic Context to be used for the edge.
         returns
                  none
         side effects
                  none
*/

#ifdef XGR
void 
draw_edge(GraphFrame *gf, struct edge *e, GC gc)
{
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
  
  if(e->lwidth > 0)
        XSetLineAttributes(XtDisplay(gf->drawing_a), gc, e->lwidth, LineSolid,
			 CapRound, JoinRound);
  if(e->attrib == 'z')
        XSetLineAttributes(XtDisplay(gf->drawing_a), gc, e->lwidth, 
			   LineOnOffDash, CapRound, JoinRound);
      
  if(gf->graph_dir == UNDIRECTED)
    {
      if(gf->draw)
	XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
		   gc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );
      if(gf->pdraw)
	XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
		   gc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );
      
      
      if(gf->show_eweight ) {
	int x, y;
	char *elabel = get_edge_display_name(gf,e);
	x = (v1->loc.x + v2->loc.x ) / 2;
	y = (v1->loc.y + v2->loc.y ) / 2;
	draw_text( gf->drawing_a, x, y-5, gc,
		   gf->font,elabel, gf );
	free(elabel); elabel = NULL;
      }
    }
  else if(gf->graph_dir == DIRECTED)
    {
      char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
      /* draw the edge in the screen */
      draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y,
      .4, 10, .4, 15, gc,elabel,gf);
      if(elabel) free(elabel); elabel = NULL;
    }

  if(e->lwidth > 0 || e->attrib == 'z')
    XSetLineAttributes(XtDisplay(gf->drawing_a), gc, 0, LineSolid,
		       CapRound, JoinRound);

} 
#endif
/* update_edge - The edge is drawn in the window and it is included in the
                 list of the edges in and out of the extreme vertices. 
                       I
         inputs  
                 gf - Graph Structure
		 e - Edge
         returns
                  none
         side effects
	          none
*/


void
update_edge(GraphFrame *gf,struct edge *e)
{
  struct pt *v1 = e->from;
  struct pt *v2 = e->to;
#ifdef XGR
  GC gc = choose_edge_gc(gf,e);
#endif

  if(gf->graph_dir == UNDIRECTED)
    {
      LNode_type *ptr;
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v1->ledges_in,ptr);
      ptr= create_new_node_llist((char *)e);
      Insert_linked_list(v2->ledges_in,ptr);
#ifdef XGR
      if(gf->draw)
	XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
		   gc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );
      if(gf->pdraw)
	XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
		   gc, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y );

      if(gf->show_eweight ) {
	char *elabel = get_edge_display_name(gf,e);
	int x, y;
	x = (v1->loc.x + v2->loc.x ) / 2;
	y = (v1->loc.y + v2->loc.y ) / 2;
	draw_text( gf->drawing_a, x, y-5, gc,
		   gf->font,elabel, gf );
	if(elabel) free(elabel); elabel = NULL;
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
      {
      char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
      /* draw the edge in the screen */
      draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v2->loc.x, v2->loc.y,
	       .4, 10, .4, 15, gc,elabel,gf);
      if(elabel) free(elabel); elabel = NULL;
      }
#endif
      
    }

  /* and then draw the boxes at the end */
#ifdef XGR
  {
    char *v1label = get_vertex_display_name(gf, v1);
    char *v2label = get_vertex_display_name(gf, v2);
    GC gc1 = choose_gc(gf,v1);
    GC gc2 = choose_gc(gf,v2);
  draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,
	       gf->vsize, v1label, v1->weight,v1->shape,gf);
  draw_vertex(gf->drawing_a,gc2, v2->loc.x, v2->loc.y,
	      gf->vsize,v2label, v2->weight,v2->shape,gf);
  if(v1label) free(v1label); v1label = NULL; 
  if(v2label) free(v2label); v2label = NULL;
  }
#endif
}
/* copy_edge - A edge is copied in a new edge.

         inputs  
                 e - edge to be copied
		 v1, v2 - extreme vertices for the new edge
		 gf - Graph Structure where the new edge is located. 
         returns
                  New edge
         side effects
                  none
*/

struct edge * 
copy_edge(struct edge * e, struct pt *v1, struct pt *v2, GraphFrame *gf)
{
  struct edge *ne;

  /* get space for the edge */
  ne = (struct edge *) calloc( 1, sizeof(struct edge) );
  if( ne == NULL ) {
      fprintf(stderr,"Out of Memory copy_edge\n");
      abort();
  }
  /* fill it up */
  ne->from = v1;
  ne->to = v2;
  if(e->label)
      GRStrAllocCopy(ne->label,e->label);
  if(e->weight)
      GRStrAllocCopy(ne->weight,e->weight);
  ne->highlight = False;
  ne->select = False;
  ne->lwidth = 0;
  ne->color = e->color;
  ne->attrib = e->attrib;
  ne->class = e->class;
  return ne;
}
/* create_edge_memory - Create new edge structure and save the two extreme
                        vertices.

         inputs  
                 gf - Graph Structure. 
		 v1, v2 - Two extreme vertices of the new edge.
         returns
                  New edge
         side effects
                  none
         notes
                 Edge Weight is not set.

*/

struct edge *
create_edge_memory(GraphFrame *gf, struct pt *v1,struct pt *v2)
{
  struct edge *e = (struct edge *) calloc( 1, sizeof(struct edge) );

  if( e == NULL )
  {
      fprintf(stderr,"Out of Memory create_edge_memory\n");
      abort();
  }
  
  /* fill it up */
  e->from = v1;
  e->to = v2;
  e->label = NULL;
  e->weight = malloc(2) ; 
  if(e->weight)
      strcpy(e->weight,"0");
  else {
      fprintf(stderr,"Out of Memory create_edge_memory\n");
      abort(); 
  }
  e->select = False;
  e->lwidth = 0;
  e->color = GR_BG;
  e->select = False;
  e->attrib = '\0';
  e->highlight = False;
  e->class = NO_EDGE_CLASS;
  return e;
}
/* delete_edge_info - Destructor of the edge structure. 

         inputs  
                 e - edge to be deleted.
         returns
                  none
         side effects
                  none
         notes
                 Only weight is deallocated. 

*/

void
delete_edge_info(struct edge *e)
{
    if(e->weight) {
	free(e->weight); e->weight = NULL;
    }
    if(e->label) {
	free(e->label); e->label = NULL;
    }
}
/* is_edge_highlighted - See if the edge is highlighted or not

         inputs  
                 e - Edge
         returns
                  True, if the edge is highlighted
         side effects
                  none

*/

Boolean
is_edge_highlighted(struct edge *e)
{
  return (e->highlight);
}

Boolean
is_edge_selected(struct edge *e)
{
  return (e->select);
}

char *
get_edge_name(PEDGE e)
{
  char *name = NULL;
  if(e && e->to && e->from && e->to->label && e->from->label)
  {
    name = malloc(strlen(e->to->label)+ strlen(e->from->label)+2);
    if(!name)
    {
      fprintf(stderr,"Out of Memory get_edge_name\n");
      abort(); 
    }
    /*sprintf(name,"%s_%s", e->from->label, e->to->label);*/
    sprintf(name,"%s|%s", e->from->label, e->to->label);
  }
  return name;
}

char *
get_reverse_edge_name(PEDGE e)
{
  char *name = NULL;
  if(e && e->to && e->from && e->to->label && e->from->label)
  {
    name = malloc(strlen(e->to->label)+ strlen(e->from->label)+2);
    if(!name)
    {
      fprintf(stderr,"Out of Memory get_edge_name\n");
      abort(); 
    }
    /*sprintf(name,"%s_%s", e->from->label, e->to->label);*/
    sprintf(name,"%s|%s", e->to->label, e->from->label);
  }
  return name;
}

short
get_edge_color(struct edge *e)
{
  return (e->color);
}

void
set_edge_color(struct edge *e, short col)
{
  e->color = col;
}
char
get_edge_attribute(struct edge *e)
{
  return (e->attrib);
}

void
set_edge_attrib(struct edge *e, char c)
{
  e->attrib = c;
}

char
get_char_edge_attribute(char *stratr)
{
  if(!strcasecmp(stratr,"next"))
    return 'n';
  else if(!strcasecmp(stratr,"previous"))
    return 'p';
  else if(!strcasecmp(stratr,"contents"))
    return 'c';
  else if(!strcasecmp(stratr,"stylesheet"))
    return 's';
  else if(!strcasecmp(stratr,"up"))
    return 'u';
  else if(!strcasecmp(stratr,"made"))
    return 'm';
  else if(!strcasecmp(stratr,"index"))
    return 'i';
  else if(!strcasecmp(stratr,"top"))
    return 't';
  else if(!strcasecmp(stratr,"image"))
    return 'z';
  else /*if(!strcasecmp(stratr,""))*/
    return '\0';
}
char *
get_string_edge_attribute(struct edge *e)
{
  char *str = NULL;
  switch(e->attrib)
    {
    case 'n' :
      GRStrAllocCopy(str,"next");
      return str;
    case 'p' :
      GRStrAllocCopy(str,"previous");
      return str;
    case 'c' :
      GRStrAllocCopy(str,"contents");
      return str;
    case 's' :
      GRStrAllocCopy(str,"stylesheet");
      return str;
    case 'u' :
      GRStrAllocCopy(str,"up");
      return str;
    case 'm' :
      GRStrAllocCopy(str,"made");
      return str;
    case 'i':
      GRStrAllocCopy(str,"index");
      return str;
    case 't':
      GRStrAllocCopy(str,"top");
      return str;
    case 'z':
      GRStrAllocCopy(str,"image");
      return str;
    case '\0' :
      GRStrAllocCopy(str,"no attribute");
      return str;
    }
  GRStrAllocCopy(str,"unknown");
  return str;
}
short
get_edge_class(struct edge *e)
{
  return (e->class);
}

void
set_edge_class(struct edge *e, short sc)
{
  e->class = sc;
}

char *
get_string_edge_class(struct edge *e)
{
  char *str = NULL;
  switch(e->class)
    {
    case NO_EDGE_CLASS :
      GRStrAllocCopy(str,"no class");
      return str;
    case TREE_CLASS :
      GRStrAllocCopy(str,"tree");
      return str;
    case BACK_CLASS :
      GRStrAllocCopy(str,"backward");
      return str;
    case FORWARD_CLASS :
      GRStrAllocCopy(str,"forward");
      return str;
    }
  GRStrAllocCopy(str,"unknown");
  return str;
}

/* get_weight_edge - Get weight of the edge

         inputs  
                 e - Edge
         returns
                  Weight of the edge
         side effects
                  none

*/

char *
get_weight_edge(struct edge *e)
{
  return (e->weight);
}
/* get_label_edge - Get label of the edge

         inputs  
                 e - Edge
         returns
                  Label of the edge
         side effects
                  none

*/

char *
get_label_edge(struct edge *e)
{
  return (e->label);
}

/* 
   get_from_vertex - Get vertex where the edge start from .

         inputs  
                 e - Edge
         returns
                  Starting Vertex  of the edge
         side effects
                  none

*/
struct pt *
get_from_vertex(struct edge *e)
{
  return (e->from);
}
/* 
   get_to_vertex - Get vertex where the edge ends .

         inputs  
                 e - Edge
         returns
                  Ending Vertex  of the edge
         side effects
                  none

*/
struct pt *
get_to_vertex(struct edge *e)
{
  return (e->to);
}
/* change_highlight_edge - Change highlight of the edge

         inputs  
                 e - Edge
		 high - Highlight value of the edge
         returns
                  none
         side effects
                  none
         notes
                 The edge should be redrawn after this. 

*/

void
change_highlight_edge(struct edge *e, Boolean high)
{
  e->highlight = high;
}

/* change_weight_edge - Change Weight of the edge

         inputs  
                 e - Edge
		 weight - New weight of the edge
         returns
                  none
         side effects
                  none
         notes
                 The edge should be redrawn after this. 

*/
void 
change_weight_edge(struct edge *e, char *weight)
{
    if(weight) {
	if(e->weight) {
	    free(e->weight); e->weight=NULL;
	}
	GRStrAllocCopy(e->weight,weight);
    }
}
void 
change_label_edge(struct edge *e, char *label)
{
    if(label) {
	if(e->label) {
	    free(e->label); e->label=NULL;
	}
	GRStrAllocCopy(e->label,label);
    }
}

void
changing_select_edge(GraphFrame *gf,struct edge *e, Boolean sel)
{
    if(!gf || !e)
	return;
  if(sel == e->select)
    return;
  e->select = sel;

#ifdef XGR
  if(sel)
    e->lwidth = 2;
  else
    {
      draw_edge(gf,e,gf->pix_erase);
      e->lwidth = 0;
    }
  redraw_edge(gf,e);
#endif
}

void
set_select_edge(struct edge *e, Boolean sel)
{
  e->select = sel;
}

void 
select_edge_selecting_vertices(GraphFrame *gf, struct edge *e)
{
  struct pt *v1 = e->to;
  struct pt *v2 = e->from;
  
  changing_select_edge(gf,e, True);

  if(!v1->select)
   select_vertex(v1, gf);
  if(!v2->select)
   select_vertex(v2, gf);

}

int 
is_weight_lower(PEDGE e, PEDGE f)
{
  int ewi = atoi(e->weight);
  int fei = atoi(f->weight);
  return (ewi < fei);
}

int 
is_weight_bigger(PEDGE e, PEDGE f)
{
  int ewi = atoi(e->weight);
  int fei = atoi(f->weight);
  return (ewi > fei);
}
/* highlighting_edge - Hightlight edge and redraw the two extreme vertices.

         inputs  
                 gf - Graph Structure.
		 e - Edge
		 gc - Graphics Context to be use for the edge. 
         returns
                  none
         side effects
                  none
*/
#ifdef XGR
void
highlighting_edge(GraphFrame *gf, struct edge *e, GC gc)
{
  struct pt *v1 = e->to;
  struct pt *v2 = e->from;
  char *v1label = get_vertex_display_name(gf, v1);
  char *v2label = get_vertex_display_name(gf, v2);

  draw_edge(gf,e,gc);

  if(v1->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, v1->loc.y, gf->vsize, 
		v1label, v1->weight, v1->shape,gf);
  else
    {
      GC gc = choose_gc(gf,v1);
    draw_vertex(gf->drawing_a, gc, v1->loc.x, v1->loc.y, gf->vsize, 
		v1label, v1->weight, v1->shape,gf);
    }
  if(v2->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y, gf->vsize, 
		v2label, v2->weight, v2->shape,gf);
  else
    {
      GC gc = choose_gc(gf,v2);
    draw_vertex(gf->drawing_a, gc, v2->loc.x, v2->loc.y, gf->vsize, 
		v2label, v2->weight, v2->shape,gf);
    }
  if(v1label) free(v1label); v1label=NULL; if(v2label) free(v2label); v2label=NULL;

}
#endif
/* redraw_edge - General function to redraw an edge. It considers if the 
                 edge is highlighted or not and it redraws the two extreme
		 vertices.
                       I
         inputs  
                 gf - Graph Structure
		 e - Edge
         returns
                  none
         side effects
                  none
*/
#ifdef XGR
void
redraw_edge(GraphFrame *gf,struct edge *e)
{
  struct pt *v1 = e->to;
  struct pt *v2 = e->from;
  char *v1label = get_vertex_display_name(gf, v1);
  char *v2label = get_vertex_display_name(gf, v2);

  if(e->highlight)
    draw_edge(gf,e,gf->pix_high);
  else
    {
      GC egc = choose_edge_gc(gf,e);
      draw_edge(gf,e,egc);
    }
  
  if(v1->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, 
		v1->loc.y, gf->vsize, v1label, v1->weight, v1->shape,gf);      
  else
    {
      GC gc = choose_gc(gf,v1);
      draw_vertex(gf->drawing_a, gc, v1->loc.x, v1->loc.y, 
		  gf->vsize, v1label, v1->weight, v1->shape,gf);
    }

  if(v2->highlight)
    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, 
		v2->loc.y, gf->vsize, v2label, v2->weight, v2->shape,gf);      
  else
    {
      GC gc = choose_gc(gf,v2);
    draw_vertex(gf->drawing_a, gc, v2->loc.x, v2->loc.y, 
		gf->vsize, v2label, v2->weight, v2->shape,gf);
    }
  if(v1label) free(v1label); v1label=NULL; 
  if(v2label) free(v2label); v2label=NULL;
}

GC 
choose_edge_gc(GraphFrame *gf, struct edge *e)
{
  GC gc = gf->gc;
  if(e->color != GR_BG)
    {
      change_gc(gf->drawing_a, gf->pix_color, e->color);
      gc = gf->pix_color;
    }
  return gc;
}
#endif

long
get_weight_node_to(PEDGE e)
{
    if(e->to && e->to->weight)
	return (atoi(e->to->weight));
    else
	return 0;
}

long
get_long_weight_edge(PEDGE e)
{
    if(e->weight)
	return (atoi(e->weight));
    else
	return 0;
}

long
get_simple_weight_edge(PEDGE e)
{
  return (1);
}

long
get_weight_out_degree_node_to(PEDGE e)
{
  return ((long)count_llist(e->to->ledges_out));
}


char *
get_edge_display_name(GraphFrame *gf, PEDGE e)
{
  char *name= NULL;

  if(gf->emode == DIS_ENO_LABEL)
      return NULL;

  if(gf->drawhighlight && !e->highlight)
      return NULL;

  if(gf->emode == DIS_ENUM)
    {
      return NULL;
    }
  else if(gf->emode == DIS_ELABEL)
      GRStrAllocCopy(name,e->label); 
  else if(gf->emode == DIS_ESHORT_LABEL)
    {
	name = get_label_edge_short_name(gf,e);
    }
  else if(gf->emode == DIS_EWEIGHT)
      GRStrAllocCopy(name,e->weight); 
  return name;
}

char *
get_label_edge_short_name(GraphFrame *gf,PEDGE e)
{
    char *label = NULL;
    char *ptr = NULL;
    char *slabel = NULL;
    label = e->label;

    if(!label || label[0]=='\0')
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
