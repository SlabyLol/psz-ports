CC = gcc
CFLAGS = -Wall -O2 -I../include

all: run_tests

run_tests: test_psz.o ../src/psz_core.o
	$(CC) $(CFLAGS) $^ -o $@
	./run_tests

test_psz.o: test_psz.c
	$(CC) $(CFLAGS) -c $< -o $@

../src/psz_core.o: ../src/psz_core.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o run_tests /tmp/test_input.txt /tmp/test_output.psz /tmp/extracted_payload.bin
