#include "gr_util.h"
#include "gr_structure.h"
#include "gr_operations.h"
#include "gr_tree.h"
#include "gr_radial.h"
#include "gr_algorithms.h"
#include "alist.h"

/* grouping_selected_nodes - This general function takes all selected vertices
                             from one graph and grouped them in a  group 
			     vertex in a new graph. All no selected vertices 
			     are copied to the new graph. 

         inputs  
                 ngf - New graph 
		 ogf - Old graph
         returns
                  none
         notes
                 Note that this function does not create a new graph structure.
*/

void
grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf)
{

  LNode_type *head = ogf->list_sel_vertex;
  
  struct pt *gv;

  copy_unselected_vertices(ngf, ogf);

  ngf->dmode = ogf->dmode;
  ngf->nvertex = ogf->nvertex;

  if(head->next != head)
    {
      LNode_type *vptr;
      gv = create_group_vertex(ngf,ogf);
      insert_v(gv, ngf);

      (ngf->count_vertex)++; /* count one more vertex */

      (ngf->nvertex)++;
      gv->num = ngf->nvertex;
      /* Insert new vertex in a list */
      vptr = create_new_node_llist((char *)gv);
      Insert_linked_list(ngf->list_vertices,vptr); 

      ngf->cgv = gv;
      ngf->draw = False; /* Don't draw in drawing_a */
#ifdef XGR
      if(ngf->pdraw){
	char *gvlabel = get_vertex_display_name(ngf, gv); 
	GC gc = choose_gc(ngf, gv);
	draw_vertex( ngf->drawing_a, gc, gv->loc.x, gv->loc.y,
		     ngf->vsize, gvlabel,gv->weight,gv->shape,ngf);
	if(gvlabel) free(gvlabel); gvlabel = NULL;
      }
#endif	
      ngf->draw = True;
    }

  create_edges_for_group_graph(ngf, ogf);

  update_list_objects(ngf);

}
void
extended_grouping_selected_nodes(GraphFrame *ngf,GraphFrame *ogf)
{

  LNode_type *head = ogf->list_sel_vertex;
  
  struct pt *gv;

  copy_unselected_vertices(ngf, ogf);

  ngf->dmode = ogf->dmode;
  ngf->nvertex = ogf->nvertex;

  if(head->next != head)
    {
      LNode_type *vptr;
      gv = create_group_vertex(ngf,ogf);
      insert_v(gv, ngf);

      (ngf->count_vertex)++; /* count one more vertex */

      (ngf->nvertex)++;
      gv->num = ngf->nvertex;
      /* Insert new vertex in a list */
      vptr = create_new_node_llist((char *)gv);
      Insert_linked_list(ngf->list_vertices,vptr); 

      ngf->cgv = gv;
      ngf->draw = False; /* Don't draw in drawing_a */
#ifdef XGR
      if(ngf->pdraw){
	char *gvlabel = get_vertex_display_name(ngf, gv); 
	GC gc = choose_gc(ngf, gv);
	draw_vertex( ngf->drawing_a, gc, gv->loc.x, gv->loc.y,
		     ngf->vsize, gvlabel,gv->weight,gv->shape,ngf);
	if(gvlabel) free(gvlabel); gvlabel = NULL;
      }
#endif	
      ngf->draw = True;
    }

  create_edges_for_extended_group_graph(ngf, ogf);

  update_list_objects(ngf);

}
/* create_edges_for_group_graph - Creates all edges for the new graph. The
                                  rules are: Copy all edges from unselected
				  vertices to the new graph,  Create new edges
				  from the group vertex to the unselected
				  vertices. 
         inputs  
                 ngf - New Graph
		 ogf - Old Graph
         returns
                  none
*/
void
create_edges_for_group_graph(GraphFrame *ngf,GraphFrame *ogf)
{
  LNode_type *poed;
  LNode_type *ptr;
  struct edge *ne = NULL;

  char weight[4];
  strcpy(weight,"1");

  untouch_edges(ogf);

  for(ptr = ogf->list_vertices->next; ptr != ogf->list_vertices; ptr = ptr->next)
    {
      struct pt *v = (struct pt *) ptr->info;
      for(poed = v->ledges_in->next; poed != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->info;
	  struct pt *v1 = e->from;
	  struct pt *v2 = e->to;

	  int isobjv2 = is_vertex_object(v2);

	  if(e->touch == False)
	    {
	      e->touch = True;
	      if((!v1->select)&& (!v2->select))
		{
		  struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		  if(nv1 && nv2)
		    {
		      ne = copy_edge(e,nv1, nv2,ngf);
		      insert_e(ne,ngf);
		      ngf->draw = False;
		      update_edge(ngf, ne);
		      ngf->draw = True;
		    }
		}
	      else if(!v1->select)
		{
		  struct pt *nv1= find_vertex_by_name(ngf, v1->label);

		  if(isobjv2)
		    {
		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(!nv2)
			{
			  nv2 = copy_vertex(v2);
			  insert_v(nv2,ngf);
			  insert_hash_vertex_name(ngf,nv2->label,nv2);
			  (ngf->count_vertex)++; /* count one more vertex */
			  /* Inserte new vertex in a list */
			  Insert_linked_list(ngf->list_vertices, 
				       create_new_node_llist((char *)nv2));
			  printf("Inserting new node %s\n", nv2->label);
			}

		      if(!select_nearest_edge(nv1,nv2,ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE OBJECT between %s - %s\n",
				 nv1->label,nv2->label);*/
			  adding_edge(ngf,nv1, nv2, weight);
			  ngf->draw = True;
			  set_edge_attrib(find_edge(ngf,nv1,nv2), 'z');
			}
		    }		  
		  else if(!select_nearest_edge(nv1,ngf->cgv, ngf))
		    {
		      ngf->draw = False;
		      /*printf("ADDING EDGE between %s - %s\n", 
			     nv1->label,ngf->cgv->label);*/
		      adding_edge(ngf,nv1, ngf->cgv, weight);
		      ngf->draw = True;
		    }
		}
	      else if(!v2->select)
		{
		  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		  if(!isobjv2)
		    {
		      if(!select_nearest_edge(ngf->cgv, nv2, ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE between %s - %s\n", 
				 ngf->cgv->label, nv2->label);*/
			  adding_edge(ngf, ngf->cgv,nv2,weight);
			  ngf->draw = True;
			}
		    }
		  /*else
		    printf("No Inserting edge to %s\n", nv2->label);*/
		}
	    }
	}
      if(ogf->graph_dir == DIRECTED)
	{
	  for(poed = v->ledges_out->next; poed != v->ledges_out; 
	      poed = poed->next)
	    {
	      struct edge *e = (struct edge *)poed->info;
	      struct pt *v1 = e->from;
	      struct pt *v2 = e->to;
	      int isobjv2 = is_vertex_object(v2);

	      if(e->touch == False)
		{
		  e->touch = True;
		  if((!v1->select)&& (!v2->select))
		    {
		      struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(nv1 && nv2)
			{
			  ne = copy_edge(e,nv1, nv2,ngf);
			  insert_e(ne,ngf);
			  ngf->draw = False;
			  update_edge(ngf, ne);
			  ngf->draw = True;
			}
		    }
		  else if(!v1->select)
		    {
		      struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		      if(isobjv2)
			{
			  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
			  if(!nv2)
			    {
			      nv2 = copy_vertex(v2);
			      insert_v(nv2,ngf);
			      insert_hash_vertex_name(ngf,nv2->label,nv2);
			      (ngf->count_vertex)++; /* count one more vertex */
			      /* Inserte new vertex in a list */
			      Insert_linked_list(ngf->list_vertices, 
						 create_new_node_llist((char *)nv2));
			      /*printf("Inserting new node %s\n", nv2->label);*/
			    }
			  if(!select_nearest_edge(nv1,nv2,ngf))
			    {
			      ngf->draw = False;
			      /*printf("ADDING EDGE OBJECT between %s - %s\n", 
				     nv1->label,nv2->label);*/
			      adding_edge(ngf,nv1, nv2, weight);
			      ngf->draw = True;
			      set_edge_attrib(find_edge(ngf,nv1,nv2), 'z');
			    }
			}
		      else if(!select_nearest_edge(nv1,ngf->cgv, ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE OUT between %s - %s\n", 
				 nv1->label,ngf->cgv->label);*/
			  adding_edge(ngf,nv1, ngf->cgv, weight);
			  ngf->draw = True;
			}
		    }
		  else if(!v2->select)
		    {
		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(!isobjv2)
			{
			  if(!select_nearest_edge(ngf->cgv, nv2, ngf))
			    {
			      ngf->draw = False;
			      /*printf("ADDING EDGE OUT between %s - %s\n", 
				     ngf->cgv->label,nv2->label);*/
			      adding_edge(ngf, ngf->cgv,nv2, weight);
			      ngf->draw = True;
			    }
			}
		      /*else
			printf("No Inserting edge to %s\n", nv2->label);*/
		    }
		}
	    }
	}
    }
}
/* create_edges_for_extended_ group_graph - Creates all edges for the new graph. The
                                  rules are: Copy all edges from unselected
				  vertices to the new graph,  Create new edges
				  from the group vertex to the unselected
				  vertices. 
         inputs  
                 ngf - New Graph
		 ogf - Old Graph
         returns
                  none
*/
void
create_edges_for_extended_group_graph(GraphFrame *ngf,GraphFrame *ogf)
{
  LNode_type *poed;
  LNode_type *ptr;
  struct edge *ne = NULL;

  char weight[4];
  strcpy(weight,"1");

  untouch_edges(ogf);

  for(ptr = ogf->list_vertices->next; ptr != ogf->list_vertices; ptr = ptr->next)
    {
      struct pt *v = (struct pt *) ptr->info;
      for(poed = v->ledges_in->next; poed != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->info;
	  struct pt *v1 = e->from;
	  struct pt *v2 = e->to;

	  int isobjv2 = is_vertex_object(v2);
	  int isleafv2 = is_vertex_sink(v2);
	  int isgroupv1 = is_vertex_group(v1);

	  if(e->touch == False)
	    {
	      e->touch = True;
	      if((!v1->select)&& (!v2->select))
		{
		  struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		  if(nv1 && nv2)
		    {
		      ne = copy_edge(e,nv1, nv2,ngf);
		      insert_e(ne,ngf);
		      ngf->draw = False;
		      update_edge(ngf, ne);
		      ngf->draw = True;
		    }
		}
	      else if(!v1->select)
		{
		  struct pt *nv1= find_vertex_by_name(ngf, v1->label);

		  if(isleafv2)
		    {
		      if(!isgroupv1) {

		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(!nv2)
			{
			  nv2 = copy_vertex(v2);
			  insert_v(nv2,ngf);
			  insert_hash_vertex_name(ngf,nv2->label,nv2);
			  (ngf->count_vertex)++; /* count one more vertex */
			  /* Inserte new vertex in a list */
			  Insert_linked_list(ngf->list_vertices, 
				       create_new_node_llist((char *)nv2));
			  printf("Inserting new node %s\n", nv2->label);
			}

		      if(!select_nearest_edge(nv1,nv2,ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE OBJECT between %s - %s\n",
				 nv1->label,nv2->label);*/
			  adding_edge(ngf,nv1, nv2, weight);
			  ngf->draw = True;

			  if(isobjv2)
			    set_edge_attrib(find_edge(ngf,nv1,nv2), 'z');
			}
		      }
		    }	  
		  else if(!select_nearest_edge(nv1,ngf->cgv, ngf))
		    {
		      ngf->draw = False;
		      /*printf("ADDING EDGE between %s - %s\n", 
			     nv1->label,ngf->cgv->label);*/
		      adding_edge(ngf,nv1, ngf->cgv, weight);
		      ngf->draw = True;
		    }
		}
	      else if(!v2->select)
		{
		  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		  if(!isobjv2)
		    {
		      if(!select_nearest_edge(ngf->cgv, nv2, ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE between %s - %s\n", 
				 ngf->cgv->label, nv2->label);*/
			  adding_edge(ngf, ngf->cgv,nv2,weight);
			  ngf->draw = True;
			}
		    }
		  /*else
		    printf("No Inserting edge to %s\n", nv2->label);*/
		}
	    }
	}
      if(ogf->graph_dir == DIRECTED)
	{
	  for(poed = v->ledges_out->next; poed != v->ledges_out; 
	      poed = poed->next)
	    {
	      struct edge *e = (struct edge *)poed->info;
	      struct pt *v1 = e->from;
	      struct pt *v2 = e->to;
	      int isobjv2 = is_vertex_object(v2);
	      int isleafv2 = is_vertex_sink(v2);
	      int isgroupv1 = is_vertex_group(v1);

	      if(e->touch == False)
		{
		  e->touch = True;
		  if((!v1->select)&& (!v2->select))
		    {
		      struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(nv1 && nv2)
			{
			  ne = copy_edge(e,nv1, nv2,ngf);
			  insert_e(ne,ngf);
			  ngf->draw = False;
			  update_edge(ngf, ne);
			  ngf->draw = True;
			}
		    }
		  else if(!v1->select)
		    {
		      struct pt *nv1= find_vertex_by_name(ngf, v1->label);
		      if(isleafv2)
			{
			  if(!isgroupv1) {

			  struct pt *nv2= find_vertex_by_name(ngf, v2->label);
			  if(!nv2)
			    {
			      nv2 = copy_vertex(v2);
			      insert_v(nv2,ngf);
			      insert_hash_vertex_name(ngf,nv2->label,nv2);
			      (ngf->count_vertex)++; /* count one more vertex */
			      /* Inserte new vertex in a list */
			      Insert_linked_list(ngf->list_vertices, 
						 create_new_node_llist((char *)nv2));
			      /*printf("Inserting new node %s\n", nv2->label);*/
			    }
			  if(!select_nearest_edge(nv1,nv2,ngf))
			    {
			      ngf->draw = False;
			      /*printf("ADDING EDGE OBJECT between %s - %s\n", 
				     nv1->label,nv2->label);*/
			      adding_edge(ngf,nv1, nv2, weight);
			      ngf->draw = True;
			      if(isobjv2)
				set_edge_attrib(find_edge(ngf,nv1,nv2), 'z');
			    }
			  }
			}
		      else if(!select_nearest_edge(nv1,ngf->cgv, ngf))
			{
			  ngf->draw = False;
			  /*printf("ADDING EDGE OUT between %s - %s\n", 
				 nv1->label,ngf->cgv->label);*/
			  adding_edge(ngf,nv1, ngf->cgv, weight);
			  ngf->draw = True;
			}
		    }
		  else if(!v2->select)
		    {
		      struct pt *nv2= find_vertex_by_name(ngf, v2->label);
		      if(!isobjv2)
			{
			  if(!select_nearest_edge(ngf->cgv, nv2, ngf))
			    {
			      ngf->draw = False;
			      /*printf("ADDING EDGE OUT between %s - %s\n", 
				     ngf->cgv->label,nv2->label);*/
			      adding_edge(ngf, ngf->cgv,nv2, weight);
			      ngf->draw = True;
			    }
			}
		      /*else
			printf("No Inserting edge to %s\n", nv2->label);*/
		    }
		}
	    }
	}
    }
}

