#ifndef DEFINES_H
#define DEFINES_H

#include <QtGlobal>

#ifdef Q_WS_SIMULATOR
	#define OS_MOBILE
#elif defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(Q_OS_LINUX)
	#define OS_DESKTOP
#else
	#define OS_MOBILE
#endif

#endif // DEFINES_H
