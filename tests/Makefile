# Test-suite driver. Three layers, all run by `make` (or `make -C tests`):
#   subject     - the official example programs from the subject
#   unit        - fast assert-based checks
#   integration - the comprehensive suite (edge cases, threads, perf, GNL)
#
# Every program is linked against the allocator built in the repo root.
# -Wl,--no-as-needed keeps libft_malloc.so in the link even when a program only
# references malloc/free (otherwise the linker can drop it and the test would
# silently run against glibc). LD_LIBRARY_PATH lets the loader find it at run.

CC      = gcc
ROOT    = ..
CFLAGS  = -Wall -Wextra -Werror -I$(ROOT)/includes -g3 -O0
LDFLAGS = -L$(ROOT) -Wl,--no-as-needed -lft_malloc -lpthread
RUN     = env LD_LIBRARY_PATH=$(ROOT)

SUBJ    = subject
UNIT    = unit
INTEG   = integration
PRINTF  = $(INTEG)/printf.c

SUBJ_BINS  = subj_test1 subj_test2 subj_test3 subj_test4 subj_test5
UNIT_BIN   = unit_tests
INTEG_BINS = test_basic test_edge_cases test_advanced test_absurd \
             test_performance test_thread test_gnl

all: subject unit integration

# Build the allocator first so every layer links against a fresh library.
lib:
	@$(MAKE) -C $(ROOT)

# ----------------------------- subject -----------------------------
subject: lib $(SUBJ_BINS)
	@echo "\n=== Subject example tests ==="
	@$(RUN) ./subj_test1
	@$(RUN) ./subj_test2
	@$(RUN) ./subj_test3
	@$(RUN) ./subj_test4
	@$(RUN) ./subj_test5
	@echo "=== Subject tests OK ==="

subj_test%: $(SUBJ)/test%.c
	@$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# ------------------------------ unit -------------------------------
unit: lib $(UNIT_BIN)
	@echo "\n=== Unit tests ==="
	@$(RUN) ./$(UNIT_BIN)

$(UNIT_BIN): $(UNIT)/main.c $(UNIT)/test.c $(UNIT)/thread.c
	@$(CC) $(CFLAGS) -I$(UNIT) $^ -o $@ $(LDFLAGS)

# -------------------------- integration ----------------------------
integration: lib $(INTEG_BINS)
	@echo "\n=== Integration tests ==="
	@$(RUN) ./test_basic
	@$(RUN) ./test_edge_cases
	@$(RUN) ./test_advanced
	@$(RUN) ./test_absurd
	@$(RUN) ./test_performance
	@$(RUN) ./test_thread
	@$(RUN) ./test_gnl $(INTEG)/gnl/
	@echo "=== Integration tests OK ==="

test_basic: $(INTEG)/basic.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_edge_cases: $(INTEG)/edge_cases.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_advanced: $(INTEG)/advanced.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_absurd: $(INTEG)/absurd.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_performance: $(INTEG)/performance.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_thread: $(INTEG)/thread.c $(PRINTF)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_gnl: $(INTEG)/gnl/gnl.c $(INTEG)/gnl/main.c $(PRINTF)
	@$(CC) $(CFLAGS) -I$(INTEG)/gnl $^ -o $@ $(LDFLAGS)

clean:
	@rm -f $(SUBJ_BINS) $(UNIT_BIN) $(INTEG_BINS)

fclean: clean

re: fclean all

.PHONY: all lib subject unit integration clean fclean re
