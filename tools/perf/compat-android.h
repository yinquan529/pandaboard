/* Android compatibility header
 * Provides missing bits in Bionic on Android, ignored
 * on regular Linux.
 *
 * Written by Bernhard.Rosenkranzer@linaro.org
 *
 * Released into the public domain. Do with this file
 * whatever you want.
 */
#ifdef ANDROID
/* Bionic has its own idea about ALIGN, and kills other definitions.
 * Done outside the multiple-inclusion wrapper to make sure we
 * can override Bionic's ALIGN by simply including compat-android.h
 * again after including Bionic headers.
 */
#undef ALIGN
#undef __ALIGN_MASK
#define ALIGN(x,a)              __ALIGN_MASK(x,(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#ifndef _COMPAT_ANDROID_H_
#define _COMPAT_ANDROID_H_ 1
#include <stdio.h>
#include <signal.h>
#include <asm/page.h> // for PAGE_SIZE
#include <asm/termios.h> // for winsize

#ifndef __WORDSIZE
#include <stdint.h>
#define __WORDSIZE _BITSIZE
#endif

#ifndef roundup
#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))
#endif

#ifndef __force
#define __force
#endif

// Assorted functions that are missing from Bionic
static void psignal(int sig, const char *s) {
	if(sig>=0 && sig<NSIG) {
		if(s)
			fprintf(stderr, "%s: %s\n", s, sys_siglist[sig]);
		else
			fprintf(stderr, "%s\n", sys_siglist[sig]);
	} else {
		if(s)
			fprintf(stderr, "%s: invalid signal\n", s);
		else
			fputs("invalid signal\n", stderr);
	}
}

static ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
	size_t ret = 0;

	if (!lineptr || !n || !stream)
		return -1;

	if(!*lineptr) {
		*n = 128;
		*lineptr = (char*)malloc(*n);
		if(!*lineptr)
			return -1;
	}

	while(!feof(stream) && !ferror(stream)) {
		int c;
		if(ret == *n) {
			*n += 128;
			*lineptr = (char*)realloc(*lineptr, *n);
			if(!*lineptr) {
				*n = 0;
				return -1;
			}
		}
		c = fgetc(stream);
		if(c == EOF)
			break;
		*lineptr[ret++] = c;
		if(c == '\n')
			break;
	}
	*lineptr[ret] = 0;
	return ret;
}
#endif
#endif
