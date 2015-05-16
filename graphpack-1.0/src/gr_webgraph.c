#include "gr_webgraph.h"
#include "gr_util.h"
#include "gr_graph_fun.h"

#define MAX_DIR_DEPTH 100

int 
classify_edges_by_directory(GraphFrame *gf)
{
  LNode_type *ptr;   

  untouch_edges(gf);

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; ptr= ptr->back)
    {
      PNODE v = (PNODE)ptr->info;

      if(v)
	{
	  LNode_type *eo;
	  for(eo = v->ledges_out->back; eo != v->ledges_out; eo=eo->back)
	    {
	      PEDGE e = (PEDGE)eo->info;
	      if(e && e->touch == False)
		{
		  set_edge_class_by_directory(e);
		  e->touch = True;
		}
	    }
	}
    }
  return 0;
}

void
set_edge_class_by_directory(PEDGE e)
{
  PNODE u = e->from;
  PNODE v = e->to;
  int unpos[MAX_DIR_DEPTH];
  int vnpos[MAX_DIR_DEPTH];

  int mu = gr_partition_url(u->label,unpos);
  int mv = gr_partition_url(v->label,vnpos);
  
  /* same server ? */
  if(chararrcmp(u->label, unpos[mu-1], v->label,vnpos[mv-1]))
  {
    e->class = OUT_CLASS;
    /*printf("u: %s v: %s OUT CLASS\n", u->label, v->label);*/
    return;
  }
  /* same directory ? */
  if(unpos[0] == vnpos[0])
  {
    if(!chararrcmp(u->label, unpos[0],v->label,vnpos[0]))
    {
      e->class = LEVEL_CLASS;
      /*printf("u: %s v: %s LEVEL CLASS\n", u->label, v->label);*/
    }
    else
    {
      e->class = FORWARD_CLASS;
      /*printf("u: %s v: %s FOR CLASS\n", u->label, v->label);*/
    }
  }
  else if(unpos[0] < vnpos[0]) 
  {
    if(!chararrcmp(u->label, unpos[0], v->label, unpos[0]))
    {
      e->class = TREE_CLASS;
      /*printf("u: %s v: %s  TREE CLASS\n", u->label, v->label);*/
    }
    else
    {
      e->class = FORWARD_CLASS;
      /*printf("u: %s v: %s FOR CLASS\n", u->label, v->label);*/
    }
  }
  else
  {
    if(!chararrcmp(u->label, vnpos[0], v->label, vnpos[0]))
    {
      e->class = BACK_CLASS;
      /*printf("u: %s v: %s BACK CLASS\n", u->label, v->label);*/
    }
    else
    {
      e->class = FORWARD_CLASS;
      /*printf("u: %s v: %s FOR CLASS\n", u->label, v->label);*/
    }
  }
}

int
gr_partition_url(char *url, int *npos)
{
  int i,j=0,ulen = url ? strlen(url) : 0;

  if(ulen == 0)
    return 0;
  for(i = ulen; i>8 ; i--)
    {
      if(url[i]=='/')
	npos[j++] = i;
    }
  npos[j] = -1;
  return j;
}

int
chararrcmp(char *s, int n, char *p , int m)
{
  int i = 0;

  if(n!=m)
    return -1;

 for(i=0;i<n; i++)
 {
   if(s[i]!=p[i])
       return -1;
 }
 return 0;
}

int
num_hyperlinks_out(PNODE v)
{
  LNode_type *eout= get_edges_out(v), *ptr = NULL; 	
  int c = 0;
  for(ptr = eout->back; ptr != eout; ptr= ptr->back)
  {
    PEDGE e = (PEDGE) ptr->info;
    if(get_edge_attribute(e) != 'z')
	c++;
  }
  return c;
}

int
num_hyperlinks_to_html(PNODE v)
{
  LNode_type *eout= get_edges_out(v), *ptr = NULL; 	
  int c = 0;
  for(ptr = eout->back; ptr != eout; ptr= ptr->back)
  {
    PEDGE e = (PEDGE) ptr->info;
    PNODE u = e->to;
    const char *smime = get_attribute(u,"mime");

    if(smime && !strcmp(smime,"text/html"))
	c++;	
  }
  return c;
}

