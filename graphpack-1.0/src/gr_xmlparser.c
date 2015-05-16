#include "gr_xmlparser.h"
#include "expat.h"
#include "gr_parser.h" /* ERROR CODES */
#include "gr_structure.h"
#include "gr_operations.h"
#include "alist.h"

enum xml_graph_tags {XML_GR_NULL, XML_GR_GRAPH, XML_GR_NODE, XML_GR_GRAPHICS, 
		     XML_GR_EDGE, XML_GR_ATT};

typedef enum xml_graph_tags xtags;

enum graph_functions {NO_FUNCTION,GR_POINTS,GR_LINEAR, GR_CIRCULAR, 
			 GR_HORIZONTAL,GR_DIAGONAL, GR_RANDOM, GR_PRETTY};

typedef enum graph_functions gfunctions;

typedef struct xml_Data {
  XML_Parser parser;
  xtags wtag;
  PNODE cv;
  PEDGE ce;
  GraphFrame *gf;
  gfunctions gfunc;
  Hashtable_type HVnumber;
  int line_no;
  int error;
} XDATA;
  
typedef XDATA *PXDATA;

xtags
find_tag(const char *name);

void
set_graph_parameters(PXDATA px, const char **atts);
void
set_node_parameters(PXDATA px, const char **atts);

void
set_edge_parameters(PXDATA px, const char **atts);

void
set_element_graphics(PXDATA px, const char **atts);
void
set_element_attributes(PXDATA px, const char **atts);
void
set_node_positions(PXDATA px);

xtags
find_tag(const char *name)
{
  if(!strcasecmp(name,"graph"))
    return XML_GR_GRAPH;
  else if(!strcasecmp(name,"node"))
    return XML_GR_NODE;
  else if(!strcasecmp(name,"graphics"))
    return XML_GR_GRAPHICS;
  else if(!strcasecmp(name,"edge"))
    return XML_GR_EDGE;
  else if(!strcasecmp(name,"att"))
    return XML_GR_ATT;
  return XML_GR_NULL;
}

void
set_graph_parameters(PXDATA px, const char **atts)
{
  GraphFrame *gf;
  int num =0,i=0;

  if(!atts || !px)
    return;

  gf= px->gf;

  gf->graph_dir = DIRECTED; /* Default */

  while(atts[i])
    {
      if(!strcasecmp(atts[i], "directed"))
	{
	  i++;
	  if(!strcmp(atts[i],"1"))
	    gf->graph_dir = DIRECTED;
	  else if(!strcmp(atts[i],"0"))
	    gf->graph_dir = UNDIRECTED;
	  else
	    {
	      gf->graph_dir = NONE;
	      fprintf(stderr,"Graph Type is unknown\n");
	      px->error = TYPE_ERROR;
	      px->line_no = XML_GetCurrentLineNumber(px->parser);
	    }
	}
      else if(!strcasecmp(atts[i], "Scale"))
	{
	  int scale;
	  i++;
	  scale = atoi(atts[i]);
	  if(scale >= LSCALE && scale <= HSCALE)
	    {
	      gf->scale = (short)scale;
	      scaling_graph(gf) ;
	    }
	  else
	    gf->scale =  0;
	}
      else if(!strcasecmp(atts[i], "Layout"))
	{
	  i++;
	  if(!strcasecmp(atts[i],"points"))
	      px->gfunc = GR_POINTS;
	  if(!strcasecmp(atts[i],"linear"))
	      px->gfunc = GR_LINEAR;
	  else if(!strcasecmp(atts[i],"circular"))
	      px->gfunc = GR_CIRCULAR;
	  else if(!strcasecmp(atts[i],"horizontal"))
	      px->gfunc = GR_HORIZONTAL;
	  else if(!strcasecmp(atts[i],"diagonal"))
	      px->gfunc = GR_DIAGONAL;
	  else if(!strcasecmp(atts[i],"pretty"))
	      px->gfunc = GR_PRETTY;
	  else if(!strcasecmp(atts[i],"random"))
	      px->gfunc = GR_RANDOM;
	}
      else if(!strcasecmp(atts[i], "graphic"))
	{
	  i++;
	  if(!strcmp(atts[i],"1"))
	    gf->graphic = GRAPHIC_ON;
	  else if(!strcmp(atts[i],"0"))
	    gf->graphic = GRAPHIC_OFF;
	  else
	    {
	      gf->graphic = NONE;
	      fprintf(stderr,"Graphic of Graph is unknown\n");
	      px->error = TYPE_ERROR;
	      px->line_no = XML_GetCurrentLineNumber(px->parser);
	    }
	}
      else if(!strcasecmp(atts[i], "Rootnode"))
      {
	  i++;
	  num = atoi(atts[i]);
	  if(num > 0)
	    gf->rootv = (PNODE)num; /* Temporal value of id of the node */
	  else
	    {
	      gf->rootv = NULL;
	      fprintf(stderr,"Root Node is unknown\n");
	      px->error = TYPE_ERROR;
	      px->line_no = XML_GetCurrentLineNumber(px->parser);
	    }
      }
      i++;
    }
}
void
set_root_node(PXDATA px)
{
  GraphFrame *gf;
  int num = 0;

  if(!px)
    return;

  gf = px->gf;
  num = (int)gf->rootv;
  if(num==0)
      return;
  gf->rootv = find_vertex_by_number(gf,num);
}
void
set_node_positions(PXDATA px)
{
  GraphFrame *gf;
  if(!px)
    return;

  gf = px->gf;

  switch(px->gfunc)
    {
    case NO_FUNCTION:
      insert_all_vertices_hash_table(gf);
      break;
    case GR_POINTS:
      insert_all_vertices_hash_table(gf);
      break;
    case GR_LINEAR:
      compute_linear_positions(gf);
      break;
    case GR_CIRCULAR:
      compute_circular_positions(gf);
      break;
    case GR_HORIZONTAL:
      compute_horizontal_positions(gf);
      break;
    case GR_DIAGONAL:
      compute_diagonal_positions(gf);
      break;
    case GR_PRETTY:
      compute_pretty_positions(gf,True);
      break;
    case GR_RANDOM:
      compute_random_positions(gf);
      break;
    }
}

