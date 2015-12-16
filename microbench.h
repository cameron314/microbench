#include "systemtime.h"

#include <cstdint>
#include <cassert>
#include <utility>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace moodycamel
{
	struct stats_t
	{
		stats_t(double* results, std::size_t count)
		{
			std::sort(results, results + count);
			
			_min = results[0];
			_max = results[count - 1];
			
			if (count == 1) {
				_q[0] = _q[1] = _q[2] = results[0];
				_avg = results[0];
				_variance = 0;
				return;
			}
			
			// Kahan summation to reduce error (see http://en.wikipedia.org/wiki/Kahan_summation_algorithm)
			double sum = 0;
			double c = 0;	// Error last time around
			for (std::size_t i = 0; i != count; ++i) {
				auto y = results[i] - c;
				auto t = sum + y;
				c = (t - sum) - y;
				sum = t;
			}
			_avg = sum / count;
			
			// Calculate unbiased (corrected) sample variance
			sum = 0, c = 0;
			for (std::size_t i = 0; i != count; ++i) {
				auto y = (results[i] - _avg) * (results[i] - _avg) - c;
				auto t = sum + y;
				c = (t - sum) - y;
				sum = t;
			}
			_variance = sum / (count - 1);
			
			// See Method 3 here: http://en.wikipedia.org/wiki/Quartile
			_q[1] = (count & 1) == 0 ? (results[count / 2 - 1] + results[count / 2]) * 0.5 : results[count / 2];
			if ((count & 1) == 0) {
				_q[0] = (count & 3) == 0 ? (results[count / 4 - 1] + results[count / 4]) * 0.5 : results[count / 4];
				_q[2] = (count & 3) == 0 ? (results[count / 2 + count / 4 - 1] + results[count / 2 + count / 4]) * 0.5 : results[count / 2 + count / 4];
			}
			else if ((count & 3) == 1) {
				_q[0] = results[count / 4 - 1] * 0.25 + results[count / 4] * 0.75;
				_q[2] = results[count / 4 * 3] * 0.75 + results[count / 4 * 3 + 1] * 0.25;
			}
			else {		// (count & 3) == 3
				_q[0] = results[count / 4] * 0.75 + results[count / 4 + 1] * 0.25;
				_q[2] = results[count / 4 * 3 + 1] * 0.25 + results[count / 4 * 3 + 2] * 0.75;	
			}
		}
		
		inline double min() const { return _min; }
		inline double max() const { return _max; }
		inline double range() const { return _max - _min; }
		inline double avg() const { return _avg; }
		inline double variance() const { return _variance; }
		inline double stddev() const { return std::sqrt(_variance); }
		inline double median() const { return _q[1]; }
		inline double q1() const { return _q[0]; }
		inline double q2() const { return _q[1]; }
		inline double q3() const { return _q[2]; }
		inline double q(std::size_t which) const { assert(which < 4 && which > 0); return _q[which - 1]; }
		
	private:
		double _min;
		double _max;
		double _q[3];
		double _avg;
		double _variance;
	};
	
	
	// Times how long it takes to run a given function for a given number of iterations; this
	// timing process is repeated for a given number of test runs; various statistics of the
	// of timing results are returned in a `stats_t` object.
	template<typename TFunc>
	stats_t microbench_stats(TFunc&& func, std::uint64_t iterations = 1, std::uint32_t testRuns = 100, bool returnTimePerIteration = true)
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
			if (returnTimePerIteration) {
				results[i] /= iterations;
			}
		}
		
		double fastest = results[0];
		for (std::uint32_t i = 1; i < testRuns; ++i) {
			if (results[i] < fastest) {
				fastest = results[i];
			}
		}
		
		stats_t stats(results, testRuns);
		delete[] results;
		return stats;
	}
	
	
	// Times how long it takes to run a given function for a given number of iterations; this
	// timing process is repeated for a given number of test runs; the fastest of the runs is
	// selected, and its time returned (in milliseconds).
	template<typename TFunc>
	double microbench(TFunc&& func, std::uint64_t iterations = 1, std::uint32_t testRuns = 100, bool returnTimePerIteration = true)
	{
		return microbench_stats(std::forward<TFunc>(func), iterations, testRuns, returnTimePerIteration).min();
	}
}

