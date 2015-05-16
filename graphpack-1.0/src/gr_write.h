#ifndef GR_WRITE_H
#define GR_WRITE_H

#define BUFSIZE 1024
struct mybuffer{
   char * buf;
   long indbuf;
   long sizebuf;
};
typedef struct mybuffer BUFFER;

char *
get_text_graph(GraphFrame *gf,long *len);

void 
write_scale(BUFFER *buffer,GraphFrame *gf);
void 
write_type(BUFFER *buffer,GraphFrame *gf);
void
write_edgeattributes(BUFFER *buffer,GraphFrame *gf);
void
write_nodepositions(BUFFER *buffer,GraphFrame *gf);
void
write_edgeweights(BUFFER *buffer,GraphFrame *gf);
void
write_nodeweights(BUFFER *buffer,GraphFrame *gf);
void
write_nodeshapes(BUFFER *buffer,GraphFrame *gf);
void
write_edges(BUFFER *buffer,GraphFrame *gf);
void
write_nodes(BUFFER *buffer,GraphFrame *gf);
void
write_nodeattributes(BUFFER *buffer,GraphFrame *gf);


/* Functions for the Buffer */
void
write_in(BUFFER *buffer, char *str);
void 
resize_buffer(BUFFER *buffer);
void 
delete_in(BUFFER *buffer);
void
initialize_buf(BUFFER *buffer);

void 
write_block(BUFFER *buffer, char * block, int len);




#endif
