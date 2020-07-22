/*   doesn't work if you want the memcpy the struct.  Sigh... I didn't
 *   think about that.  So now, when the extensions are enabled, we
 *   malloc space when necessary and keep the timezone names in a linked
 *   list.
 *
 *   Fix a dst-related bug which resulted in use of uninitialized data.
 *
 * Nov 15, 2003 I forgot to update the thread locking in the last dst fix.
 *
 * Dec 14, 2003 Fix some dst issues in _time_mktime().
 *   Normalize the tm_isdst value to -1, 0, or 1.
 *   If no dst for this timezone, then reset tm_isdst to 0.
 *
 * May 7, 2004
 *   Change clock() to allow wrapping.
 *   Add timegm() function.
 *   Make lookup_tzname() static (as it should have been).
 *   Have strftime() get timezone information from the passed struct
 *     for the %z and %Z conversions when using struct tm extensions.
 *
 * Jul 24, 2004
 *   Fix 2 bugs in strftime related to glibc struct tm extensions.
 *   1) Need to negate tm_gmtoff field value when used. (bug 336).
 *   2) Deal with NULL ptr case for tm_zone field, which was causing
 *      segfaults in both the NIST/PCTS tests and the Python 2.4.1
 *      self-test suite.
 *      NOTE: We set uninitialized timezone names to "???", and this
 *            differs (intentionally) from glibc's behavior.
 */

#include "NewTime.h"

struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};

static void GetCurTime(struct rtc_time *tm)
{
	#define BCD2NUM(x)	(((x) & 0x0F) + (((x) >> 4) * 10))
	UINT8 buf[8];
	
	sysGetTime(buf);

	tm->tm_year = BCD2NUM(buf[0]) + 100;
	tm->tm_mon  = BCD2NUM(buf[1]) - 1;
	tm->tm_mday = BCD2NUM(buf[2]);
	tm->tm_hour = BCD2NUM(buf[3]);
	tm->tm_min  = BCD2NUM(buf[4]);
	tm->tm_sec  = BCD2NUM(buf[5]);
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
static unsigned long
maketime(const unsigned int year0, const unsigned int mon0,
       const unsigned int day, const unsigned int hour,
       const unsigned int min, const unsigned int sec)
{
	unsigned int mon = mon0, year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int) (mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((unsigned long)
		  (year/4 - year/100 + year/400 + 367*mon/12 + day) +
		  year*365 - 719499
	    )*24 + hour /* now have hours */
	  )*60 + min /* now have minutes */
	)*60 + sec; /* finally seconds */
}

/*
 * Convert Gregorian date to seconds since 01-01-1970 00:00:00.
 */
static int rtc_tm_to_time(struct rtc_time *tm, unsigned long *time)
{
	*time = maketime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
	return 0;
}

static const unsigned char rtc_days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

#define SYS_LEAPS_THRU_END_OF(y)	((y)/4 - (y)/100 + (y)/400)
#define SYS_LEAP_YEAR(year)			((!(year % 4) && (year % 100)) || !(year % 400))

static int rtc_month_days(unsigned int month, unsigned int year)
{
	return rtc_days_in_month[month] + (SYS_LEAP_YEAR(year) && month == 1);
}

/*
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
static void rtc_time_to_tm(unsigned long time, struct rtc_time *tm)
{
	register int days, month, year;

	days = time / 86400;
	time -= days * 86400;

	/* day of the week, 1970-01-01 was a Thursday */
	tm->tm_wday = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365
		+ SYS_LEAPS_THRU_END_OF(year - 1)
		- SYS_LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + SYS_LEAP_YEAR(year);
	}
	tm->tm_year = year - 1900;
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - rtc_month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month;
	tm->tm_mday = days + 1;

	tm->tm_hour = time / 3600;
	time -= tm->tm_hour * 3600;
	tm->tm_min = time / 60;
	tm->tm_sec = time - tm->tm_min * 60;
	tm->tm_isdst = 0;
}


time_t SYS_TIME(time_t *t)
{
	struct rtc_time rt;
	unsigned long time;
	
//	ssl_debug("(t=%p)\n", t);
	
	GetCurTime(&rt);
	rtc_tm_to_time(&rt, &time);

	if (t)
		*t = time;
	
	return time;
}

time_t SYS_MKTIME(struct tm *tm)
{	
	return maketime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec);
}

struct tm *SYS_GMTIME(const time_t *timer)
{
	static struct tm tms;
	struct rtc_time rtctm;
			
	rtc_time_to_tm(*timer, &rtctm);
	tms.tm_sec	= rtctm.tm_sec;
	tms.tm_min	= rtctm.tm_min;
	tms.tm_hour	= rtctm.tm_hour;
	tms.tm_mday = rtctm.tm_mday;
	tms.tm_mon	= rtctm.tm_mon;
	tms.tm_year	= rtctm.tm_year;
	tms.tm_wday	= rtctm.tm_wday;
	tms.tm_yday	= rtctm.tm_yday;
	tms.tm_isdst= rtctm.tm_isdst;
	
	return &tms;
}

struct tm *SYS_LOCALTIME(const time_t *timer)
{	
	return SYS_GMTIME(timer);
}


void CalcTime(time_t interval, struct tm *newtime)
{
	struct tm basetime;
	time_t base;

	basetime.tm_sec = 0;
	basetime.tm_min = 0;
	basetime.tm_hour = 0;
	basetime.tm_mday = 1;
	basetime.tm_mon = 0;
	basetime.tm_year = 1995-1900;

	base = SYS_MKTIME((struct tm *)&basetime) + interval;
	
	*newtime = *SYS_GMTIME(&base);
	
	newtime->tm_year += 1900;
	newtime->tm_mon += 1;
}




