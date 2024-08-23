# Cache Effects
Benchmarks illustrating various performance implications of cache effects. 

# Benchmark Results
The following benchmarks are run on a Apple M1 Max (10 cores, 8 performance and 2 efficiency, clocked at 24 MHz) on MacOS 14 Sonoma with the following specs:
```
$ sysctl -a | grep -E 'hw\.(cachelinesize|l1icachesize|l1dcachesize|l2cachesize)'
hw.cachelinesize: 128
hw.l1icachesize: 131072
hw.l1dcachesize: 65536
hw.l2cachesize: 4194304
```

## False Sharing
Illustrates performance effect of `N` variables occupying the same cacheline which are modified by different threads. 

The below shows the Google Benchmark output from the `false_sharing` benchmark where the columns are `Time` (average wall clock time over the `Iterations` tested) and `CPU` (the `CPU` time of the main thread). Note that the `CPU` time only measures time spent by the main thread (see [Google Benchmark Docs](https://github.com/google/benchmark/blob/main/docs/user_guide.md#cpu-timers)), not the `N` spawned threads, so the wallclock time is analyzed in this case. 

The tests are segmented by the atomic type (unaligned or aligned to a multiple of the cache line size), the number of threads (up to 5) and the number of `fetch_add` operations for each thread to execute on its atomic variable. 

For the `AlignedAtomic`, the results show approximately linear growth in the wall clock time with respect to the number of threads due to the larger number of total atomic operations done across all threads with thread context switching contributing a smaller proportion. In contrast, the `UnalignedAtomic` shows an order of magnitude higher wall clock latency due to the false sharing of the single cache line containing the atomic variables.

![alt text](assets/false_sharing.png)

# Running
To run the repository, clone the repo locally and then from root run

```
cmake .
bin/[bench_name]
```

Where `bench_name` is any of:
```
false_sharing_bench
```
