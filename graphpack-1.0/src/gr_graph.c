#include "gr_graph_fun.h"
#include "gr_graph_p.h"

/* initialize_graph_hash_tables - Initializes the Hash Tables for Vertices, 
                                  Name of the vertices and Edges.
         inputs  
                 cgr - Graph Structure
         returns
                  none
*/
void 
initialize_graph_hash_tables(GraphFrame *cgr)
{
  init_hash_table(cgr->HV);
  init_hash_table(cgr->HVname);
  init_hash_table(cgr->Hedge);
}
/* create_current_graph - Allocates memory for the new Graph Structure and
                          initializes all Hash Tables and Lists of the Graph.
         inputs  
                 none
         returns
                  New Graph Structure
	 notes
	         This function does not set any value in the Graph Structure.
*/

GraphFrame *
create_current_graph(void)
{
  GraphFrame *cgr = (GraphFrame *) calloc(1,sizeof(GraphFrame));
  if(!cgr) {
      fprintf(stderr,"Out of Memory create_current_graph\n");
      abort();
  }      
  cgr->nver = NULL;
  initialize_graph_hash_tables(cgr);
  cgr->list_vertices = init_linked_list();
  cgr->list_sel_vertex = init_linked_list();
  cgr->list_high_vertex = init_linked_list();
  if(!cgr->lobjects)
      cgr->lobjects = init_linked_list();
  if(!cgr->attrib_name)
      cgr->attrib_name = init_linked_list();
  
      /*cgr->list_visited_vertex = init_linked_list();
      cgr->the_cycle = init_linked_list();
      cgr->the_rest =init_linked_list();
      cgr->level_zero = init_linked_list();*/
  return cgr;
}

void
init_basic_graph(GraphFrame *gf)
{
  set_dimensions(gf, GR_WIDTH, GR_HEIGHT);
  set_show_vertex_weight(gf, False);
  set_show_edge_weight(gf, False);
  set_shape(gf, BOX);
  set_dim_vertex(gf,BSIDE,HSIDE);
  set_scale(gf, 0);
  set_flag_draw(gf, True);
  set_num_vertex(gf, 0);
  set_num_ver_group(gf, 0);
  set_count_vertex(gf,0);
  set_method_tree(gf,BFS_TREE);
  set_graph_dir(gf, NONE);
  set_graphic_flag(gf, NONE);
}

/* set_drawing_area - Set the Widget of the Drawing Area to be used to display
                      the graph.

         inputs  
                 cgr - Graph Structure
		 w - Widget of the Drawing Area
         returns
                  none

*/
#ifdef XGR
void
set_drawing_area(GraphFrame *cgr, Widget w)
{
  cgr->drawing_a = w;
}
#endif

/* set_dimensions - Set Dimensions of the Drawing Area. 

         inputs  
                 cgr - Graph Structure
		 w, h - Dimensions of the Drawing Area. 
         returns
                  none
*/

void
set_dimensions(GraphFrame * cgr, Dimension w, Dimension h)
{
  cgr->w = w;
  cgr->h = h;
}

void
set_pixmap_dimensions(GraphFrame * cgr, Dimension w, Dimension h)
{
  cgr->wp = w;
  cgr->hp = h;
}
/* set_pixmap - Set Pixmap to be backup of the drawing area

         inputs  
                 cgr - Graph Structure
		 pixmap - Pixmap 
         returns
                  none
*/
#ifdef XGR
void
set_pixmap(GraphFrame * cgr, Pixmap pixmap)
{
  cgr->pixmap = pixmap;
}
#endif
/* set_font - Font to be used for the graph. 

         inputs  
                 cgr - Graph Structure
		 font - Font
         returns
                  none
*/
#ifdef XGR
void
set_font(GraphFrame * cgr, Font font)
{
  cgr->font = font;
}
#endif
/* set_show_vertex_weight - Set the value of the flag to show weight of the
                            vertices or not.

         inputs  
                 cgr - Graph Structure
		 val - Boolean value.
         returns
                  none
*/
void
set_show_vertex_weight(GraphFrame * cgr, Boolean val)
{
  cgr->show_vweight = val;
}
/* set_show_edge_weight - Set the value of the flag to show weight of the
                            edges or not.

         inputs  
                 cgr - Graph Structure
		 val - Boolean value.
         returns
                  none
*/
void
set_show_edge_weight(GraphFrame * cgr, Boolean val)
{
  cgr->show_eweight = val;
}

void
set_cost_function(GraphFrame * cgr, long (*val)())
{
  cgr->cost_weight = val;
}

