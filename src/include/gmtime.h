#ifndef __GMTIME_H
#define __GMTIME_H

struct tm
{
        int     tm_sec;         /* seconds */
        int     tm_min;         /* minutes */
        int     tm_hour;        /* hours */
        int     tm_mday;        /* day of the month */
        int     tm_mon;         /* month */
        int     tm_year;        /* year */
        int     tm_wday;        /* day of the week */
        int     tm_yday;        /* day in the year */
        int     tm_isdst;       /* daylight saving time */
};

struct tm *gmtime(const time_t *timep);

#endif /* __GMTIME_H */
