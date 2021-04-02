run:
	gcc  threadpool.c main.c -lpthread -lm -o main

test:
	gcc  threadpool.c perf_test.c -lpthread -lm -o perf_test


test-plot: test
	./perf_test > perf_test_data
	gnuplot ./scripts/perf_test.gp
	eog perf_test.png

clean:
	rm main perf_test perf_test.png perf_test_data 2>/dev/null