/* copy_unselected_vertices - Copy unselected vertices from the old graph to
                              the new Graph.

         inputs  
                 ngf - New Graph
		 ogf - Old Graph
         returns
                  none

*/

void 
copy_unselected_vertices(GraphFrame *ngf,GraphFrame *ogf)
{
  int i;
  for(i=0; i< HASHSIZE ; i++)
    {
      if(ogf->HV[i] != NULL)
	{
	  Node_type * loc = ogf->HV[i];
	  for(;loc!=NULL;loc=loc->next)
	    {
	      struct pt *v = (struct pt *)(loc->info.gr);
	      struct pt *nv;
	      LNode_type *vptr;

	      if(v->select == False)
		{
		  nv = copy_vertex(v);
		  insert_v(nv,ngf);
		  insert_hash_vertex_name(ngf,nv->label,nv);
		   
		  (ngf->count_vertex)++; /* count one more vertex */

		  /* Inserte new vertex in a list */
		  vptr = create_new_node_llist((char *)nv);
		  Insert_linked_list(ngf->list_vertices,vptr); 

		  ngf->draw = False; /* Don't draw in drawing_a */
#ifdef XGR		  
		  if(ngf->pdraw){
		    char *nvlabel = get_vertex_display_name(ngf, nv);
		    GC gc = choose_gc(ngf, nv);
		    draw_vertex( ngf->drawing_a, gc, nv->loc.x, nv->loc.y,
				 ngf->vsize, nvlabel,nv->weight,nv->shape,ngf);
		    if(nvlabel) free(nvlabel); nvlabel = NULL;
		  }
#endif
		  ngf->draw = True;

		}
	    }
	}
    }

}
/* create_group_vertex - Create a new group vertex for a Graph. The names of
                         these group vertices are an underscore and a number.
			 This number is kept in the Graph Structure to assign
			 new number to future group vertices.

         inputs  
                 ngf - New Graph 
		 ogf - Old Graph
         returns
                  Group Vertex
         notes
                 R 

*/

