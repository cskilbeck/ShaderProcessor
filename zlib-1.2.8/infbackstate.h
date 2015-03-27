#ifndef __INFBACKSTATE_H
#define __INFBACKSTATE_H

struct inflateBackState
{
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
	int status;					// 0 = fresh, 1 = used
};

inline int SizeOfInflateBackState()
{
	return sizeof(struct inflateBackState);
}

#endif