/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   closest_dot.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpaixao- <lpaixao-@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 15:53:33 by lpaixao-          #+#    #+#             */
/*   Updated: 2024/09/27 00:45:02 by lpaixao-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLOSEST_DOT_H
# define CLOSEST_DOT_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include "my_libft/libft.h"
# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>
# include <math.h>
# include <stdio.h>


typedef struct  s_stream	t_stream;
typedef struct  s_dots		t_dots;

typedef struct	s_dots
{
	int			x;
	int			y;
	double		hypotenuse;
	t_dots		*next;
}	t_dots;

typedef struct	s_stream
{
	t_dots			*closest_dots;
	t_stream		*next;
}	t_stream;

//closest_dot
t_stream			*closest_dot_stream(t_dots *dots);
int					find_hypotenuse(int	x, int y);
int					my_sqrt(int nb);

//linked list
t_dots				*create_first_dot_node(char *c, t_dots *list);
t_dots				*create_last_dot_node(char *c, t_dots *prev);
void				print_list(t_stream *list);
void				clear_dots_list(t_dots *node);
void				clear_ret_list(t_stream *node);
t_stream			*create_first_ret_node(t_dots *dot, t_stream *list);
t_stream			*create_last_ret_node(t_dots *dot, t_stream *prev);

//get next line
char				*get_next_line(int fd);
char				*read_line(char *str, int fd, int ret_r);
char				*cut_line(char *str, char **extra);
char				*ft_strchr(const char *s, int c);
int					my_strchr(const char *s, int c);
long unsigned int	ft_strlen(const char *str);
char				*ft_strjoin(char *s1, char *s2);
char				*my_join(char *s1, char *s2, char *string);
char				*ft_substr(char const *s, unsigned int start, size_t len);
size_t				ft_strlcpy(char *dst, const char *src, size_t size);
#endif
