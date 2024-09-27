#include "closest_dot.h"

t_dots	*create_first_dot_node(char *str, t_dots *list)
{
	char	*s;

	list = (t_dots *)malloc(sizeof(t_dots));
	s = my_strcdup(str, ',');
	list->x = my_atoi(s);
	free(s);
	s = fromstrcdup(str, ',');
	list->y = my_atoi(s);
	free(s);
	list->next = NULL;
	return (list);
}

t_dots	*create_last_dot_node(char *str, t_dots *prev)
{
	char		*s;
	t_dots		*new;

	new = (t_dots *)malloc(sizeof(t_dots));
	prev->next = new;
	s = my_strcdup(str, ',');
	new->x = my_atoi(s);
	free(s);
	s = fromstrcdup(str, ',');
	new->y = my_atoi(s);
	free(s);
	new->next = NULL;
	return (new);
}

t_stream	*create_first_ret_node(t_dots *dot, t_stream *list)
{
	list = (t_stream *)malloc(sizeof(t_stream));
	list->closest_dots = dot;
	list->next = NULL;
	return (list);
}

t_stream	*create_last_ret_node(t_dots *dot, t_stream *prev)
{
	t_stream		*new;

	new = (t_stream *)malloc(sizeof(t_stream));
	prev->closest_dots = dot;
	prev->next = new;
	new->next = NULL;
	return (new);
}

void	clear_dots_list(t_dots *node)
{
	t_dots	*temp;

	while (node)
	{
		temp = node;
		node = node->next;
		free(temp);
	}
}

void	clear_ret_list(t_stream *node)
{
	t_stream	*temp;

	while (node)
	{
		temp = node;
		node = node->next;
		free(temp);
	}
}

void	print_list(t_stream *list)
{
	t_stream	*temp;
	t_dots		*dots;
	int			i;

	temp = list;
	i = 0;
	while (temp)
	{
		dots = temp->closest_dots;
		printf("Ponto %i mais próximo\n", i);
		printf("x: %i & ", dots->x);
		printf("y: %i\n", dots->y);
		i++;
		temp = temp->next;
	}
}
