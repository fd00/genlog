#include "gr_parser.h"
#include "gr_structure.h"
#include "gr_operations.h"
#include "gr_window.h"
#include "timek.h"
#include "alist.h"

static int error;
static int morder;
static int setpos;
static Hashtable_type HVnumber;

/* parse_text_graph - Given a string that describes the graph, it constructs
                      a new graph. 
         inputs  
                 gf - new Graph structure
		 text - String that describes the Graph. (see documentation)
         returns
                  NO_ERROR - No error found.
		  TYPE_ERROR - Error in the description of the type.
		  NODE_ERROR - Error in the description of the vertices.
		  EDGE_ERROR - Error in the description of the edges.
		  NSHAPE_ERROR - Error in the description of the shapes.
		  NWEIGHT_ERROR - Error in the description of the weight of the
		                  vertices.
		  EWEIGHT_ERROR - Error in the description of the weight of the
		                  edges.
		  NPOSITION_ERROR - Error in the description of the positions
		                    of the vertices.
		  MODE_ERROR - Error in one of the modes of the description.
		  FILE_ORDER_ERROR - Error in the order of the description.

         notes
                 If a error is found the graph should be reseted. 

*/

int
parse_text_graph(GraphFrame *gf,char *text)
{
  char line[SIZELINE];
  int i = 0;
  int mode = NO_MODE;
  Boolean endfile = False, change = True;
  int line_no = 0;
  error = NO_ERROR;
  morder = NO_MODE;
  setpos = False;

  init_hash_table(HVnumber);

  while (!endfile)
    {
      if(change)
	{
	  endfile = read_text_line(text,line,&i);
	  line_no++;
	  if(endfile && !setpos)
	    {
	      /*fprintf(stderr,"Nodes have no position\n");*/
	      save_graph_cur_positions(gf);
	      /*error = NPOSITION_ERROR;*/
	    }
	}
      else
	mode = NO_MODE;

      if(line[0]=='#' || line[0]=='\0')
	continue;

      if(mode == NO_MODE)
	{
	  mode = select_mode_operation(gf,line);
	  change = True;
	}
      else
	change = save_graph_data(gf, mode, line);

      if(error != NO_ERROR)
	break;
    }

  if(error == NO_ERROR )
    {
      if(gf->draw == True)
	{
	  error = move_neg_graph(gf);
	  if(error == RESIZE_WINDOW)
	    {
	      Delete_hash_table(gf->HV);
	      insert_all_vertices_hash_table(gf);
	    }
	  if(gf->scale !=0)
	    {
	      scaling_graph(gf);
	      Delete_hash_table(gf->HV);
	      insert_all_vertices_hash_table(gf);
	    }
	}
      /*refresh_graph_window(gf);*/
    }
  else
    {
      fprintf(stderr,"Error in line %d\n",line_no);
      reset_graph_info(gf);
#ifdef XGR
      clear_graph_area(gf);
#endif
      Delete_hash_table(HVnumber);
      return error;
    }
  Delete_hash_table(HVnumber);
  return error;
}

