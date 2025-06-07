#include "RMD.h"

void addNode(t_RMD *data)
{
	t_RMD *new_node = malloc(sizeof(t_RMD));
	if (new_node == NULL)
	{
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	char buffer[256];
	
	printf("Enter ID for new node: ");
	fgets(buffer, sizeof(buffer), stdin);
	new_node->id = atoi(buffer);

	printf("Enter Name for new node: ");
	fgets(buffer, sizeof(buffer), stdin);
	new_node->name = strdup(buffer);
	
	new_node->next = NULL;

	// Insert the new node at the end of the list
	t_RMD *current = data;
	while (current->next != NULL)
		current = current->next;
	
	current->next = new_node;
}

void	removeNode(t_RMD **data)
{
	if (*data == NULL || (*data)->next == NULL)
	{
		printf(RED "List is empty, nothing to remove." RESET "\n");
		return;
	}

	int idToRemove;
	char buffer[256];
	
	printList(*data);
	printf("Enter ID of node to remove: ");
	fgets(buffer, sizeof(buffer), stdin);
	idToRemove = atoi(buffer);

	t_RMD *current = *data;
	t_RMD *previous = NULL;

	while (current != NULL && current->id != idToRemove)
	{
		previous = current;
		current = current->next;
	}

	if (current == NULL)
	{
		printf(RED "Node with ID %d not found." RESET "\n", idToRemove);
		return;
	}

	if (previous == NULL)
	{
		(*data)->next = current->next;
		*data = current->next;
	}
	else
	{
		previous->next = current->next;
	}
	
	free_node(current);
	printf(GREEN "Node with ID %d removed successfully." RESET "\n", idToRemove);
	return;
}

void printList(t_RMD *data)
{
	if (data == NULL)
	{
		printf("List is empty.\n");
		return;
	}

	t_RMD *current = data;
	printf("\n");
	while (current != NULL)
	{
		printf(GREEN "ID: %d, Name: %s" RESET, current->id, current->name);
		current = current->next;
	}
}

void changeNode(t_RMD *data)
{
	if (data == NULL)
	{
		printf(RED "List is empty, nothing to change." RESET "\n");
		return;
	}

	int idToChange;
	char buffer[256];
	
	printList(data);
	printf("Enter ID of node to change: ");
	fgets(buffer, sizeof(buffer), stdin);
	idToChange = atoi(buffer);

	t_RMD *current = data;

	while (current != NULL && current->id != idToChange)
	{
		current = current->next;
	}

	if (current == NULL)
	{
		printf(RED "Node with ID %d not found." RESET "\n", idToChange);
		return;
	}

	printf("Enter new ID to replace ID %d: ", idToChange);
	fgets(buffer, sizeof(buffer), stdin);
	current->id = atoi(buffer);

	printf("Enter new Name for node with new ID %d: ", current->id);
	fgets(buffer, sizeof(buffer), stdin);
	free(current->name);
	current->name = strdup(buffer);

	printf(GREEN "Node %d has been changed successfully to %d." RESET "\n", idToChange, current->id);
}