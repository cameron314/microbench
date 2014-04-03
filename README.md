# moodycamel::microbench

Provides a dead-simple micro-benchmarking function for C++. Executes a given function
some number of times per test run, and returns the time it took for the fastest test run to execute.

Works on Windows, Linux, and Mac OS X. Compile with VS2010 or better, or a C++11 compliant compiler (e.g. gcc 4.7+).

## Basic Example

    #include "microbench/microbench.h"
    #include <cstdio>

    void some_function();

    printf("some_function takes %.1fms to execute\n",
        moodycamel::microbench(&some_function)
    );

## Detailed Example

    // GCC command-line: g++ -std=c++11 -DNDEBUG -O3 -lrt main.cpp microbench/systemtime.cpp -o bench
    // (omit -lrt if on Windows or Mac OS X)

    #include "microbench/microbench.h"
    #include <cstdio>
    #include <atomic>

    std::atomic<int> x(0);
    int y = 0;
    printf("CAS takes %.4fns on average\n",
        moodycamel::microbench(
            [&]() { x.compare_exchange_strong(y, 0); },  /* function to benchmark */
            100000, /* iterations per test run */
            20, /* number of test runs */
            true /* whether to use the average time per iteration (the default) or total time per run (pass false) */
        ) * 1000 * 1000    // ms -> ns
    );
    
    // Result in my environment: Clocks in at ~24ns per CAS operation
    
## Moar statistics

`microbench` can also give you the minimum, maximum, range, quartiles, median, mean, and standard deviation
if you're feeling particularly peckish. Example:

    std::atomic<int> x(0);
    int y = 0;
    moodycamel::stats_t stats =
        moodycamel::microbench_stats([&]() { x.compare_exchange_strong(y, 0); }, 100000, 20);
    printf("CAS statistics: avg: %.2fns, min: %.2fns, max: %.2fns, stddev: %.2fns, Q1: %.2fns, median: %.2fns, Q3: %.2fns\n",
        stats.avg() * 1000 * 1000,
        stats.min() * 1000 * 1000,
        stats.max() * 1000 * 1000,
        stats.stddev() * 1000 * 1000,
        stats.q1() * 1000 * 1000,
        stats.median() * 1000 * 1000,
        stats.q3() * 1000 * 1000);
        
    // Result in my environment: avg: 26.47ns, min: 24.12ns, max: 32.98ns, stddev: 3.07ns, Q1: 24.15ns, median: 24.64ns, Q3: 30.19ns

## Why?

Yeah, I'm sure there's lots of other libraries that do the same thing, and a good portion of programmers
have no doubt cobbled together their own microbenchmark shims over the years. But I wanted something
reusable with minimal dependencies, and something that was **easy to use** so that I wouldn't have to waste
time wondering how long something might take -- I could *just check* (without having to worry about
icky platform-specific high-resolution timer routines and bothersome boilerplate).

## License

Released under the [simplified BSD license](https://github.com/cameron314/microbench/blob/master/LICENSE.md).
