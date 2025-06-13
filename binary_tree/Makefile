NAME = BinarySearchTree

CC = cc
CFLAGS = -Wall -Wextra -Werror -g

# Colors
RESET=\033[0m
PURPLE=\033[35m
GREEN=\033[32m
PINK=\033[35m
RED=\033[31m
BLUE=\033[34m

SRCS = \
main.c \
createTree.c \
changeTree.c \
searchTree.c \
cleaning.c \
options.c \
removeNode.c

OBJS = ${SRCS:.c=.o}

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)
	@echo "$(PINK) Your data operations system is ready to be used!$(RESET)"

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
	@rm -f $(OBJS)
	@echo "$(BLUE) 📤 Objects deleted$(RESET)"

fclean: clean
		@rm -f $(NAME)
		@echo "$(BLUE) 🧼 All cleaned$(RESET)"

re: fclean all

run: re 
	./$(NAME)

val: re
	valgrind  --leak-check=full --show-leak-kinds=all --track-fds=yes ./$(NAME)