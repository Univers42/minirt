# **************************************************************************** #
#                                                                              #
#    Root Makefile – miniRT                                                    #
#                                                                              #
#    Builds:  vendor libs ➜ librt.a (protocol engine) ➜ miniRT + studio tests  #
#                                                                              #
# **************************************************************************** #

NAME        := miniRT

# ──────────────────────────────────────────────────────────────────────────── #
#  Directories                                                                 #
# ──────────────────────────────────────────────────────────────────────────── #

BUILD       := build
OBJ_DIR     := $(BUILD)/obj
DEP_DIR     := $(BUILD)/deps
LIB_DIR     := $(BUILD)/lib
BIN_DIR     := $(BUILD)/bin

SRC_PROTO   := srcs/protocol
SRC_PARSER  := srcs/parser
SRC_PPM     := srcs/studio/ppms
SRC_LIBX    := srcs/studio/libx
SRC_SDL     := srcs/studio/sdl

V_LIBFT     := vendor/libft
V_MLX       := vendor/minilibx-linux
V_PNG       := vendor/png_writer
V_WF        := vendor/wavefront

# ──────────────────────────────────────────────────────────────────────────── #
#  Toolchain                                                                   #
# ──────────────────────────────────────────────────────────────────────────── #

CC          := cc
AR          := gcc-ar
ARFLAGS     := rcs

PROFILES	:= opt debug debug_mem debug_mem_sanitize debug_mem_sanitize_mem

CFLAGS      := -std=c99 -O3 -march=native -fno-math-errno -flto \
               -DNDEBUG -pipe -fopenmp \
               -D_POSIX_C_SOURCE=200809L \
               -Wall -Wextra -Werror -Wno-unused-function \
               -fPIE -std=c99

ifdef BONUS
	BUILD_NAME := ${PROFILE}_BONUS
	CFLAGS += -DBONUS
endif
ifndef BONUS
	BUILD_NAME := ${PROFILE}
endif


DEPFLAGS     = -MMD -MP

LDFLAGS     := -flto -fopenmp
LDLIBS      := -lm

# X11 / MLX link flags
XLIBS       := -lXext -lX11

# SDL2 flags (only evaluated when needed)
SDL_CFLAGS   = $(shell sdl2-config --cflags 2>/dev/null)
SDL_LDLIBS   = $(shell sdl2-config --libs   2>/dev/null) -lGL

# ──────────────────────────────────────────────────────────────────────────── #
#  Include paths                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

INC_PROTO   := -I $(SRC_PROTO)
INC_PARSER  := -I $(SRC_PARSER)
INC_PNG     := -I $(V_PNG)
INC_WF      := -I $(V_WF)
INC_MLX     := -I $(V_MLX)/include
INC_LIBFT   := -I $(V_LIBFT)/include -I $(V_LIBFT)

# Combined include sets per target type
INC_ENGINE  := $(INC_PROTO) $(INC_PNG)
INC_MINIRT  := $(INC_PROTO) $(INC_PARSER) $(INC_MLX) $(INC_LIBFT) $(INC_PNG)
INC_STUDIO  := $(INC_PROTO) $(INC_PNG) $(INC_WF)

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor libraries (external Makefiles)                                       #
# ──────────────────────────────────────────────────────────────────────────── #

LIBFT_A     := $(V_LIBFT)/build/lib/libft.a
MLX_A       := $(V_MLX)/libmlx.a
LIBPNG_A    := $(V_PNG)/build/lib/libpnglode.a
LIBWF_A     := $(V_WF)/build/lib/libwavefront.a

# ──────────────────────────────────────────────────────────────────────────── #
#  Source → Object mappings                                                    #
# ──────────────────────────────────────────────────────────────────────────── #

