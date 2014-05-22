/*
 * Copyright (c) 2000, 2001, 2002, 2003, 2004, 2005, 2008, 2009
 *	The President and Fellows of Harvard College.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <types.h>
#include <kern/errmsg.h>
#include <lib.h>
#include <thread.h>

/*
 * Like strdup, but calls kmalloc.
 */
char *
kstrdup(const char *s)
{
	char *z;

	z = kmalloc(strlen(s)+1);
	if (z == NULL) {
		return NULL;
        }
	strcpy(z, s);
	return z;
}

/*
 * Standard C function to return a string for a given errno.
 * Kernel version; panics if it hits an unknown error.
 */

const char *
strerror(int errcode)
{
	if (errcode>=0 && errcode < sys_nerr) {
		return sys_errlist[errcode];
	}
	panic("Invalid error code %d\n", errcode);
	return NULL;
}

/*
 * Helper functions used by testing and problem driver code
 * to establish better mixtures of threads.
 */

void
random_yielder(uint32_t max_yield_count)
{
	uint32_t i;
	for (i = 0; i < random() % max_yield_count; i++) {
		thread_yield();
	}
}

void
random_spinner(uint32_t max_spin_count)
{
	uint32_t i;
	volatile int spin;
	for (i = 0; i < random() % max_spin_count; i++) {
		spin += i;
	}
}