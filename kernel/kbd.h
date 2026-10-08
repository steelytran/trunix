/*
 * keyboard
 * Copyright (C) 2026  spenna
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _KBD_H
#define _KBD_H

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

#endif