char *
get_different_label(char *label)
{
    char *nlabel = NULL;
    static int n = 1;
    int len = strlen(label);
    nlabel = malloc(len+5);	
    if(!nlabel)
    {
	fprintf(stderr,"Out of Memory get_different_label\n");
	abort();
    }
    sprintf(nlabel,"%s%d",label,n++);
    return nlabel;
}

void
set_node_parameters(PXDATA px, const char **atts)
{
  GraphFrame *gf;
  int i=0;
  int num = -1;
  char *nnum = NULL;
  char *label=NULL;
  char *weight=NULL;

  if(!atts || !px)
    return;

  gf= px->gf;

  while(atts[i])
    {
      if(!strcasecmp(atts[i], "id"))
	{
	  i++;
	  nnum = (char *)atts[i];
	  num = atoi(atts[i]);
	}
      else if(!strcasecmp(atts[i], "label"))
	{
	  i++;
	  GRStrAllocCopy(label,(char *)atts[i]);
	}
      else if(!strcasecmp(atts[i],"weight"))
	{
	  i++;
	  weight = (char *)atts[i];
	}
      i++;
    } 
 
  if(num >= 0 && !label)
  {
      char slabel[100];
      sprintf(slabel,"%d",num);
      GRStrAllocCopy(label,(char *)slabel);
  }

  if(label)
  {	
      PNODE v = NULL;
      v = (PNODE)get_element_hashed(gf->HVname,label);
      if(v)
      {	
	  char *label2 = get_different_label(label);
	  v = (PNODE)get_element_hashed(gf->HVname,label2);
	  if(v)
	  {
	      fprintf(stderr, "Node %s has already this label\n", v->label);
	      px->error = TYPE_ERROR;
	      px->line_no = XML_GetCurrentLineNumber(px->parser);
	      free(label); label = NULL;
	      free(label2); label2 = NULL;
	  }
	  else
	  {
	      GRStrAllocCopy(label,label2);
	      free(label2); label2 = NULL;
	  }
      }
  }

  if(num >= 0 && label)
    {
      PNODE v = create_node_memory(label);
      v->num = num;

      if(v->num > gf->nvertex)
	gf->nvertex = v->num;

      (gf->count_vertex)++; /* count one more vertex */

      /*Insert new vertex in a list */
      Insert_linked_list(gf->list_vertices,create_new_node_llist((char *)v)); 
      
      insert_hash_vertex_name(gf,v->label,v);

      insert_hash_info(px->HVnumber,nnum,(char *)v);

      /* Set weight of the node */

      if(v->weight)
	{
	  fprintf(stderr, "Node %s has already weight\n", v->label);
	  px->error = NWEIGHT_ERROR;
	  px->line_no = XML_GetCurrentLineNumber(px->parser);
	}
      else if(weight)
	  GRStrAllocCopy(v->weight,weight);
      else
	v->weight = NULL;

      px->cv = v;
      px->wtag = XML_GR_NODE;
      px->ce = NULL;
      
    }

  if(label) {
      free(label); label = NULL;
  }
      
}
void
set_edge_parameters(PXDATA px, const char **atts)
{
  GraphFrame *gf;
  int i=0;
  char *nsource = NULL;
  char *ntarget = NULL;
  char *nweight = NULL;
  char *nlabel = NULL;

  PNODE sv = NULL;
  PNODE tv = NULL;
  PEDGE ne = NULL;
  
  if(!atts || !px)
    return;

  gf= px->gf;

  while(atts[i])
    {
      if(!strcasecmp(atts[i], "source"))
	{
	  i++;
	  nsource = (char *)atts[i];
	}
      else if(!strcasecmp(atts[i], "target"))
	{
	  i++;
	  ntarget = (char *)atts[i];
	}
      else if(!strcasecmp(atts[i], "weight"))
	{
	  i++;
	  nweight = (char *)atts[i];
	}      
      else if(!strcasecmp(atts[i], "label"))
	{
	  i++;
	  nlabel = (char *)atts[i];
	}
      i++;
    }
  
  if(nsource && ntarget)
    {
      Node_type *nptr;
      nptr = Retrieve(px->HVnumber,(Key_type)nsource);
      if(!nptr)
	{
	  fprintf(stderr,"Node %s does not exist \n", nsource);
	  px->error = EDGE_ERROR;
	  px->line_no = XML_GetCurrentLineNumber(px->parser);
	}
      else
	sv= (struct pt *)nptr->info.gr;

      nptr = Retrieve(px->HVnumber,(Key_type)ntarget);
      if(!nptr)
	{
	  fprintf(stderr,"Node %s does not exist \n", ntarget);
	  px->error = EDGE_ERROR;
	  px->line_no = XML_GetCurrentLineNumber(px->parser);
	}
      else
	tv = (struct pt *)nptr->info.gr;

      if(sv == NULL || tv == NULL)
	{
	  fprintf(stderr,"Edge can not be created, error in a vertex \n");
	  px->error = EDGE_ERROR;
	  px->line_no = XML_GetCurrentLineNumber(px->parser);
	}
      else if(!(ne = find_edge(gf, sv,tv)))
	{
	  ne = create_edge_memory(gf,sv,tv);

	  insert_e( ne,gf );

	  if(gf->graph_dir == UNDIRECTED)
	    {
	      LNode_type *ptr;
	      ptr= create_new_node_llist((char *)ne);
	      Insert_linked_list(sv->ledges_in,ptr);

	      ptr= create_new_node_llist((char *)ne);
	      Insert_linked_list(tv->ledges_in,ptr);
	    }
	  else if(gf->graph_dir == DIRECTED)
	    {
	      LNode_type *ptr,*ptr2;
	      ptr= create_new_node_llist((char *)ne);
	      Insert_linked_list(sv->ledges_out,ptr);
	      ptr2= create_new_node_llist((char *)ne);
	      Insert_linked_list(tv->ledges_in,ptr2);
	    }

	}
      if(nweight && ne)
	{
	    if(ne->weight) {
		free(ne->weight); ne->weight = NULL;
	    }
	    GRStrAllocCopy(ne->weight,nweight);
	}      
      if(nlabel && ne)
	{
	    if(ne->label) {
		free(ne->label); ne->label = NULL;
	    }
	    GRStrAllocCopy(ne->label,nlabel);
	}
      px->cv = NULL;
      px->wtag = XML_GR_EDGE;
      px->ce = ne;
    }
}
void
set_element_graphics(PXDATA px, const char **atts)
{
  GraphFrame *gf;
  int i=0;
  char *stype = NULL;
  char *sx = NULL;
  char *sy = NULL;
  static int ic=0;

  if(!atts || !px)
    return;

  gf= px->gf;


  while(atts[i])
    {
      if(!strcasecmp(atts[i], "type"))
	{
	  i++;
	  stype = (char *)atts[i];
	}
      else if(!strcasecmp(atts[i], "x"))
	{
	  i++;
	  sx = (char *)atts[i];
	}
      else if(!strcasecmp(atts[i], "y"))
	{
	  i++;
	  sy = (char *)atts[i];
	}
      i++;
    }

  if(px->wtag == XML_GR_NODE)
    {
      PNODE v = px->cv;
      if(stype)
	{
	  char ishape = get_shape_number(stype);
	  v->shape = ishape; 
	}
      if(sx && sy)
	{
	  struct pr_pos loc;

	  loc.x = atoi(sx);
	  loc.y = atoi(sy);

	  ic++;
	  if(ic%100 == 0)
	    {
	      /*printf("Inserting %d vertices\n", ic);*/
	      /*prn_time();*/
	    }

	  if(v)
	    {
	      v->loc.x = loc.x;
	      v->loc.y = loc.y;
	    }
	}
    }
}
void
set_element_attributes(PXDATA px, const char **atts)
{
  GraphFrame *gf;
  int i=0;
  char *sname = NULL;
  char *svalue = NULL;

  if(!atts || !px)
    return;

  gf= px->gf;

  while(atts[i])
    {
      if(!strcasecmp(atts[i], "name"))
	{
	  i++;
	  sname = (char *)atts[i];
	}
      else if(!strcasecmp(atts[i], "value"))
	{
	  i++;
	  svalue = (char *)atts[i];
	}
      i++;
    }
  if(px->wtag == XML_GR_NODE)
    {
      PNODE v = px->cv;
      if(sname && svalue)
	{
	  if(v->attrib == NULL)
	    v->attrib = init_assoc_list();
	  add_object_assoc_list(v->attrib,sname,svalue);
	}
      else
	{
	  fprintf(stderr, "Attribute was not updated\n");
	  px->error = NATTRIB_ERROR;
	  px->line_no = XML_GetCurrentLineNumber(px->parser);
	}

    }  
  else if(px->wtag == XML_GR_EDGE)
    {
      PEDGE e = px->ce;
      PNODE vt = e->to;
      if(svalue)
	{
	  e->attrib = svalue ? get_char_edge_attribute(svalue) : '\0';
	  if(e->attrib == 'z')
	    {
	      if(vt->pick == False)
		{
		  vt->pick = True;
		  Insert_linked_list(gf->lobjects,
				     create_new_node_llist((char *)vt));
		}
	    }
	}
    }
}

