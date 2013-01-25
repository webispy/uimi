#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <asm/unistd.h>
#include <uimi.h>

static char log_priority_mark[] = {
	'V',
	'I',
	'D',
	'W',
	'E',
	'F',
};

static pid_t get_tid (void)
{
	return syscall (__NR_gettid);
}

void uimi_log(enum uimi_log_priority priority,
		const char *tag, const char *fmt, ...)
{
	va_list ap;
	char buf[1024] = { 0, };

	va_start(ap, fmt);
	vsnprintf(buf, 1023, fmt, ap);
	va_end(ap);

	printf ("%c/%-10s(%5d): %s", log_priority_mark[priority],
			tag, get_tid(), buf);
}