int
select_mode_operation(GraphFrame *gf, char *line)
{
  char *p;
  char *s = " ";
  char *mode=NULL;
  int type_mode = NO_MODE;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(mode,p); 


  if(mode[0]!='/' || mode[1]!='/')
    {
      fprintf(stderr, "Graph file is wrong\n");
      error = MODE_ERROR;
      free(mode); mode = NULL;
      return type_mode;
    }

  if(!strcmp(mode,"//scale"))
    {
      if(morder != NO_MODE)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      type_mode = MSCALE;
    }
  
  else if(!strcmp(mode,"//type"))
    {
      if(morder != NO_MODE)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MTYPE;
    }
  else if(!strcmp(mode,"//nodes"))
    {
      if(morder != MTYPE)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MNODES;
    }
  else if(!strcmp(mode,"//edges"))
    {
      if(morder != MNODES)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MEDGES;
    }
  else if(!strcmp(mode,"//nodeshapes"))
    {
      if(morder != MEDGES)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MNODESHAPES;
    }
  else if(!strcmp(mode,"//nodeweights"))
    {
      if(morder != MNODESHAPES)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MNODEWEIGHTS;
    }
  else if(!strcmp(mode,"//edgeweights"))
    {
      if(morder != MNODEWEIGHTS)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MEDGEWEIGHTS;
    }
  else if(!strcmp(mode,"//nodepositions"))
    {
      /*start_time();*/
      if(morder != MEDGEWEIGHTS)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MNODEPOSITIONS;
    }
  else if(!strcmp(mode,"//attributes"))
    {
      if(morder != MNODEPOSITIONS)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      morder++;
      type_mode = MATTRIB;

      do { 
	p = strtok(NULL,s);
	if(p)
	  {
	    char *str = NULL;
	    GRStrAllocCopy(str,p); 
	    Insert_linked_list(gf->attrib_name,
			       create_new_node_llist(str));
	  }
      } while(p);
    }
  else if(!strcmp(mode,"//edgeattributes"))
    {
      if(morder != MATTRIB && morder != MNODEPOSITIONS)
	{
	  fprintf(stderr,"Graph File is not in order\n");
	  error = FILE_ORDER_ERROR;
	}
      type_mode = MEDGEATTRIB;
    }
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in file graph :\n %s\n", line);
      error = MODE_ERROR;
    }
  
  free(mode); mode = NULL;
  printf("Mode %d \n", type_mode);
  return type_mode;
}

Boolean
save_graph_data(GraphFrame *gf, int mode, char *line)
{
  if(!strncmp(line,"//",2))
    return False;
  
  if(isspace((int)line[0])|| line[0]=='\0')
    return True;

  if(mode == MSCALE)
    save_graph_scale(gf,line);
  else if(mode == MTYPE)
    save_graph_type(gf,line);
  else if(mode == MNODES)
    save_graph_node(gf,line);
  else if(mode == MEDGES)
    save_graph_edge(gf,line);
  else if(mode == MNODESHAPES)
    save_graph_node_shapes(gf,line);
  else if(mode == MNODEWEIGHTS)
    save_graph_node_weights(gf,line);
  else if(mode == MEDGEWEIGHTS)
    save_graph_edge_weights(gf,line);
  else if(mode == MNODEPOSITIONS)
    save_graph_node_positions(gf,line);
  else if(mode == MATTRIB)
    save_graph_attributes(gf,line);
  else if(mode == MEDGEATTRIB)
    save_graph_edge_attributes(gf,line);

  return True;
}

void
save_graph_edge_attributes(GraphFrame *gf, char *line)
{
  char *p;
  char *s = " ";
  char *ename=NULL,*eattrib=NULL;
  char *nfrom, *nto;
  Node_type *nptr;
  struct pt *v1,*v2;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(ename,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(eattrib,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in attribute of edge %s\n", ename);
      error = EATTRIB_ERROR;
    }
  
  get_names_nodes(line,&nfrom,&nto);

  nptr = Retrieve(HVnumber,(Key_type)nfrom);
  if(nptr)
    v1 = (struct pt *)(nptr->info.gr);
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nfrom);
      error = EATTRIB_ERROR;
    }
  nptr = Retrieve(HVnumber,(Key_type)nto);
  
  if(nptr)
    v2 = (struct pt *)(nptr->info.gr);
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nto);
      error = EATTRIB_ERROR;
    }
  
  nptr = select_nearest_edge(v1,v2,gf);
  if(nptr)
    {
      struct edge *e = (struct edge *)(nptr->info.gr);
      e->attrib = eattrib ? get_char_edge_attribute(eattrib) : '\0';
      if(e->attrib == 'z')
	{
	  if(v2->pick == False)
	    {
	      v2->pick = True;
	      Insert_linked_list(gf->lobjects,
				 create_new_node_llist((char *)v2));
	    }
	}
    }
  else
    {
      fprintf(stderr, "Edge %s has not been created \n", ename);
      error = EATTRIB_ERROR;
    }

  free(nto);  free(nfrom);  free(ename); free(eattrib);
  nto= NULL; nfrom= NULL; ename= NULL; eattrib = NULL;
}

