# AI Assistance Log

Maintain this file throughout the project. Include the exact prompts and relevant outputs required by the course policy. This initial entry records the project-planning and implementation assistance from 2026-11-15.

## Entry 1 — Project selection and initial implementation

**Tool:** ChatGPT (OpenAI), GPT-6

**User request (exact):**
> Yeah. that sounds like a great plan. Let's proceed with and start the project and start implementing

**Work performed:**
- Proposed a water-distribution maximum-flow model using a super-source and super-sink.
- Proposed one-dimensional cloud workload placement as Bin Packing.
- Selected Dinic's maximum-flow algorithm and First Fit Decreasing with a segment tree.
- Generated an initial C++17 implementation, example inputs, unit tests, and benchmark harness.

**Verification status:** The source files have been generated. Compilation and tests must be run and recorded before treating the implementation as verified. The benchmark outputs must be generated on the student's machine before any performance claims are included in the report.

**Important:** Add the exact full assistant outputs and any subsequent prompts/results required by the instructor's AI-use policy. Do not treat this brief entry as a substitute if the instructor requires complete transcripts.

### Entry 2 — Initial topic recommendation (2026-11-15)

**Exact user prompt:**
> Yeah. that sounds like a great plan. Let's proceed with and start the project and start implementing

**Relevant assistant output:**
> We will now move from planning to implementation. I recommend the following two problems because both have clear mathematical formulations, defensible proofs, practical applications, and manageable C++ implementations.
>
> Problem 1 — Water distribution using Network Flow. Model a water distribution system connecting treatment plants, pipes, and demand zones. Determine the maximum quantity of water that can be delivered without exceeding plant supplies, pipe capacities, or zone demands. Mathematical model: directed capacitated flow network. Algorithm: Dinic's maximum-flow algorithm. Reduction: add a super-source connected to treatment plants and a super-sink connected to demand zones. General running time: O(V^2 E) for Dinic's algorithm.
>
> Problem 2 — Cloud workload placement using Bin Packing. Model placement of computing workloads onto servers with fixed capacity. Minimize the number of servers while respecting capacity. Mathematical model: one-dimensional Bin Packing. NP-hardness: reduction from strongly NP-complete 3-Partition. Greedy algorithm: First Fit Decreasing (FFD). Implementation: C++ with a segment tree to find the first server with sufficient remaining capacity in O(log n) per workload. Running time: O(n log n).
>
> The water model is a simplified capacitated-flow abstraction, not complete hydraulic simulation. The bin-packing model isolates one resource dimension and does not model all Kubernetes scheduling constraints.

**Implementation generated:**
- `include/dinic.hpp`: Dinic maximum-flow implementation.
- `include/water_distribution.hpp`: super-source/super-sink reduction and water allocation interpretation.
- `include/bin_packing.hpp`: First Fit Decreasing using a max segment tree.
- `src/main.cpp`: command-line entry points for `water` and `binpack`.
- `tests/test_algorithms.cpp`: deterministic tests plus randomized comparisons against independent reference implementations.
- `src/benchmark.cpp`: benchmark harness for both algorithms.
- `docs/initial_design.md`: mathematical formulations, reduction proof outline, correctness outline, and complexity analysis.

**Verification and observed results:**
- C++17 Release build completed successfully using CMake and GNU C++ 14.2.0 in the execution container.
- All unit tests passed, including 300 randomized max-flow comparisons against Edmonds–Karp and 500 randomized FFD comparisons against a scan-based FFD reference.
- Water example delivered 8 units, matching the expected bottleneck/demand cap.
- Bin-packing example produced a feasible 3-bin packing for capacity 10.
- Benchmark CSVs and preliminary plots were generated. These measurements are from the execution container, not the student's computer, and are not final report evidence. Dinic timing showed variability and must be re-measured under a controlled environment before drawing conclusions.

**External sources consulted:**
- U.S. EPA, Drinking Water Distribution System Tools and Resources: https://www.epa.gov/dwreginfo/drinking-water-distribution-system-tools-and-resources
- U.S. EPA, Drinking Water Distribution Systems: https://www.epa.gov/dwsixyearreview/drinking-water-distribution-systems
- Kubernetes, Resource Management for Pods and Containers: https://kubernetes.io/docs/concepts/configuration/manage-resources-containers/
- Garey and Johnson, 3-Partition definition and strong NP-completeness discussion: https://perso.limos.fr/~palafour/PAPERS/PDF/Garey-Johnson79.pdf
- MIT course notes, 3-Partition: https://hardness.mit.edu/drafts/2022-09-21.pdf

**Review status:** Initial implementation is a prototype, not the final submission. Proofs, input model, and benchmark methodology still require detailed review. The student must understand and independently review the code and arguments.

## Prompt ledger

The verbatim user-prompt ledger is maintained separately in [`PROMPTS_LOG.md`](PROMPTS_LOG.md). Add every new project-related user prompt there in chronological order. Historical prompts whose exact wording is unavailable are explicitly marked rather than reconstructed.
