#include "RMD.h"

void	getFirstUserInput(t_RMD *data)
{
	char	buffer[256];
	
	printf("Enter ID: ");
	fgets(buffer, sizeof(buffer), stdin);
	data->id = atoi(buffer);
	
	printf("Enter Name: ");
	fgets(buffer, sizeof(buffer), stdin);
	data->name = strdup(buffer);
	
	data->next = NULL;
}

int	main()
{
	t_RMD	*data = NULL;

	data = malloc(sizeof(t_RMD));
	getFirstUserInput(data);
	operations(&data);
	clear_all(&data);
	return (0);
}