struct pt *
create_group_vertex(GraphFrame *ngf,GraphFrame *ogf)
{
  PNODE gv; 

  gv = (PNODE) calloc( 1, sizeof(struct pt) );
  if( gv == NULL )
  {      
      fprintf(stderr,"Out of Memory create_group_vertex\n");
      abort();
  }

  gv->loc.x = 0; /* we have to change this */
  gv->loc.y = 0;
  get_new_location_group_vertex(&(gv->loc),ogf);
  gv->select = False;
  gv->group = True;
  gv->label = (char *)malloc(MDIGITS); /* we have to change this */
  if(!gv->label) {
      fprintf(stderr,"Out of Memory create_group_vertex\n");
      abort();
  }
  (ngf->nvgroup)++;
  sprintf(gv->label,"_%d",ngf->nvgroup);
  gv->weight = NULL;
  gv->num = 0;
  gv->counter = 0;
  gv->color = GR_BG;
  gv->shape = PENTAGON;
  gv->attrib = NULL;
  gv->ledges_in = init_linked_list();
  gv->ledges_out = init_linked_list();
  return(gv);

}

/* get_new_location_group_vertex - The position of the new group vertex is
                                   the average position of all selected 
				   vertices in the old graph.
                       I
         inputs  
                 loc - Location of the New group vertex.
		 ogf - Old Graph.
         returns
                  none
*/

void
get_new_location_group_vertex(struct pr_pos *loc,GraphFrame *ogf)
{

  int x=0 ,y=0;
  LNode_type * ptr;
  int count = 0;

  for(ptr= ogf->list_sel_vertex; ptr->next != ogf->list_sel_vertex; 
      ptr = ptr->next)
    {
      struct pt *v = (struct pt *)(ptr->next->info);
      count++;
      x += v->loc.x;
      y += v->loc.y;
    }
  if(count > 0)
    {
      loc->x = (int)(x/count);
      loc->y = (int)(y/count);
    }
}

