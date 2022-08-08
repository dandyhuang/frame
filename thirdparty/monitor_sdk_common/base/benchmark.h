// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>
//  Notes: Idea got from benchmark framework of re2

#ifndef COMMON_BASE_BENCHMARK_H_
#define COMMON_BASE_BENCHMARK_H_

#include <stdint.h>

namespace common {
struct Benchmark {
    const char* name;
    void (*fn)(int a);
    void (*fnr)(int a, int b);
    int lo;
    int hi;
    int threadlo;
    int threadhi;

    void Register();
    Benchmark(const char* name, void (*f)(int)) {  // NOLINT
        Clear(name);
        fn = f;
        Register();
    }
    Benchmark(const char* name, void (*f)(int, int), int l, int h) {  // NOLINT
        Clear(name);
        fnr = f;
        lo = l;
        hi = h;
        Register();
    }
    void Clear(const char* n) {
        name = n;
        fn = 0;
        fnr = 0;
        lo = 0;
        hi = 0;
        threadlo = 0;
        threadhi = 0;
    }
    Benchmark* ThreadRange(int lo, int hi) {
        threadlo = lo;
        threadhi = hi;
        return this;
    }
};


void SetBenchmarkBytesProcessed(int64_t bytes_processed);
void StopBenchmarkTiming();
void StartBenchmarkTiming();
void BenchmarkMemoryUsage();
void SetBenchmarkItemsProcessed(int n);

void RunBench(common::Benchmark* b, int nthread, int siz);

extern int nbenchmarks;
extern common::Benchmark* benchmarks[10000];
}  // namespace common

//  It's implemented in file: thirdparty/gperftools-2.0/src/base/sysinfo.cc
extern int NumCPUs();

#define COMMON_BENCHMARK(f) \
    ::common::Benchmark* _benchmark_##f = (new ::common::Benchmark(#f, f))

#define COMMON_BENCHMARK_RANGE(f, lo, hi) \
    ::common::Benchmark* _benchmark_##f = \
    (new ::common::Benchmark(#f, f, lo, hi))

#endif  // COMMON_BASE_BENCHMARK_H_