void
save_graph_attributes(GraphFrame *gf, char *line)
{
  char *p;
  char *s = "\\";
  char *nname=NULL;
  LNode_type *list_attr = init_assoc_list();
  Node_type *nptr;
  int i=1;
  
  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nname,p); 
  do
    {
      p = strtok(NULL,s);
      if(p)
	{
	  char *aname = find_info_pos(gf->attrib_name,i);
	  i++;
	  add_object_assoc_list(list_attr,aname,p);
	}
    }
  while(p);

  if(error != NATTRIB_ERROR)
    {
      nptr = Retrieve(HVnumber,(Key_type)nname);
      if(nptr)
	{
	  struct pt *v = (struct pt *)(nptr->info.gr);

	  if(v)
	    {
	      if(v->attrib == NULL)
		v->attrib = list_attr;
	    }	      
	}
      else
	{
	  fprintf(stderr, "Node %s can not been created \n", nname);
	  error = NPOSITION_ERROR;
	}
    }
  else
    {
      delete_assoc_list(list_attr); list_attr = NULL;
    }

  free(nname);  nname = NULL;
}

void
save_graph_scale(GraphFrame *gf, char *line)
{
  char *p;
  char *s = " ";
  char *nscale = NULL;
  int scale;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nscale,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in scale of graph\n");
      error = SCALE_ERROR;
    }

  scale = atoi(nscale);
  
  if(scale >= LSCALE && scale <= HSCALE)
    {
      gf->scale = (short)scale;
      scaling_graph(gf) ;
    }
  else
    gf->scale =  0;

  free(nscale); nscale = NULL;
}

void
save_graph_type(GraphFrame *gf, char *line)
{
  char *p;
  char *s = " ";
  char *ntype = NULL;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(ntype,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in type of graph\n");
      error = TYPE_ERROR;
    }
  
  if(!strcmp(ntype, "u"))
    gf->graph_dir = UNDIRECTED;
  else if(!strcmp(ntype, "d"))
     gf->graph_dir = DIRECTED;
  else
    {
      gf->graph_dir = NONE;
      fprintf(stderr,"Graph Type is unknown\n");
      error = TYPE_ERROR;
    }
  free(ntype); ntype = NULL;
}

void
save_graph_node(GraphFrame *gf, char *line)
{
  if(!strncmp(line,"/!default",9))
    create_sequence_nodes(gf,line);
  else 
    save_name_node(gf,line);
}
void
save_graph_edge(GraphFrame *gf, char *line)
{
  char *nfrom, *nto;
  struct pt *v1 = NULL, *v2 = NULL;
  struct edge *e;
  Node_type *nptr;
  
  get_names_nodes(line,&nfrom,&nto);

  nptr = Retrieve(HVnumber,(Key_type)nfrom);
  if(!nptr)
    {
      fprintf(stderr,"Node %s does not exist \n", nfrom);
      error = EDGE_ERROR;
    }
  else
      v1 = (struct pt *)nptr->info.gr;

  nptr = Retrieve(HVnumber,(Key_type)nto);
  if(!nptr)
    {
      fprintf(stderr,"Node %s does not exist \n", nto);
      error = EDGE_ERROR;
    }
  else
      v2 = (struct pt *)nptr->info.gr;
   
  free(nfrom);  free(nto); nfrom = NULL; nto=NULL;

  if(v1 == NULL || v2 == NULL)
    {
      fprintf(stderr,"Edge can not be created, error in a vertex \n");
      error = EDGE_ERROR;
    }
  else if(!find_edge(gf, v1,v2))
    {
      e = create_edge_memory(gf,v1,v2);
      insert_e( e,gf );

      if(gf->graph_dir == UNDIRECTED)
	{
	  LNode_type *ptr;
	  ptr= create_new_node_llist((char *)e);
	  Insert_linked_list(v1->ledges_in,ptr);

	  ptr= create_new_node_llist((char *)e);
	  Insert_linked_list(v2->ledges_in,ptr);
	}
      else if(gf->graph_dir == DIRECTED)
	{
	  LNode_type *ptr,*ptr2;
	  ptr= create_new_node_llist((char *)e);
	  Insert_linked_list(v1->ledges_out,ptr);
	  ptr2= create_new_node_llist((char *)e);
	  Insert_linked_list(v2->ledges_in,ptr2);
     	}
    }
}