int
num_hyperlinks_to_nhtml(PNODE v)
{
  LNode_type *eout= get_edges_out(v), *ptr = NULL; 	
  int c = 0;
  for(ptr = eout->back; ptr != eout; ptr= ptr->back)
  {
    PEDGE e = (PEDGE) ptr->info;
    PNODE u = e->to;
    const char *smime = get_attribute(u,"mime");

    if(get_edge_attribute(e) == 'z')
	continue;

    if(smime && strcmp(smime,"text/html"))
	c++;	
  }
  return c;
}

int
num_hyperlinks_to_ext(PNODE v)
{
  LNode_type *eout= get_edges_out(v), *ptr = NULL; 	
  int c = 0;
  for(ptr = eout->back; ptr != eout; ptr= ptr->back)
  {
    PEDGE e = (PEDGE) ptr->info;
    PNODE u = e->to;
    char shape = get_shape(u);
    if(shape == OCTAGON)
	c++;	
  }
  return c;
}

int
is_vertex_num_hyperlinks_lower(PNODE u, PNODE v)
{
  int nueo = num_hyperlinks_out(u);
  int nveo = num_hyperlinks_out(v);
  return (nueo < nveo);
}

int
is_vertex_num_hyperlinks_bigger(PNODE u,PNODE v)
{
  int nueo = num_hyperlinks_out(u);
  int nveo = num_hyperlinks_out(v);
  return (nueo > nveo);
}

int
num_ref_objects_out(PNODE v)
{
  LNode_type *eout= get_edges_out(v), *ptr = NULL; 	
  int c = 0;
  for(ptr = eout->back; ptr != eout; ptr= ptr->back)
  {
    PEDGE e = (PEDGE) ptr->info;
    if(get_edge_attribute(e) == 'z')
	c++;
  }
  return c;
}

int
is_vertex_num_ref_objects_lower(PNODE u, PNODE v)
{
  int nueo = num_ref_objects_out(u);
  int nveo = num_ref_objects_out(v);
  return (nueo < nveo);
}

int
is_vertex_num_ref_objects_bigger(PNODE u,PNODE v)
{
  int nueo = num_ref_objects_out(u);
  int nveo = num_ref_objects_out(v);
  return (nueo > nveo);
}


int 
vertex_http_code(PNODE v)
{
  const char *scode = get_attribute(v,"code");
  int ncode = scode ? atoi(scode) : -1;
  return ncode;
}

int 
num_vertices_http_code(GraphFrame *gf, int code)
{
  LNode_type *nodes = get_list_vertices(gf);
  LNode_type *ptr = nodes->back;
  int count = 0;
  for(; ptr != nodes; ptr=ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      if(vertex_http_code(v) == code && !is_vertex_object(v))
	  count++;
    }
  return count;
}

int
num_html_pages_with_error_http_code(GraphFrame *gf, int code)
{
  LNode_type *nodes = get_list_vertices(gf);
  LNode_type *ptr = nodes->back;
  int count = 0;

  for(; ptr != nodes; ptr=ptr->back)
  {
    PNODE v = (PNODE)ptr->info;
    LNode_type *peo, *ledges_out = get_edges_out(v);
    for(peo = ledges_out->back ; peo != ledges_out ; peo = peo->back)
    {
      PEDGE e = (PEDGE)peo->info;
      PNODE u = get_to_vertex(e);

      if(get_edge_attribute(e) == 'z')
	  continue;

      if(vertex_http_code(u) == code)
	  count++;
    }
  }
  return count;
}

int
num_orphan_html_pages(GraphFrame *gf)
{
  LNode_type *nodes = get_list_vertices(gf);
  LNode_type *ptr = nodes->back;
  int count = 0;

  for(; ptr != nodes; ptr=ptr->back)
  {
    PNODE v = (PNODE)ptr->info;
    LNode_type *ledges_out = get_edges_out(v);
    const char *smime = get_attribute(v,"mime");
      
    if(!smime || (smime && strcmp(smime,"text/html")))
      continue;

    if(is_empty_list(ledges_out))
	count++;
  }
  return count;
}

