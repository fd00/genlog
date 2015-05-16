#ifdef XGR
#include "gr_window.h"

/* redraw_vertex_edges - This function draws a given vertex, all edges in
                         and out from that vertex and all vertices of the
			 other extreme of the edges. 
         inputs  
                 gf - Graph Structure
		 gc - Graphics Context
		 v - Vertex
		 erase - If this is true, erase all vertices. 
         returns
                  none
         notes
                 The vertices can be normal (single line), selected (double
		 line) or highlighted (red color).
*/

void
redraw_vertex_edges(GraphFrame *gf,GC gc,struct pt *v,Boolean erase)
{
  if(!gf->pdraw)
    return;
  if(gf->graph_dir == UNDIRECTED) /* Undirected Graph */
    {
      LNode_type *ne;
      for(ne = v->ledges_in; ne->next != v->ledges_in; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v2 = e->from;
	  GC egc = gc;
	  if(erase) egc = choose_edge_gc(gf,e);

	  if(v2 == v)
	    v2 = e->to;

	  if(e->lwidth > 0)
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 e->lwidth, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineSolid, CapRound, JoinRound);
	    }
	  if(e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
			 e->lwidth, LineOnOffDash, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineOnOffDash, CapRound, JoinRound);
	    }
	  
	  /* draw the edge in the screen */
	  if((e->highlight)&&erase)
	    {
	      XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
		      gf->pix_high, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );
	      XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
		      gf->pix_high, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );
	    }
	  else
	    {
	      XDrawLine( XtDisplay(gf->drawing_a), XtWindow(gf->drawing_a ),
			 egc, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );
	      XDrawLine( XtDisplay(gf->drawing_a), gf->pixmap,
			 egc, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y );

	    }

	  if(e->lwidth > 0 || e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 0, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, 0, 
				 LineSolid, CapRound, JoinRound);
	    }


	  if(gf->show_eweight ) {
	    int x, y;
	    char *elabel = get_edge_display_name(gf,e);
	    x = (v->loc.x + v2->loc.x ) / 2;
	    y = (v->loc.y + v2->loc.y ) / 2;
	    draw_text( gf->drawing_a, x, y-5, egc,
		       gf->font,elabel, gf );
	    if(elabel) free(elabel); elabel = NULL;
	  }
	  {
	    char *v2label = get_vertex_display_name(gf, v2);
	  if((v2->highlight)&&erase)
	    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y,
			gf->vsize, v2label, v2->weight, v2->shape,gf);
	  else
	    {
	      GC gc1 = gc;
	      if(erase) gc1 = choose_gc(gf, v2);
	      draw_vertex(gf->drawing_a, gc1, v2->loc.x, v2->loc.y, gf->vsize, 
			  v2label, v2->weight, v2->shape,gf);
	    }
	    if(v2label) free(v2label); v2label = NULL;
	  }

	}
    }
  else if(gf->graph_dir == DIRECTED) /* Directed Graph */
    {
      LNode_type *ne;
      
      for(ne = v->ledges_in; ne->next != v->ledges_in; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v1 = e->from;
	  GC egc = gc;

	  if(erase) egc = choose_edge_gc(gf, e);

	  if(e->lwidth > 0)
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 e->lwidth, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineSolid, CapRound, JoinRound);
	    }
	  if(e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				e->lwidth, LineOnOffDash, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineOnOffDash, CapRound, JoinRound);
	    }

	  /* draw the edge in the screen */
          {
	 char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
	  if((e->highlight)&&erase) 
	    draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v->loc.x, v->loc.y,
		     .4, 10, .4, 15, gf->pix_high, elabel,gf);
	  else
	    draw_arc(gf->drawing_a, v1->loc.x, v1->loc.y, v->loc.x, v->loc.y,
		     .4, 10, .4, 15, egc, elabel,gf);

	  if(elabel) free(elabel); elabel = NULL;
	  }

	  if(e->lwidth > 0 || e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 0, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, 0, 
				 LineSolid, CapRound, JoinRound);
	    }

	  if(v1->select)
	    {
	      GC gc1 = gc;
	      if(erase) gc1 = choose_gc(gf, v1);
	      draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y,
			  gf->hsize, NULL,NULL,v1->shape,gf);
	    }
	  {
	    char *v1label = get_vertex_display_name(gf, v1);
	  if((v1->highlight)&&erase)
	    draw_vertex(gf->drawing_a, gf->pix_high, v1->loc.x, v1->loc.y, 
			gf->vsize, v1label, v1->weight,v1->shape,gf);
	  else
	    {
	      GC gc1 = gc;
	      if(erase) gc1 = choose_gc(gf,v1); 
	      draw_vertex(gf->drawing_a, gc1, v1->loc.x, v1->loc.y, gf->vsize, 
			  v1label, v1->weight,v1->shape,gf);
	    }
	  if(v1label) free(v1label); v1label = NULL;
	  }
	  
	}

      for(ne = v->ledges_out; ne->next != v->ledges_out; ne = ne->next)
	{
	  struct edge *e = (struct edge *) ne->next->info;
	  struct pt *v2 = e->to;
	  GC egc = gc;
	  if(erase) egc = choose_edge_gc(gf, e);

	  if(e->lwidth > 0)
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 e->lwidth, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineSolid, CapRound, JoinRound);
	    }
	  if(e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				e->lwidth, LineOnOffDash, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, e->lwidth, 
				 LineOnOffDash, CapRound, JoinRound);
	    }
	  /* draw the edge in the screen */
          {
	 char *elabel = gf->show_eweight ? get_edge_display_name(gf,e) : NULL;
	  if((e->highlight)&&erase)
	    draw_arc(gf->drawing_a, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y,
		     .4, 10, .4, 15, gf->pix_high, elabel,gf);
	  else
	    draw_arc(gf->drawing_a, v->loc.x, v->loc.y, v2->loc.x, v2->loc.y,
		     .4, 10, .4, 15, egc, elabel,gf);

	  if(elabel) free(elabel); elabel = NULL;
	  }

	  if(e->lwidth > 0 || e->attrib == 'z')
	    {
	      XSetLineAttributes(XtDisplay(gf->drawing_a), gf->pix_high, 
				 0, LineSolid, CapRound, JoinRound);
	      XSetLineAttributes(XtDisplay(gf->drawing_a), egc, 0, 
				 LineSolid, CapRound, JoinRound);
	    }


	  if(v2->select)
	    {
	      GC gc1 = gc;
	      if(erase) gc1 = choose_gc(gf,v2);
	      draw_vertex(gf->drawing_a, gc1, v2->loc.x, v2->loc.y,
			  gf->hsize, NULL,NULL,v2->shape,gf);
	    }
	  {
	    char *v2label = get_vertex_display_name(gf, v2);
	  if((v2->highlight)&&erase)
	    draw_vertex(gf->drawing_a, gf->pix_high, v2->loc.x, v2->loc.y, 
		       gf->vsize, v2label, v2->weight, v2->shape,gf);
	  else
	    {
	      GC gc1 = gc;
	      if(erase) gc1 = choose_gc(gf,v2);
	      draw_vertex(gf->drawing_a, gc1, v2->loc.x, v2->loc.y, gf->vsize, 
			  v2label, v2->weight, v2->shape,gf);
	    }
	    if(v2label) free(v2label); v2label = NULL;
	  }

	}

    }
  /* Draw the given vertex */
  if(v->select)
    {
      GC gc1 = gc;
      if(erase) gc1 = choose_gc(gf, v);

      draw_vertex(gf->drawing_a, gc1, v->loc.x, v->loc.y,
		gf->hsize, NULL,NULL,v->shape,gf);
    }
  {
    char *vlabel = get_vertex_display_name(gf, v);
  if((v->highlight)&&erase)
    draw_vertex(gf->drawing_a, gf->pix_high, v->loc.x, v->loc.y, gf->vsize, 
		vlabel, v->weight, v->shape,gf);
  else
    {
      GC gc1 = gc;
      if(erase) gc1 = choose_gc(gf, v);

      draw_vertex(gf->drawing_a, gc1, v->loc.x, v->loc.y, gf->vsize, 
		  vlabel, v->weight, v->shape,gf);
    }
    if(vlabel) free(vlabel); vlabel = NULL;
  }
}
/* Draw_all_edges - Draw all edges in the graph. 

         inputs  
                 gf - Graph Structure
         returns
                  none
*/