void
save_graph_node_shapes(GraphFrame *gf, char *line)
{
  if(!strncmp(line,"/!default",9))
    set_all_nodes_shape(gf,line);
  else 
    set_node_shape(gf,line);
}
void
save_graph_node_weights(GraphFrame *gf, char *line)
{
  if(!strncmp(line,"/!default",9))
    set_all_node_weight(gf,line);
  else 
    set_node_weight(gf,line);
}
void
save_graph_edge_weights(GraphFrame *gf, char *line)
{
  if(!strncmp(line,"/!default",9))
    set_all_edge_weight(gf,line);
  else 
    set_edge_weight(gf,line);
}

void
save_graph_node_positions(GraphFrame *gf, char *line)
{
  if(!strncmp(line,"/!default",9))
    set_all_node_position(gf,line);
  else 
    set_node_position(gf,line);
}

void
save_graph_cur_positions(GraphFrame *gf)
{
  LNode_type *ptr = gf->list_vertices->next;
  Dimension maxh;
  int inix=0, iniy=0, iniw=0, inih=0;

  find_graph_bounding_box(gf, &inix, &iniy,&iniw,&inih);
  /*printf("BOUNDING BOX x= %d y=%d w=%d h=%d\n", inix,iniy, iniw,inih);*/
  maxh = iniw > inih ? iniw : inih;
  increase_window_size(gf,&maxh,&maxh);

  for(;ptr != gf->list_vertices; ptr=ptr->next)
    {
      PNODE v = (PNODE)ptr->info;
      verify_insert_v(gf,v);
    }
  /*printf("Window size %d %d , PIXMAP size %d %d\n", gf->w, gf->h, gf->wp, 
	 gf->hp);*/
}

