#ifndef __CODE_H
typedef struct
{
	unsigned char op;           /* operation, extra bits, table bits */
	unsigned char bits;         /* bits in this part of the code */
	unsigned short val;         /* offset in table or code value */
} code;
#define __CODE_H
#endif
