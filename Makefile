# **************************************************************************** #
#                                                                              #
#    Root Makefile – miniRT                                                    #
#                                                                              #
#    Builds:  vendor libs ➜ librt.a (core+engine) ➜ miniRT + studio tests      #
#                                                                              #
# **************************************************************************** #

NAME        := rt

# ──────────────────────────────────────────────────────────────────────────── #
#  Directories                                                                 #
# ──────────────────────────────────────────────────────────────────────────── #

BUILD       := build
OBJ_DIR     := $(BUILD)/obj
DEP_DIR     := $(BUILD)/deps
LIB_DIR     := $(BUILD)/lib
BIN_DIR     := $(BUILD)/bin

# --- New source layout -------------------------------------------------------
SRC_CORE    := srcs/core
SRC_ENGINE  := srcs/engine
SRC_APP     := srcs/app
STUDIO      := studio
SRC_PPM     := $(STUDIO)/tests/ppms
SRC_LIBX    := $(STUDIO)/tests/libx
SRC_SDL     := $(STUDIO)/tests/sdl

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
               -fPIE

ifdef BONUS
	BUILD_NAME := ${PROFILE}_BONUS
	CFLAGS += -DBONUS
endif
ifndef BONUS
	BUILD_NAME := ${PROFILE}
endif

DEPFLAGS     = -MMD -MP

LDFLAGS     := -flto -fopenmp
LDLIBS      := -lm -lz

# X11 / MLX link flags
XLIBS       := -lXext -lX11

# SDL2 flags (only evaluated when needed)
SDL_CFLAGS   = $(shell sdl2-config --cflags 2>/dev/null)
SDL_LDLIBS   = $(shell sdl2-config --libs   2>/dev/null) -lGL

# ──────────────────────────────────────────────────────────────────────────── #
#  Include paths                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

# Core & engine subdivisions
INC_CORE    := -I $(SRC_CORE)/types \
               -I $(SRC_CORE)/math \
               -I $(SRC_CORE)/geometry \
               -I $(SRC_CORE)/io \
               -I $(SRC_CORE)/utils

INC_ENGINE  := -I $(SRC_ENGINE)/accelerators \
               -I $(SRC_ENGINE)/camera \
               -I $(SRC_ENGINE)/materials \
               -I $(SRC_ENGINE)/sampling \
               -I $(SRC_ENGINE)/shading \
               -I $(SRC_ENGINE)/textures

INC_APP     := -I $(SRC_APP)
INC_PNG     := -I $(V_PNG)
INC_WF      := -I $(V_WF)
INC_MLX     := -I $(V_MLX)/include
INC_LIBFT   := -I $(V_LIBFT)/include -I $(V_LIBFT)

INC_RT_PARSER := -I $(SRC_APP)/parser/rt
INC_JSON      := -I $(SRC_APP)/parser/json
INC_CSV       := -I $(SRC_APP)/parser/csv
INC_OBJ       := -I $(SRC_APP)/parser/obj
INC_LIVE      := -I $(SRC_APP)/live
INC_BUILDERS  := -I $(STUDIO)/builders

# Combined include sets
INC_LIB     := $(INC_CORE) $(INC_ENGINE) $(INC_APP) $(INC_PNG) $(INC_BUILDERS)
INC_MINIRT  := $(INC_LIB) $(INC_RT_PARSER) $(INC_JSON) $(INC_CSV) $(INC_OBJ) \
               $(INC_LIVE) $(INC_MLX) $(INC_LIBFT) $(INC_WF) -I $(STUDIO)
INC_STUDIO  := $(INC_LIB) $(INC_PNG) $(INC_WF) -I $(STUDIO)

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

# --- Core library (core/** + engine/** + app/settings.c) → librt.a -----------
CORE_SRC    := $(shell find $(SRC_CORE) $(SRC_ENGINE) -name '*.c') $(SRC_APP)/settings.c
# Map  srcs/core/math/vector.c  →  build/obj/core/math/vector.o
CORE_OBJ    := $(patsubst srcs/%.c,$(OBJ_DIR)/%.o,$(CORE_SRC))
LIBRT_A     := $(LIB_DIR)/librt.a