/* insert_selected_nodes  - Copy all selected vertices from the old graph in
                            the new graph. All edges between the selected 
			    vertices are also copied in the new graph.
         inputs  
                 ngf - New Graph
		 ogf - Old Graph
         returns
                  none
*/

void
insert_selected_nodes(GraphFrame *ngf,GraphFrame *ogf)
{
  LNode_type *head = ogf->list_sel_vertex;
  
  LNode_type *ptr;

  /* Copy all selected vertices in the new graph */
  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    {
      struct pt *v, *ov;
      LNode_type *vptr;

      ov = (struct pt *)(ptr->next->info);
      v = copy_vertex(ov);
      insert_v(v,ngf);

      (ngf->count_vertex)++; /* count one more vertex */

      /*Inserte new vertex in a list */
      vptr = create_new_node_llist((char *)v);
      Insert_linked_list(ngf->list_vertices,vptr); 

      insert_hash_vertex_name(ngf,v->label,v);
      ngf->draw = False; /* Don't draw in drawing_a */
#ifdef XGR      
      if(ngf->pdraw){
	char *vlabel = get_vertex_display_name(ngf, v);
	GC gc = choose_gc(ngf, v);
	draw_vertex( ngf->drawing_a, gc, v->loc.x, v->loc.y,
		     ngf->vsize, vlabel,v->weight,v->shape,ngf);
	if(vlabel) free(vlabel); vlabel = NULL;
      }
#endif
      ngf->draw = True;
      
    }
  untouch_edges(ogf);
  /* copy all edges between the selected vertices in the new graph */

  for(ptr= head; ptr->next != head ; ptr = ptr->next)
    {
      struct pt *nv, *ov;
      struct edge *e;
      ov = (struct pt *)(ptr->next->info);
      if(ogf->graph_dir != NONE) /* Directed or undirected edges */
	{
	  struct edge *ne;
	  LNode_type *ptred =ov->ledges_in ;

	  for( ; ptred->next != ov->ledges_in ;ptred =  ptred->next)
	    {
	      e = (struct edge *)(ptred->next->info);

	      if(e->touch == True)
		continue;
	      else
		e->touch = True;

	      nv = e->from;

	      if(nv == ov)
		nv = e->to;

	      if (nv->select)
		{
		  Node_type *nptr = select_nearest_edge(nv,ov,ngf);
		  if(!nptr)
		    {
		      Node_type *tptr;
		      struct pt *v1,*v2;

		      tptr = select_nearest_vertex(&(nv->loc), ngf );
		      v1 = (struct pt *)(tptr->info.gr);
		      tptr = select_nearest_vertex(&(ov->loc), ngf );
		      v2 = (struct pt *)(tptr->info.gr);
		      ne = copy_edge(e,v1, v2, ngf);
		      insert_e(ne,ngf);

		      ngf->draw = False;
		      update_edge(ngf, ne);
		      ngf->draw = True;
		    }
		}
	    }
	}
      if(ogf->graph_dir == DIRECTED) /* Directed Edges */
	{
	  struct edge *ne;
	  LNode_type *ptred =ov->ledges_out ;

	  for( ; ptred->next != ov->ledges_out ;ptred =  ptred->next)
	    {
	      e = (struct edge *)(ptred->next->info);

	      if(e->touch == True)
		continue;
	      else
		e->touch = True;

	      nv = e->to;
	      if (nv->select)
		{
		  Node_type *nptr = select_nearest_edge(ov,nv,ngf);
		  if(!nptr)
		    {
		      Node_type *tptr;
		      struct pt *v1,*v2;

		      tptr = select_nearest_vertex(&(ov->loc), ngf );
		      v1 = (struct pt *)(tptr->info.gr);
		      tptr = select_nearest_vertex(&(nv->loc), ngf );
		      v2 = (struct pt *)(tptr->info.gr);

		      ngf->draw = False;
		      ne = copy_edge(e, v1, v2, ngf);
		      insert_e(ne,ngf);
		      update_edge(ngf, ne);
		      ngf->draw = True;
		    }
		}
	    }
	}
    }

  update_list_objects(ngf);
} 

void
insert_edge_and_nodes(GraphFrame *ngf, GraphFrame *ogf,PEDGE e)
{
    PNODE u = e->from;
    PNODE v = e->to;
    PNODE nu = NULL;
    PNODE nv = NULL;
    PEDGE ne = NULL;
    nu = find_vertex_by_name(ngf,u->label);
    if(!nu)
	nu = insert_vertex_in_graph(ngf,u);
    nv = find_vertex_by_name(ngf,v->label);
    if(!nv)
	nv = insert_vertex_in_graph(ngf,v);

    ne = find_edge_by_name(ngf,nu->label,nv->label);
    if(!ne)
    {
	ne = copy_edge(e,nu, nv,ngf);
	insert_e(ne,ngf);
	ngf->draw = False;
	update_edge(ngf, ne);
	ngf->draw = True;
    }
}

void
insert_selected_edges(GraphFrame *ngf, GraphFrame *ogf)
{
  LNode_type *ptr = NULL;

  for(ptr= ogf->list_vertices->next; ptr != ogf->list_vertices ; 
      ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->info;
      LNode_type *ei;

      for(ei = v->ledges_in->next; ei != v->ledges_in; ei=ei->next)
	{
	    PEDGE e = (PEDGE)ei->info;
	    if(e->select)
		insert_edge_and_nodes(ngf,ogf,e);
	}
    }
}

void
insert_one_vertex_in_new_graph(GraphFrame *ngf, PNODE ov)
{
  PNODE v = copy_vertex(ov);
  insert_v(v,ngf);
  
  (ngf->count_vertex)++; /* count one more vertex */
      

  /*Inserte new vertex in a list */
  Insert_linked_list(ngf->list_vertices,create_new_node_llist((char *)v));
  
  insert_hash_vertex_name(ngf,v->label,v);
  ngf->draw = False; /* Don't draw in drawing_a */
#ifdef XGR      
  if(ngf->pdraw){
    char *vlabel = get_vertex_display_name(ngf, v);
    GC gc = choose_gc(ngf, v);
    draw_vertex( ngf->drawing_a, gc, v->loc.x, v->loc.y,
		 ngf->vsize, vlabel,v->weight,v->shape,ngf);
    if(vlabel) free(vlabel); vlabel = NULL;
  }
#endif
  ngf->draw = True;
}

