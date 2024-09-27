NAME = closest_dot

CC = cc

FLAGS = -Wall -Wextra -Werror -g

SRCS = closest_dot_stream.c get_next_line.c get_next_line_utils.c list_functions.c

OBJS = ${SRCS:.c=.o}

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

LIBFT = my_libft/libft.a

all: $(NAME)

$(LIBFT):
	make -s -C my_libft

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(LIBFT) -lm -o $(NAME)

clean:
	make clean -C my_libft
	rm -f $(OBJS)

fclean: clean
	make fclean -C my_libft
	rm -f $(NAME)

re: fclean all

run: re
	./$(NAME)

val: re
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME)
