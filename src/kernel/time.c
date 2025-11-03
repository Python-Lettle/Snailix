/*
 * @Author: Lettle && 1071445082@qq.com
 * @Date: 2025-11-01 15:46:36
 * @LastEditors: Lettle && 1071445082@qq.com
 * @LastEditTime: 2025-11-01 16:00:57
 * @Copyright: MIT License
 * @Description: 
 */
#include <snailix/time.h>
#include <snailix/stdlib.h>
#include <snailix/printk.h>
#include <snailix/asmfuncs.h>


#define CMOS_ADDR 0x70 // CMOS address register
#define CMOS_DATA 0x71 // CMOS data register

// CMOS information register indices
#define CMOS_SECOND 0x00  // (0 ~ 59)
#define CMOS_MINUTE 0x02  // (0 ~ 59)
#define CMOS_HOUR 0x04    // (0 ~ 23)
#define CMOS_WEEKDAY 0x06 // (1 ~ 7) Sunday = 1, Saturday = 7
#define CMOS_DAY 0x07     // (1 ~ 31)
#define CMOS_MONTH 0x08   // (1 ~ 12)
#define CMOS_YEAR 0x09    // (0 ~ 99)
#define CMOS_CENTURY 0x32 // May not exist
#define CMOS_NMI 0x80

#define MINUTE 60          // Seconds per minute
#define HOUR (60 * MINUTE) // Seconds per hour
#define DAY (24 * HOUR)    // Seconds per day
#define YEAR (365 * DAY)   // Seconds per year (365 days)

// Days passed at the start of each month
static int month[13] = {
    0, // Placeholder, no 0th month, starts from January
    0,
    (31),
    (31 + 29),
    (31 + 29 + 31),
    (31 + 29 + 31 + 30),
    (31 + 29 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)};

time_t startup_time;
int century;


// The generated time may differ from UTC time
// Depends on the system's timezone, but it's fine, at most a few hours difference
time_t mktime(tm *time)
{
    time_t res;
    int year; // Years since 1970
    // Calculate from years since 1900
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    // Seconds passed in these years
    res = YEAR * year;

    // Add one day for each leap year passed
    res += DAY * ((year + 1) / 4);

    // Time for completed months
    res += month[time->tm_mon] * DAY;

    // If February has passed and it's not a leap year, subtract one day
    if (time->tm_mon > 2 && ((year + 2) % 4))
        res -= DAY;

    // Days passed in this month
    res += DAY * (time->tm_mday - 1);

    // Hours passed today
    res += HOUR * time->tm_hour;

    // Minutes passed this hour
    res += MINUTE * time->tm_min;

    // Seconds passed this minute
    res += time->tm_sec;

    return res;
}

int get_yday(tm *time)
{
    int res = month[time->tm_mon]; // Days passed in completed months
    res += time->tm_mday;          // Days passed this month

    int year;
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    // If not a leap year and February has passed, subtract one day
    // Note: 1972 is a leap year, this calculation is not precise, ignoring leap year rules for century years
    if ((year + 2) % 4 && time->tm_mon > 2)
    {
        res -= 1;
    }

    return res;
}

u8 cmos_read(u8 addr)
{
    outb(CMOS_ADDR, CMOS_NMI | addr);
    return inb(CMOS_DATA);
};

void time_read_bcd(tm *time)
{
    // CMOS access is slow. To minimize time errors, after reading all values in the loop below,
    // if the seconds value in CMOS has changed, then re-read all values.
    // This allows the kernel to keep the time error with CMOS within 1 second.
    do
    {
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_min = cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_wday = cmos_read(CMOS_WEEKDAY);
        time->tm_mday = cmos_read(CMOS_DAY);
        time->tm_mon = cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);
        century = cmos_read(CMOS_CENTURY);
    } while (time->tm_sec != cmos_read(CMOS_SECOND));
}

void time_read(tm *time)
{
    time_read_bcd(time);
    time->tm_sec = bcd_to_bin(time->tm_sec);
    time->tm_min = bcd_to_bin(time->tm_min);
    time->tm_hour = bcd_to_bin(time->tm_hour);
    time->tm_wday = bcd_to_bin(time->tm_wday);
    time->tm_mday = bcd_to_bin(time->tm_mday);
    time->tm_mon = bcd_to_bin(time->tm_mon);
    time->tm_year = bcd_to_bin(time->tm_year);
    time->tm_yday = get_yday(time);
    time->tm_isdst = -1;
    century = bcd_to_bin(century);
}

void time_init()
{
    tm time;
    time_read(&time);
    startup_time = mktime(&time);
    kernel_info("startup time: %d%d-%02d-%02d %02d:%02d:%02d\n",
         century,
         time.tm_year,
         time.tm_mon,
         time.tm_mday,
         time.tm_hour,
         time.tm_min,
         time.tm_sec);
}