PNODE
insert_vertex_in_graph(GraphFrame *ngf, PNODE ov)
{
  PNODE v = copy_vertex(ov);
  insert_v(v,ngf);
  
  (ngf->count_vertex)++; /* count one more vertex */
      
  /*Inserte new vertex in a list */
  Insert_linked_list(ngf->list_vertices,create_new_node_llist((char *)v));
  
  insert_hash_vertex_name(ngf,v->label,v);

  return v;
}

void
extended_browsing(GraphFrame *ngf,GraphFrame *ogf)
{
  LNode_type *head = ogf->list_sel_vertex;
  LNode_type *ptr;
  LNode_type *eout;

  if(ogf->graph_dir != DIRECTED)
    return;

  /* Copy all selected vertices and children in the new graph */

  for(ptr= head->next; ptr != head ; ptr = ptr->next)
    {
      PNODE ov = (PNODE)ptr->info;
      insert_one_vertex_in_new_graph(ngf, ov);
      
      for(eout = ov->ledges_out->next; eout != ov->ledges_out; 
	  eout = eout->next)
	{
	  PEDGE ei = (PEDGE) eout->info;
	  PNODE u = ei->to;

	  if (u->select == False)
	    {
	      if(find_vertex_by_name(ngf,u->label) == NULL)
		{
		  insert_one_vertex_in_new_graph(ngf, u);
		}
	    }
	}
    }

  /* copy all edges between the selected vertices in the new graph */

  for(ptr= head->next; ptr != head ; ptr = ptr->next)
    {
      PNODE ou, ov = (PNODE)(ptr->info) ;
      PNODE nu, nv;
      PEDGE ne, e;
      LNode_type *ptred = ov->ledges_out->next ;
      
      nv = find_vertex_by_name(ngf, ov->label);

      for( ; ptred != ov->ledges_out ;ptred =  ptred->next)
	{
	  e = (PEDGE)(ptred->info);
	  
	  ou = e->to;

	  nu = find_vertex_by_name(ngf, ou->label);

	  ne = copy_edge(e,nv, nu, ngf);

	  insert_e(ne,ngf);

	  ngf->draw = False;
	  update_edge(ngf, ne);
	  ngf->draw = True;
	  
	}
    }

  update_list_objects(ngf);
}


void 
reenumerate_vertices(GraphFrame *gf)
{
  LNode_type *ptr;
  int c = 1;
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	v->num = c++;
    }
  gf->nvertex = c-1;
}

short
gr_get_color_int(char *colorstr)
{
    if(!colorstr)
	return -1;
    if(!strcmp(colorstr,"Green"))
	return GR_GREEN;
    if(!strcmp(colorstr,"Navy"))
	return GR_NAVY;
    if(!strcmp(colorstr,"Yellow"))
	return GR_YELLOW;
    if(!strcmp(colorstr,"Magenta"))
	return GR_MAGENTA;
    if(!strcmp(colorstr,"Cyan"))
	return GR_CYAN;
    if(!strcmp(colorstr,"Gray"))
	return GR_GRAY;
    if(!strcmp(colorstr,"Orange"))
	return GR_ORANGE;
    if(!strcmp(colorstr,"Black"))
	return GR_BLACK;
    if(!strcmp(colorstr,"Maroon"))
	return GR_MAROON;
    if(!strcmp(colorstr,"White"))
	return GR_WHITE;
    if(!strcmp(colorstr,"Blue"))
	return GR_BLUE;
    if(!strcmp(colorstr,"Pink"))
	return GR_PINK;
    if(!strcmp(colorstr,"Brown"))
	return GR_BROWN;
    if(!strcmp(colorstr,"Grey"))
	return GR_GREY;
    if(!strcmp(colorstr,"LimeGreen"))
	return GR_LIMEGREEN;
    if(!strcmp(colorstr,"Turquoise"))
	return GR_TURQUOISE;
    if(!strcmp(colorstr,"Violet"))
	return GR_VIOLET;
    if(!strcmp(colorstr,"Wheat"))
	return GR_WHEAT;
    if(!strcmp(colorstr,"Purple"))
	return GR_PURPLE;
    if(!strcmp(colorstr,"Beige"))
	return GR_BEIGE;
    if(!strcmp(colorstr,"Red"))
	return GR_RED;

    return GR_BG;
}

void
color_edges_by_class(GraphFrame *gf, short nc, short tc, short bc, short fc,
		     short oc, short lc)
{
  LNode_type *ptr;
  
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  struct edge *e = (struct edge *)ei->next->info;

	  if(!e)
	    break;

	  switch(e->class)
	    {
	    case NO_EDGE_CLASS:
	      e->color = nc >= 0 ? nc : e->color;
	      break;
	    case TREE_CLASS:
	      e->color = tc >= 0 ? tc : e->color;
	      break;
	    case BACK_CLASS:
	      e->color = bc >= 0 ? bc : e->color;
	      break;
	    case FORWARD_CLASS:
	      e->color = fc >= 0 ? fc : e->color;
	      break;
	    case OUT_CLASS:
	      e->color = oc >= 0 ? oc : e->color;
	      break;
	    case LEVEL_CLASS:
	      e->color = lc >=0 ? lc : e->color;
	      break;
	    }
	}
    }
}


/*      String Matching
**      ---------------
**      String comparison function for file names with one wildcard * in the
**      template. Arguments are:
**
**      tmpl    is a template string to match the name against.
**              agaist, may contain a single wildcard character * which
**              matches zero or more arbitrary characters.
**      name    is the name to be matched agaist the template.
**
**      return: - Empty string if perfect match
**              - pointer to part matched by wildcard if any
**              - NULL if no match
*/
char *
GRStrMatch (const char * tmpl, const char * name)
{
    while (*tmpl && *name && *tmpl==*name) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : (char *) NULL;
}    

char * 
GRStrCaseMatch (const char * tmpl, const char * name)
{
    while (*tmpl && *name && TOUPPER(*tmpl)==TOUPPER(*name)) tmpl++, name++;
    return ((!*tmpl && !*name) || *tmpl=='*') ? (char *) name : (char *) NULL;
}    


