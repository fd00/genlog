#ifndef GR_PARSER_H
#define GR_PARSER_H

#include "gr_graph.h"
#include "gr_node.h"
#include "gr_edge.h"

#define SIZELINE 1024

#define NO_MODE 0
#define MSCALE 100
#define MTYPE 1
#define MNODES 2
#define MEDGES 3
#define MNODESHAPES 4
#define MNODEWEIGHTS 5
#define MEDGEWEIGHTS 6
#define MNODEPOSITIONS 7
#define MATTRIB 8
#define MEDGEATTRIB 9

#define NO_ERROR 0
#define TYPE_ERROR -1
#define NODE_ERROR -2
#define EDGE_ERROR -3
#define NSHAPE_ERROR -4
#define NWEIGHT_ERROR -5
#define EWEIGHT_ERROR -6
#define NPOSITION_ERROR -7
#define MODE_ERROR -8
#define FILE_ORDER_ERROR -9
#define SCALE_ERROR -10
#define NATTRIB_ERROR -11
#define EATTRIB_ERROR -12


int
parse_text_graph(GraphFrame *gf,char *text);
Boolean
read_text_line(char *text,char *line, int *i);
int
select_mode_operation(GraphFrame *gf,char *line);

Boolean
save_graph_data(GraphFrame *gf, int mode, char *line);
void
save_graph_scale(GraphFrame *gf, char *line);
void
save_graph_type(GraphFrame *gf, char *line);
void
save_graph_node(GraphFrame *gf, char *line);
void 
create_sequence_nodes(GraphFrame *gf, char *line);
void 
save_name_node(GraphFrame *gf, char *line);

void
save_graph_edge(GraphFrame *gf, char *line);
void
get_names_nodes(char *line,char **nfrom,char **nto);

void
save_graph_node_shapes(GraphFrame *gf, char *line);
void
set_node_shape(GraphFrame *gf,char *line);

void
set_all_nodes_shape(GraphFrame *gf,char *line);
void
set_node_weight(GraphFrame *gf,char *line);
void
set_all_node_weight(GraphFrame *gf,char *line);
void
save_graph_node_weights(GraphFrame *gf, char *line);
void
set_all_edge_weight(GraphFrame *gf,char *line);
void
set_edge_weight(GraphFrame *gf,char *line);
void
save_graph_edge_weights(GraphFrame *gf, char *line);
void
save_graph_node_positions(GraphFrame *gf, char *line);
void
save_graph_cur_positions(GraphFrame *gf);
void
set_all_node_position(GraphFrame *gf,char *line);
void
set_node_position(GraphFrame *gf,char *line);
void
save_graph_attributes(GraphFrame *gf, char *line);
void
save_graph_edge_attributes(GraphFrame *gf, char *line);

#endif