void
set_all_node_position(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " =";
  char *nfunction=NULL;

  p = strtok(line,s);
  p = strtok(NULL,s);
  if(p)
    if(strcmp(p,"function"))
      {
	fprintf(stderr, "Error in default of node positions\n");
	error = NPOSITION_ERROR;
      }
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nfunction,p); 

  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in default of node positions\n");
      error = NPOSITION_ERROR;
    }

  setpos = True;

  if(!strcmp(nfunction,"circular"))
    {
      compute_circular_positions(gf);
    }
  else if(!strcmp(nfunction,"horizontal"))
    {
      compute_horizontal_positions(gf);
    }
  else if(!strcmp(nfunction,"diagonal"))
    {
      compute_diagonal_positions(gf);
    }
  else if(!strcmp(nfunction,"random"))
    {
      compute_random_positions(gf);
    }  
  else if(!strcmp(nfunction,"pretty"))
    {
      compute_pretty_positions(gf,True);
    }
  else if(!strcmp(nfunction,"linear"))
    {
      compute_linear_positions(gf);
    }
  else
    {
      fprintf(stderr,"Error in function for Node positions\n");
      error = NPOSITION_ERROR;
    }
  if(nfunction) { free(nfunction); nfunction = NULL; }
}
void
set_node_position2(GraphFrame *gf,char *line)
{  
  char *p;
  char *s = " ";
  char *nname=NULL,*nposition=NULL;
  char *nx, *ny;
  struct pr_pos loc;
  Node_type *nptr;
  static int i=0;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nname,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nposition,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in position of node %s\n", nname);
      error = NPOSITION_ERROR;
    }


  get_names_nodes(nposition,&nx,&ny);
  loc.x = atoi(nx);
  loc.y = atoi(ny);

  nptr = Retrieve(HVnumber,(Key_type)nname);
  if(nptr)
    {
      struct pt *v = (struct pt *)(nptr->info.gr);
      /*Node_type *tptr;*/
      setpos = True;
      i++; 

      if(i%100 == 0)
	{
	  prn_time();
	}

      if(v)
	{
	  v->loc.x = loc.x;
	  v->loc.y = loc.y;
	  verify_insert_v(gf,v);
	  /*
	  tptr = select_nearest_vertex(&loc,gf );
	  if(!tptr)
	    insert_v(v,gf);
	  else
	    {
	      fprintf(stderr, "Node %s has repeated position \n", nname);
	      error = NPOSITION_ERROR;
	    } */
	}
    }
  else
    {
      fprintf(stderr, "Node %s can not been created \n", nname);
      error = NPOSITION_ERROR;
    }
  free(nx); free(ny); free(nname); free(nposition);
  nx=NULL; ny=NULL; nname=NULL; nposition=NULL;
}
void
set_node_position(GraphFrame *gf,char *line)
{  
  char *p;
  char *s = " ";
  char *nname=NULL,*nposition=NULL;
  char *nx, *ny;
  struct pr_pos loc;
  Node_type *nptr;
  static int i=0;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nname,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nposition,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in position of node %s\n", nname);
      error = NPOSITION_ERROR;
    }


  get_names_nodes(nposition,&nx,&ny);
  loc.x = atoi(nx);
  loc.y = atoi(ny);

  nptr = Retrieve(HVnumber,(Key_type)nname);
  if(nptr)
    {
      struct pt *v = (struct pt *)(nptr->info.gr);
      i++; 

      if(i%100 == 0)
	{
	  /*printf("Inserting %d vertices\n", i);*/
	  /*prn_time();*/
	}

      if(v)
	{
	  v->loc.x = loc.x;
	  v->loc.y = loc.y;
	}
    }
  else
    {
      fprintf(stderr, "Node %s can not been created \n", nname);
      error = NPOSITION_ERROR;
    }
  free(nx); free(ny); free(nname); free(nposition);
  nx=NULL; ny=NULL; nname=NULL; nposition=NULL;
}
void
set_all_edge_weight(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " =";
  char *nweight=NULL;
  LNode_type *ptr;

  p = strtok(line,s);
  p = strtok(NULL,s);
  if(p)
    if(strcmp(p,"weight"))
      {
	fprintf(stderr, "Error in default of edge weights\n");
	error = EWEIGHT_ERROR;
      }
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nweight,p); 

  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in default of edge weights\n");
      error = EWEIGHT_ERROR;
    }
  

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->next != v->ledges_in; ei=ei->next)
	    {
	      struct edge *e = (struct edge *)ei->next->info;
	      if(e->weight) {
		free(e->weight); e->weight = NULL;
	      }
	      if(nweight)
		  GRStrAllocCopy(e->weight,nweight); 
	    }
	}
    }
  free(nweight); nweight = NULL;
}
void
set_edge_weight(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " ";
  char *ename=NULL,*eweight=NULL;
  char *nfrom, *nto;
  Node_type *nptr;
  struct pt *v1,*v2;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(ename,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(eweight,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in weight of edge %s\n", ename);
      error = EWEIGHT_ERROR;
    }
  
  get_names_nodes(line,&nfrom,&nto);

  nptr = Retrieve(HVnumber,(Key_type)nfrom);
  if(nptr)
    v1 = (struct pt *)(nptr->info.gr);
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nfrom);
      error = EWEIGHT_ERROR;
    }
  nptr = Retrieve(HVnumber,(Key_type)nto);
  
  if(nptr)
    v2 = (struct pt *)(nptr->info.gr);
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nto);
      error = EWEIGHT_ERROR;
    }
  
  nptr = select_nearest_edge(v1,v2,gf);
  if(nptr)
    {
      struct edge *e = (struct edge *)(nptr->info.gr);
      if(e->weight) {
	free(e->weight); e->weight=NULL;
      }
      if(eweight)
	  GRStrAllocCopy(e->weight,eweight); 
    }
  else
    {
      fprintf(stderr, "Edge %s has not been created \n", ename);
      error = EWEIGHT_ERROR;
    }

  free(nto);  free(nfrom);  free(ename); free(eweight);
  nto= NULL; nfrom= NULL; ename= NULL; eweight = NULL;
}