# --- Protocol engine --------------------------------------------------------
PROTO_SRC   := $(wildcard $(SRC_PROTO)/*.c)
PROTO_OBJ   := $(PROTO_SRC:$(SRC_PROTO)/%.c=$(OBJ_DIR)/protocol/%.o)
LIBRT_A     := $(LIB_DIR)/librt.a

# --- Parser (miniRT) ---------------------------------------------------------
PARSER_SRC  := $(wildcard $(SRC_PARSER)/*.c)
PARSER_OBJ  := $(PARSER_SRC:$(SRC_PARSER)/%.c=$(OBJ_DIR)/parser/%.o)
MINIRT_BIN  := $(BIN_DIR)/$(NAME)

# --- Studio / PPM tests ------------------------------------------------------
PPM_SRC     := $(wildcard $(SRC_PPM)/*.c)
PPM_BIN     := $(PPM_SRC:$(SRC_PPM)/%.c=$(BIN_DIR)/ppms/%)

# --- Studio / MLX tests ------------------------------------------------------
LIBX_SRC    := $(wildcard $(SRC_LIBX)/*.c)
LIBX_BIN    := $(LIBX_SRC:$(SRC_LIBX)/%.c=$(BIN_DIR)/libx/%)

# --- Studio / SDL tests ------------------------------------------------------
SDL_SRC     := $(wildcard $(SRC_SDL)/*.c)
SDL_BIN     := $(SDL_SRC:$(SRC_SDL)/%.c=$(BIN_DIR)/sdl/%)

# --- All dependency files ----------------------------------------------------
ALL_DEPS    := $(PROTO_OBJ:$(OBJ_DIR)/%.o=$(DEP_DIR)/%.d) \
               $(PARSER_OBJ:$(OBJ_DIR)/%.o=$(DEP_DIR)/%.d) \
               $(PPM_SRC:$(SRC_PPM)/%.c=$(DEP_DIR)/studio/ppms/%.d) \
               $(LIBX_SRC:$(SRC_LIBX)/%.c=$(DEP_DIR)/studio/libx/%.d) \
               $(SDL_SRC:$(SRC_SDL)/%.c=$(DEP_DIR)/studio/sdl/%.d)

# ══════════════════════════════════════════════════════════════════════════════#
#  TARGETS                                                                     #
# ══════════════════════════════════════════════════════════════════════════════#

.PHONY: all clean fclean re libs miniRT ppms libx sdl studio dirs

all: $(MINIRT_BIN)

studio: ppms libx sdl

# ──────────────────────────────────────────────────────────────────────────── #
#  Directory creation                                                          #
# ──────────────────────────────────────────────────────────────────────────── #

DIR_LIST := $(OBJ_DIR)/protocol $(OBJ_DIR)/parser \
            $(DEP_DIR)/protocol $(DEP_DIR)/parser \
            $(DEP_DIR)/studio/ppms $(DEP_DIR)/studio/libx $(DEP_DIR)/studio/sdl \
            $(LIB_DIR) $(BIN_DIR) $(BIN_DIR)/ppms $(BIN_DIR)/libx $(BIN_DIR)/sdl

$(DIR_LIST):
	@mkdir -p $@

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: libft                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

$(LIBFT_A):
	@printf "\033[36m[vendor]\033[0m  Building libft …\n"
	@$(MAKE) -C $(V_LIBFT) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: minilibX                                                            #
# ──────────────────────────────────────────────────────────────────────────── #

$(MLX_A):
	@printf "\033[36m[vendor]\033[0m  Building minilibX …\n"
	@$(MAKE) -C $(V_MLX) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: png_writer ➜ libpnglode.a                                           #
# ──────────────────────────────────────────────────────────────────────────── #

$(LIBPNG_A):
	@printf "\033[36m[vendor]\033[0m  Building png_writer …\n"
	@$(MAKE) -C $(V_PNG) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: wavefront ➜ libwavefront.a                                          #
# ──────────────────────────────────────────────────────────────────────────── #

$(LIBWF_A):
	@printf "\033[36m[vendor]\033[0m  Building wavefront …\n"
	@$(MAKE) -C $(V_WF) all EXTRA_INC="-I $(CURDIR)/$(SRC_PROTO) -I $(CURDIR)/$(V_PNG)" --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Protocol engine ➜ librt.a                                                   #
# ──────────────────────────────────────────────────────────────────────────── #

$(OBJ_DIR)/protocol/%.o: $(SRC_PROTO)/%.c | $(OBJ_DIR)/protocol $(DEP_DIR)/protocol
	@$(CC) $(CFLAGS) $(INC_ENGINE) $(DEPFLAGS) -MF $(DEP_DIR)/protocol/$*.d -c $< -o $@

$(LIBRT_A): $(PROTO_OBJ) | $(LIB_DIR)
	@printf "\033[35m[lib]\033[0m     librt.a\n"
	@$(AR) $(ARFLAGS) $@ $?

libs: $(LIBFT_A) $(MLX_A) $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A)

# ──────────────────────────────────────────────────────────────────────────── #
#  miniRT binary                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

$(OBJ_DIR)/parser/%.o: $(SRC_PARSER)/%.c | $(OBJ_DIR)/parser $(DEP_DIR)/parser
	@$(CC) $(CFLAGS) $(INC_MINIRT) $(DEPFLAGS) -MF $(DEP_DIR)/parser/$*.d -c $< -o $@

$(MINIRT_BIN): $(PARSER_OBJ) $(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) $(MLX_A) | $(BIN_DIR)
	@printf "\033[32m[link]\033[0m    $(NAME)\n"
	@$(CC) $(LDFLAGS) $(PARSER_OBJ) \
		$(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) $(MLX_A) \
		$(XLIBS) $(LDLIBS) -o $@
	@cp -f $@ $(NAME)

miniRT: $(MINIRT_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – PPM tests (each .c ➜ one binary)                                   #
# ──────────────────────────────────────────────────────────────────────────── #

# Default PPM link recipe (no wavefront)
$(BIN_DIR)/ppms/%: $(SRC_PPM)/%.c $(LIBRT_A) $(LIBPNG_A) | $(BIN_DIR)/ppms $(DEP_DIR)/studio/ppms
	@printf "\033[33m[ppm]\033[0m     $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) \
		-MMD -MP -MF $(DEP_DIR)/studio/ppms/$*.d \
		$< $(LIBRT_A) $(LIBPNG_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

# scene_obj needs wavefront library
$(BIN_DIR)/ppms/scene_obj: $(SRC_PPM)/scene_obj.c $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A) | $(BIN_DIR)/ppms $(DEP_DIR)/studio/ppms
	@printf "\033[33m[ppm]\033[0m     scene_obj (+ wavefront)\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) \
		-MMD -MP -MF $(DEP_DIR)/studio/ppms/scene_obj.d \
		$< $(LIBWF_A) $(LIBRT_A) $(LIBPNG_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

ppms: $(PPM_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – MinilibX tests (each .c ➜ one binary)                             #
# ──────────────────────────────────────────────────────────────────────────── #

$(BIN_DIR)/libx/%: $(SRC_LIBX)/%.c $(LIBRT_A) $(LIBPNG_A) $(MLX_A) | $(BIN_DIR)/libx $(DEP_DIR)/studio/libx
	@printf "\033[34m[libx]\033[0m    $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) $(INC_MLX) \
		-MMD -MP -MF $(DEP_DIR)/studio/libx/$*.d \
		$< $(LIBRT_A) $(LIBPNG_A) $(MLX_A) \
		$(LDFLAGS) $(XLIBS) $(LDLIBS) -o $@

libx: $(LIBX_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – SDL tests (each .c ➜ one binary)                                   #
# ──────────────────────────────────────────────────────────────────────────── #

$(BIN_DIR)/sdl/%: $(SRC_SDL)/%.c $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A) | $(BIN_DIR)/sdl $(DEP_DIR)/studio/sdl
	@printf "\033[34m[sdl]\033[0m     $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) $(INC_WF) $(SDL_CFLAGS) \
		-MMD -MP -MF $(DEP_DIR)/studio/sdl/$*.d \
		$< $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A) \
		$(LDFLAGS) $(SDL_LDLIBS) $(LDLIBS) -o $@

sdl: $(SDL_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  House-keeping                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

clean:
	@rm -rf $(BUILD)
	@$(MAKE) -C $(V_LIBFT)  clean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_MLX)    clean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_PNG)    clean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_WF)     clean --no-print-directory 2>/dev/null || true
	@printf "\033[31m[clean]\033[0m   build/\n"

fclean: clean
	@rm -f $(NAME)
	@$(MAKE) -C $(V_LIBFT)  fclean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_PNG)    fclean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_WF)     fclean --no-print-directory 2>/dev/null || true
	@printf "\033[31m[fclean]\033[0m  $(NAME)\n"

re: fclean all

# ──────────────────────────────────────────────────────────────────────────── #
#  Auto-dependency inclusion                                                   #
# ──────────────────────────────────────────────────────────────────────────── #

-include $(wildcard $(DEP_DIR)/protocol/*.d)
-include $(wildcard $(DEP_DIR)/parser/*.d)
-include $(wildcard $(DEP_DIR)/studio/ppms/*.d)
-include $(wildcard $(DEP_DIR)/studio/libx/*.d)
-include $(wildcard $(DEP_DIR)/studio/sdl/*.d)
