
/* config.y - yacc input file for the Xinu config program */

/************************************************************************/
/*									*/
/*	This file contains the yacc grammar and	semantic functions	*/
/*    that read a Xinu Configuration file and generate output files	*/
/*    conf.h and conf.c that are then compiled along with the rest of	*/
/*    Xinu.								*/
/*									*/
/************************************************************************/

%token	DEFBRK IFBRK COLON OCTAL INTEGER IDENT CSR IRQ INTR INIT OPEN
	CLOSE READ WRITE SEEK CONTROL IS ON GETC PUTC
%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Avoid warnings for undeclared items */

extern	char	*yytext;
extern	int	yyleng;
extern	int	yylex(void);


/********************************************************************************/
/*										*/
/*			Start of Definitions					*/
/*										*/
/********************************************************************************/

#define	NIL	(struct dev_ent *)0x00

#define	CONFC	 "conf.c"		/* Name of .c output			*/
#define CONFH	 "conf.h"		/* Name of .h output			*/
#define	CONFHREF "<conf.h>"		/* How conf.h referenced		*/
#define	INFILE	 "Configuration"	/* Name of input file			*/
#define	MAXNAME  16			/* Max length of names			*/

#define	NDEVS		250		/* Max devices				*/
#define	NTYPES		250		/* Max device types			*/

int	linectr = 1;

FILE	*confc;
FILE	*confh;

int	brkcount = 0;			/* Count of %% separators till now in	*/
					/*  the input file			*/
char	*doing = "device type declarations";

struct	dev_ent	{			/* Entry for a device or device type	*/
	char	name[MAXNAME];		/* device name (unused in a type)	*/
	char	tname[MAXNAME];		/* Type name				*/
	char	ison[MAXNAME];		/* Name is "on" XXX			*/
	int	tindex;			/* Index in dtypes (unused in a type)	*/
	int	csr;			/* Control Status Register addr		*/
	int	irq;			/* interrupt request			*/
	char	intr[MAXNAME];		/* interrupt function name		*/
	char	init[MAXNAME];		/* init function name			*/
	char	open[MAXNAME];		/* open function name			*/
	char	close[MAXNAME];		/* close function name			*/
	char	read[MAXNAME];		/* read function name			*/
	char	write[MAXNAME];		/* write function name			*/
	char	control[MAXNAME];	/* control function name		*/
	char	seek[MAXNAME];		/* seek function name			*/
	char	getc[MAXNAME];		/* getc function name			*/
	char	putc[MAXNAME];		/* putc function name			*/
	int	minor;			/* In a device, the minor device	*/
					/*  assigned to the device 0,1,...	*/
					/*  in a type, the next minor number	*/
					/*  to assign				*/
};
struct	dev_ent		dtypes[NTYPES];/* Table of all device types		*/
int	ntypes	= 0;			/* Number of device types found		*/

struct	dev_ent		devs[NDEVS];	/* Table of all devices			*/
int	ndevs = 0;			/* Number of devices found		*/

char *devstab[] = {
	"/* Device table entry */",
	"struct\tdentry\t{",
	"\tint32   dvnum;",
	"\tint32   dvminor;",
	"\tchar    *dvname;",
	"\tdevcall (*dvinit) (struct dentry *);",
	"\tdevcall (*dvopen) (struct dentry *, char *, char *);",
	"\tdevcall (*dvclose)(struct dentry *);",
	"\tdevcall (*dvread) (struct dentry *, void *, uint32);",
	"\tdevcall (*dvwrite)(struct dentry *, void *, uint32);",
	"\tdevcall (*dvseek) (struct dentry *, int32);",
	"\tdevcall (*dvgetc) (struct dentry *);",
	"\tdevcall (*dvputc) (struct dentry *, char);",
	"\tdevcall (*dvcntl) (struct dentry *, int32, int32, int32);",
	"\tvoid    *dvcsr;",
	"\tvoid    (*dvintr)(void);",
	"\tbyte    dvirq;",
	"};\n",
	"extern	struct	dentry	devtab[]; /* one entry per device */",
	NULL
};

char	saveattrid[MAXNAME];		/* Holds the IDENT from an attribute	*/

/********************************************************************************/
/*										*/
/*			       Function prototypes				*/
/*										*/
/********************************************************************************/

