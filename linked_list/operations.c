#include "RMD.h"

#define MAX_CHOICE 5

void	operations(t_RMD **data)
{
	int	choice;
	int	choiceLimit = MAX_CHOICE;;

	while (choiceLimit > 0)
	{
		choice = chooseOperation();
		switch (choice)
		{
			case ADD:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Add operation selected.\n" RESET);
				addNode(*data);
				break;
			case REMOVE:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Remove operation selected.\n" RESET);
				removeNode(data);
				break;
			case PRINT:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Print operation selected.\n" RESET);
				printList(*data);
				break;
			case CHANGE:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Change operation selected.\n" RESET);
				changeNode(*data);
				break;
			case EXIT:
				printf("\n" BLUE "Exiting program.\n" RESET);
				return;
			default:
				choiceLimit--;
				printf("\n" RED "Invalid choice. Please try again.\n" RESET);
		}
	}
	return;
}

int	chooseOperation()
{
	int choice;
	char buffer[256];

	printf("\nChoose an operation:\n");
	printf("1. Add\n");
	printf("2. Remove\n");
	printf("3. Print\n");
	printf("4. Change\n");
	printf("5. Exit\n");
	printf("Enter your choice: ");
	fgets(buffer, sizeof(buffer), stdin);
	choice = atoi(buffer);
	if (choice < ADD || choice > EXIT)
		return 0;
	return choice;
}