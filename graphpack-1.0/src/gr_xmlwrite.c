#include "gr_xmlwrite.h"
#include "gr_write.h"
#include "alist.h"

#define XML_DIGITS 20
#define XML_LINE 1024

void
write_xml_head(BUFFER *buffer);
void
write_start_xml_graph(BUFFER *buffer,GraphFrame *gf,int type);
void
write_xml_nodes(BUFFER *buffer,GraphFrame *gf,int type);
void
write_xml_edges(BUFFER *buffer,GraphFrame *gf);
void
write_xml_edge_attribute(BUFFER *buffer,GraphFrame *gf, PEDGE e);
void
write_end_xml_graph(BUFFER *buffer,GraphFrame *gf);

char *
get_xml_text_graph(GraphFrame *gf,long *len,int type)
{
  BUFFER *buffer;
  char *text;
  /*int i=0;*/

  if(gf->count_vertex <=0)
    return NULL;

  buffer = (BUFFER *)malloc(sizeof(BUFFER));
  if(!buffer) {
      fprintf(stderr,"Out of Memory get_xml_text_graph\n");
      abort();
  }
  initialize_buf(buffer);
  /*start_time();  */
  write_xml_head(buffer);
  write_start_xml_graph(buffer,gf,type);
  write_xml_nodes(buffer,gf,type);
  write_xml_edges(buffer,gf);
  write_end_xml_graph(buffer,gf);
  *len = buffer->indbuf;
  text = buffer->buf;
  free(buffer); buffer = NULL;
  return (text);
}
void
write_xml_head(BUFFER *buffer)
{
  write_in(buffer, "<?xml version=\"1.0\"?>\n");
  write_in(buffer, "<!DOCTYPE graph SYSTEM \"xgmml.dtd\">\n");
  write_in(buffer, "<!-- Graph File Generated for GraphPack 2.0 -->\n");
}
void
write_start_xml_graph(BUFFER *buffer,GraphFrame *gf, int type)
{

  write_in(buffer,"<graph");

  if(gf->graph_dir == DIRECTED)
    write_in(buffer," directed=\"1\"");
  else if(gf->graph_dir == UNDIRECTED)
    write_in(buffer," directed=\"0\"");

  if(type == GR_STANDARD_FORMAT)
  {
    write_in(buffer," graphic=\"1\"");
    write_in(buffer," Layout=\"points\"");
  }

  if(gf->rootv != NULL)
  {
      char strscale[XML_LINE];
      sprintf(strscale," Rootnode=\"%d\"",gf->rootv->num);
      write_in(buffer,strscale);
  }
  if(gf->scale !=0 && type == GR_STANDARD_FORMAT)
    {
      char strscale[XML_LINE];
      sprintf(strscale," Scale=\"%d\"",gf->scale);
      write_in(buffer,strscale);
    }
  write_in(buffer,">\n");
}

void
write_xml_graphic(BUFFER *buffer,GraphFrame *gf, PNODE v)
{
  char strtype[XML_LINE];
  write_in(buffer,"<graphics");
  sprintf(strtype," type=\"%s\" x=\"%d\" y=\"%d\" >\n",
	  get_string_shape_file(v),v->loc.x, v->loc.y);
  write_in(buffer,strtype);
  write_in(buffer,"</graphics>\n");
}

/*void
write_xml_attributes(BUFFER *buffer,GraphFrame *gf, PNODE v)
{
  LNode_type *ptr;
  if(!v->attrib)
    return;
  for(ptr= v->attrib->back; ptr != v->attrib ; ptr = ptr->back)
    {
      char *aname = NULL;
      char *avalue = NULL;
      char strattr[XML_LINE];
      if(!get_node_info_assoc_list(ptr,&aname,&avalue))
	{
	  if(!strcmp(aname,"title"))
	    {
	      char *xmlvalue = HTXMLEscape(avalue);

	      sprintf(strattr,
	          "<att name=\"%s\" value=\"%s\">\n</att>\n",aname,xmlvalue);
	      free(xmlvalue); xmlvalue = NULL;
	    }
	  else
	    sprintf(strattr,
		    "<att name=\"%s\" value=\"%s\">\n</att>\n",aname,avalue);
	  write_in(buffer,strattr);
	}
    }
}*/