void
untouch_edges(GraphFrame *gf)
{
  LNode_type *poed;
  LNode_type *ptr;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices; ptr = ptr->next)
    {
      struct pt *v = (struct pt *) ptr->info;
      for(poed = v->ledges_in->next; poed != v->ledges_in; poed = poed->next)
	{
	  struct edge *e = (struct edge *)poed->info;
	  e->touch = False; 
	}
      if(gf->graph_dir == DIRECTED)
	{
	  for(poed = v->ledges_out->next; poed != v->ledges_out; 
	      poed = poed->next)
	    {
	      struct edge *e = (struct edge *)poed->info;
	      e->touch = False; 
	    }
	}
    }
}

#define FV_BIG 300000

long
get_weight_out_degree_node_to_fv(PEDGE e)
{
    PNODE v = e->to;
    if(v->shape == PENTAGON)
	return (long)FV_BIG;
    else
	return ((long)count_llist(v->ledges_out));
}


void
compute_tree_graph(GraphFrame *gf, struct pt *v)
{
  if(!gf || !v)
	return;
  gf->rootv = v;
  reset_mark_pick_vertices(gf);
  reset_edges(gf);
  if(gf->mtree == DFS_TREE)
    {
      if(gf->graph_dir == DIRECTED)
	dfs(v);
      else
	dfs_undirected(v);
    }
  else if(gf->mtree == BFS_TREE)
    bfs(gf,v);
  else if(gf->mtree == BFS_FV_TREE)
    bfs_first_variation(gf,v);
  else if(gf->mtree == SHORTEST_PATH_TREE)
      compute_tree_from_shortest_path(gf,v,gf->cost_weight);
  else if(gf->mtree == SHORTEST_PATH_FV_TREE)
    compute_tree_from_shortest_path(gf,v,get_weight_out_degree_node_to_fv);
}

void
compute_tree_graph_highlight(GraphFrame *gf, struct pt *v)
{
  if(!gf || !v)
	return;
  if(!v->highlight)
      return;
  gf->rootv = v;

  reset_mark_pick_vertices(gf);
  reset_edges(gf);

  if(gf->mtree == DFS_TREE)
    {
      if(gf->graph_dir == DIRECTED)
	dfs_highlight(v);
    }
  else if(gf->mtree == BFS_TREE)
    bfs_highlight(gf,v);
}


void
link_tree(GraphFrame *gf)
{
  LNode_type *ptr;

  reset_edges(gf);

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->next->info;
      LNode_type *ei;

      for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	{
	  PEDGE e = (PEDGE)ei->next->info;

	  if(e->attrib =='n')
	    e->class = TREE_CLASS;
	  else if(e->attrib == 'p')
	    e->class = BACK_CLASS;
	  else if(e->attrib == 'c')
	    e->class = FORWARD_CLASS;
	  else if(e->attrib == 'u')
	    e->class = FORWARD_CLASS;
	  else if(e->attrib == 'i')
	    e->class = FORWARD_CLASS;
	  else if(e->attrib == 's')
	    e->class = FORWARD_CLASS;
	  else if(e->attrib == 't')
	    e->class = FORWARD_CLASS;
	  else
	    e->class = NO_EDGE_CLASS;
	}
    }
}

LNode_type *
list_sources_nodes(GraphFrame *gf,int *count)
{
  LNode_type *ptr, *lnodes = init_linked_list();
  int i=0;
  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *ov = (struct pt *)ptr->next->info;
      if(!PARENT(gf,ov))
	{
	  if(FIRSTCHILD(gf,ov))
	     {
	       i++;
	       Insert_linked_list(lnodes,create_new_node_llist((char *)ov));
	     }
	}
    }

  if(i > 0)
    {
      *count = i;
      return lnodes;
    }
  Delete_all_list(lnodes);
  return NULL;
}

void
reset_pick_in_objects(GraphFrame *gf)
{
  LNode_type *ptr;
  if(gf->lobjects == NULL)
    return;
  for(ptr = gf->lobjects->next; ptr != gf->lobjects ; ptr = ptr->next)
    {
      PNODE vobj = (PNODE) ptr->info;
      vobj->pick = False;
    }
}

void
update_list_objects(GraphFrame *gf)
{
  LNode_type *ptr;

  if(gf->lobjects->next != gf->lobjects)
    return;

  for(ptr = gf->list_vertices->next; ptr != gf->list_vertices ; 
      ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(is_vertex_object(v))
	Insert_linked_list(gf->lobjects, create_new_node_llist((char *)v));
    }
}

int 
move_graph_to_given_position(GraphFrame *gf, int xi, int yi)
{
  int xorg=0, yorg=0, wiorg=0, heorg=0;

  if(!find_graph_bounding_box(gf,&xorg,&yorg, &wiorg, &heorg))
    {
      move_graph_lin(gf,xi-xorg, yi-yorg);
      return 0;
    }
  return 1;
}

int 
find_graph_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h)
{
  int maxx = MAX_INT_NEG, maxy=MAX_INT_NEG ,
    minx = MAX_INT_POS, miny = MAX_INT_POS;

  LNode_type *ptr = gf->list_vertices->next;
  
  if(gf->count_vertex < 2)
    return 1;

  for(; ptr!= gf->list_vertices; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(v->loc.x > maxx)
	maxx = v->loc.x;
      if(v->loc.y > maxy)
	maxy = v->loc.y;
      if(v->loc.x < minx)
	minx = v->loc.x;
      if(v->loc.y < miny)
	miny = v->loc.y;
    }
 *x = minx-gf->hsize;
 *y = miny-gf->vsize;
 *w = maxx - minx + 2*gf->hsize;
 *h = maxy - miny + 2*gf->vsize;

 return 0;
}

int 
find_selected_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h)
{
  int maxx = MAX_INT_NEG, maxy=MAX_INT_NEG ,
    minx = MAX_INT_POS, miny = MAX_INT_POS;

  LNode_type *ptr = gf->list_sel_vertex->next;
  
  if(gf->count_vertex < 2)
    return 1;

  if(ptr == gf->list_sel_vertex)
      return 1;

  for(; ptr!= gf->list_sel_vertex; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;

      if(v->loc.x > maxx)
	maxx = v->loc.x;
      if(v->loc.y > maxy)
	maxy = v->loc.y;
      if(v->loc.x < minx)
	minx = v->loc.x;
      if(v->loc.y < miny)
	miny = v->loc.y;
    }
 *x = minx-gf->hsize;
 *y = miny-gf->vsize;
 *w = maxx - minx + 2*gf->hsize;
 *h = maxy - miny + 2*gf->vsize;

 return 0;
}