void
set_graph_info(void *userData, const char *name, const char **atts)
{
  PXDATA px = (PXDATA)userData;
  xtags ctag = find_tag(name);
  switch(ctag)
    {
    case XML_GR_GRAPH:
      set_graph_parameters(px,atts);
      break;
    case XML_GR_NODE:
      set_node_parameters(px,atts);
      break;
    case XML_GR_EDGE:
      set_edge_parameters(px,atts);
      break;
    case XML_GR_GRAPHICS:
      set_element_graphics(px,atts);
      break;
    case XML_GR_ATT:
      set_element_attributes(px,atts);
      break;
    case XML_GR_NULL:
      break;
    }
}

void 
startElement(void *userData, const char *name, const char **atts)
{
  PXDATA px = (PXDATA)userData;
  /*int i;
  puts(name);
  i=0;
  while(atts && atts[i++])
    {
      puts(atts[i-1]);
    }
    */
  if(px->error == NO_ERROR)
    set_graph_info(userData, name, atts);
}

void 
endElement(void *userData, const char *name)
{

}

int
parse_xml_text_graph(GraphFrame *gf, char *text, long len)
{
  XML_Parser parser = XML_ParserCreate(NULL);

  PXDATA px = (PXDATA) calloc(1,sizeof(XDATA));
  if(!px) {
      fprintf(stderr,"Out of Memory parse_xml_text_graph\n");
      abort();
  }
  px->gf = gf;
  px->parser = parser;
  px->gfunc = GR_LINEAR;
  init_hash_table(px->HVnumber);
 
  XML_SetUserData(parser,px);
  XML_SetElementHandler(parser, startElement, endElement);

  if (!XML_Parse(parser, text, (size_t)len, 1)) 
    {
      fprintf(stderr,
	      "%s at line %d\n",
	      XML_ErrorString(XML_GetErrorCode(parser)),
	      XML_GetCurrentLineNumber(parser));
      reset_graph_info(gf);

#ifdef XGR
      clear_graph_area(gf);
#endif

      Delete_hash_table(px->HVnumber);
      free(px); px = NULL;
      return 1;
    }

  if(px->error != NO_ERROR)
    {
      fprintf(stderr,"Error in line %d\n",px->line_no);
      reset_graph_info(gf);
#ifdef XGR
      clear_graph_area(gf);
#endif
      Delete_hash_table(px->HVnumber);
      return px->error;
    }
  set_node_positions(px);
  set_root_node(px);
  Delete_hash_table(px->HVnumber);
  XML_ParserFree(parser);
  free(px); px = NULL;
  return 0;
}