void
write_xml_attributes(BUFFER *buffer,GraphFrame *gf, PNODE v)
{
  LNode_type *ptr;
  if(!v->attrib)
    return;
  for(ptr= v->attrib->back; ptr != v->attrib ; ptr = ptr->back)
    {
      char *aname = NULL;
      char *avalue = NULL;
      
      char strattr[XML_LINE];
      if(!get_node_info_assoc_list(ptr,&aname,&avalue))
	{
	    char *xmlvalue = HTXMLEscape(avalue);
	    sprintf(strattr,"<att name=\"%s\" value=\"" , aname);
	    write_in(buffer,strattr);
	    write_in(buffer,xmlvalue);
	    sprintf(strattr,"\"></att>\n");
	    write_in(buffer,strattr);
	    free(xmlvalue); xmlvalue = NULL;
	}
    }
}


void
write_xml_node(BUFFER *buffer,GraphFrame *gf, PNODE v, int type)
{
    if(!v || !gf || v->num < 0)
	return;
  write_in(buffer,"<node");
  if(v->num >= 0)
    {
      char strnum[XML_LINE];
      sprintf(strnum," id=\"%d\"",v->num);
      write_in(buffer,strnum);
    }
  if(v->label)
    {
      char *xmllabel = HTXMLEscape(v->label);
      write_in(buffer," label=\"");
      write_in(buffer,xmllabel);
      write_in(buffer,"\"\n\t");
      free(xmllabel); xmllabel = NULL;
    }
  if(v->weight)
    {
      write_in(buffer," weight=\"");
      write_in(buffer,v->weight);
      write_in(buffer,"\"");
    }
  else
    write_in(buffer," weight=\"0\"");

  write_in(buffer,">\n");
  if(type == GR_STANDARD_FORMAT)
      write_xml_graphic(buffer,gf,v);
  write_xml_attributes(buffer,gf,v);
  write_in(buffer,"</node>\n");
}
void
write_xml_nodes(BUFFER *buffer,GraphFrame *gf, int type)
{
  LNode_type *ptr;
  for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; ptr = ptr->back)
    {
      PNODE v = (struct pt *)ptr->info;
      if(v)
	{
	  write_xml_node(buffer,gf,v,type);
	}
    }
}
void
write_xml_edge_attribute(BUFFER *buffer,GraphFrame *gf, PEDGE e)
{
  if(e->attrib != '\0')
    {
      char stredgeatt[XML_LINE];
      char *strea = HTXMLEscape(get_string_edge_attribute(e));
      if(strlen(strea) + 20 > XML_LINE)
	  strea[XML_LINE - 20] = '\0';
      sprintf(stredgeatt,"<att value=\"%s\"></att>\n", strea);
      write_in(buffer,stredgeatt);
      free(strea); strea = NULL;
    }
}
void
write_xml_edge(BUFFER *buffer,GraphFrame *gf, PEDGE e)
{
  PNODE v1, v2;
  char stredge[XML_LINE];
  v1 = e->from;
  v2 = e->to;
  write_in(buffer,"<edge");
  sprintf(stredge," source=\"%d\" target=\"%d\" weight=\"%s\" ",
	  v1->num,v2->num,e->weight ? e->weight : "0");
  if(e->label) {
      char *strea = HTXMLEscape(e->label);
      int len = strlen(strea) + 10;
      int elen = strlen(stredge);
      if(len + elen > XML_LINE)
	  strea[XML_LINE - elen - 1] = '\0';
      strcat(stredge,"\nlabel=\"");
      strcat(stredge,strea);
      strcat(stredge,"\" ");
      free(strea); strea = NULL;
  }
  strcat(stredge,">\n"); 
  write_in(buffer,stredge);
  write_xml_edge_attribute(buffer,gf,e);
  write_in(buffer,"</edge>\n");
}

void
write_xml_edges(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;

  for(ptr= gf->list_vertices->back ;ptr != gf->list_vertices ; ptr = ptr->back)
    {
      PNODE v = (struct pt *)ptr->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in->back; ei != v->ledges_in; ei=ei->back)
	    {
	      PEDGE e = (struct edge *)ei->info;
	      if(e)
		e->touch = False;
	    }
	}
    }

  for(ptr= gf->list_vertices->back; ptr != gf->list_vertices ; ptr = ptr->back)
    {
      PNODE v = (struct pt *)ptr->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in->back; ei != v->ledges_in; ei=ei->back)
	    {
	      PEDGE e = (struct edge *)ei->info;
	      if(e && e->touch == False)
		{
		  write_xml_edge(buffer,gf, e);
		  e->touch = True;
		}
	    }
	}
    }
}

