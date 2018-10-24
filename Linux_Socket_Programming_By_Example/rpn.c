/* rpnsrv.c:
 * 
 * Example RPN Server :
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

static void
bail(const char *on_what) {
    if ( errno != 0 ) {
        fputs(strerror(errno),stderr);
        fputs(": ",stderr);
    }
    fputs(on_what,stderr);
    fputc('\n',stderr);
    exit(1);
}

int
main(int argc,char **argv) {
    int z;
    int n;
double a;
double b;
double c;
    char buf[512];
     read(1,buf,sizeof buf);
a = atof(buf);
     read(1,buf,sizeof buf);
b = atof(buf);
read(1,buf,sizeof buf);

if (*buf == '+') c=a+b;
 if (*buf == '-') c=a-b;
 if (*buf == '*') c=a*b;
if(*buf == '/') c=a/b;
sprintf(buf,"%f",c);
z = sizeof(double);
   write(1,buf,z);
    return 0;
}



