#ifndef _PERROR_EXIT_H_
# define _PERROR_EXIT_H_

#ifdef DEBUG
#	define MALLOCATE(var,type,size) {fprintf(stderr,"allocating %d bytes\n",size); if(!(var=(type)malloc(size))) perror_exit("%s:%d ",__FILE__,__LINE__);}
#	define REMALLOCATE(var,type,size) if(!(var=(type)realloc(var,size))) perror_exit("%s:%d ",__FILE__,__LINE__);
#
#	define DMESG(fmt,...) fprintf(stderr,"debug: "); fprintf(stderr,fmt,##__VA_ARGS__);
#
#else
#
#	define MALLOCATE(var,type,size) if(!(var=(type)malloc(size))) perror_exit("%s:%d ",__FILE__,__LINE__);
#	define REMALLOCATE(var,type,size) if(!(var=(type)realloc(var,size))) perror_exit("%s:%d ",__FILE__,__LINE__);
#
#	define DMESG(fmt,...)
#
#endif

#define perror_exit(fmt,...)   { perror(cat(fmt,##__VA_ARGS__)); exit(EXIT_FAILURE); }
#define perror_noexit(fmt,...) { perror(cat(fmt,##__VA_ARGS__)); }
#define error_exit(fmt,...)    { fprintf(stderr,fmt,##__VA_ARGS__); exit(EXIT_FAILURE); }
#define error_noexit(fmt,...)  { fprintf(stderr,fmt,##__VA_ARGS__); }

char *
cat(const char *fmt, ...);

/*
char *
concatenate(char *str1, const char *str2);
*/

#endif