void
set_all_node_weight(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " =";
  char *nweight = NULL;
  LNode_type *ptr;

  p = strtok(line,s);
  p = strtok(NULL,s);
  if(p)
    if(strcmp(p,"weight"))
      {
	fprintf(stderr, "Error in default of node weights\n");
	error = NWEIGHT_ERROR;
      }
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nweight,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in default of node weights\n");
      error = NWEIGHT_ERROR;
    }
  

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(v)
	{
	  if(v->weight)
	    {
	      fprintf(stderr, "Node %s has already weight\n", v->label);
	      error = NWEIGHT_ERROR;
	    }
	  else
	      GRStrAllocCopy(v->weight,nweight); 

	}
    }
  free(nweight); nweight = NULL;
}
void
set_node_weight(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " ";
  char *nname=NULL,*nweight=NULL;
  Node_type *nptr;
  struct pt *v = NULL;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nname,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nweight,p); 

  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in weight of node %s\n", nname);
      error = NWEIGHT_ERROR;
    }
    
  nptr = Retrieve(HVnumber,(Key_type)nname);
  if(nptr)
    {
      v = (struct pt *)(nptr->info.gr);
      if(v->weight)
	{
	  fprintf(stderr, "Node %s has already weight\n", v->label);
	  error = NWEIGHT_ERROR;
	}
      else if(nweight)
	  GRStrAllocCopy(v->weight,nweight); 
      else
	v->weight = NULL;
    }
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nname);
      error = NWEIGHT_ERROR;
    }
  free(nweight); nweight = NULL;
  free(nname); nname = NULL;
  
}

void
set_all_nodes_shape(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " =";
  char *nshape = NULL;
  char ishape;
  LNode_type *ptr;

  p = strtok(line,s);
  p = strtok(NULL,s);
  if(p)
    if(strcmp(p,"shape"))
      {
	fprintf(stderr, "Error in default of node shapes\n");
	error = NSHAPE_ERROR;
      }
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nshape,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in default of node shapes\n");
      error = NSHAPE_ERROR;
    }
  
  ishape = get_shape_number(nshape);

  free(nshape); nshape = NULL;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      struct pt *v = (struct pt *)ptr->next->info;
      if(v)
	v->shape = ishape;
    }
}
void
set_node_shape(GraphFrame *gf,char *line)
{
  char *p;
  char *s = " ";
  char *nname=NULL,*nshape=NULL;
  char ishape;
  Node_type *nptr;
  struct pt *v = NULL;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nname,p); 

  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nshape,p); 
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in shape of node %s\n", nname);
      error = NSHAPE_ERROR;
    }
  
  ishape = get_shape_number(nshape);
  
  nptr = Retrieve(HVnumber,(Key_type)nname);
  if(nptr)
    {
      v = (struct pt *)(nptr->info.gr);
      v->shape = ishape;
    }
  else
    {
      fprintf(stderr, "Node %s has not been created \n", nname);
      error = NSHAPE_ERROR;
    }
  free(nname); nname = NULL;
  free(nshape); nshape = NULL;
}

