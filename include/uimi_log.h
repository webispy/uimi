#ifndef __UIMI_LOG_H__
#define __UIMI_LOG_H__

__BEGIN_DECLS

#ifndef UIMI_LOG_TAG
#define UIMI_LOG_TAG	"UNKNOWN"
#endif

#define C_NORMAL		"\e[0m"

#define C_BLACK			"\e[0;30m"
#define C_RED			"\e[0;31m"
#define C_GREEN			"\e[0;32m"
#define C_BROWN			"\e[0;33m"
#define C_BLUE			"\e[0;34m"
#define C_MAGENTA		"\e[0;35m"
#define C_CYAN			"\e[0;36m"
#define C_LIGHTGRAY		"\e[0;37m"

#define C_DARKGRAY		"\e[1;30m"
#define C_LIGHTRED		"\e[1;31m"
#define C_LIGHTGREEN	"\e[1;32m"
#define C_YELLOW		"\e[1;33m"
#define C_LIGHTBLUE		"\e[1;34m"
#define C_LIGHTMAGENTA	"\e[1;35m"
#define C_LIGHTCYAN		"\e[1;36m"
#define C_WHITE			"\e[1;37m"

#define PRETTY_FUNC_BEGIN	C_DARKGRAY
#define PRETTY_FUNC_END		C_NORMAL

#define msg(fmt,args...)	{ uimi_log (UIMI_LOG_VERBOSE, UIMI_LOG_TAG, \
		fmt "\n", ##args); }

#define info(fmt,args...)	{ uimi_log (UIMI_LOG_INFO,  UIMI_LOG_TAG, \
		fmt "\n", ##args); }

#define dbg(fmt,args...)	{ uimi_log (UIMI_LOG_DEBUG, UIMI_LOG_TAG, \
		PRETTY_FUNC_BEGIN "<%s:%d> " PRETTY_FUNC_END fmt "\n", \
		__func__, __LINE__, ##args); }

#define warn(fmt,args...)	{ uimi_log (UIMI_LOG_WARN,  UIMI_LOG_TAG, \
		PRETTY_FUNC_BEGIN "<%s:%d> " PRETTY_FUNC_END fmt "\n", \
		__func__, __LINE__, ##args); }

#define err(fmt,args...)	{ uimi_log (UIMI_LOG_FATAL, UIMI_LOG_TAG, \
		PRETTY_FUNC_BEGIN "<%s:%d> " PRETTY_FUNC_END fmt "\n", \
		__func__, __LINE__, ##args); }

enum uimi_log_priority {
	UIMI_LOG_VERBOSE,
	UIMI_LOG_INFO,
	UIMI_LOG_DEBUG,
	UIMI_LOG_WARN,
	UIMI_LOG_ERROR,
	UIMI_LOG_FATAL,
};

void uimi_log (enum uimi_log_priority priority, const char *tag,
		const char *fmt, ...);


__END_DECLS

#endif
