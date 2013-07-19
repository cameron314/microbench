# moodycamel::microbench

Provides a dead-simple micro-benchmarking function for C++. Executes a given function
some number of times per test run, and returns the time it took for the fastest test run to execute.

Works on Windows and Linux. Compile with VS2010 or better, or a C++11 compliant compiler (e.g. gcc 4.7+).

## Basic Example

    #include "microbench/microbench.h"
    #include <cstdio>

    void some_function();

    printf("some_function takes %.1fms to execute\n",
        moodycamel::microbench(&some_function)
    );

## Detailed Example

    // GCC command-line: g++ -std=c++11 -DNDEBUG -O3 -lrt main.cpp microbench/systemtime.cpp -o bench

    #include "microbench/microbench.h"
    #include <cstdio>
    #include <atomic>

    std::atomic<int> x(0);
    int y = 0;
    printf("CAS takes %.4fms to execute 100000 iterations\n",
        moodycamel::microbench(
            [&]() { x.compare_exchange_strong(y, 0); },  /* function to benchmark */
            100000, /* iterations per test run */
            20 /* number of test runs */
        )
    );

    // Result: Clocks in at 1.2ms (12ns per CAS operation) in my environment

## Why?

Yeah, I'm sure there's lots of other libraries that do the same thing, and a good portion of programmers
have no doubt cobbled together their own microbenchmark shims over the years. But I wanted something
reusable with minimal dependencies, and something that was **easy to use** so that I wouldn't have to waste
time wondering how long something might take -- I could *just check* (without having to worry about
icky platform-specific high-resolution timer routines and bothersome boilerplate).

## License

Released under the [simplified BSD license](https://github.com/cameron314/microbench/blob/master/LICENSE.md).

