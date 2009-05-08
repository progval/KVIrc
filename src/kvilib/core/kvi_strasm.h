#ifndef _KVI_STRASM_H_
#define _KVI_STRASM_H_

//=============================================================================
//
//   File : kvi_strasm.h
//   Creation date : Sun Jun 18 2000 18:38:26 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2000 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

//=============================================================================
//
//   Inline assembly implementations of the commonly used string functions
//   These will work only on i386 based machines and can be compiled
//   only by gcc
//
//=============================================================================

extern inline bool kvi_strEqualCS(const char * str1,const char * str2)
{
	// An instruction pattern is really useful in this case.
	// When inlining, GCC can optimize to load esi and edi
	// directly with the strings , without pushing and getting it
	// from the stack...
	register bool eax;
	__asm__ __volatile__ (
		"	cld\n"
		"1:\n"
		"	lodsb %%ds:(%%esi),%%al\n"
		"	scasb %%es:(%%edi),%%al\n"
		"	jne 2f\n"
		"	testb %%al,%%al\n"
		"	jne 1b\n"
		"	movl $0x1,%%eax\n"
		"	jmp 3f\n"
		"2:\n"
		"	xorl %%eax,%%eax\n"
		"3:"
		: "=a" (eax), "=&S" (str1), "=&D" (str2)
		:             "1"   (str1), "2"   (str2)
	);
	return eax;
}

extern inline bool kvi_strEqualCSN(const char * str1,const char * str2,int len)	
{
	register bool eax;
	__asm__ __volatile__ (
		"1:\n"
		"	decl %3\n"
		"	js 2f\n"
		"	movb (%1),%%al\n"
		"	incl %1\n"
		"	cmpb %%al,(%2)\n"
		"	jne 3f\n"
		"	incl %2\n"
		"	testb %%al,%%al\n"
		"	jne 1b\n"
		"2:\n"
		"	movl $0x1,%%eax\n"
		"	jmp 4f\n"
		"3:\n"
		"	xorl %%eax,%%eax\n"
		"4:\n"
		: "=a" (eax),  "=r" (str1), "=r" (str2), "=r" (len)
		:              "1"  (str1), "2"  (str2), "3"  (len)
	);
	return eax;
}

// OPTIMIZATION
// The following two functions are used to compare a variable string with one in that
// only A-Z<->a-z case insensivity is significant.
// For example 
// kvi_strEqualNoLocalCI("a string that does not contain any strange char",str2)
// will always give the correct result
// These will NOT work with localizable characters:
// 'a' with umlaut will be not equal to 'A' with umlaut

extern inline bool kvi_strEqualNoLocaleCI(const char *str1,const char *str2)
{
	// Trivial implementation
	// Ignores completely locales....only A-Z chars are transformed to a-z
	// Anyway...it will work for IRC :)
	register int reg;
	register bool eax;
	__asm__ __volatile__ (
		"1:\n"
		"	movb (%2),%%al\n"
		"	cmpb $65,%%al\n"
		"	jb 2f\n"
		"	cmpb $90,%%al\n"
		"	ja 2f\n"
		"	addb $32,%%al\n"
		"2:\n"
		"	movb (%3),%b1\n"
		"	cmpb $65,%b1\n"
		"	jb 3f\n"
		"	cmpb $90,%b1\n"
		"	ja 3f\n"
		"	addb $32,%b1\n"
		"3:\n"
		"	cmpb %%al,%b1\n"
		"	jne 4f\n"
		"	incl %2\n"
		"	incl %3\n"
		"	testb %%al,%%al\n"
		"	jne 1b\n"
		"	movl $1,%%eax\n"
		"	jmp 5f\n"
		"4:\n"
		"	xorl %%eax,%%eax\n"
		"5:\n"
		: "=a" (eax), "=q" (reg), "=r" (str1), "=r" (str2)
		:                         "2"  (str1), "3"  (str2)
	);
	return eax;
}

extern inline bool kvi_strEqualNoLocaleCIN(const char *str1,const char *str2,int len)
{

	register int reg;
	register bool eax;
	__asm__ __volatile__ (
		"1:\n"
		"	decl %4\n"
		"	js 4f\n"
		"	movb (%2),%%al\n"
		"	cmpb $65,%%al\n"
		"	jb 2f\n"
		"	cmpb $90,%%al\n"
		"	ja 2f\n"
		"	addb $32,%%al\n"
		"2:\n"
		"	movb (%3),%b1\n"
		"	cmpb $65,%b1\n"
		"	jb 3f\n"
		"	cmpb $90,%b1\n"
		"	ja 3f\n"
		"	addb $32,%b1\n"
		"3:\n"
		"	cmpb %%al,%b1\n"
		"	jne 5f\n"
		"	incl %2\n"
		"	incl %3\n"
		"	testb %%al,%%al\n"
		"	jne 1b\n"
		"4:\n"
		"	movl $1,%%eax\n"
		"	jmp 6f\n"
		"5:\n"
		"	xorl %%eax,%%eax\n"
		"6:\n"
		: "=a" (eax), "=q" (reg), "=r" (str1), "=r" (str2), "=r" (len)
		:                         "2"  (str1), "3"  (str2), "4"  (len)
	);
	return eax;
}


extern inline int kvi_strLen(const char * str)
{
	register int ecx;
	__asm__ __volatile__(
		"	cld\n"
		"	repne\n"
		"	scasb\n"
		"	notl %0\n"
		"	decl %0"
		: "=c" (ecx),        "=&D" (str)
		: "0"  (0xffffffff), "1"   (str), "a" (0)
	);
	return ecx;
}

#endif //_KVI_STRASM_H_