void
write_end_xml_graph(BUFFER *buffer,GraphFrame *gf)
{
  write_in(buffer,"</graph>\n");
}



int
is_escaped_char(char c)
{
  char charesc[] = { '>', '<', '&', '\"','\''}; /* " */
  int i,n = 5;
  for(i=0;i<n;i++)
    {
      if(c == charesc[i])
	return 1;
    }
  return 0;
}

char *
HTXMLEscape(char * str)
{
  char *samp = "amp;"; int iamp = 4;
  char *squot = "quot;"; int iquot = 5;
  char *slt = "lt;"; int ilt = 3;
  char *sgt = "gt;"; int igt = 3;
  char *sapos = "apos;"; int iapos = 5;
  int b,c;
  char *q = str;
  char *t = NULL,*result = NULL;
  if(!str)
    return NULL;
  b = c = strlen(str);
  while(*q!='\0')
    {
      if((*q) < 32 || (*q) >= 128)
	{
	  c = 0;
	  break;
	}
      if(!is_escaped_char(*q))
	{
	  q++;
	  continue;
	}
      if(*q=='>')
	{
	  q++;
	  c+=igt;
	  continue;
	}      
      if(*q=='<')
	{
	  q++;
	  c+=ilt;
	  continue;
	}
      if(*q=='\"') /*"*/
	{
	  q++;
	  c+=iquot;
	  continue;
	}
      if(*q=='\'')
	{
	  q++;
	  c+=iapos;
	  continue;
	}
      if(*q=='&')
	{
	  char *r = q+1;
	  if(!strncmp(r,samp,iamp))
	    {
	      q+=iamp+1;
	      continue;
	    }
	  if(!strncmp(r,squot,iquot))
	    {
	      q+=iquot+1;
	      continue;
	    }
	  if(!strncmp(r,slt,ilt))
	    {
	      q+=ilt+1;
	      continue;
	    }
	  if(!strncmp(r,sgt,igt))
	    {
	      q+=igt+1;
	      continue;
	    }
	  if(!strncmp(r,sapos,iapos))
	    {
	      q+=iapos+1;
	      continue;
	    }
	  c+=iamp;
	  q++;
	}
    }
  if((result = calloc(1,c+1)) == NULL)
    {
      fprintf(stderr, "Out of Memory in HTXMLEscape");
      abort();
    }
  if(b==c || c == 0)
    {
      //strcpy(result, str);
		strncpy(result,str,c+1);
      return result;
    }
  q = str; t=result;
  while(*q!='\0')
    {
      if(!is_escaped_char(*q))
	{
	  *t=*q;
	  q++; t++;
	  continue;
	}
      if(*q=='>')
	{
	  strcpy(t,"&gt;");
	  q++; t+=igt+1;
	  continue;
	}      
      if(*q=='<')
	{
	  strcpy(t,"&lt;");
	  q++; t+=ilt+1;
	  continue;
	}
      if(*q=='\"') /* " */
	{
	  strcpy(t,"&quot;");
	  q++; t+=iquot+1;
	  continue;
	}
      if(*q=='\'')
	{
	  strcpy(t,"&apos;");
	  q++; t+=iapos+1;
	  continue;
	}
      if(*q=='&')
	{
	  char *r = q+1;
	  if(!strncmp(r,samp,iamp))
	    {
	      strcpy(t,"&amp;");
	      t+=iamp+1;
	      q+=iamp+1;
	      continue;
	    }
	  if(!strncmp(r,squot,iquot))
	    {
	      strcpy(t,"&quot;");
	      t+=iquot+1;
	      q+=iquot+1;
	      continue;
	    }
	  if(!strncmp(r,slt,ilt))
	    {
	      strcpy(t,"&lt;");
	      t+=ilt+1;
	      q+=ilt+1;
	      continue;
	    }
	  if(!strncmp(r,sgt,igt))
	    {
	      strcpy(t,"&gt;");
	      t+=igt+1;
	      q+=igt+1;
	      continue;
	    }
	  if(!strncmp(r,sapos,iapos))
	    {
	      strcpy(t,"&apos;");
	      t+=iapos+1;
	      q+=iapos+1;
	      continue;
	    }
	  strcpy(t,"&amp;");
	  t+=iamp+1;
	  q++;
	}
    }
  return result;
}



