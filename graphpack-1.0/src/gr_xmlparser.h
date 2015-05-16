#ifndef GR_XMLPARSER_H
#define GR_XMLPARSER_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

void 
startElement(void *userData, const char *name, const char **atts);
void 
endElement(void *userData, const char *name);
int
parse_xml_text_graph(GraphFrame *gf, char *text, long len);

void
set_graph_info(void *userData, const char *name, const char **atts);


#endif