void
get_names_nodes(char *line,char **nfrom,char **nto)
{
  int i, p1=-1,p2=-1,p3=-1;
  for(i = 0 ; i< strlen(line) ; i++)
    {
      if(line[i] == '(')
	p1 = i;
      else if(line[i] == ',')
	p2 = i;
      else if(line[i] == ')')
	p3 = i;
    }
  if(p1 == -1 || p2 == -1 || p3 == -1 || p1 == p2 || p2 == p3)
    {
      fprintf(stderr, "Error reading a edge\n");
      error =  EDGE_ERROR;
      return;
    }
  *nfrom = (char *) malloc(p2-p1);
  if(!nfrom) {
      fprintf(stderr,"Out of Memory get_names_nodes\n");
      abort();
  }
  *nto = (char *) malloc(p3-p2);
  if(!nto) {
      fprintf(stderr,"Out of Memory get_names_nodes\n");
      abort();
  }

  strncpy(*nfrom, &(line[p1+1]),p2-p1-1);
  (*nfrom)[p2-p1-1]='\0';
  strncpy(*nto, &(line[p2+1]),p3-p2-1);
  (*nto)[p3-p2-1]='\0';

}
void 
create_sequence_nodes(GraphFrame *gf, char *line)
{
  char *p;
  char *s = " ";
  int ist=0,iend=-1,istep=0;
  unsigned int i = 0;
  strtok(line,s);

  while((p = strtok(NULL,s)) != NULL)
    {
      if(i == 3)
	ist = atoi(p);
      if(i == 5)
	iend = atoi(p);
      if(i == 7)
	istep = atoi(p);
      i++;
    }
  if(iend < ist)
    {
      fprintf(stderr, "Error: Nodes can not be created\n");
      error = NODE_ERROR;
      return;
    }
  gf->nvertex = iend;
  for(i=ist; i <= iend ; i+=istep)
    {
      struct pt *v;
      LNode_type *vptr;
      char nname[MDIGITS];
      sprintf(nname,"%d",i);
      v = create_node_memory(nname);
      (gf->count_vertex)++; /* count one more vertex */
      v->num = i;
      vptr = create_new_node_llist((char *)v);

      /*Insert new vertex in a list */
      Insert_linked_list(gf->list_vertices,vptr); 

      insert_hash_vertex_name(gf,v->label,v);
    }
}

void 
save_name_node(GraphFrame *gf, char *line)
{
  struct pt *v;
  char *p;
  char *s = " ";
  char *nname=NULL,*nnum=NULL;

  p = strtok(line,s);
  if(p)
      GRStrAllocCopy(nnum,p); 
  p = strtok(NULL,s);
  if(p)
      GRStrAllocCopy(nname,p); 
  else
    {
      fprintf(stderr, "Error in name of node %s\n", nnum);
      error = NODE_ERROR;
    }
  p = strtok(NULL,s);
  if(p)
    {
      fprintf(stderr, "Error in name of node %s\n", nname);
      error = NODE_ERROR;
    }

  if(nname)
    {
      v = create_node_memory(nname);
      free(nname); nname = NULL;
    }
  else
    {
      fprintf(stderr, "Error in name of node %s\n", nnum);
      error = NODE_ERROR;
      if(nnum) { free(nnum); nnum = NULL; }
      return;
    }

  (gf->count_vertex)++; /* count one more vertex */

  v->num = atoi(nnum);

  if(v->num > gf->nvertex)
    gf->nvertex = v->num;

  
  /*Insert new vertex in a list */
  Insert_linked_list(gf->list_vertices,create_new_node_llist((char *)v)); 

  insert_hash_vertex_name(gf,v->label,v);
  insert_hash_info(HVnumber,nnum,(char *)v);
  
  free(nnum); nnum = NULL;

}
Boolean
read_text_line(char *text,char *line, int *i)
{
  int k,j=0;
  k = *i;
  while((j < SIZELINE-1) && (text[k]!= '\n')&&(text[k]!='\0'))
      line[j++]=text[k++];

  line[j]='\0';

  *i = k + 1;

  if(text[k]=='\0')
   return True;
  else
    return False;
}


