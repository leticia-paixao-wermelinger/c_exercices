#include "closest_dot.h"
#include <math.h>
#include <stdio.h>

/*
The following function takes a stream (saved as a list of t_dots structs) as an argument and returns another
linked list with each node pointing to the closest dots to (0,0).
*/

t_dots	*closest_dot_stream(t_dots *dots)
{
	t_dots		*stream = dots;
	t_dots		*ret = NULL;
	t_dots		*temp;
	int			count = 0;
	double		closest_hypotenuse;
	int			flag_first = 0;

	dots->hypotenuse = find_hypotenuse(dots->x, dots->y);
	closest_hypotenuse = dots->hypotenuse;
	while (stream)
	{
		stream->hypotenuse = find_hypotenuse(stream->x, stream->y);
		if (stream->hypotenuse < closest_hypotenuse)
		{
			closest_hypotenuse = stream->hypotenuse; 
			count = 0;
		}
		else if (stream->hypotenuse == closest_hypotenuse)
			count++;
		stream = stream->next;
	}
	stream = dots;
	while (stream && count >= 0)
	{
		if (stream->hypotenuse == closest_hypotenuse)
		{
			if (flag_first == 0)
			{
				ret = create_first_ret_node(stream, ret);
				temp = ret;
				flag_first = 1;
			}
			else
				temp = create_last_ret_node(stream, temp);
			count--;
		}
		stream = stream->next;
	}
	return (ret);
}

double	find_hypotenuse(int x, int y)
{
	double	hypotenuse = 0;

	hypotenuse = sqrt((x * x) + (y * y));
	return (hypotenuse);
}

int	main()
{
	char		*str = NULL;
	char		*pathname = "./pathname.txt";
	int			fd = open(pathname, O_RDONLY);
	t_dots		*dots = NULL;
	t_dots		*temp = NULL;
	t_dots		*ret = NULL;

	if (fd < 0)
		return (0);
	str = get_next_line(fd);
	dots = create_first_dot_node(str, dots);
	temp = dots;
	free(str);
	while (42)
	{
		str = get_next_line(fd);
		if (str == NULL)
			break ;
		temp = create_last_dot_node(str, temp);
		free(str);
	}
	close(fd);
	ret = closest_dot_stream(dots);
	print_list(ret);
	clear_list(ret);
	clear_list(dots);
}
