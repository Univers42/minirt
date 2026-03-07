# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/07 22:00:00 by dlesieur          #+#    #+#              #
#    Updated: 2026/03/08 00:41:14 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := libwavefront.a
CC := cc
CFLAGS := -std=c99 -Wall -Wextra -Werror -I. -O2 -pipe
AR := gcc-ar
ARFLAGS := rcs

# Build dirs
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin

# Library sources (all wf_*.c except wf_viewer.c and wf_rt.c)
LIB_SRCS := wf_str.c wf_model.c wf_model2.c wf_model3.c \
            wf_face.c wf_parse.c wf_parse2.c \
            wf_mesh.c wf_mesh2.c wf_mesh3.c wf_mesh4.c
LIB_OBJS := $(addprefix $(OBJ_DIR)/,$(LIB_SRCS:.c=.o))

# Ray tracer integration (needs monte_carlo headers)
RT_DIR := ../monte_carlo
RT_LIB := $(RT_DIR)/build/librt.a
PNG_LIB := ../png_writer/libpnglode.a

# SDL2 flags for the viewer
SDL2_CFLAGS := $(shell sdl2-config --cflags 2>/dev/null)
SDL2_LDFLAGS := $(shell sdl2-config --libs 2>/dev/null) -lGL

# Ray tracer flags (match monte_carlo build – no -Werror)
RT_CFLAGS := -std=c99 -I. -I$(RT_DIR) -O3 -march=native \
             -fno-math-errno -flto -DNDEBUG -pipe -fopenmp
RT_LDFLAGS := -lm -flto -fopenmp

# Default: build the library
all: $(BUILD_DIR)/$(NAME)

$(BUILD_DIR) $(OBJ_DIR) $(BIN_DIR):
	@mkdir -p $@

# Library object compilation
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build the library archive
$(BUILD_DIR)/$(NAME): $(LIB_OBJS) | $(BUILD_DIR)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)
	@echo "Library built: $@"

# wf_rt.c needs both wavefront + monte_carlo headers (use RT_CFLAGS)
$(OBJ_DIR)/wf_rt.o: wf_rt.c | $(OBJ_DIR)
	$(CC) $(RT_CFLAGS) -c $< -o $@

# Build ray tracer integration object
rt_obj: $(OBJ_DIR)/wf_rt.o
	@echo "RT integration object built"

# OpenGL viewer (links wavefront lib + SDL2 + OpenGL)
viewer: $(BUILD_DIR)/$(NAME) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -o $(BIN_DIR)/wf_viewer \
		wf_viewer.c $(BUILD_DIR)/$(NAME) $(SDL2_LDFLAGS) -lm
	@echo "Viewer built: $(BIN_DIR)/wf_viewer"

# Cornell box SDL2 test (ray tracer + wavefront + SDL2/OpenGL)
cornell_sdl: $(BUILD_DIR)/$(NAME) $(OBJ_DIR)/wf_rt.o | $(BIN_DIR)
	$(CC) $(RT_CFLAGS) $(SDL2_CFLAGS) -o $(BIN_DIR)/test_cornell_sdl \
		tests/test_cornell_sdl.c $(OBJ_DIR)/wf_rt.o \
		$(RT_LIB) $(BUILD_DIR)/$(NAME) $(PNG_LIB) \
		$(SDL2_LDFLAGS) $(RT_LDFLAGS)
	@echo "Cornell SDL test built: $(BIN_DIR)/test_cornell_sdl"

# Spotlight scene SDL2 test (colored lights on OBJ model)
spotlight_sdl: $(BUILD_DIR)/$(NAME) $(OBJ_DIR)/wf_rt.o | $(BIN_DIR)
	$(CC) $(RT_CFLAGS) $(SDL2_CFLAGS) -o $(BIN_DIR)/test_spotlight_sdl \
		tests/test_spotlight_sdl.c $(OBJ_DIR)/wf_rt.o \
		$(RT_LIB) $(BUILD_DIR)/$(NAME) $(PNG_LIB) \
		$(SDL2_LDFLAGS) $(RT_LDFLAGS)
	@echo "Spotlight SDL test built: $(BIN_DIR)/test_spotlight_sdl"

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Cleaned object files"

fclean: clean
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"

re: fclean all

.PHONY: all clean fclean re viewer rt_obj cornell_sdl spotlight_sdl
