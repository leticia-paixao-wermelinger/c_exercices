#include "closest_dot.h"

/*
The following function takes an array of t_dots structs as an argument and returns a linked list
with each node pointing to the closest dots to (0,0).
The 
*/

t_stream	*closest_dot_stream(t_dots *dots)
{
	t_dots		*stream;		// Array de structs com os pontos mais próximos
	t_dots		closest_mark;
	t_stream	*ret;
	int			count = 0;		// Contador de pontos mais próximos
	int			i = 0;
	int			closest_hypotenuse;
	int			current_hypotenuse;

	closest_mark.x = 0;
	closest_mark.y = 0;
	closest_hypotenuse = find_hypotenuse(closest_mark.x, closest_mark.y);
	stream = dots;
	while (stream[i])
	{
		current_hypotenuse = find_hypotenuse(dots->x, dots->x);
		if (current_hypotenuse < closest_hypotenuse)
		{
			closest_hypotenuse = current_hypotenuse; 
			count = 0;
		}
		else if (current_hypotenuse == closest_hypotenuse)
			count++;
		i++;
	}
	return (ret);
}

int	find_hypotenuse(int x, int y)
{
	int	hypotenuse = 0;

	hypotenuse = my_sqrt((x * x) + (y * y));
	return (hypotenuse);
}

int	my_sqrt(int nb)
{
	long int	x;

	x = 0;
	while (x * x <= nb)
	{
		if (x * x == nb)
			return (x);
		x++;
	}
	return (0);
}

int	main()
{
	int			fd;
	char		*pathname = "./stream.txt";
	t_dots		dots;
	t_stream	*ret;
	char		*str;
	int			i = 0;

	fd = open(pathname, O_RDONLY);
	while (42)
	{
		str = get_next_line(fd);
		if (!str)
			break ;
		dots[i].x = atoi(str[0]);
		dots[i].y = atoi(str[1]);
		free(str);
		i++;
	}
	str[i] = "\0";
	close(fd);
	ret = closest_dot_stream(dots);
	free(dots);
	print_list(ret);
	free(list);
}
