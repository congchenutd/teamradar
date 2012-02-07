#ifndef DEFINES_H
#define DEFINES_H

#if defined Q_OS_WIN32 || defined Q_OS_MAC || defined Q_OS_LINUX
	#define OS_DESKTOP
#else
	#define OS_MOBILE
#endif


#endif // DEFINES_H
