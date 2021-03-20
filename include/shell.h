/* shell.h - Declarations and constants used by the Xinu shell */

/* Size constants */

#define SHELL_BUFLEN	TY_IBUFLEN+1	/* Length of input buffer	*/
#define SHELL_MAXTOK	32		/* Maximum tokens per line	*/
#define SHELL_CMDSTK	8192		/* Size of stack for process	*/
					/*    that executes command	*/
#define	SHELL_ARGLEN	(SHELL_BUFLEN+SHELL_MAXTOK) /* Argument area	*/
#define SHELL_CMDPRIO	20		/* Process priority for command	*/

/* Message constants */

/* Shell banner (assumes VT100) */

#define	SHELL_BAN0	"\033[31;1m"
#define SHELL_BAN1      "------------------------------------------"
#define SHELL_BAN2      "   __    __   _____    _   _    _    _    "
#define SHELL_BAN3      "   \\ \\  / /  |__ __|  | \\ | |  | |  | |   "
#define SHELL_BAN4      "    \\ \\/ /     | |    |  \\| |  | |  | |   "
#define SHELL_BAN5      "    / /\\ \\    _| |_   | \\   |  | |  | |   "
#define SHELL_BAN6      "   / /  \\ \\  |     |  | | \\ |  \\  --  /   "
#define SHELL_BAN7      "   --    --   -----    -   -     ----     "
#define SHELL_BAN8      "------------------------------------------"
#define	SHELL_BAN9	"\033[0m\n"

/* Messages shell displays for user */

#define SHELL_PROMPT	"xsh $ "	/* Command prompt		*/
#define SHELL_STRTMSG	"Welcome to Xinu!\n"/* Welcome message		*/
#define SHELL_EXITMSG	"Shell closed\n"/* Shell exit message		*/
#define SHELL_SYNERRMSG	"Syntax error\n"/* Syntax error message		*/
#define SHELL_CREATMSG	"Cannot create process\n"/* command error	*/
#define SHELL_INERRMSG	"Cannot open file %s for input\n" /* Input err	*/
#define SHELL_OUTERRMSG	"Cannot open file %s for output\n"/* Output err	*/
					/* Builtin cmd error message	*/
#define SHELL_BGERRMSG	"Cannot redirect I/O or background a builtin\n"

/* Constants used for lexical analysis */

#define	SH_NEWLINE	'\n'		/* New line character		*/
#define	SH_EOF		'\04'		/* Control-D is EOF		*/
#define	SH_AMPER	'&'		/* Ampersand character		*/
#define	SH_BLANK	' '		/* Blank character		*/
#define	SH_TAB		'\t'		/* Tab character		*/
#define	SH_SQUOTE	'\''		/* Single quote character	*/
#define	SH_DQUOTE	'"'		/* Double quote character	*/
#define	SH_LESS		'<'		/* Less-than character	*/
#define	SH_GREATER	'>'		/* Greater-than character	*/

/* Token types */

#define	SH_TOK_AMPER	0		/* Ampersand token		*/
#define	SH_TOK_LESS	1		/* Less-than token		*/
#define	SH_TOK_GREATER	2		/* Greater-than token		*/
#define	SH_TOK_OTHER	3		/* Token other than those	*/
					/*   listed above (e.g., an	*/
					/*   alphanumeric string)	*/

/* Shell return constants */

#define	SHELL_OK	 0
#define	SHELL_ERROR	 1
#define	SHELL_EXIT	-3

/* Structure of an entry in the table of shell commands */

struct	cmdent	{			/* Entry in command table	*/
	char	*cname;			/* Name of command		*/
	bool8	cbuiltin;		/* Is this a builtin command?	*/
	int32	(*cfunc)(int32,char*[]);/* Function for command		*/
};

extern	uint32	ncmd;
extern	const	struct	cmdent	cmdtab[];
