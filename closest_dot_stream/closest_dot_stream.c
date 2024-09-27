#include "closest_dot.h"
#include <math.h>
#include <stdio.h>

/*
The following function takes an array of t_dots structs as an argument and returns a linked list
with each node pointing to the closest dots to (0,0).
The 
*/

t_stream	*closest_dot_stream(t_dots *dots)
{
	t_dots		*stream;
	t_stream	*ret;
	int			count = 0;
	int			closest_hypotenuse;
	int			flag_first = 0;

	ret = NULL;
	dots->hypotenuse = find_hypotenuse(dots->x, dots->y);
	closest_hypotenuse = dots->hypotenuse;
	stream = dots;
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
	while (stream && count > 0)
	{
		if (stream->hypotenuse == closest_hypotenuse)
		{
			if (flag_first == 0)
			{
				ret = create_first_ret_node(stream, ret);
				flag_first = 1;
			}
			else
				ret = create_last_ret_node(stream, ret);
			count--;
		}
		stream = stream->next;
	}
	return (ret);
}

int	find_hypotenuse(int x, int y)
{
	double	hypotenuse = 0;

	hypotenuse = sqrt((x * x) + (y * y));
	return (hypotenuse);
}

int	main()
{
	int			fd;
	char		*pathname = "./pathname.txt";
	t_dots		*dots;
	t_dots		*temp;
	t_stream	*ret;
	char		*str;

	dots = NULL;;
	fd = open(pathname, O_RDONLY);
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
	clear_ret_list(ret);
	clear_dots_list(dots);
}