/* set_shape - Set shape of the vertices of the graph.

         inputs  
                 cgr - Graph Structure
		 shape - Shape of the vertices. 
         returns
                  none
*/
void
set_shape(GraphFrame * cgr, char shape)
{
  cgr->shape = shape;
}
/* set_dim_vertex - Set Dimension of the vertices of the Graph.

         inputs  
                 cgr - Graph Structure
		 vsize, hsize - Dimension of the vertices. 
         returns
                  none
*/
void
set_dim_vertex(GraphFrame *cgr,short vsize,short hsize)
{
  cgr->vsize = vsize;
  cgr->hsize = hsize;
}
/* set_scale - Set Scale value (-4 .. 10) for the Graph. This value is used
               to zoom the graph.
         inputs  
                 cgr - Graph Structure
		 scale - Scale value of the Graph.
         returns
                  none
*/
void
set_scale(GraphFrame *cgr, short scale)
{
  cgr->scale = scale;
}
/* set_flag_draw - Set the flag to draw or not in the Drawing Area. If the
                   value is false, the graph is just drawn in the pixmap. 
         inputs  
                 cgr - Graph Structure
		 flag - Boolean value
         returns
                  none
*/
void
set_flag_draw(GraphFrame *cgr, Boolean flag)
{
  cgr->draw = flag;
}

Boolean
get_flag_draw(GraphFrame *cgr)
{
  return cgr->draw;
}
/* set_flag_draw - Set the flag to draw or not in the Drawing Area. If the
                   value is false, the graph is just drawn in the pixmap. 
         inputs  
                 cgr - Graph Structure
		 flag - Boolean value
         returns
                  none
*/
void
set_flag_pixdraw(GraphFrame *cgr, Boolean flag)
{
  cgr->pdraw = flag;
}

Boolean
get_flag_pixdraw(GraphFrame *cgr)
{
  return cgr->pdraw;
}

void
set_flag_draw_highlight(GraphFrame *cgr, Boolean flag)
{
  cgr->drawhighlight = flag;
}

Boolean
get_flag_draw_highlight(GraphFrame *cgr)
{
  return cgr->drawhighlight;
}

void
set_num_clusters(GraphFrame *cgr, int nclusters)
{
  cgr->num_clusters = nclusters;
}
int
get_num_clusters(GraphFrame *cgr)
{
    return cgr->num_clusters;
}

void
set_num_components(GraphFrame *cgr, int ncomponent)
{
  cgr->num_components = ncomponent;
}
int
get_num_components(GraphFrame *cgr)
{
    return cgr->num_components;
}

/* set_num_vertex - This sets the value of the last number to be used as
                    a name of a vertex. 
         inputs  
                 cgr - Graph Structure
		 nvertex - number of a vertex
         returns
                  none
*/
void
set_num_vertex(GraphFrame *cgr, int nvertex)
{
  cgr->nvertex = nvertex;
}
/* set_num_ver_group - This sets the value of the last number to be used for
                       a name of a group vertex. 
         inputs  
                 cgr - Graph Structure
		 nvgroup - Number of a Group vertex. 
         returns
                  none
*/
void
set_num_ver_group(GraphFrame *cgr, int nvgroup)
{
  cgr->nvgroup = nvgroup;
}

/* set_count_vertex - Set the Counter of the number of vertices of the Graph

         inputs  
                 cgr - Graph Structure
		 count_v - Number of the initial value of the counter 
		           of vertices
         returns
                  none
*/
void
set_count_vertex(GraphFrame *cgr,int count_v)
{
  cgr->count_vertex = count_v;
}

int
get_count_vertex(GraphFrame *cgr)
{
  return cgr->count_vertex;
}

/* set_graph_dir - Set Direction of the Graph (Directed or Undirected)

         inputs  
                 cgr - Graph Structure
		 gdir - Graph Direction
         returns
                  none
*/
void
set_graph_dir(GraphFrame *cgr, int gdir)
{
  cgr->graph_dir = gdir;
}
/* set_graphic_flag - Set Graphics flag in the Graph (ON or OFF)

         inputs  
                 cgr - Graph Structure
		 gdir - Graph Direction
         returns
                  none
*/
void
set_graphic_flag(GraphFrame *cgr, int graphic)
{
  cgr->graphic = graphic;
}
/* set_current_group_vertex - Set current group Vertex of the Graph.

         inputs  
                 cgr - Graph Structure
		 cgv - Current Group Vertex.
         returns
                  none
*/
void
set_current_group_vertex(GraphFrame *cgr, PNODE cgv)
{
  cgr->cgv = cgv;
}
/* set_root_vertex - Set root Vertex of the Graph.

         inputs  
                 cgr - Graph Structure
		 crv - root Vertex.
         returns
                  none
*/
void
set_root_vertex(GraphFrame *cgr, PNODE crv)
{
  cgr->rootv = crv;
}
/* set_graph_gcs - Set the three General Graphics Contexts used for the Graph

         inputs  
                 cgr - Graph Structure
		 gc - Graphic Context to draw an item in the Graph
		 pix_erase - Graphic Context to erase an item in the Graph
		 pix_high - Graphic Context to highlight an item in the Graph.
         returns
                  none
*/
#ifdef XGR
void 
set_graph_gcs(GraphFrame *cgr, GC gc, GC pix_erase, GC pix_high, GC pix_color)
{
  cgr->gc = gc;
  cgr->pix_erase = pix_erase;
  cgr->pix_high = pix_high;
  cgr->pix_color = pix_color;
}
#endif
/* get_num_ver_group - Get the number of the last Group Vertex of the Graph

         inputs  
                 cgr - Graph Structure
         returns
                  number of the Group Vertex.
*/
int
get_num_ver_group(GraphFrame *cgr)
{
  return (cgr->nvgroup);
}
/* get_num_vertex - Get number of the last vertex of the Graph

         inputs  
                 cgr - Graph Structure
         returns
                  number of the last Vertex.
*/
int
get_num_vertex(GraphFrame *cgr)
{
  return (cgr->nvertex);
}