void	addattr(int, int);
int	addton(char *);
int	config_atoi(char *, int);
void	devisid(char *);
void	devonid(char *);
void	getattrid(char *);
void	newdev(char *);
int	newtype(char *);
void	yyerror(char *);


%}

%%
/************************************************************************/
/*									*/
/*		Grammar rules for the entire Configuration		*/
/*									*/
/************************************************************************/

configuration:	devtypes DEFBRK devices
;

/************************************************************************/
/*									*/
/*		    Grammar rules for device types			*/
/*									*/
/************************************************************************/

devtypes:	/* nothing */ { doing = "device definitions"; }
		| devtypes devtype
;

devtype:	tname COLON dev_tlist
;

tname:		IDENT { $$ = newtype(yytext); }
;

dev_tlist:	theader attr_list
		| dev_tlist theader attr_list
;

theader:	ON tonid { $$ = $2; }
;

tonid:		IDENT { $$ = addton(yytext); }
;

attr_list:	/* nothing */
		| attr_list attr
;

attr:		  CSR number		{ addattr(CSR,    $2);	}
		| IRQ number		{ addattr(IRQ,    $2);	}
		| INTR id		{ addattr(INTR,    0);	}
		| OPEN id		{ addattr(OPEN,    0);	}
		| CLOSE id		{ addattr(CLOSE,   0);	}
		| INIT id		{ addattr(INIT,    0);	}
		| GETC id		{ addattr(GETC,    0);	}
		| PUTC id		{ addattr(PUTC,    0);	}
		| READ id		{ addattr(READ,    0);	}
		| WRITE id		{ addattr(WRITE,   0);	}
		| SEEK id		{ addattr(SEEK,    0);	}
		| CONTROL id		{ addattr(CONTROL, 0);	}
;

id:		IDENT { $$ = 0; getattrid(yytext); }
;

number:		INTEGER { $$ = config_atoi(yytext, yyleng); }
;


/************************************************************************/
/*									*/
/*		Grammar rules for device definitions			*/
/*									*/
/************************************************************************/

devices:	/* nothing */ { doing = "interface types"; }
		| devices device
;

device:		dheader attr_list
;

dheader:	dname devis devon
;

dname:		IDENT { newdev(yytext); }
;

devis:		IS devisid
;

devisid:	IDENT { devisid(yytext); }
;

devon:		ON devonid
;

devonid:	IDENT { devonid(yytext); }
;

%%

#include "lex.yy.c"


/************************************************************************/
/*									*/
/* main  -  main program: parse arguments, invoke the parser, and	*/
/*		write the conf.h and conf.c files			*/
/*									*/
/************************************************************************/


