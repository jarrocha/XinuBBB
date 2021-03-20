/* ascdate.c - ascdate */

#include <xinu.h>
#include <stdio.h>

struct dateinfo Date = {0, FALSE, DATE_DST_AUTO,
		       {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
		       {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
		        "Aug", "Sep","Oct","Nov","Dec"},
		       {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}};

/*------------------------------------------------------------------------
 *  ascdate  -  Format a date in ascii including day, year hours:mins:secs
 *------------------------------------------------------------------------
 */
status	ascdate (
	  uint32	now,		/* Date and time in xinu format	*/
	  char		*str		/* String ( >= 20 chars long)	*/
	)
{
	uint32	tmp;			/* Counts remaining seconds	*/
					/*   during computations	*/
	int32	year, month, day,	/* Values for various parts of	*/
		hour, minute, second;	/*   the date			*/
	uint32	days;			/* Number of days in a year	*/
	uint32	leapyrs;		/* Number of leap years between	*/
					/*   1970 and now		*/
	uint32	shift;			/* Number of times the calendar	*/
					/*   shifted by a day		*/
	uint32	dayofweek;		/* Day of the week (0 - 6)	*/
					/* For DST, day of the week for:*/
	int32	jan1;			/*   January 1 this year	*/
	int32	mar1;			/*   March 1 this year		*/
	int32	nov1;			/*   November 1 this year	*/
	int32	marss;			/* Day of second sunday in march*/
	int32	novfs;			/* Day of second sunday in march*/
	bool8	dst;			/* Should we adjust for DST?	*/
	int32	i;			/* Indexes through months	*/

	char	*zones[]  = {"EST", "CST", "MST", "PST"};
	char	*dzones[] = {"EDT", "CDT", "MDT", "PDT"};

	/* Compute the year (1970-2099) */

	for (year=1970 ; TRUE ; year++) {
		days = isleap(year) ? 366 : 365;
		tmp = days * SECPERDY;
		if (tmp > now)
			break;
		now -= tmp;
	}

	/* Compute the number of whole days that have already passed	*/
	/*   during the current year (0 through 365)			*/

	days = now / SECPERDY;

	/* Compute the month (0-11) */

	for (month=0 ; month<12 ; month++) {
		tmp = Date.dt_msize[month] * SECPERDY;
		if ((month == 1) && isleap(year)) {
			tmp += SECPERDY;
		}
		if (tmp > now)
			break;
		now -= tmp;
	}

	/* Compute the day of month (1-31) */

	day = (int32)( now/SECPERDY ) + 1;
	now %= SECPERDY;

	/* Compute the hour (0-23) */

	hour = (int32) ( now/SECPERHR );
	now %= SECPERHR;

	/* Compute the minutes (0-59) */

	minute = now / SECPERMN;
	now %= SECPERMN;

	/* Compute the seconds (0-59) */
	second = (int32) now;

	/* Compute the day of the week (0-6)				*/
	/*								*/
	/*   Note: Jan 1, 1970 was a Thursday (4 on a 0 to 6 range)	*/
	/* Add one day of the week each	full year (365%7 == 1) plus	*/
	/*    one extra day for each leap year				*/

	/* Compute number of leap years prior to current year */

	leapyrs = (year-1969)/4;

	/* Compute shift for all previous years (i.e., day of week of	*/
	/* Jan 1 of the current year)					*/

	shift = 4 + (year - 1970) + leapyrs;

	/* Remember day of the week for Jan 1 this year (for DST)	*/

	jan1 = shift % 7;

	/* Add the number of days so far this year */

	shift += days;

	/* Convert to integer from 0 through 6 */

	dayofweek = shift % 7;

	/*								*/
	/* 		Handle daylight savings time			*/
	/*								*/
	/*   Date.dt_daylight specifies whether to set it on, off,	*/
	/*		or automatically compute the setting		*/
	/*								*/

	dst = FALSE;
	if (Date.dt_daylight == DATE_DST_ON) {
		dst = TRUE;
	} else if (Date.dt_daylight == DATE_DST_AUTO) {
		
		/* Automatic DST calculation: DST is on between 2:00 AM	*/
		/*   the second Sunday of March and 2:00 AM the first	*/
		/*   sunday in November					*/

		/* Calculate day of the week for March 1		*/

		mar1 = (jan1 + Date.dt_msize[0] + Date.dt_msize[1] +
						    isleap(year)) % 7;

		/* Caculate day (1-31) of the second sunday in March	*/

		if (mar1 == 0) {	/* Mar. 1 is a Sunday		*/
			marss = 8;
		} else {		/* Mar. 1 is Monday - Saturday	*/
			marss = 15 - mar1;
		}

		/* Calculate day of the week for November 1		*/

		nov1 = jan1;
		for (i=0; i<10; i++) {
			nov1 += Date.dt_msize[i];
		}
		if (isleap(year)) {
			nov1++;
		}
		nov1 = nov1 % 7;
		if (nov1 == 0) {	/* Nov. 1 is a Sunday		*/
			novfs = 1;
		} else {		/* Nov. 1 is Monday - Saturday	*/
			novfs = 8 - nov1;
		}

		/* Set dst based on time of the year */

		/* DST is off during December, January, and Feburary */

		if ( (month == 11) || (month == 0) || (month == 1) ) {
			dst = FALSE;

		/* DST is on from April through October */

		} else if ( (month>2) && (month<10) ) {
			dst = TRUE;

		/* DST is on in March past 2 AM on the second Sunday	*/

		} else if (month == 2) {

			dst = FALSE;
			if (day > marss) {
				dst = TRUE;
			} else if ( (day == marss) && (hour >= 2) ) {
				dst = TRUE;
			}

		/* DST is on in November until 2 am on first Sunday	*/


		} else if (month == 10) {

			dst = TRUE;
			if (day > novfs) {
				dst = FALSE;
			} else if ( (day == novfs) && (hour >= 1) ) {
				dst = FALSE;
			}
		}
	}

	/* If we are doing DST, move ahead one hour and handle the	*/
	/*   case where we move to the next day or the next month	*/

	if (dst) {
		hour++;

		/* If day exceeded, move to next day */

		if (hour > 23) {
			hour = 0;
			day++;

			/* If month exceeded, move to next month */

			if (day > Date.dt_msize[month]) {
				day = 1;
				month++;

				/* Stop here because DST does not occur	*/
				/*  during the year boundary		*/
			}
		}
	}

	sprintf(str, "%3s  %3s %2d  %2d:%02d:%02d %s %d",
		Date.dt_dnam[dayofweek], Date.dt_mnam[month],
		day, hour, minute, second, dst? dzones[TIMEZONE-5]:
			zones[TIMEZONE-5], year);
	return OK;
}
