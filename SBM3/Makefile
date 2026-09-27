CC = cc
CFLAGS = -std=c11 -O3 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wvla
SANFLAGS = -std=c11 -O1 -g -fsanitize=address,undefined -fno-sanitize-recover=all \
           -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Wvla

.PHONY: all check clean sanitize check-sanitize
all: sbm3 sbm3_test

sbm3: sbm3.c sbm3_cli.c sbm3.h
	$(CC) $(CFLAGS) sbm3.c sbm3_cli.c -o sbm3

sbm3_test: sbm3_test.c sbm3.c sbm3.h
	$(CC) $(CFLAGS) -DSBM3_TEST_HOOKS sbm3.c sbm3_test.c -o sbm3_test

check: all
	./sbm3_test
	python3 verify.py ./sbm3

sanitize: sbm3_sanitize sbm3_test_sanitize

sbm3_sanitize: sbm3.c sbm3_cli.c sbm3.h
	$(CC) $(SANFLAGS) sbm3.c sbm3_cli.c -o sbm3_sanitize

sbm3_test_sanitize: sbm3_test.c sbm3.c sbm3.h
	$(CC) $(SANFLAGS) -DSBM3_TEST_HOOKS sbm3.c sbm3_test.c -o sbm3_test_sanitize

check-sanitize: sanitize
	./sbm3_test_sanitize
	python3 verify.py ./sbm3_sanitize

clean:
	$(RM) sbm3 sbm3_test sbm3_sanitize sbm3_test_sanitize *.o
