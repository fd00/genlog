#include "gr_graph_fun.h"
#include "gr_write.h"
#include "alist.h"
/*#include "timek.h"*/

/* get_text_graph - Given a graph, generates a string that describes this 
                    graph. (see documentation)

         inputs  
                 gf - Graph Structure
		 len - lenght of the string
         returns
                  String that describes the graph.
         notes
                 If the string is too long, it returns NULL. 
*/

char *
get_text_graph(GraphFrame *gf,long *len)
{
  BUFFER *buffer;
  char *text;
  int i=0;

  if(gf->count_vertex <=0)
    return NULL;

  buffer = (BUFFER *)malloc(sizeof(BUFFER));
  if(!buffer) {
      fprintf(stderr,"Out of Memory get_text_graph\n");
      abort();
  }
  initialize_buf(buffer);
  /*start_time();  */
  write_in(buffer, "# Graph File Generated for GraphPack 2.0\n");
  write_scale(buffer,gf);
  printf("MODE %d\n",i++);
  /*prn_time();*/
  write_type(buffer,gf);
  printf("MODE %d\n",i++);
  write_nodes(buffer,gf);
  printf("MODE %d\n",i++);
  write_edges(buffer,gf);
  printf("MODE %d\n",i++);
  write_nodeshapes(buffer,gf);
  printf("MODE %d\n",i++);
  write_nodeweights(buffer,gf);
  printf("MODE %d\n",i++);
  write_edgeweights(buffer,gf);
  printf("MODE %d\n",i++);
  write_nodepositions(buffer,gf);
  printf("MODE %d\n",i++);
  write_nodeattributes(buffer,gf);
  printf("MODE %d\n",i++);
  write_edgeattributes(buffer,gf);
  printf("MODE %d\n",i++);
  *len = buffer->indbuf;
  text = buffer->buf;
  free(buffer); buffer = NULL;
  return (text);
}

void
write_edgeattributes(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//edgeattributes\n");

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      if(e)
		e->touch = False;
	    }
	}
    }

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      struct pt *v1, *v2;
	      if(e)
		{
		  if(e->touch == False)
		    {
		      char numb[MDIGITS+MDIGITS +5];
		      if(e->attrib != '\0')
			{
			  char *astr = get_string_edge_attribute(e);
			  v1 = e->from;
			  v2 = e->to;
			  sprintf(numb,"(%d,%d) %s", v1->num,v2->num,astr);
			  write_in(buffer,numb);
			  write_in(buffer,"\n");
			  free(astr); astr = NULL;
			}
		      e->touch = True;
		    }
		}
	    }
	}
    }
}

void
write_nodeattributes(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  struct pt *v;
  int f = 0;

  for(ptr= gf->list_vertices; ptr->next != gf->list_vertices ; ptr = ptr->next)
    {
      v = (struct pt *)ptr->next->info;
      if(v && v->attrib)
	{
	  char numb[BUFSIZE];
	  LNode_type *aptr;

	  if(!f)
	    {
	      LNode_type *aptr;
	      char *stratr = numb;
	      numb[0]='\0';
	      if(!is_empty_list(gf->attrib_name))
		{
		  for(aptr= gf->attrib_name->back; aptr != gf->attrib_name; 
		      aptr = aptr->back)
		    {
		      if(aptr->info)
			strcat(stratr,aptr->info);
		      strcat(stratr," ");
		    }
		}
	      else
		{
		  for(aptr = v->attrib->back; aptr != v->attrib; 
		      aptr = aptr->back)
		    {
		      char *aname= NULL, *avalue = NULL;
		      if(!get_node_info_assoc_list(aptr,&aname,&avalue))
			{
			  char *str = NULL;
			  GRStrAllocCopy(str,aname);
			  strcat(stratr,aname);
			  Insert_linked_list(gf->attrib_name,
					create_new_node_llist(str));
			}
		      strcat(stratr," ");
		    }
		}
	      strcat(stratr,"\n");
	      write_in(buffer,"//attributes "); 
	      write_in(buffer,stratr);  
	      f = 1;
	    }
	  sprintf(numb,"%d", v->num);

	  for(aptr= gf->attrib_name->back; aptr != gf->attrib_name; 
	      aptr = aptr->back)
		{
		  const char *aname = aptr->info;
		  const char *avalue = find_value_assoc_list(v->attrib,aname);

		  if(avalue)
		    {
		      strcat(numb,"\\");
		      strcat(numb,avalue);
		    }
		}
	  strcat(numb,"\n");
	  write_in(buffer,numb);
	}
    }
}
void
write_nodepositions(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//nodepositions\n");
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  char pos[30];
	  char numb[MDIGITS];
	  sprintf(numb,"%d ", v->num);
	  sprintf(pos,"(%d,%d)\n", v->loc.x, v->loc.y);
	  write_in(buffer,numb);
	  write_in(buffer,pos);
	}
    }
}

void
write_edgeweights(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//edgeweights\n");

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      if(e)
		e->touch = False;
	    }
	}
    }

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      struct pt *v1, *v2;
	      if(e)
		{
		  if(e->touch == False)
		    {
		      char numb[MDIGITS+MDIGITS +30];
		      v1 = e->from;
		      v2 = e->to;
		      sprintf(numb,"(%d,%d) %s\n", v1->num,v2->num,
			      e->weight ? e->weight : "0");
		      write_in(buffer,numb);
		      e->touch = True;
		    }
		}
	    }
	}
    }
}