int
num_html_pages_without_title(GraphFrame *gf)
{
  LNode_type *nodes = get_list_vertices(gf);
  LNode_type *ptr = nodes->back;
  int count = 0;
  for(; ptr != nodes; ptr=ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      const char *att = NULL;
      const char *smime = get_attribute(v,"mime");
      
      if(!smime || (smime && strcmp(smime,"text/html")))
	  continue;

      att = get_attribute(v,"title");

      if(att && !strcmp(att,"No title"))
	count++;
    }
  return count++;
}

int
num_html_pages_with_images_without_alt(GraphFrame *gf)
{ 
  LNode_type *nodes = get_list_vertices(gf);
  LNode_type *ptr = nodes->back;   
  int count = 0;
  for(; ptr != nodes; ptr=ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      LNode_type *peo, *ledges_out = get_edges_out(v);

      for(peo = ledges_out->back ; peo != ledges_out ; peo = peo->back)
	{
	  PEDGE e = (PEDGE)peo->info;
	  char *sanchor = get_label_edge(e);

	  if(get_edge_attribute(e) != 'z')
	    continue;

	  if(!sanchor)
	      continue;

	  if(sanchor && sanchor[0] !='S')
	      continue;

	  count++;
	  break;
	}
    }
  return count;
}

int
num_objects_http_code(GraphFrame *gf, int code)
{
  LNode_type *nodesobj = get_list_objects(gf);
  LNode_type *ptr = NULL;
  int count = 0;

  for(ptr = nodesobj->back; ptr != nodesobj; ptr=ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      if(vertex_http_code(v) == code)
	  count++;
    }
  return count;
}

char *
get_label_short_title(GraphFrame *gf,PNODE v)
{
    char *title = NULL;
    char ctitle[10];
    char *stitle = NULL;
    int len = 0;
    title = (char *)get_attribute(v,"title");
    if(title)
    {
	len = strlen(title);
	if(len < 10)
	    GRStrAllocCopy(stitle,title); 
	else
	{
	    strncpy(ctitle,title,10);	
	    ctitle[10]='\0';
	    GRStrAllocCopy(stitle,ctitle); 
	}
    }
    return stitle;
}


char *
get_label_long_title(GraphFrame *gf,PNODE v)
{
    char *title = NULL;
    char *stitle = NULL;
    title = (char *)get_attribute(v,"title");
    GRStrAllocCopy(stitle,title); 
    return stitle;
}

#define GR_SHAPE_HTTP_HTML_DOC BOX
#define GR_SHAPE_HTTP_ANY_DOC CIRCULAR
#define GR_SHAPE_HTTP_DOC OCTAGON
#define GR_SHAPE_FTP_DOC RHOMBUS
#define GR_SHAPE_MAILTO_DOC HOR_ELLIPSIS
#define GR_SHAPE_PROT_DOC VER_ELLIPSIS
#define GR_SHAPE_BROKEN_DOC TRIANGLE

void
GR_set_shape_from_attributes(GraphFrame *gf)
{
  LNode_type *ptr, *lv = get_list_vertices(gf);
  for(ptr = lv->back  ; ptr !=  lv ; ptr = ptr->back)
    {
      struct pt *v = (struct pt * )ptr->info;
      char *label = get_label_vertex(v);
      const char *mtype = get_attribute(v, "mime");
      const char *msize =get_attribute(v,"size");
      int size = msize ? atoi(msize) : -1;
      const char *mcode =get_attribute(v,"code");
      int code = mcode ? atoi(mcode) : -1;

       if(!strncmp(label,"http://",7))
	 {
	   if(mtype && !strcmp(mtype,"text/html"))
	     set_vertex_shape(v,GR_SHAPE_HTTP_HTML_DOC);
	   else if(size >=0)
	     set_vertex_shape(v,GR_SHAPE_HTTP_ANY_DOC);
	   else
	     set_vertex_shape(v,GR_SHAPE_HTTP_DOC);
	 }
       else if(!strncmp(label,"ftp://",6))
	 set_vertex_shape(v,GR_SHAPE_FTP_DOC);
       else if(!strncmp(label,"mailto:",7))
	 set_vertex_shape(v,GR_SHAPE_MAILTO_DOC);
       else
	 set_vertex_shape(v,GR_SHAPE_PROT_DOC);

       if(code > 400)
	 set_vertex_shape(v,GR_SHAPE_BROKEN_DOC);
    }
}