# --- miniRT binary (app/** — includes parser/**) ------------------------------
MINIRT_SRC  := $(shell find $(SRC_APP) -name '*.c')
MINIRT_OBJ  := $(patsubst srcs/%.c,$(OBJ_DIR)/%.o,$(MINIRT_SRC))
MINIRT_BIN  := $(BIN_DIR)/$(NAME)

# --- Studio / PPM tests (each .c → one binary) -------------------------------
PPM_SRC     := $(wildcard $(SRC_PPM)/*.c)
PPM_BIN     := $(PPM_SRC:$(SRC_PPM)/%.c=$(BIN_DIR)/ppms/%)

# --- Studio / MLX tests ------------------------------------------------------
LIBX_SRC    := $(wildcard $(SRC_LIBX)/*.c)
LIBX_BIN    := $(LIBX_SRC:$(SRC_LIBX)/%.c=$(BIN_DIR)/libx/%)

# --- Studio / SDL tests ------------------------------------------------------
SDL_SRC     := $(wildcard $(SRC_SDL)/*.c)
SDL_BIN     := $(SDL_SRC:$(SRC_SDL)/%.c=$(BIN_DIR)/sdl/%)

# ══════════════════════════════════════════════════════════════════════════════#
#  TARGETS                                                                     #
# ══════════════════════════════════════════════════════════════════════════════#

.PHONY: all bonus clean fclean re libs miniRT ppms libx sdl studio cie test tests

all: $(MINIRT_BIN)

# Bonus build: re-invoke with -DBONUS enabled. Bonus features are additive;
# object dirs get profile-tagged once #ifdef BONUS code lands (Phase 4).
bonus:
	@$(MAKE) BONUS=1 all --no-print-directory

studio: ppms libx sdl

# ──────────────────────────────────────────────────────────────────────────── #
#  CIE codegen (build-time CSV → C)                                           #
# ──────────────────────────────────────────────────────────────────────────── #

CIE_CSV   := $(STUDIO)/assets/cie_database/value.csv
CIE_GEN_H := $(SRC_APP)/parser/csv/cie_table.h
CIE_GEN_C := $(SRC_APP)/parser/csv/cie_table.c
CIE_SCRIPT := vendor/scripts/cie_csv_to_c.sh

$(CIE_GEN_C) $(CIE_GEN_H): $(CIE_CSV) $(CIE_SCRIPT)
	@printf "\033[36m[codegen]\033[0m CIE tables\n"
	@bash $(CIE_SCRIPT) $(CIE_CSV) $(SRC_APP)/parser/csv

cie: $(CIE_GEN_C) $(CIE_GEN_H)

# ──────────────────────────────────────────────────────────────────────────── #
#  Directory creation (auto from object paths)                                 #
# ──────────────────────────────────────────────────────────────────────────── #

# Gather every directory we will need
ALL_OBJ_DIRS := $(sort $(dir $(CORE_OBJ) $(MINIRT_OBJ)))
ALL_DEP_DIRS := $(subst $(OBJ_DIR),$(DEP_DIR),$(ALL_OBJ_DIRS))
DIR_LIST     := $(ALL_OBJ_DIRS) $(ALL_DEP_DIRS) \
                $(LIB_DIR) $(BIN_DIR) $(BIN_DIR)/ppms $(BIN_DIR)/libx $(BIN_DIR)/sdl \
                $(DEP_DIR)/tests/ppms $(DEP_DIR)/tests/libx $(DEP_DIR)/tests/sdl

$(DIR_LIST):
	@mkdir -p $@

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: libft                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

# Build order: libft → minilibX → png_writer → wavefront
# Order-only prerequisites enforce sequencing without forcing rebuilds.

$(LIBFT_A):
	@printf "\033[36m[vendor]\033[0m  Building libft …\n"
	@$(MAKE) -C $(V_LIBFT) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: minilibX                                                            #
# ──────────────────────────────────────────────────────────────────────────── #

$(MLX_A): | $(LIBFT_A)
	@printf "\033[36m[vendor]\033[0m  Building minilibX …\n"
	@$(MAKE) -C $(V_MLX) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: png_writer → libpnglode.a                                           #
# ──────────────────────────────────────────────────────────────────────────── #

$(LIBPNG_A): | $(LIBFT_A) $(MLX_A)
	@printf "\033[36m[vendor]\033[0m  Building png_writer …\n"
	@$(MAKE) -C $(V_PNG) all --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Vendor: wavefront → libwavefront.a                                          #
# ──────────────────────────────────────────────────────────────────────────── #

$(LIBWF_A): | $(LIBFT_A) $(MLX_A) $(LIBPNG_A)
	@printf "\033[36m[vendor]\033[0m  Building wavefront …\n"
	@$(MAKE) -C $(V_WF) all EXTRA_INC="-I $(CURDIR)/$(SRC_CORE)/types -I $(CURDIR)/$(SRC_CORE)/math -I $(CURDIR)/$(SRC_CORE)/geometry -I $(CURDIR)/$(SRC_ENGINE)/materials -I $(CURDIR)/$(SRC_ENGINE)/textures -I $(CURDIR)/$(SRC_CORE)/io -I $(CURDIR)/$(SRC_CORE)/utils -I $(CURDIR)/$(SRC_ENGINE)/accelerators -I $(CURDIR)/$(SRC_ENGINE)/camera -I $(CURDIR)/$(SRC_APP) -I $(CURDIR)/$(V_PNG) -I $(CURDIR)/$(STUDIO)" --no-print-directory

# ──────────────────────────────────────────────────────────────────────────── #
#  Core + Engine → librt.a                                                     #
# ──────────────────────────────────────────────────────────────────────────── #

# Pattern rule:  srcs/X/Y/Z.c  →  build/obj/X/Y/Z.o
# Objects depend on all vendor libs (order-only) so vendors build first.
$(OBJ_DIR)/%.o: srcs/%.c | $(DIR_LIST) $(LIBFT_A) $(MLX_A) $(LIBPNG_A) $(LIBWF_A)
	@printf "\033[90m[cc]\033[0m      $<\n"
	@$(CC) $(CFLAGS) $(INC_MINIRT) $(DEPFLAGS) -MF $(DEP_DIR)/$*.d -c $< -o $@

$(LIBRT_A): $(CORE_OBJ) | $(LIB_DIR)
	@printf "\033[35m[lib]\033[0m     librt.a (%d objects)\n" $(words $(CORE_OBJ))
	@$(AR) $(ARFLAGS) $@ $?

libs: $(LIBFT_A) $(MLX_A) $(LIBPNG_A) $(LIBWF_A) $(LIBRT_A)

# ──────────────────────────────────────────────────────────────────────────── #
#  miniRT binary                                                               #
# ──────────────────────────────────────────────────────────────────────────── #

$(MINIRT_BIN): $(MINIRT_OBJ) $(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) $(LIBWF_A) $(MLX_A) | $(BIN_DIR)
	@printf "\033[32m[link]\033[0m    $(NAME)\n"
	@$(CC) $(LDFLAGS) $(MINIRT_OBJ) \
		$(LIBRT_A) $(LIBWF_A) $(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) $(MLX_A) \
		$(XLIBS) $(LDLIBS) -o $@
	@[ -L $(NAME) ] && rm -f $(NAME) || true
	@[ ! -d $(NAME) ] && ln -sf $(BIN_DIR)/$(NAME) $(NAME) || true

miniRT: $(MINIRT_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – PPM tests (each .c → one binary)                                   #
# ──────────────────────────────────────────────────────────────────────────── #

$(BIN_DIR)/ppms/%: $(SRC_PPM)/%.c $(LIBRT_A) $(LIBPNG_A) | $(BIN_DIR)/ppms $(DEP_DIR)/tests/ppms
	@printf "\033[33m[ppm]\033[0m     $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) \
		-MMD -MP -MF $(DEP_DIR)/tests/ppms/$*.d \
		$< $(LIBRT_A) $(LIBPNG_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

# scene_obj needs wavefront library
$(BIN_DIR)/ppms/scene_obj: $(SRC_PPM)/scene_obj.c $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A) | $(BIN_DIR)/ppms $(DEP_DIR)/tests/ppms
	@printf "\033[33m[ppm]\033[0m     scene_obj (+ wavefront)\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) $(INC_WF) \
		-MMD -MP -MF $(DEP_DIR)/tests/ppms/scene_obj.d \
		$< $(LIBWF_A) $(LIBRT_A) $(LIBPNG_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

ppms: $(PPM_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Parser tests (link against miniRT objects + librt)                           #
# ──────────────────────────────────────────────────────────────────────────── #

# JSON parser objects needed by test
JSON_OBJ := $(OBJ_DIR)/app/parser/json/json_lexer.o \
            $(OBJ_DIR)/app/parser/json/json_parser.o \
            $(OBJ_DIR)/app/parser/json/json_helpers.o \
            $(OBJ_DIR)/app/parser/json/json_helpers2.o

CIE_OBJ  := $(OBJ_DIR)/app/parser/csv/cie_table.o \
            $(OBJ_DIR)/core/math/spectrum.o

$(BIN_DIR)/ppms/test_json_parser: $(SRC_PPM)/test_json_parser.c $(JSON_OBJ) $(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) | $(BIN_DIR)/ppms
	@printf "\033[33m[test]\033[0m    test_json_parser\n"
	@$(CC) $(CFLAGS) $(INC_MINIRT) \
		$< $(JSON_OBJ) $(LIBRT_A) $(LIBPNG_A) $(LIBFT_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

$(BIN_DIR)/ppms/test_cie_spectrum: $(SRC_PPM)/test_cie_spectrum.c $(CIE_OBJ) $(LIBRT_A) $(LIBPNG_A) | $(BIN_DIR)/ppms
	@printf "\033[33m[test]\033[0m    test_cie_spectrum\n"
	@$(CC) $(CFLAGS) $(INC_MINIRT) \
		$< $(CIE_OBJ) $(LIBRT_A) $(LIBPNG_A) \
		$(LDFLAGS) $(LDLIBS) -o $@

test: ppms libx sdl
	@printf "\033[32m[test]\033[0m    All test binaries built in $(BIN_DIR)/{ppms,libx,sdl}/\n"

tests: test

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – MinilibX tests (each .c → one binary)                             #
# ──────────────────────────────────────────────────────────────────────────── #

$(BIN_DIR)/libx/%: $(SRC_LIBX)/%.c $(LIBRT_A) $(LIBPNG_A) $(MLX_A) | $(BIN_DIR)/libx $(DEP_DIR)/tests/libx
	@printf "\033[34m[libx]\033[0m    $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) $(INC_MLX) \
		-MMD -MP -MF $(DEP_DIR)/tests/libx/$*.d \
		$< $(LIBRT_A) $(LIBPNG_A) $(MLX_A) \
		$(LDFLAGS) $(XLIBS) $(LDLIBS) -o $@

libx: $(LIBX_BIN)

# ──────────────────────────────────────────────────────────────────────────── #
#  Studio – SDL tests (each .c → one binary)                                   #
# ──────────────────────────────────────────────────────────────────────────── #

$(BIN_DIR)/sdl/%: $(SRC_SDL)/%.c $(LIBRT_A) $(LIBPNG_A) $(LIBWF_A) | $(BIN_DIR)/sdl $(DEP_DIR)/tests/sdl
	@printf "\033[34m[sdl]\033[0m     $*\n"
	@$(CC) $(CFLAGS) $(INC_STUDIO) $(INC_WF) $(SDL_CFLAGS) \
		-MMD -MP -MF $(DEP_DIR)/tests/sdl/$*.d \
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
	@[ -L $(NAME) ] && rm -f $(NAME) || true
	@$(MAKE) -C $(V_LIBFT)  fclean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_PNG)    fclean --no-print-directory 2>/dev/null || true
	@$(MAKE) -C $(V_WF)     fclean --no-print-directory 2>/dev/null || true
	@printf "\033[31m[fclean]\033[0m  $(NAME)\n"

re: fclean all

# ──────────────────────────────────────────────────────────────────────────── #
#  Auto-dependency inclusion                                                   #
# ──────────────────────────────────────────────────────────────────────────── #

-include $(wildcard $(DEP_DIR)/**/*.d)
-include $(wildcard $(DEP_DIR)/**/**/*.d)
-include $(wildcard $(DEP_DIR)/**/**/**/*.d)
