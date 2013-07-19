#include "systemtime.h"

#include <cstdint>
#include <cassert>


namespace moodycamel
{
	// Times how long it takes to run a given function for a given number of iterations; this
	// timing process is repeated for a given number of test runs; the fastest of the runs is
	// selected, and its time returned (in milliseconds).
	template<typename TFunc>
	double microbench(TFunc const& func, std::uint64_t iterations = 1, std::uint32_t testRuns = 100)
	{
		assert(testRuns >= 1);
		assert(iterations >= 1);

		auto results = new double[testRuns];
		for (std::uint32_t i = 0; i < testRuns; ++i) {
			auto start = getSystemTime();
			for (std::uint64_t j = 0; j < iterations; ++j) {
				func();
			}
			results[i] = getTimeDelta(start);
		}
		
		double fastest = results[0];
		for (std::uint32_t i = 1; i < testRuns; ++i) {
			if (results[i] < fastest) {
				fastest = results[i];
			}
		}

		delete[] results;
		return fastest;
	}
}

