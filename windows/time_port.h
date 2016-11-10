#ifndef __TIME_PORT_H__
#define __TIME_PORT_H__

#include <sys/timeb.h> /* For prototype of "_ftime()" */
#include <time.h>

typedef struct timeval {
	long tv_sec;
	long tv_usec;
} timeval;

int gettimeofday(struct timeval *curTimeP, struct timezone *);

void gmtime_r(const time_t *timep, struct tm *result);
void asctime_r(const struct tm *tm, char *buf);

#endif // __TIME_PORT_H__
