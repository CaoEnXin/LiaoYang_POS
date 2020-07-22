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

#ifndef _NEWTIME_H
#define _NEWTIME_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <time.h>
#include "posapi.h"
#include "posixlib.h"

#ifndef COMPILED_IN_VC	
#include "basefunc.h"
#endif

time_t SYS_TIME(time_t *t);

time_t SYS_MKTIME(struct tm *tm);

struct tm *SYS_GMTIME(const time_t *timer);

struct tm *SYS_LOCALTIME(const time_t *timer);

void CalcTime(time_t interval, struct tm *newtime);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	// _UITL_H

// end of file

