# Minimal Makefile to build library and test programs
CC := gcc
CFLAGS := -std=c99 -I.
AR := ar
ARFLAGS := rcs
LDFLAGS := -lm

SRCDIR := .
OBJDIR := obj
TESTDIR := tests
BINDIR := bin

# All top-level .c in project root (library sources)
LIB_SRCS := $(wildcard $(SRCDIR)/*.c)
LIB_OBJS := $(addprefix $(OBJDIR)/,$(notdir $(LIB_SRCS:.c=.o)))

LIBNAME := libpnglode.a

# Test programs
TEST_SRC := $(TESTDIR)/test_png.c
TEST_OBJ := $(OBJDIR)/test_png.o
TEST_BIN := $(BINDIR)/test_png

MAIN_SRC := $(TESTDIR)/main.c
MAIN_OBJ := $(OBJDIR)/main.o
MAIN_BIN := $(BINDIR)/main

.PHONY: all dirs clean re test

all: dirs $(LIBNAME) $(MAIN_BIN) $(TEST_BIN)

dirs:
	@mkdir -p $(BINDIR)
	@mkdir -p $(OBJDIR)

# Build library archive from object files in obj/
$(LIBNAME): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

# Compile top-level sources into obj/<basename>.o
# e.g. init.c -> obj/init.o
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build test object
$(TEST_OBJ): $(TEST_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Build main object
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Link main (simple smoke test)
$(MAIN_BIN): $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJ) $(LDFLAGS)

# Link test program with library
$(TEST_BIN): $(LIBNAME) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJ) $(LIBNAME) $(LDFLAGS)

# run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

re: clean all

clean:
	-rm -rf $(OBJDIR) $(BINDIR) $(LIBNAME)
