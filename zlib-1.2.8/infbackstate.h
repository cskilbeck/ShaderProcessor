#ifndef __INFBACKSTATE_H
#define __INFBACKSTATE_H

#include "code.h"

struct inflateBackState
{
	// common to infback and infback9
	z_const unsigned char FAR *next;    /* next input */
	unsigned char FAR *put;     /* next output */
	unsigned have, left;        /* available input and output */
	unsigned long hold;         /* bit buffer */
	unsigned bits;              /* bits in bit buffer */
	unsigned copy;              /* number of stored or match bytes to copy */
	unsigned char FAR *from;    /* where to copy match bytes from */
	code here;                  /* current decoding table entry */
	code last;                  /* parent table entry */
	unsigned len;               /* length to copy for repeats, bits to drop */
	int ret;                    /* return code */
	int mode;				      /* current inflate mode */
	int lastblock;              /* true if processing last block */
	int wrap;                   /* true if the window has wrapped */
	unsigned char FAR *window;  /* allocated sliding window, if needed */
	unsigned extra;             /* extra bits needed */
	unsigned long length;       /* literal or length of data to copy */
	unsigned long offset;       /* distance back to copy string from */
	code const FAR *lencode;    /* starting table for length/literal codes */
	code const FAR *distcode;   /* starting table for distance codes */
	unsigned lenbits;           /* index bits for lencode */
	unsigned distbits;          /* index bits for distcode */
	int status;
};

static int SizeOfInflateBackState()
{
	return sizeof(struct inflateBackState);
}

#endif