int main(int argc, char **argv) {
	int n, i, j, l, fcount;
	struct dev_ent *s;
	int   verbose = 0;
	char *p;
	int  c;

	if ( argc > 1 && (strncmp("-v", argv[1], 2) == 0) ) {
		argc--;
		argv++;
		verbose++;
	}

	if ( argc > 4 ) {
		fprintf(stderr, "use: config [-v] [input_file] [conf.c] [conf.h]\n");
		exit(1);
	}

	if (verbose) { printf("Opening input file...\n"); }

	if (argc >= 2) {
		if (freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "Can't open %s\n", argv[1]);
			exit(1);
		}
	}
	else {	/* try to open Configuration file */
		if (freopen(INFILE, "r", stdin) == NULL) {
			fprintf(stderr, "Can't open %s\n", INFILE);
			exit(1);
		}
	}

	/****************************************************************/
	/*								*/
	/*		Parse the Configuration file			*/
	/*								*/
	/****************************************************************/


	if (verbose) { printf("Parsing configuration specs...\n"); }

	if ( (n = yyparse()) != 0 ) { exit(n); }

	/* Open conf.h and conf.c for writing */

	if (verbose) { printf("Opening output files...\n"); }

	if (argc >= 3) {
		if ( (confc = fopen(argv[2],"w") ) == NULL) {
			fprintf(stderr, "Can't write on %s\n", argv[2]);
			exit(1);
		}
	}
	else { 	/* try to open conf.c file */
		if ( (confc = fopen(CONFC,"w") ) == NULL) {
			fprintf(stderr, "Can't write on %s\n", CONFC);
			exit(1);
		}
	}

	if (argc >= 4) {
		if ( (confh = fopen(argv[3],"w") ) == NULL) {
			fprintf(stderr, "Can't write on %s\n", argv[3]);
			exit(1);
		}
	}
	else { 	/* try to open conf.h file */
		if ( (confh = fopen(CONFH,"w") ) == NULL) {
			fprintf(stderr, "Can't write on %s\n", CONFH);
			exit(1);
		}
	}

	/****************************************************************/
	/*								*/
	/*			produce conf.h				*/
	/*								*/
	/****************************************************************/


	fprintf(confh, "/* conf.h (GENERATED FILE; DO NOT EDIT) */\n\n");

	if (verbose) { printf("Writing output...\n"); }

	fprintf(confh, "/* Device switch table declarations */\n\n");

	for (i = 0; (p = devstab[i]) != NULL; i++) {
		fprintf(confh, "%s\n", p);
	}

	fprintf(confh, "\n");

	/* write device declarations and definitions */

	fprintf(confh, "/* Device name definitions */\n\n");
	for (i = 0; i<ndevs; i++) {
		s = &devs[i];
		fprintf(confh, "#define %-20s%2d\t/* type %-8s */\n",
			s->name, i, s->tname);
	}
	fprintf(confh, "\n");

	/* write count of device types */

	fprintf(confh, "/* Control block sizes */\n\n");
	for (i = 0; i < ntypes;  i++) {
		s = &dtypes[i];
		if (s->minor > 0) {
			fprintf(confh, "#define\tN%s\t%d\n",
				s->tname, s->minor);
		}
	}

	fprintf(confh, "\n");

	if (ndevs > 0) { fprintf(confh, "#define NDEVS %d\n", ndevs); }

	/* Copy definitions to output */

	if (brkcount >= 4 && verbose) {
		printf("Copying definitions to %s...\n", CONFH);
	}

	if (brkcount >= 2) {
		while ( (c = input()) > 0) {	/* lex input function	*/
			putc(c, confh);
		}
	}
	fclose(confh);


	/****************************************************************/
	/*								*/
	/*			produce conf.c				*/
	/*								*/
	/****************************************************************/


	fprintf(confc, "/* conf.c (GENERATED FILE; DO NOT EDIT) */\n\n");
	fprintf(confc, "#include <xinu.h>\n\n");
	fprintf(confc, "\n");

	fprintf(confc, "extern\tdevcall\tioerr(void);\n");
	fprintf(confc, "extern\tdevcall\tionull(void);\n\n");

	/* produce devtab (giant I/O switch table) */
	fprintf(confc, "/* Device independent I/O switch */\n\n");
	if (ndevs > 0)
	{
		fprintf(confc, "struct	dentry	devtab[NDEVS] =\n{\n");
		fprintf(confc, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
			"/**",
			" * Format of entries is:",
			" * dev-number, minor-number, dev-name,",
			" * init, open, close,",
			" * read, write, seek,",
			" * getc, putc, control,",
			" * dev-csr-address, intr-handler, irq",
			" */");
	}

	for (i=0; i<ndevs; i++) {
		s = &devs[i];
		fprintf(confc, "/* %s is %s */\n", s->name, s->tname);
		fprintf(confc, "\t{ %d, %d, \"%s\",\n", i, s->minor, s->name);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
			s->init, s->open, s->close);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
			s->read, s->write, s->seek);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
			s->getc, s->putc, s->control);
		fprintf(confc, "\t  (void *)0x%x, (void *)%s, %d }",
			s->csr, s->intr, s->irq);
		if (i< ndevs-1) {
			fprintf(confc, ",\n\n");
		} else {
			fprintf(confc, "\n};");
		}
	}

	/* we must guarantee conf.c written later than conf.h for make */
	fprintf(confc, "\n");
	fclose(confc);

	/* finish up and write report for user if requested */
	if (verbose) {
		printf("Configuration complete. Number of devs=%d:\n", ndevs);
		for (i=0; i<ndevs; i++) {
			s = &devs[i];
			printf("Device %s (on %s)\n", s->name, s->ison);
			printf("    csr=0x%04x, irq=0x%04x, minor=%d\n",
				s->csr, s->irq, s->minor);
		}
	}
}

