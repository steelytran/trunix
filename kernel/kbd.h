#ifndef _SYS_KBD_H
#define _SYS_KBD_H

#define NIL 0

static unsigned char normalmap[256] = {
	NIL,  0x1B, '1',  '2',  '3',  '4',  '5',  '6',
	'7',  '8',  '9',  '0',  '-',  '=',  '\b', '\t',
	'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
	'o',  'p',  '{',  '}',  '\n', NIL,  'a',  's',
	'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
	'\'', '`',  NIL,  '\\', 'z',  'x',  'c',  'v',
	'b',  'n',  'm',  ',',  '.',  '/',  NIL,  '*',
	NIL,  ' ',  NIL,  NIL,  NIL,  NIL,  NIL,  NIL,
	NIL,  NIL,  NIL,  NIL,  NIL,  NIL,  NIL,  '7',
	'8',  '9',  '-',  '4',  '5',  '6',  '+',  '1',  
	'2',  '3',  '0',  '.',  NIL,  NIL,  NIL,  NIL,
};

extern uint8_t keybuffer[12];
extern unsigned short keysp;

//void got_char(uint8_t *);
void init_kbd(void);
void got_char(void);

#endif