int 
find_highlighted_nodes_bounding_box(GraphFrame *gf, int *x, int *y, int *w, int *h)
{
  int maxx = MAX_INT_NEG, maxy=MAX_INT_NEG ,
    minx = MAX_INT_POS, miny = MAX_INT_POS;
  int count = 0;
  LNode_type *ptr = gf->list_vertices->next;
  *x=0;*y=0;*w=0;*h=0;
  if(gf->count_vertex < 2)
    return 1;

  for(; ptr!= gf->list_vertices; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(!v->highlight)
	  continue;
      count++;
      if(v->loc.x > maxx)
	maxx = v->loc.x;
      if(v->loc.y > maxy)
	maxy = v->loc.y;
      if(v->loc.x < minx)
	minx = v->loc.x;
      if(v->loc.y < miny)
	miny = v->loc.y;
    }
  if(count > 2) {
      *x = minx-gf->hsize;
      *y = miny-gf->vsize;
      *w = maxx - minx + 2*gf->hsize;
      *h = maxy - miny + 2*gf->vsize;
  }
 return count;
}


Boolean
compare_vertices_equivalents(PNODE v , PNODE u, GraphFrame *gf)
{
  int cv = count_llist(v->ledges_out);
  int cu = count_llist(u->ledges_out);
  int diffc = cv > cu ? cv - cu : cu - cv;
  Boolean isuv = select_nearest_edge(v,u,gf) ? True : False;
  Boolean isvu = select_nearest_edge(u,v,gf) ? True : False;

  if(v->shape != u->shape)
      return False;

  if((isuv && isvu) || (!isuv && !isvu))
    {
      if(diffc != 0)
	return False;
    }
  else
    {
      if(diffc != 1)
	return False;
    }
  
  if(strcmp(v->weight, u->weight))
      return False;
  /*
  int lv = strlen(v->label);
  int lu = strlen(u->label);

  if(lu < lv)
    {
      if(strncmp(u->label, v->label, lu))
	  return False;
    }
  else
    {
      if(strncmp(u->label, v->label, lv))
	  return False;
    }*/

  /* different titles differents */
  {
    const char *vtitle = find_value_assoc_list(v->attrib, "title");
    const char *utitle = find_value_assoc_list(u->attrib, "title");
    if(vtitle && utitle)
      {
	if(strcmp(vtitle,utitle))
	  return False;
      }
    else
      return False;
  }
  /*if(strcmp((char*)get_attribute(v,1),(char *)get_attribute(u,1)))
    return False;*/
  /* different dates */
  {
    const char *vdate = find_value_assoc_list(v->attrib, "date");
    const char *udate = find_value_assoc_list(u->attrib, "date");
    if(vdate && udate)
      {
	if(strcmp(vdate,udate))
	  return False;
      }
    else
      return False;
  }
  /*if(strcmp((char*)get_attribute(v,4),(char *)get_attribute(u,4)))
    return False;*/

  return True;
}

LNode_type *
get_list_of_equivalent_nodes(GraphFrame *gf)
{
  LNode_type *leqnodes = init_linked_list();
  LNode_type *iptr = gf->list_vertices->next;
  LNode_type *jptr;

  reset_mark_pick_vertices(gf);

  for(; iptr != gf->list_vertices; iptr= iptr->next)
    {
      PNODE v = (PNODE)iptr->info;

      if(v->shape!= BOX)
        continue;

      if(v->mark)
	continue;

      for(jptr = iptr->next ; jptr != gf->list_vertices; jptr= jptr->next)
	{
	  PNODE u = (PNODE)jptr->info;

	  if(u->shape!= BOX)
	    continue;

	  if(v->mark)
	    break;
	  if(u->mark)
	    continue;
	  /*printf("COMPARING %d %s\n\t%d %s\n",v->num, v->label, u->num,u->label);*/
	  if(compare_vertices_equivalents(v ,u,gf))
	    {
	      int lu = strlen(u->label);
	      int lv = strlen(v->label);

      printf("FOUND %d %s\n\t%d %s\n",v->num, v->label, u->num,u->label);
	      u->mark = True;
	      v->mark = True;
	      
	      if(lv > lu)
		{
		 Insert_linked_list(leqnodes, create_new_node_llist((char*)v));
		 Insert_linked_list(leqnodes, create_new_node_llist((char*)u));
		}
	      else
		{
		 Insert_linked_list(leqnodes, create_new_node_llist((char*)u));
		 Insert_linked_list(leqnodes, create_new_node_llist((char*)v));
		}
	    }
	}
    }
  if(is_empty_list(leqnodes))
    {
      Delete_all_list(leqnodes);
      return NULL;
    }
  return leqnodes;
}

int
reduce_graph_with_equivalent_nodes(GraphFrame *ogf, GraphFrame *ngf, 
				   LNode_type *leqnodes)
{
  if(!leqnodes)
    return 1;

  if(ogf->graph_dir != DIRECTED)
    return 1;
  
  copy_unmarked_vertices(ngf,ogf);

  copy_edges_between_unmarked_vertices(ngf,ogf);

  copy_reducing_marked_vertices(ngf,ogf,leqnodes);

  ngf->dmode = ogf->dmode;
  ngf->nvertex = ogf->nvertex;

  update_list_objects(ngf);

  return 0;
}

PNODE 
find_alternate_vertex(LNode_type *leqnodes, PNODE v)
{
  LNode_type *ptr = leqnodes->next;
  for(; ptr != leqnodes ; ptr = ptr->next)
    {
      PNODE u = (PNODE)ptr->info;
      if(u == v)
	return ((PNODE)(ptr->back->info));
    }
  return NULL;
}

void
copy_reducing_marked_vertices(GraphFrame *ngf, GraphFrame *ogf, 
			      LNode_type *leqnodes)
{
  LNode_type *ptr = leqnodes->next;
  for(; ptr != leqnodes ; ptr = ptr->next)
    {
      PNODE v2, v1 = (PNODE) ptr->info;
      ptr = ptr->next;
      v2 = (PNODE) ptr->info;
      printf("INSERTING REDUCED NODE %d %s\n", v1->num, v1->label);
      copy_complete_vertex(ngf,v1);
    }
  for(ptr = leqnodes->next; ptr != leqnodes ; ptr = ptr->next)
    {
      PNODE v2, v1 = (PNODE) ptr->info;
      ptr = ptr->next;
      v2 = (PNODE) ptr->info;
      copy_complete_edges_out(ngf, v1,leqnodes);
      copy_verify_edges_out(ngf, v2,v1,leqnodes);
      copy_complete_edges_in(ngf, v1,leqnodes);
      copy_verify_edges_in(ngf, v2,v1,leqnodes);
    }
}

