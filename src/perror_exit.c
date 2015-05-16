#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


char *
cat(const char *fmt, ...)
{
	va_list ap;
	int d;
	char *s = NULL;
	char *msg = strdup("");
	char tmp[32];

	assert(fmt != NULL);
	
	va_start(ap, fmt);
	while(*fmt) {
		switch(*fmt) {
		case '%':
			++fmt;
			switch(*fmt) {
			case 's':
				s = va_arg(ap, char *);
				break;
			case 'd':
				d = va_arg(ap, int);
				snprintf(tmp,32,"%d",d);
				s = &(tmp[0]);
				break;
			case 'c':
				d = va_arg(ap, int);
				snprintf(tmp,32,"%d",(int)d);
				s = &(tmp[0]);
			}
			break;
		default:
			snprintf(tmp,32,"%c",*fmt);
			s = &(tmp[0]);
		}
		if(!(msg = (char *)realloc(msg,sizeof(char)*(strlen(msg)+strlen(s)+1)))) {
			perror("malloc()");
			abort();
		}
		strcat(msg,s);
		va_end(ap);
		++fmt;
	}
	return msg;
}

/*
char *
concatenate(char *str1, const char *str2) {

	long len1, len2;

	assert(str2 != NULL);

	len1 = str1 == NULL ? 0 : strlen(str1);
	len2 = strlen(str2);

	if(!(str1 = (char *)realloc(str1,sizeof(char)*(len1+len2+1))))
		perror_exit("%s %c", __FILE__, __LINE__);

	memset(&(str1[len1]),'\0',len2);
	strncat(str1,str2,len1+len2);

	return str1;
}
*/
