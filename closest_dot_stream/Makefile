NAME = closest_dot_stream

CC = cc

FLAGS = -Wall -Wextra -Werror -g

SRCS = closest_dot_stream.c get_next_line.c get_next_line_utils.c

OBJS = ${SRCS:.c=.o}

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

run: re
	./$(NAME)

val: re
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)
