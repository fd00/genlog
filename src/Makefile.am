bin_PROGRAMS = genlog

genlog_SOURCES = genlog.c user.c user.h \
	perror_exit.c perror_exit.h \
	getopt.c getopt.h getopt1.c


INCLUDES = -I${GRAPHPACK}

genlog_LDADD = -L${GRAPHPACK}

LIBS = -lgraphpack -lexpat -lm
