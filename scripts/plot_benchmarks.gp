set datafile separator ","
set terminal pngcairo size 1200,800 enhanced
set grid
set key left top

# Maximum Flow
set output "flow_runtime.png"
set title "Dinic Maximum Flow Runtime"
set xlabel "Vertices (V)"
set ylabel "Median Runtime (ms)"
plot "flow_benchmark.csv" using 1:3 with linespoints title "Dinic"

# Bin Packing
set output "binpack_runtime.png"
set title "First Fit Decreasing Runtime"
set xlabel "Number of items (n)"
set ylabel "Median Runtime (ms)"
plot "binpack_benchmark.csv" using 1:2 with linespoints title "FFD"

# Bin Packing vs n log n
set output "binpack_runtime_nlogn.png"
set title "First Fit Decreasing vs n log_2(n)"
set xlabel "n log_2(n)"
set ylabel "Median Runtime (ms)"
plot "binpack_benchmark.csv" using 3:2 with linespoints title "FFD"