/************************************************************************/
/*									*/
/* addattr - add a new attribute spec to current type/device description*/
/*		tok: token type (attribute type)			*/
/*		val: symbol number of value				*/
/*									*/
/************************************************************************/

void	addattr(int tok, int val) {
	struct dev_ent *s;
	char *c;

	if (brkcount == 0) {
		/* Doing types */
		s = &dtypes[ntypes-1];
	} else {
		/* Doing devices */
		s = &devs[ndevs-1];
	}

	switch (tok) {
	case CSR:	s->csr = val;			break;
	case IRQ:	s->irq = val;			break;
	case INTR:	strcpy(s->intr, saveattrid);	break;
	case READ:	strcpy(s->read, saveattrid);	break;
	case WRITE:	strcpy(s->write,saveattrid);	break;
	case GETC:	strcpy(s->getc, saveattrid);	break;
	case PUTC:	strcpy(s->putc, saveattrid);	break;
	case OPEN:	strcpy(s->open, saveattrid);	break;
	case CLOSE:	strcpy(s->close,saveattrid);	break;
	case INIT:	strcpy(s->init, saveattrid);	break;
	case SEEK:	strcpy(s->seek, saveattrid);	break;
	case CONTROL:	strcpy(s->control,saveattrid);	break;
	default:	fprintf(stderr, "Internal error 1\n");
	}
}


/************************************************************************/
/*									*/
/* addton -- add an "on XXX" to the current type			*/
/*									*/
/************************************************************************/

int	addton(char *tonid) {
	int	currtype;		/* The current type		*/

	if (strlen(tonid) >= MAXNAME) {
		fprintf(stderr,"string %s is too long on line %d\n",
				tonid, linectr);
		exit(1);
	}
	currtype = ntypes - 1;
	strcpy(dtypes[currtype].ison, tonid);

	return currtype;
}


/************************************************************************/
/*									*/
/* config_atoi - convert an ascii string of text to an integer,		*/
/*			honoring octal, decimal, and hex		*/
/*									*/
/************************************************************************/

int	config_atoi(char *p, int len) {
	int base, rv;

	if (*p == '0')
	{
		++p;
		--len;
		if (*p == 'x' || *p == 'X')
		{
			++p; --len; /* skip 'x' */
			base = 16;
		}
		else
		{
			base = 8;
		}
	}
	else
	{
		base = 10;
	}

	rv = 0;
	for (; len > 0; ++p, --len)
	{
		rv *= base;
		if      (isdigit(*p)) { rv += *p - '0'; }
		else if (isupper(*p)) { rv += *p - 'A' + 10; }
		else                  { rv += *p - 'a' + 10; }
	}

	return rv;
}

/************************************************************************/
/*									*/
/* devisid -- add an "is XXX" to the current device			*/
/*									*/
/************************************************************************/

void	devisid(char *tname) {
	int	currdev;		/* The current device		*/
	int	i;

	if (strlen(tname) >= MAXNAME) {
		fprintf(stderr,"string %s is too long on line %d\n",
				tname, linectr);
		exit(1);
	}
	/* Verify the type exists */

	for (i=0; i<ntypes; i++) {
		if (strcmp(tname, dtypes[i].tname) == 0) {
			break;
		}
	}
	if (i >= ntypes) {
		fprintf(stderr, "Illegal type name %s on line %d\n",
				tname, linectr);
		exit(1);
	}
	currdev = ndevs - 1;
	strcpy(devs[currdev].tname, tname);

	return;
}


/************************************************************************/
/*									*/
/* devonid -- add an "on XXX" to the current device, lookup the type,	*/
/*		and copy attributes into the device from the type	*/
/*									*/
/************************************************************************/

