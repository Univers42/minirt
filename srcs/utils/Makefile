# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:   #
#                                                     +:+ +:+         +:+      #
#    By: marvin <marvin@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/12/28 15:14:49 by marvin            #+#    #+#              #
#    Updated: 2025/12/28 15:14:49 by marvin           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libutils.a
BUILD_DIR = ../../build
OBJ_DIR = $(BUILD_DIR)/obj
DEP_DIR = $(BUILD_DIR)/dpdc
STATIC_LIB_DIR = $(BUILD_DIR)/static_libs
BIN_DIR = $(BUILD_DIR)/bin

CC = cc
CFLAGS = -Wall -Wextra -Werror -I. -I../../includes -MMD -MP

SRC = vec3.c point3.c normal.c matrix.c transform.c
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:.c=.o))
DEP = $(addprefix $(DEP_DIR)/, $(SRC:.c=.d))

TESTS_SRC := $(wildcard tests/*.c)
TESTS_OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(TESTS_SRC))
TESTS_DEP := $(patsubst %.c,$(DEP_DIR)/%.d,$(TESTS_SRC))
TESTS_BIN := $(patsubst tests/%.c,$(BIN_DIR)/%,$(TESTS_SRC))

all: $(STATIC_LIB_DIR)/$(NAME)

$(STATIC_LIB_DIR)/$(NAME): $(OBJ)
	@mkdir -p $(STATIC_LIB_DIR)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@) $(DEP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@if [ -f "$(basename $@).d" ]; then mv "$(basename $@).d" "$(DEP_DIR)/$(notdir $(basename $@)).d"; fi

$(OBJ_DIR)/tests/%.o: tests/%.c
	@mkdir -p $(dir $@) $(DEP_DIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@if [ -f "$(basename $@).d" ]; then mv "$(basename $@).d" "$(DEP_DIR)/$(notdir $(basename $@)).d"; fi

$(BIN_DIR)/%: $(OBJ_DIR)/tests/%.o $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lm

# Include dependency files from dpdc/
-include $(DEP)
-include $(TESTS_DEP)

clean:
	rm -rf $(OBJ_DIR) $(DEP_DIR)

fclean: clean
	rm -rf $(BUILD_DIR)

re: fclean all

# Build all test programs in tests/
tests: $(TESTS_BIN)

# Interactive test build: asks for a path to a test .c file and builds it to bin/
test:
	@read -p "Enter path to test source (e.g. tests/foo.c): " TESTSRC; \
	OBJ_FILE="$(OBJ_DIR)/$${TESTSRC%.c}.o"; \
	DEP_FILE="$(DEP_DIR)/$${TESTSRC%.c}.d"; \
	NAME=$$(basename $$TESTSRC .c); \
	OUT="$(BIN_DIR)/$$NAME"; \
	mkdir -p $$(dirname $$OBJ_FILE) $(BIN_DIR) $(DEP_DIR); \
	$(CC) $(CFLAGS) -c $$TESTSRC -o $$OBJ_FILE; \
	if [ -f "$$(dirname $$OBJ_FILE)/$$NAME.d" ]; then mv "$$(dirname $$OBJ_FILE)/$$NAME.d" $(DEP_DIR)/; fi; \
	$(CC) $(CFLAGS) $$OBJ_FILE $(OBJ) -o $$OUT -lm && echo "Built $$OUT"

.PHONY: all clean fclean re tests test