void
write_nodeweights(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//nodeweights\n");
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  char numb[MDIGITS];
	  sprintf(numb,"%d ", v->num);
	  write_in(buffer,numb);
	  if(v->weight)
	    write_in(buffer,v->weight);
	  write_in(buffer,"\n");
	}
    }
}

void
write_nodeshapes(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//nodeshapes\n");
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  char *str = get_string_shape_file(v);
	  char numb[MDIGITS];
	  sprintf(numb,"%d ", v->num);
	  write_in(buffer,numb);
	  write_in(buffer,str);
	  write_in(buffer,"\n");
	  free(str); str = NULL;
	}
    }
}
void
write_edges(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//edges\n");

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      if(e)
		e->touch = False;
	    }
	}
    }

  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in; ei->back != v->ledges_in; ei=ei->back)
	    {
	      struct edge *e = (struct edge *)ei->back->info;
	      struct pt *v1, *v2;
	      if(e)
		{
		  if(e->touch == False)
		    {
		      char numb[MDIGITS+MDIGITS +4];
		      v1 = e->from;
		      v2 = e->to;
		      sprintf(numb,"(%d,%d)\n", v1->num,v2->num);
		      write_in(buffer,numb);
		      e->touch = True;
		    }
		}
	    }
	}
    }

}

void
write_nodes(BUFFER *buffer,GraphFrame *gf)
{
  LNode_type *ptr;
  write_in(buffer,"//nodes\n");
  for(ptr= gf->list_vertices; ptr->back != gf->list_vertices ; ptr = ptr->back)
    {
      struct pt *v = (struct pt *)ptr->back->info;
      if(v)
	{
	  char numb[MDIGITS];
	  sprintf(numb,"%d ", v->num);
	  write_in(buffer,numb);
	  write_in(buffer,v->label);
	  write_in(buffer,"\n");
	}
    }
}


void 
write_type(BUFFER *buffer,GraphFrame *gf)
{
  write_in(buffer,"//type\n");
  if((gf->graph_dir==UNDIRECTED)||(gf->graph_dir==NONE))
    write_in(buffer,"u");
  else if (gf->graph_dir==DIRECTED)
    write_in(buffer,"d");
  write_in(buffer,"\n");
}


void 
write_scale(BUFFER *buffer,GraphFrame *gf)
{
  if(gf->scale != 0)
    {
      char nscale[20];
      sprintf(nscale,"%d",gf->scale);
      write_in(buffer,"//scale\n");
      write_in(buffer,nscale);
      write_in(buffer,"\n");
    }
}



    


/* Functions for the Buffer */

void
write_in2(BUFFER *buffer, char *str)
{
  long len = strlen(str);
   while(buffer->indbuf + len >= buffer->sizebuf)
     resize_buffer(buffer);
   strcat(buffer->buf, str);
   buffer->indbuf+=len;
}

void
write_in(BUFFER *buffer, char *str)
{
  write_block(buffer,str,strlen(str));
}

void 
write_block(BUFFER *buffer, char * block, int len)
{
   if (buffer && block && len) {
	int needed = buffer->indbuf+len;
	if (needed >= buffer->sizebuf) {
	    buffer->sizebuf = needed - needed%buffer->sizebuf + 
	      buffer->sizebuf;
	    if (buffer->buf) {
		if ((buffer->buf = (char *) 
		     realloc(buffer->buf,buffer->sizebuf)) == NULL) {
		  fprintf(stderr, "Out of Memory write_block \n");
		  abort();
		}
	        memset((void *) (buffer->buf + needed), 
		       '\0', buffer->sizebuf-needed);
	    } else {
	      if ((buffer->buf = (char *) calloc(1, buffer->sizebuf)) == NULL)
	      {
		  fprintf(stderr, "Out of Memory write_block \n");
		  abort();
	      }
	    }
	}
	memcpy((void *) (buffer->buf+buffer->indbuf), block, len);
	buffer->indbuf = needed;
    }
}

void 
resize_buffer(BUFFER *buffer)
{

  buffer->buf = (char *)realloc(buffer->buf, 
				 (buffer->sizebuf * 2)*sizeof(char));
  buffer->buf[buffer->indbuf]='\0';
   if(buffer->buf)
   {
      buffer->sizebuf += buffer->sizebuf;
   }
   else
   {
      fprintf(stderr, "Out of Memory resize_buffer \n");
      abort();
   }
}

void 
delete_in(BUFFER *buffer)
{
    if(buffer->buf) {
      free(buffer->buf); buffer->buf = NULL;
    }
   buffer->indbuf = 0;
   buffer->sizebuf = 0;
}

void
initialize_buf(BUFFER *buffer)
{
  buffer->buf = (char *)malloc(sizeof(char)*BUFSIZE);
  if(!buffer->buf) {
      fprintf(stderr,"Out of Memory initialize_buf\n");
      abort();
  }
  strcpy(buffer->buf,"");
  buffer->indbuf = 0;
  buffer->sizebuf = (long)BUFSIZE;
}
 


