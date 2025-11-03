/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 15:58:04
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 16:02:45
 * @Copyright: MIT License
 * @Description: 
 */
#ifndef SNAILIX_TIME_H
#define SNAILIX_TIME_H

#include <snailix/types.h>

typedef struct tm
{
    int tm_sec;   // Seconds [0,59]
    int tm_min;   // Minutes [0,59]
    int tm_hour;  // Hours [0,23]
    int tm_mday;  // Day of the month [1,31]
    int tm_mon;   // Month of the year [0,11]
    int tm_year;  // Years since 1900
    int tm_wday;  // Day of the week [0,6] (Sunday = 0)
    int tm_yday;  // Day of the year [0,365]
    int tm_isdst; // Daylight saving time flag
} tm;

void time_read_bcd(tm *time);
void time_read(tm *time);
time_t mktime(tm *time);

#endif //SNAILIX_TIME_H