void 
Draw_all_edges2(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 0;
  int ce = 0;
  Boolean dr = gf->draw;
  if(!gf->pdraw)
    return;
  
  gf->draw = False;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  GC egc = choose_edge_gc(gf,e);
	  ce++;
	  if(!line_is_in_box(e->to->loc.x, e->to->loc.y, 
			    e->from->loc.x, e->from->loc.y,
			    gf->wp, gf->hp))
	    {
	      c++;
	      continue;
	    }
	    
	  if(e->highlight)
	    draw_edge(gf,e, gf->pix_high);
	  else
	    draw_edge(gf,e, egc);
	}
    }

  gf->draw = dr;

  if(gf->draw)
    XCopyArea (XtDisplay(gf->drawing_a), gf->pixmap, XtWindow(gf->drawing_a),
	       gf->gc,0, 0, gf->wp, gf->hp, 0, 0);
  printf("%d edges were not drawn of %d edges\n",c,ce);
}

void 
Draw_all_edges(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 0;
  int ce = 0;
  Boolean dr = gf->draw;
  int onetree = 0;
  if(!gf->pdraw)
    return;
  
  gf->draw = False;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  GC egc;

	  if(e->class == TREE_CLASS)
	  {
	      onetree = 1;
	      continue;
	  }

	  egc =choose_edge_gc(gf,e);
	  ce++;
	  if(!line_is_in_box(e->to->loc.x, e->to->loc.y, 
			    e->from->loc.x, e->from->loc.y,
			    gf->wp, gf->hp))
	    {
	      c++;
	      continue;
	    }
	    
	  if(e->highlight)
	    draw_edge(gf,e, gf->pix_high);
	  else
	    draw_edge(gf,e, egc);
	}
    }

  if(onetree) {
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;
	  GC egc;

	  if(e->class != TREE_CLASS)
	      continue;

	  egc = choose_edge_gc(gf,e);
	  ce++;
	  if(!line_is_in_box(e->to->loc.x, e->to->loc.y, 
			    e->from->loc.x, e->from->loc.y,
			    gf->wp, gf->hp))
	    {
	      c++;
	      continue;
	    }
	    
	  if(e->highlight)
	    draw_edge(gf,e, gf->pix_high);
	  else
	    draw_edge(gf,e, egc);
	}
    }
  }

  gf->draw = dr;

  if(gf->draw)
    XCopyArea (XtDisplay(gf->drawing_a), gf->pixmap, XtWindow(gf->drawing_a),
	       gf->gc,0, 0, gf->wp, gf->hp, 0, 0);
  printf("%d edges were not drawn of %d edges\n",c,ce);
}