/* get_ver_size - Get vertical Dimension of a Vertex in the Graph.

         inputs  
                 cgr - Graph Structure
         returns
	         Vertical Dimension of a Vertex.
*/
int
get_ver_size(GraphFrame *cgr)
{
  return (cgr->vsize);
}
/* get_hor_size - Get horizontal Dimension of a Vertex in the Graph.

         inputs  
                 cgr - Graph Structure
         returns
	         Horizontal Dimension of a Vertex.
*/
int
get_hor_size(GraphFrame *cgr)
{
  return (cgr->hsize);
}
/* get_graph_dir - Get edge Direction of the Graph.

         inputs  
                 cgr - Graph Structure
         returns
	         direction of the edges.
*/
int
get_graph_dir(GraphFrame *cgr)
{
  return (cgr->graph_dir);
}
/* get_graphic_flag - Get Graphic Flag of Graph.

         inputs  
                 cgr - Graph Structure
         returns
	         Graphic Flag.
*/
int
get_graphic_flag(GraphFrame *cgr)
{
  return (cgr->graphic);
}
/* get_scale - Get Scale of the Graph (-4 .. 10)

         inputs  
                 cgr - Graph Structure
         returns
	         scale of the graph.
*/
int
get_scale(GraphFrame *cgr)
{
  return (cgr->scale);
}
/* get_list_vertices - Get List of the vertices of the Graph.

         inputs  
                 cgr - Graph Structure
         returns
	         List of the vertices. 
*/

LNode_type *
get_list_vertices(GraphFrame *cgr)
{
  return (cgr->list_vertices);
}

LNode_type *
get_list_selected_vertices(GraphFrame *cgr)
{
  return (cgr->list_sel_vertex);
}

LNode_type *
get_list_objects(GraphFrame *cgr)
{
  return (cgr->lobjects);
}

LNode_type *
get_list_selected_edges(GraphFrame *cgr)
{
  LNode_type *lseledges = init_linked_list();
  LNode_type *ptr = cgr->list_vertices->back;
  for( ; ptr != cgr->list_vertices; ptr = ptr->back)
    {
      PNODE v = (PNODE) ptr->info;
      LNode_type *ei = v->ledges_in->back;
      for( ; ei!=v->ledges_in; ei = ei->back)
	{
	  PEDGE e = (PEDGE) ei->info;

	  if(e->select)
	    Insert_linked_list(lseledges, create_new_node_llist((char *)e));
	}
    }
  return lseledges;
}

LNode_type *
get_list_highlighted_vertices(GraphFrame *cgr)
{
  return (cgr->list_high_vertex);
}
/* get_pixmap - Get Pixmap of the Graph. 

         inputs  
                 cgr - Graph Structure
         returns
	         pixmap of the Graph.
*/

#ifdef XGR
Pixmap
get_pixmap(GraphFrame *cgr)
{
  return(cgr->pixmap);
}
#endif

/* get_width - Get Width of the Drawing Area. 

         inputs  
                 cgr - Graph Structure
         returns
	        Width of the Drawing Area.
*/

Dimension
get_width(GraphFrame *cgr)
{
  return(cgr->w);
}

Dimension
get_pixmap_width(GraphFrame *cgr)
{
  return(cgr->wp);
}
/* get_height - Get Height of the Drawing Area. 

         inputs  
                 cgr - Graph Structure
         returns
	        Height of the Drawing Area.
*/
Dimension
get_height(GraphFrame *cgr)
{
  return(cgr->h);
}
Dimension
get_pixmap_height(GraphFrame *cgr)
{
  return(cgr->hp);
}


int
get_name_display_mode(GraphFrame *cgr)
{
  return(cgr->dmode);
}

void
set_name_display_mode(GraphFrame *cgr, int dm)
{
  cgr->dmode = dm;
}

int
get_name_edge_display_mode(GraphFrame *cgr)
{
  return(cgr->emode);
}

void
set_name_edge_display_mode(GraphFrame *cgr, int em)
{
  cgr->emode = em;
}

void
set_method_tree(GraphFrame *cgr, short mt)
{
  cgr->mtree = mt;
}

short
get_method_tree(GraphFrame *cgr)
{
  return cgr->mtree;
}

void
set_no_class_edge(GraphFrame *cgr, Boolean ncg)
{
  cgr->no_class_edge = ncg;
}

Boolean
get_no_class_edge(GraphFrame *cgr)
{
  return cgr->no_class_edge;
}

PNODE
get_root_vertex(GraphFrame *cgr)
{
    return cgr->rootv;
}

