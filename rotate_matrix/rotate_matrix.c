#include "my_libft/libft.h"
#include <stdio.h>

void	find_sizes(char	**str, int *y, int *x)
{
	while (str[*y][*x])
		(*x)++;
	while(str[*y])
		(*y)++;	
}

char	**rotate_matrix_90dg(char **matrix)
{
	char	**ret = NULL;
	int		y_original_size = 0;
	int		x_original_size = 0;
	int		y1 = 0;
	int		x1 = 0;
	int		y2 = 0;
	int		x2 = 0;

	find_sizes(matrix, &y_original_size, &x_original_size);
	ret = my_calloc((x_original_size + 1), sizeof(char *));
	if (!ret)
		return NULL;
	while (y1 < y_original_size)
	{
		ret[y1] = my_calloc((y_original_size + 1), sizeof(char));
		ret[y1][y_original_size] = '\0';
		y1++;
	}
	y1 = 0;
	x2 = x_original_size;
	while (matrix[y1])
	{
		y2 = 0;
		x1 = y2; // Eu sei que eu poderia utilizar uma única variável, mas preferi separair por fins de melhor visualização
		x2--;
		while (matrix[y1][x1])
		{
			ret[y2][x2] = matrix[y1][x1];
			y2++;
			x1++;
		}
		y1++;
	}
	return (ret);
}

int main()
{
	char	*str = "123 456 789";
	char	**matrix_or = NULL;
	char	**matrix_new = NULL;

	matrix_or = my_split(str, ' ');
	matrix_new = rotate_matrix_90dg(matrix_or);
	if (!matrix_new)
		return (0);
	print_matrix(matrix_new);
	my_clean_vect(matrix_or);
	my_clean_vect(matrix_new);
}
