/****************************************************************\
*                                                                *
* Copyright (C) 2007 by Markus Duft <markus.duft@salomon.at>     *
*                                                                *
* This file is part of parity.                                   *
*                                                                *
* parity is free software: you can redistribute it and/or modify *
* it under the terms of the GNU Lesser General Public License as *
* published by the Free Software Foundation, either version 3 of *
* the License, or (at your option) any later version.            *
*                                                                *
* parity is distributed in the hope that it will be useful,      *
* but WITHOUT ANY WARRANTY; without even the implied warranty of *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  *
* GNU Lesser General Public License for more details.            *
*                                                                *
* You should have received a copy of the GNU Lesser General      *
* Public License along with parity. If not,                      *
* see <http://www.gnu.org/licenses/>.                            *
*                                                                *
\****************************************************************/

#include "time.h"
#include "sys/time.h"
#include "sys/timeb.h"
#include "sys/socket.h"

//
// TODO: 64 bit time_t? hmm... i think we can't do anything about it.
//
int gettimeofday(struct timeval *tv, void* unused)
{
    struct timeb tb; 
    ftime(&tb);

    tv->tv_sec  = (long)tb.time;
    tv->tv_usec = (long)tb.millitm * 1000L;
    return(0);
}

extern size_t strftime(char*,size_t,const char*,const struct tm*);

static struct tm zero_time = {0,0,0,0,0,0,0,0,0}; 
static struct tm default_time = {0,0,0,1,0,70,0,0,0};

#undef strftime
size_t _pcrt_strftime_safe(char* buf, size_t size, const char* fmt, const struct tm* time) {
	if(time == NULL || memcmp(time, &zero_time, sizeof(struct tm)) == 0) {
        return strftime(buf, size, fmt, &default_time);
    } else {
        // assume non zero date to be valid.
        return strftime(buf, size, fmt, time);
    }  	
}
