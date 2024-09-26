/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   closest_dot.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpaixao- <lpaixao-@student.42.rio>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/23 15:53:33 by lpaixao-          #+#    #+#             */
/*   Updated: 2024/09/26 17:57:47 by lpaixao-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLOSEST_DOT_H
# define CLOSEST_DOT_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# include <stdlib.h>
# include <fcntl.h>
# include <unistd.h>
# include <stdlib.h>

typedef struct  s_closest_dots	t_closest_dots;

typedef struct	s_dots
{
	int	x;
	int	y;
}	t_dots;

typedef struct	s_stream
{
	t_dots			*closest_dots;
	t_closest_dots	*next;
}	t_stream;

//closest_dot
t_stream			*closest_dot_stream(t_dots *dots);
int					find_hypotenuse(int	x, int y);
int					my_sqrt(int nb);

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
