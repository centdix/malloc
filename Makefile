# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g
NAME = libft_malloc_$(HOSTTYPE).so
LINK_NAME = libft_malloc.so
PATH_SRC = src
PATH_INC = inc
PATH_OBJ = obj
PATH_TEST = tests
TEST_BIN = test_mymalloc

SOURCES = $(PATH_SRC)/core.c $(PATH_SRC)/logger.c $(PATH_SRC)/utils.c
OBJECTS = $(SOURCES:$(PATH_SRC)/%.c=$(PATH_OBJ)/%.o)

# Ensure HOSTTYPE is set
ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

# Rules
.PHONY: all clean fclean re test run_test

# Default rule: build the shared library
all: $(NAME) $(LINK_NAME)

# Create shared library
$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^ -I$(PATH_INC)
	@echo "Shared library $(NAME) created!"

# Create symbolic link
$(LINK_NAME): $(NAME)
	ln -sf $(NAME) $(LINK_NAME)
	@echo "Symbolic link $(LINK_NAME) -> $(NAME) created!"

# Compile object files
$(PATH_OBJ)/%.o: $(PATH_SRC)/%.c
	@mkdir -p $(PATH_OBJ)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@ -I$(PATH_INC)

# Clean object files
clean:
	@rm -rf $(PATH_OBJ)
	@echo "Object files cleaned!"

# Full clean: remove objects, shared library, and symbolic link
fclean: clean
	@rm -f $(NAME) $(LINK_NAME) $(TEST_BIN)
	@echo "All built files removed!"

# Rebuild everything
re: fclean all

# Compile the test binary
test: $(NAME)
	$(CC) $(CFLAGS) -o $(TEST_BIN) $(PATH_TEST)/thread.c -I$(PATH_INC) -L. -lft_malloc
	@echo "Test binary $(TEST_BIN) created!"

# Run the test binary
run_test: test
	LD_LIBRARY_PATH=.:$(LD_LIBRARY_PATH) LD_PRELOAD=./libft_malloc.so ./$(TEST_BIN)