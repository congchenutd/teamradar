#ifndef DEFINES_H
#define DEFINES_H

#include <QtGlobal>

namespace TeamRadar {

// defines platforms
// ensure this file in included the first (in main.cpp)
#ifdef Q_WS_SIMULATOR
	#define OS_MOBILE
#elif defined(Q_OS_WIN) || defined(Q_OS_MAC) || defined(Q_OS_LINUX)
	#define OS_DESKTOP
#else
	#define OS_MOBILE
#endif

} // namespace TeamRadar

#endif // DEFINES_H