void	devonid(char *onname) {
	int	currdev;		/* The current device		*/
	int	i;
	struct	dev_ent	*dptr;		/* Pointer to current device	*/
	struct	dev_ent	*tptr;		/* Pointer to a type		*/
	char	tmp[MAXNAME];		/* Holds the device name during	*/
					/*   copy			*/

	if (strlen(onname) >= MAXNAME) {
		fprintf(stderr,"string %s is too long on line %d\n",
				onname, linectr);
		exit(1);
	}
	if (ndevs <=0) {
		fprintf(stderr,"Internal error 3\n");
		exit(1);
	}
	currdev = ndevs - 1;
	dptr = &devs[currdev];

	strcpy(dptr->ison, onname);

	/* Lookup the device type */

	for (i=0; i<ntypes; i++) {
		tptr = &dtypes[i];
		if ( (strcmp(dptr->tname,tptr->tname) == 0 ) &&
		     (strcmp(dptr->ison, tptr->ison)  == 0 )  ){

			/* The specified type matches the ith entry, so	*/
			/*  set all attributes equal to the ones in the	*/
			/*  type definition.				*/

			strcpy(tmp, dptr->name);
			bcopy (tptr, dptr, sizeof(struct dev_ent));
			/* Increment the minor device number for the	*/
			/*  next time the type is used			*/
			tptr->minor++;
			strcpy(dptr->name, tmp);
			return;
		}
	}

	fprintf(stderr, "Ileagal device specification on line %d\n", linectr);
	exit(1);
}


/************************************************************************/
/*									*/
/* getattrid -- pick up and save the attribute string from an id	*/
/*									*/
/************************************************************************/

void	getattrid(char *str) {

	if (strlen(str) >= MAXNAME) {
		fprintf(stderr,"atribute string %s is too long on line %d\n",
				str, linectr);
		exit(1);
	}
	strcpy(saveattrid, str);
	return;
}


/************************************************************************/
/*									*/
/* newdev -- allocate an entry in devs, initialize, and fill in the name*/
/*									*/
/************************************************************************/

void	newdev(char *name) {

	struct	dev_ent	*dptr;		/* Ptr. to an entry in devs	*/
	int	i;

	if (ndevs >= NDEVS) {
		fprintf(stderr,"Too many devices on line %d", linectr);
		exit(1);
	}
	if (strlen(name) >= MAXNAME) {
		fprintf(stderr,"Device name %s is too long on line %d\n",
				name, linectr);
		exit(1);
	}

	/* Verify that the device name is unique */

	for (i=0; i<ndevs; i++) {
		if (strcmp(name, devs[i].name) == 0) {
			fprintf(stderr, "Duplicate device name %s on line %d\n",
				name, linectr);
			exit(1);
		}
	}

	dptr = &devs[ndevs];

	/* Initialize fields in the entry */

	bzero((void *)dptr, sizeof(struct dev_ent));
	strcpy(dptr->name,	 name);
	ndevs++;
	return;
}


/************************************************************************/
/*									*/
/* newtype -- allocate an entry in the type array and fill in the name	*/
/*									*/
/************************************************************************/

int	newtype(char *name) {

	struct	dev_ent	*dptr;		/* Ptr. to an entry in dtypes	*/
	int	i;			/* Index into the type table	*/

	if (ntypes >= NTYPES) {
		fprintf(stderr,"Too many types on line %d", linectr);
		exit(1);
	}
	if (strlen(name) >= MAXNAME) {
		fprintf(stderr,"Type name %s is too long on line %d\n",
				name, linectr);
		exit(1);
	}

	/* Verify that the type name is unique */

	for (i=0; i<ntypes; i++) {
		if (strcmp(name, dtypes[i].tname) == 0) {
			fprintf(stderr, "Duplicate type name %s on line %d\n",
				name, linectr);
			exit(1);
		}
	}

	dptr = &dtypes[ntypes];

	/* Initialize fields in the entry */

	bzero((void *)dptr, sizeof(struct dev_ent));
	strcpy(dptr->tname,	 name);
	strncpy(dptr->intr,	"ioerr", 5);
	strncpy(dptr->init,	"ioerr", 5);
	strncpy(dptr->open,	"ioerr", 5);
	strncpy(dptr->close,	"ioerr", 5);
	strncpy(dptr->read,	"ioerr", 5);
	strncpy(dptr->write,	"ioerr", 5);
	strncpy(dptr->control,	"ioerr", 5);
	strncpy(dptr->seek,	"ioerr", 5);
	strncpy(dptr->getc,	"ioerr", 5);
	strncpy(dptr->putc,	"ioerr", 5);

	return ntypes++;
}


/************************************************************************/
/*									*/
/* yyerror - print an error message with the line number		*/
/*									*/
/************************************************************************/

void yyerror(char *s) {

	fprintf(stderr, "Syntax error in %s on line %d\n", doing, linectr);
}
