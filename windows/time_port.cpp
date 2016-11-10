#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)

#include <time_port.h>
#include <errno.h>
#include <string.h>

/* $Id: gettimeofday.c 91813 2010-09-17 07:52:52Z johnnyw $ */

/*
*  This file defines functions that are required for unix compatibility.
*
*  These functions are not available in the Microsoft C/C++ Run Time
*  and the Win32 API.
*
*  The following functions list may not be complete
*
*  FUNCTIONS:
*     SHARED   _gettimeofday
*
*/

/*
* gettimeofday() --  gets the current time in elapsed seconds and
*                     microsends since GMT Jan 1, 1970.
*
* ARGUMENTS: - Pointer to a timeval struct to return the time into
*
* RETURN CODES: -  0 on success
*                 -1 on failure
*/

int gettimeofday(struct timeval *curTimeP, struct timezone *)
{
	struct _timeb  localTime;

	if (curTimeP == (struct timeval *) 0)
	{
		errno = EFAULT;
		return (-1);
	}

	/*
	*  Compute the elapsed time since Jan 1, 1970 by first
	*  obtaining the elapsed time from the system using the
	*  _ftime(..) call and then convert to the "timeval"
	*  equivalent.
	*/

	_ftime(&localTime);

	curTimeP->tv_sec = long(localTime.time + localTime.timezone);
	curTimeP->tv_usec = localTime.millitm * 1000;

	return(0);
}

void gmtime_r(const time_t *timep, struct tm *result)
{
	*result = *gmtime(timep);
}

void asctime_r(const struct tm *tm, char *buf)
{
	char * timeBuf = asctime(tm);
	memcpy(buf, timeBuf, strlen(timeBuf) + 1);
}

#endif // defined(WIN32) || defined(_WIN32) || defined(_WIN64)
