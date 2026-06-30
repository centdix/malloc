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
      $(SRC_DIR)/calloc.c \
      $(SRC_DIR)/utils.c

# Object files
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Library name. The subject requires the real library to be
# libft_malloc_$HOSTTYPE.so with a libft_malloc.so symlink pointing to it.
ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif
NAME = libft_malloc_$(HOSTTYPE).so
SYMLINK = libft_malloc.so

# Header files
INCLUDES = -I$(INC_DIR) -I$(LIBFT_DIR)

# Colors for pretty printing
GREEN = \033[0;32m
NC = \033[0m

# Libft
LIBFT = $(LIBFT_DIR)/libft.a

all: $(NAME)

$(NAME): $(OBJ)
	@echo "Creating dynamic library..."
	@$(MAKE) -C $(LIBFT_DIR)
	@$(CC) $(LDFLAGS) $(OBJ) $(LIBFT_DIR)/libft.a -lpthread -o $(NAME)
	@ln -sf $(NAME) $(SYMLINK)
	@echo "${GREEN}Library $(NAME) created successfully!${NC}"

$(LIBFT):
	@echo "Compiling libft..."
	@$(MAKE) -C $(LIBFT_DIR)

# Create object directory
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean object files
clean:
	@echo "Cleaning object files..."
	@rm -rf $(OBJ_DIR)
	@$(MAKE) -C $(LIBFT_DIR) clean
	@$(MAKE) -C tests clean

# Clean everything
fclean: clean
	@echo "Cleaning library files..."
	@rm -f $(NAME) $(SYMLINK)
	@$(MAKE) -C $(LIBFT_DIR) fclean

# Rebuild everything
re: fclean all

# Build and run the full test suite (subject + unit + integration).
test: $(NAME)
	@$(MAKE) -C tests

.PHONY: all clean fclean re test