/* Draw_all_vertices - Draw all vertices in the graph. 

         inputs  
                 gf - Graph Structure
         returns
                  none
*/
void 
Draw_all_vertices(GraphFrame *gf)
{
  LNode_type *ptr;

  if(!gf->pdraw)
    return;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      char *vlabel = get_vertex_display_name(gf, v);
      GC gc = choose_gc(gf, v);
      if(v->select)
	draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y, gf->hsize, 
		    NULL, NULL, v->shape,gf);
      if(v->highlight)
	draw_vertex(gf->drawing_a, gf->pix_high, v->loc.x, v->loc.y,gf->vsize, 
		    vlabel, v->weight, v->shape,gf);
      else
	draw_vertex(gf->drawing_a, gc, v->loc.x, v->loc.y,gf->vsize, 
		    vlabel, v->weight, v->shape,gf);
      if(vlabel) free(vlabel); vlabel = NULL;
    }
}

/* refresh_graph_window - This is a general function that redraws all elements
                          in the graph in the Drawing Area and Pixmap.

         inputs  
                 gf - Graph Structure
         returns
                  none
         notes
                 R 
*/

void 
refresh_graph_window(GraphFrame *gf)
{
  Dimension w, h;
  Display *d = XtDisplay(gf->drawing_a);
  Window win = XtWindow(gf->drawing_a);

  if(!gf->pdraw)
    return;

  /* get the size of the screen and calc the center */
  XtVaGetValues( gf->drawing_a, XmNwidth, &w,
		 XmNheight, &h, NULL);
    
  /* clear the screen */
  if(gf->draw)
    XFillRectangle( d, win, gf->pix_erase, 0, 0, w,h );
  if(gf->pdraw)
    XFillRectangle( d, gf->pixmap, gf->pix_erase, 0, 0, w, h );
  Draw_all_edges(gf);
  Draw_all_vertices(gf);
  XFlush(d);
}


int 
line_is_in_box(int x1, int y1, int x2, int y2, int w, int h)
{
  if((x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) ||
     (x1 > w && x2 > w) || (y1 > h && y2 > h))
    return 0;
  return 1;
}

int 
point_is_in_box(int x1, int y1, int w, int h)
{
  if(x1 > 0 && x1 < w && y1 > 0 && y1 < h)
    return 1;
  return 0;
}

void
move_vertex_draw(GraphFrame *cgr,struct pt *ov,struct pr_pos loc)
{
  redraw_vertex_edges(cgr,cgr->pix_erase,ov,False);
  ov->loc.x = loc.x;
  ov->loc.y = loc.y;
  redraw_vertex_edges(cgr,cgr->gc,ov,True);
}

#endif



