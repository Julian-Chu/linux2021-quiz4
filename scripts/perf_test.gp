reset
set xlabel 'count'
set ylabel 'time (ns)'
set title 'PI Calc'
set term png enhanced font 'Verdana,10'
set output 'perf_test.png'
set grid
plot [0:1000][0:600000] \
'perf_test_data' using 1:2 with linespoints linewidth 2 title "PI calc",\
