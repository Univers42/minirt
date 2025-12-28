NAME = test_brdf
BIN_DIR = ../../bin

CC = gcc
CFLAGS = -Wall -Wextra -Werror -I../utils -I. -I../

SRC_UTILS = ../utils/vec3.c ../utils/point3.c ../utils/normal.c ../utils/matrix.c
OBJ_UTILS = ../utils/vec3.o ../utils/point3.o ../utils/normal.o ../utils/matrix.o

SRC = principle_brdf.c main.c
OBJ = $(SRC:.c=.o)

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): $(OBJ) $(OBJ_UTILS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(OBJ_UTILS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

../utils/%.o: ../utils/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o ../utils/*.o

fclean: clean
	rm -f $(BIN_DIR)/$(NAME)

re: fclean all

.PHONY: all clean fclean re
