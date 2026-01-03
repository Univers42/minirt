CC = gcc
CFLAGS = -Wall -Wextra -Werror -O3 -march=native

# Enable pthread support (comment out to disable)
USE_PTHREAD = 1

ifdef USE_PTHREAD
	CFLAGS += -DUSE_PTHREAD -pthread
	LDFLAGS += -pthread
endif

NAME = minirt

SRCS = main.c
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
