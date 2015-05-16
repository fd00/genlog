#ifndef GR_WEBGRAPH_H
#define GR_WEBGRAPH_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

int 
classify_edges_by_directory(GraphFrame *gf);
int
chararrcmp(char *s, int n, char *p , int m);
int
gr_partition_url(char *url, int *npos);
void
set_edge_class_by_directory(PEDGE e);

int
num_hyperlinks_out(PNODE v);
int
num_hyperlinks_to_ext(PNODE v);
int
num_hyperlinks_to_nhtml(PNODE v);
int
num_hyperlinks_to_html(PNODE v);

int
is_vertex_num_hyperlinks_lower(PNODE u, PNODE v);
int
is_vertex_num_hyperlinks_bigger(PNODE u,PNODE v);
int
num_ref_objects_out(PNODE v);
int
is_vertex_num_ref_objects_lower(PNODE u, PNODE v);
int
is_vertex_num_ref_objects_bigger(PNODE u,PNODE v);
int 
vertex_http_code(PNODE v);

int 
num_vertices_http_code(GraphFrame *gf, int code);
int
num_html_pages_with_error_http_code(GraphFrame *gf, int code);
int
num_orphan_html_pages(GraphFrame *gf);
int
num_html_pages_without_title(GraphFrame *gf);
int
num_html_pages_with_images_without_alt(GraphFrame *gf);
int
num_objects_http_code(GraphFrame *gf, int code);

void
GR_set_shape_from_attributes(GraphFrame *gf);

#endif

