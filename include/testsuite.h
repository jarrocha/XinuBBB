/* testsuite.h - failif */

process	test_addargs(bool8);
process	test_bigargs(bool8);
process	test_schedule(bool8 verbose);
process	test_preempt(bool8 verbose);
process	test_recursion(bool8 verbose);
process	test_semaphore(bool8 verbose);
process	test_semaphore2(bool8 verbose);
process	test_semaphore3(bool8 verbose);
process	test_semaphore4(bool8 verbose);
process	test_semaphore5(bool8 verbose);
process	test_libStdio(bool8 verbose);

void	testPass(bool8, const char *);
void	testFail(bool8, const char *);
void	testSkip(bool8, const char *);
void	testPrint(bool8, const char *);

/*------------------------------------------------------------------------
 * failif - report failure by displaying a message is condition is met
 *------------------------------------------------------------------------
 */
#define	failif(cond, failmsg) \
	if ( cond ) { testFail(verbose, failmsg); passed = FALSE; } \
	else { testPass(verbose, ""); }

/* Define the strcuture of an entry in the table of test cases */

struct	testcase {
    char	*name;		/* Name of test case			*/
    process	(*test) (bool8);/* Test case function			*/
};

extern	int	ntests;		/* total number of tests		*/
extern	struct	testcase testtab[]; /* table of test cases		*/

#define	TESTSTK	8192		/* size of process stack used for test	*/