PNODE
copy_complete_vertex(GraphFrame *ngf, PNODE v)
{
  PNODE nv = copy_vertex(v);
  insert_v(nv,ngf);
  insert_hash_vertex_name(ngf,nv->label,nv);
  (ngf->count_vertex)++; /* count one more vertex */
  Insert_linked_list(ngf->list_vertices,
		     create_new_node_llist((char *)nv)); 
  return nv;
}

void
copy_unmarked_vertices(GraphFrame *ngf, GraphFrame *ogf)
{
  LNode_type *ptr = ogf->list_vertices->next;

  for(;ptr != ogf->list_vertices; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(!v->mark)
	{
	  copy_complete_vertex(ngf, v);
	}
    }
}

void
copy_edges_between_unmarked_vertices(GraphFrame *ngf, GraphFrame *ogf)
{
  LNode_type *ptr = ogf->list_vertices->next;

  for(;ptr != ogf->list_vertices; ptr = ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      if(!v->mark)
	{
	  LNode_type *eptr = v->ledges_out->next;
	  for(;eptr != v->ledges_out; eptr = eptr->next)
	    {
	      PEDGE e = (PEDGE)eptr->info;
	      PNODE u = e->to;
	      if(!u->mark)
		{
		  PNODE nv = find_vertex_by_name(ngf,v->label);
		  PNODE nu = find_vertex_by_name(ngf,u->label);
		  copy_complete_edge(ngf,e, nv, nu);
		}
	    }
	}
    }
}


PEDGE
copy_complete_edge(GraphFrame *ngf, PEDGE e, PNODE nv, PNODE nu)
{
  PEDGE ne = copy_edge(e,nv, nu, ngf);
  insert_e(ne,ngf);
  ngf->draw = False;
  update_edge(ngf, ne);
  ngf->draw = True;
  return ne;
}



void
copy_complete_edges_out(GraphFrame *ngf, PNODE v, LNode_type *leqnodes)
{
  LNode_type *eptr = v->ledges_out->next;
  PNODE nv = find_vertex_by_name(ngf, v->label);

  for(;eptr != v->ledges_out; eptr = eptr->next)
    {
      PEDGE e = (PEDGE)eptr->info;
      PNODE nu = find_vertex_by_name(ngf, e->to->label);
      if(!nu)
	{
	  PNODE ut = find_alternate_vertex(leqnodes, e->to);
	  nu = find_vertex_by_name(ngf, ut->label);
	  printf("ALternate name of %d %s is %d %s\n", e->to->num,
		 e->to->label, ut->num, ut->label);
	}

      if(!select_nearest_edge(nv,nu,ngf))
	copy_complete_edge(ngf,e, nv, nu);
      else
	printf("Edge %d %s - %d %s is already created\n", nv->num, nv->label,
	       nu->num,nu->label);
    }
}

void
copy_complete_edges_in(GraphFrame *ngf, PNODE v, LNode_type *leqnodes)
{
  LNode_type *eptr = v->ledges_in->next;
  PNODE nv = find_vertex_by_name(ngf, v->label);

  for(;eptr != v->ledges_in; eptr = eptr->next)
    {
      PEDGE e = (PEDGE)eptr->info;
      PNODE nu = find_vertex_by_name(ngf, e->from->label);
      if(!nu)
	{
	  PNODE ut = find_alternate_vertex(leqnodes, e->from);
	  nu = find_vertex_by_name(ngf, ut->label);
	  printf("ALternate name of %d %s is %d %s\n", e->from->num,
		 e->from->label, ut->num, ut->label);
	}

      if(!select_nearest_edge(nu,nv,ngf))
	copy_complete_edge(ngf,e, nu, nv);
      else
	printf("Edge %d %s - %d %s is already created\n", nu->num, nu->label,
	       nv->num,nv->label);
    }
}


void
copy_verify_edges_out(GraphFrame *ngf, PNODE v, PNODE vr, LNode_type *leqnodes)
{
  LNode_type *eptr = v->ledges_out->next;

  PNODE nv = find_vertex_by_name(ngf,vr->label);

  for(;eptr != v->ledges_out; eptr = eptr->next)
    {
      PEDGE e = (PEDGE)eptr->info;

      PNODE nu = find_vertex_by_name(ngf,e->to->label);

      if(!nu)
	{
	  PNODE ut = find_alternate_vertex(leqnodes, e->to);
	  nu = find_vertex_by_name(ngf, ut->label);
	  printf("ALternate name of %d %s is %d %s\n", e->to->num,
		 e->to->label, ut->num, ut->label);
	}

      if(!select_nearest_edge(nv,nu,ngf))
	{
	  printf("FOUND EXTRA EDGE %d %s - %d %s\n", nv->num, nv->label,
		 nu->num, nu->label);
	  copy_complete_edge(ngf, e, nv, nu);
	}
    }
}



void
copy_verify_edges_in(GraphFrame *ngf, PNODE v, PNODE vr, LNode_type *leqnodes)
{
  LNode_type *eptr = v->ledges_in->next;

  PNODE nv = find_vertex_by_name(ngf,vr->label);

  for(;eptr != v->ledges_in; eptr = eptr->next)
    {
      PEDGE e = (PEDGE)eptr->info;

      PNODE nu = find_vertex_by_name(ngf,e->from->label);

      if(!nu)
	{
	  PNODE ut = find_alternate_vertex(leqnodes, e->from);
	  nu = find_vertex_by_name(ngf, ut->label);
	  printf("ALternate name of %d %s is %d %s\n", e->from->num,
		 e->from->label, ut->num, ut->label);
	}

      if(!select_nearest_edge(nu,nv,ngf))
	{
	  copy_complete_edge(ngf, e, nu, nv);
	}
    }
}

long
gr_factorial(long n)
{
    if(n < 0)
	return 0;
    else if(n == 0)
	return 1;
    else
	return n * gr_factorial(n-1);
}




