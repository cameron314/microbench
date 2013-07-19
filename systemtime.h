#pragma once

#if defined(_MSC_VER)
#define ST_WINDOWS
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__) || defined(BSD)
#define ST_NIX
#endif

namespace moodycamel
{
	void sleep(int milliseconds);
}

#if defined(ST_WINDOWS)
namespace moodycamel { typedef unsigned __int64 SystemTime; }
#elif defined(ST_NIX)
#include <time.h>
namespace moodycamel { typedef timespec SystemTime; }
#endif

namespace moodycamel
{
	SystemTime getSystemTime();

	// Returns the delta time, in milliseconds
	double getTimeDelta(SystemTime start);
}
