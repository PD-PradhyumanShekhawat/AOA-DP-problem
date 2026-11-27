# Initial Mathematical Design

## Problem 1 — Water distribution as maximum flow

### Application model
A simplified water distribution network contains physical junctions, treatment plants, demand zones, and directed pipes. A plant has a finite available supply; each pipe has a maximum throughput; each demand zone has a maximum requested amount.

This is a planning abstraction, not a hydraulic simulator. It omits pressure, head loss, pump curves, water quality, time dependence, and nonlinear fluid mechanics.

### Input
- A directed physical network `G=(V,E)`.
- Pipe capacity `c(u,v) >= 0` for every directed pipe.
- Supply amount `s(p) >= 0` for each treatment plant `p`.
- Demand cap `d(z) >= 0` for each demand zone `z`.

### Construction of the flow network
Create a super-source `S` and super-sink `T`.
- For every treatment plant `p`, add `(S,p)` with capacity `s(p)`.
- For every physical pipe `(u,v)`, add `(u,v)` with capacity `c(u,v)`.
- For every demand zone `z`, add `(z,T)` with capacity `d(z)`.

Run a maximum-flow algorithm from `S` to `T`. The flow on `(z,T)` is the amount delivered to zone `z`; the maximum-flow value is the maximum total delivered amount.

### Correctness proof outline
**Forward direction.** Given a feasible water allocation, assign the same quantity to each corresponding pipe edge. Add the plant supply on the super-source edge and the zone delivery on the super-sink edge. Pipe capacities, plant supply limits, demand caps, and conservation at intermediate junctions imply a feasible `S-T` flow with the same total delivered amount.

**Reverse direction.** Given a feasible `S-T` flow, interpret the flow on physical pipe edges as water transported through those pipes. The capacities of the super-source and super-sink edges enforce plant supply and zone demand limits. Flow conservation enforces conservation at intermediate junctions. Thus the flow induces a feasible water allocation with the same total delivery.

The two directions preserve the objective value, so a maximum flow gives a maximum-total-delivery allocation.

### Complexity
For Dinic's algorithm on a general directed capacitated graph, the standard worst-case bound is `O(V^2 E)`. The constructed graph adds two vertices and `P+D` edges, so its size is polynomial in the original input size. The construction itself is `O(V+E+P+D)`.

## Problem 2 — Cloud workload placement as one-dimensional bin packing

### Application model
A cluster has identical servers, each with capacity `B` for one selected resource dimension (for example, reserved memory). Workloads have positive resource requests `a_i`. Each workload must be assigned to exactly one server, and the sum assigned to any server cannot exceed `B`. The objective is to minimize the number of servers used.

This deliberately isolates one resource dimension. Real orchestrators may also account for CPU, memory, accelerators, affinity, topology, and other constraints.

### Decision version
Given item sizes `a_1,...,a_n`, bin capacity `B`, and an integer `k`, is there a packing using at most `k` bins?

A certificate is a bin assignment for every item. In polynomial time, verify that every item is assigned exactly once and each bin's total size is at most `B`; therefore the decision problem is in NP.

### NP-hardness reduction from 3-Partition
A 3-Partition instance consists of `3m` positive integers `a_1,...,a_{3m}` and a bound `B` such that:

- `B/4 < a_i < B/2` for every item;
- `sum_i a_i = mB`.

Ask whether the items can be partitioned into `m` triples, each summing to `B`.

Construct a bin-packing instance with the same item sizes, bin capacity `B`, and a limit of `k=m` bins. This construction is polynomial.

If the 3-Partition instance is a YES-instance, put each triple into one bin, obtaining a packing in `m` bins.

Conversely, suppose the bin-packing instance fits in at most `m` bins. Since the total item size is `mB`, at least `m` bins are necessary, so exactly `m` bins are used and every bin is full. Since each item is strictly larger than `B/4`, a full bin cannot contain four or more items. Since each item is strictly smaller than `B/2`, a full bin cannot contain one or two items. Hence every full bin contains exactly three items, and those triples each sum to `B`. This is a valid 3-partition.

Thus 3-Partition reduces to the decision version of Bin Packing. Together with membership in NP, the decision problem is NP-complete; the optimization version is NP-hard. Because 3-Partition is strongly NP-complete, this also establishes strong NP-hardness.

### Greedy heuristic: First Fit Decreasing
Sort workloads in non-increasing order. Process them in that order and place each workload in the lowest-indexed existing server with sufficient remaining capacity. If none fits, open a new server. A segment tree storing the maximum remaining capacity in each subtree finds the first fitting server in `O(log n)` time and supports updates in `O(log n)` time.

Sorting costs `O(n log n)`; there are `n` segment-tree queries and updates, each `O(log n)`. Total time is `O(n log n)` and space is `O(n)`. The heuristic always returns a feasible packing for valid input, but it does not guarantee an optimal number of servers.

## Experimental plan
- Build in C++17 Release mode.
- Validate Dinic against an independent Edmonds–Karp reference on small random directed graphs.
- Validate segment-tree FFD against a simple scan-based FFD reference on random instances.
- Measure median runtime over repeated runs for increasing graph sizes and item counts.
- Record compiler, optimization flags, CPU, OS, input generation seed, graph vertex/edge counts, and item distributions.
- Plot observed time against input size and the theoretical growth proxies `V^2 E` and `n log n`.
- Do not treat container benchmark numbers as the student's final hardware measurements.
