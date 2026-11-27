# COT5405 Project — Initial C++ Implementation

Two independent application models:

1. **Water distribution:** maximum flow in a directed capacitated network.
2. **Cloud workload placement:** one-dimensional bin packing, solved with First Fit Decreasing (FFD).

## Requirements

- C++17 or later
- CMake 3.16+

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Run tests

```bash
ctest --test-dir build --output-on-failure
```

## Water distribution input

Run:

```bash
./build/cot5405 water < examples/water_small.txt
```

Input format (all node IDs are zero-based):

```text
N P D M
plant_node_1 supply_1
...
demand_node_1 demand_1
...
pipe_from_1 pipe_to_1 capacity_1
...
```

- `N`: number of physical network nodes
- `P`: number of treatment plants / supply nodes
- `D`: number of demand zones
- `M`: number of directed pipes
- Each plant is connected to a super-source.
- Each demand zone is connected to a super-sink.

The program reports maximum delivered flow and the amount delivered to each demand zone. The reported amount is the maximum-total-delivery allocation; it does not guarantee a prescribed minimum for every zone unless that is added as a separate constraint.

## Bin packing input

Run:

```bash
./build/cot5405 binpack < examples/binpack_small.txt
```

Input:

```text
number_of_items bin_capacity
item_size_1 item_size_2 ...
```

The implementation uses First Fit Decreasing with a segment tree to find the first existing bin with enough remaining capacity. It returns a feasible packing; it does not claim to return an optimal packing.

## Benchmarks

```bash
./build/benchmark
```

This writes:
- `flow_benchmark.csv`
- `binpack_benchmark.csv`

Benchmark results are machine-dependent. Do not report them until the program has actually been run on the target machine. The CSVs record measured times and input characteristics.

## Important modeling limitations

- The water model is a capacitated flow abstraction. It does not model hydraulic pressure, head loss, water quality, pump curves, or nonlinear fluid dynamics.
- The bin-packing model considers one resource dimension (for example, memory reservation). Real cluster schedulers commonly consider multiple resources and additional constraints.
