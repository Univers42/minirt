# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/01/03 23:03:35 by dlesieur          #+#    #+#              #
#    Updated: 2026/03/07 22:08:22 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := librt.a
CC := cc
CFLAGS := -std=c99 -I.
OPTFLAGS := -O3 -march=native -fno-math-errno -flto -DNDEBUG -pipe -fopenmp
CFLAGS += $(OPTFLAGS)
AR := gcc-ar
ARFLAGS := rcs

# Build directory structure
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
SRCDIR := .
TESTDIR := tests

# External dependency
PNG_WRITER_LIB := ../png_writer/libpnglode.a
WF_DIR := ../wavefront
WF_LIB := $(WF_DIR)/build/libwavefront.a
WF_RT_OBJ := $(WF_DIR)/build/obj/wf_rt.o
LDFLAGS := -lm -flto -fopenmp

# Source and object files
LIB_SRCS := $(filter-out $(SRCDIR)/main.c,$(wildcard $(SRCDIR)/*.c))
LIB_OBJS := $(addprefix $(OBJ_DIR)/,$(notdir $(LIB_SRCS:.c=.o)))
LIB_PATH := $(BUILD_DIR)/$(NAME)

# Test sources and binaries
TEST_SRCS := $(wildcard $(TESTDIR)/*.c)
TEST_BINS := $(addprefix $(BIN_DIR)/,$(notdir $(TEST_SRCS:.c=)))

# Default target
all: $(LIB_PATH)

# Create build directories
$(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Check and build PNG writer dependency if needed
$(PNG_WRITER_LIB):
	@echo "Building PNG writer library..."
	$(MAKE) -C ../png_writer all

# Compile library object files
$(OBJ_DIR)/%.o: $(SRCDIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build library archive
$(LIB_PATH): $(PNG_WRITER_LIB) $(LIB_OBJS) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@echo "Library built: $@"

# Compile test binaries: pattern rule for build/bin/%
$(BIN_DIR)/%: $(TESTDIR)/%.c $(LIB_PATH) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LIB_PATH) $(PNG_WRITER_LIB) $(LDFLAGS)

# Check and build wavefront library if needed
$(WF_LIB):
	@echo "Building wavefront library..."
	$(MAKE) -C $(WF_DIR) all

$(WF_RT_OBJ): $(WF_LIB)
	$(MAKE) -C $(WF_DIR) rt_obj CFLAGS="$(CFLAGS) -I."

# Special rule for scene_obj (needs wavefront lib + rt integration)
$(BIN_DIR)/scene_obj: $(TESTDIR)/scene_obj.c $(LIB_PATH) $(WF_LIB) $(WF_RT_OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -I$(WF_DIR) -o $@ $< $(WF_RT_OBJ) \
		$(LIB_PATH) $(WF_LIB) $(PNG_WRITER_LIB) $(LDFLAGS)

# Build all test programs
test: $(TEST_BINS)
	@echo "All tests compiled into $(BIN_DIR)/"
	@ls -1 $(BIN_DIR)/

# Clean build artifacts (keep binaries)
clean:
	@rm -rf $(OBJ_DIR)
	@echo "Cleaned object files"

# Full clean (remove entire build directory)
fclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

# Rebuild library
re: fclean all

# Phony targets
.PHONY: all clean fclean re test