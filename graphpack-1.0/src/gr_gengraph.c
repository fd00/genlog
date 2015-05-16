#include "gr_gengraph.h"
#include "gr_util.h"


void 
create_nodes_line_graph(GraphFrame *ngf,GraphFrame *gf,Hashtable_type VT);

void 
create_edges_line_graph(GraphFrame *ngf,GraphFrame *gf,Hashtable_type VT);

void 
generate_line_graph(GraphFrame *ngf,GraphFrame *ogf)
{
  Hashtable_type VeTable;
  init_hash_table(VeTable);
  create_nodes_line_graph(ngf,ogf,VeTable);
  create_edges_line_graph(ngf,ogf,VeTable);
  Delete_hash_table(VeTable);
}

void 
create_nodes_line_graph(GraphFrame *ngf,GraphFrame *gf,Hashtable_type VT)
{
  LNode_type *ptr;   

  untouch_edges(gf);

  for(ptr = gf->list_vertices->back; ptr != gf->list_vertices; ptr= ptr->back)
    {
      PNODE v = (PNODE)ptr->info;

      if(v)
	{
	  LNode_type *ei;
	  for(ei = v->ledges_in->back; ei != v->ledges_in; ei=ei->back)
	    {
	      PEDGE e = (struct edge *)ei->info;
	      if(e->from == v)
		  continue;
	      if(e && e->touch == False)
		{
		  PNODE nv;
		  char *ename = NULL;
		  e->touch = True;
		  ename = get_edge_name(e);
		  /*sprintf(ename,"%s_%s",e->from->label,e->to->label);*/
		  nv = create_node_in_graph_name(ngf,ename);
		  set_vertex_shape(nv, get_shape(e->to));
		  if(e->label)
		      add_attribute(nv,"title", e->label);
		  insert_hash_info(VT,ename,(char *)e);
		  free(ename); ename = NULL;
		}
	      if(gf->graph_dir == UNDIRECTED)
	      {
		PNODE u = e->from;
		PEDGE et = find_edge_by_name(gf,v->label,u->label);
		if(et) et->touch = True;
	      }
	    }
	}
    }
}

void 
create_edges_line_graph(GraphFrame *ngf,GraphFrame *gf,Hashtable_type VT)
{
  LNode_type *ptr;   

  for(ptr = ngf->list_vertices->back; ptr != ngf->list_vertices; 
      ptr= ptr->back)
    {
      PNODE v = (PNODE)ptr->info;
      PEDGE oe;
      oe = (PEDGE)get_element_hashed(VT, (Key_type)v->label);
      if(oe)
      {
	PNODE ou = oe->from;
	PNODE ov = oe->to;
	LNode_type *ei;
	if(gf->graph_dir == DIRECTED) {
	for(ei = ou->ledges_in->back; ei != ou->ledges_in; ei=ei->back)
	    {
	      PEDGE ine = (PEDGE)ei->info;
	      PNODE u;
	      char *ename = NULL;
	      ename = get_edge_name(ine);
	      /*sprintf(ename,"%s_%s",ine->from->label, ine->to->label);*/
	      u = find_vertex_by_name(ngf,ename);
	      if(u)
	      {
		PEDGE ne = find_edge(ngf,u,v);
		if(!ne)
		   adding_edge(ngf,u,v,"1");
	      }
	      free(ename); ename = NULL;
	    }
	for(ei = ov->ledges_out->back; ei != ov->ledges_out; ei=ei->back)
	    {
	      PEDGE oute = (PEDGE)ei->info;
	      PNODE s;
	      char *ename = NULL;
	      /*sprintf(ename,"%s_%s",oute->from->label, oute->to->label);*/
	      ename = get_edge_name(oute);
	      s = find_vertex_by_name(ngf,ename);
	      if(s)
	      {
		PEDGE ne = find_edge(ngf,v,s);
		if(!ne)
		   adding_edge(ngf,v,s,"1");
	      }
	      free(ename); ename = NULL;
	    }
	} else if (gf->graph_dir== UNDIRECTED) {
	    for(ei = ou->ledges_in->back; ei != ou->ledges_in; ei=ei->back)
	    {
	      PEDGE ine = (PEDGE)ei->info;
	      PNODE u;
	      char *ename = NULL;
	      char *rname = NULL;
	      ename = get_edge_name(ine);
	      rname = get_reverse_edge_name(ine);
	      /*sprintf(ename,"%s_%s",ine->from->label, ine->to->label);*/
	      u = find_vertex_by_name(ngf,ename);
	      if(!u)
		  u = find_vertex_by_name(ngf,rname);
	      if(u == v)
		  continue;
	      if(u)
	      {
		PEDGE ne = find_edge(ngf,u,v);
		if(!ne)
		   adding_edge(ngf,u,v,"1");
	      } 
	      free(ename); ename = NULL;
	      free(rname); rname = NULL;
	    }
	for(ei = ov->ledges_in->back; ei != ov->ledges_in; ei=ei->back)
	    {
	      PEDGE oute = (PEDGE)ei->info;
	      PNODE s;
	      char *ename = NULL;
	      char *rname = NULL;
	      /*sprintf(ename,"%s_%s",oute->from->label, oute->to->label);*/
	      ename = get_edge_name(oute);
	      rname = get_reverse_edge_name(oute);
	      s = find_vertex_by_name(ngf,ename);
	      if(!s)
		  s = find_vertex_by_name(ngf,rname);
	      if(s == v)
		  continue;
	      if(s)
	      {
		PEDGE ne = find_edge(ngf,v,s);
		if(!ne)
		   adding_edge(ngf,v,s,"1");
	      }
	      free(ename); ename = NULL;
	      free(rname); rname = NULL;
	    }
	}
      }
    }
}


