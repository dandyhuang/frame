// Copyright (c) 2013, The Toft Authors. All rights reserved.
// Author: Ye Shunping <yeshunping@gmail.com>

#include <algorithm>

#include "thirdparty/monitor_sdk_common/base/benchmark.h"

#include "thirdparty/gflags/gflags.h"

extern int common::nbenchmarks;
extern common::Benchmark* common::benchmarks[];

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, false);
    for (int i = 0; i < common::nbenchmarks; i++) {
        common::Benchmark* b = common::benchmarks[i];
        for (int j = b->threadlo; j <= b->threadhi; j++)
            for (int k = std::max(b->lo, 1); k <= std::max(b->hi, 1); k <<= 1)
                RunBench(b, j, k);
    }
}
