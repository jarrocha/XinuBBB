/* mark.h - notmarked */

#define	MAXMARK	20		/* Maximum number of marked locations	*/

extern	int32	*(marks[]);
extern	int32	nmarks;
extern	sid32	mkmutex;
typedef	int32	memmark[1];	/* Declare a memory mark to be an array	*/
				/*   so user can reference the name	*/
				/*   without a leading &		*/

/*------------------------------------------------------------------------
 *  notmarked  -  Return nonzero if a location has not been marked
 *------------------------------------------------------------------------
 */
#define	notmarked(L)		(L[0]<0 || L[0]>=nmarks || marks[L[0]]!=L)
