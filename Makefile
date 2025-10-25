# Compiler and flags
CC = gcc
# CFLAGS = -Wall -Wextra -Werror -fPIC -O2 -g # Uncomment for debugging
CFLAGS = -Wall -Wextra -Werror -fPIC
LDFLAGS = -shared

# Directories
SRC_DIR = src
INC_DIR = includes
OBJ_DIR = obj
LIBFT_DIR = libft

# Source files
SRC = $(SRC_DIR)/malloc.c \
      $(SRC_DIR)/free.c \
      $(SRC_DIR)/realloc.c \
      $(SRC_DIR)/utils.c

# Test source files
TEST_SRCS = tests/main.c \
            tests/test.c \
            tests/thread.c

# Object files
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJ = $(TEST_SRCS:%.c=$(OBJ_DIR)/%.o)

# Library name
NAME = libft_malloc.so
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif
SYMLINK = libft_malloc_$(HOSTTYPE).so

# Header files
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

# Colors for pretty printing
GREEN = \033[0;32m
NC = \033[0m

# Test executable name
TEST_NAME = test_malloc

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Creating dynamic library..."
	@$(MAKE) -C $(LIBFT_DIR)
	@$(CC) $(LDFLAGS) $(OBJ) $(LIBFT_DIR)/libft.a -o $(NAME)
	@ln -sf $(NAME) $(SYMLINK)
	@echo "${GREEN}Library $(NAME) created successfully!${NC}"

$(LIBFT):
	@echo "Compiling libft..."
	@$(MAKE) -C $(LIBFT_DIR)

# Create object directory
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)/tests

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile test files
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDES) -I./tests -c $< -o $@

# Clean object files
clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean

# Clean everything
fclean: clean
	@echo "Cleaning library files..."
	@rm -f $(NAME) $(SYMLINK) $(TEST_NAME)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# Rebuild everything
re: fclean all

# Test the malloc implementation
test: $(NAME) $(TEST_OBJ)
	@echo "Compiling and running tests..."
	@$(CC) $(CFLAGS) $(TEST_OBJ) -o $(TEST_NAME) -L. -lft_malloc -L$(LIBFT_DIR) -lft -lpthread
	@echo "Running tests..."
	@LD_LIBRARY_PATH=. ./$(TEST_NAME)

.PHONY: all clean fclean re test 