#ifndef GR_XMLWRITE_H
#define GR_XMLWRITE_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

char *
get_xml_text_graph(GraphFrame *gf,long *len,int type);

char *
HTXMLEscape(char * str);
#endif

