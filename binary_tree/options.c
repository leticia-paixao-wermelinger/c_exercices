
#include "BinaryTree.h"

#define MAX_CHOICE 5

void	options(t_BT **tree)
{
	int	choice;
	int	choiceLimit = MAX_CHOICE;;

	while (choiceLimit > 0)
	{
//		printf("\n" YELLOW "Current tree structure:\n" RESET);
//		printTree(*tree);
		choice = chooseOption();
		switch (choice)
		{
			case INSERT:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Insert new number:\n" RESET);
				insertNumber(*tree);
				break;
			case REMOVE:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Remove a number.\n" RESET);
				removeNumber(*tree);
				break;
			case SEARCH_PRE:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Search by pre-order.\n" RESET);
				searchPreOrder(*tree);
				break;
			case SEARCH_ORDER:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Search by order.\n" RESET);
				searchInOrder(*tree);
				break;
			case SEARCH_POST:
				choiceLimit = MAX_CHOICE;
				printf("\n" BLUE "Search by post-order.\n" RESET);
				searchPostOrder(*tree);
				break;
			case CLOSE:
				printf("\n" BLUE "Exiting program.\n" RESET);
				return;
			default:
				choiceLimit--;
				printf("\n" RED "Invalid choice. Please try again.\n" RESET);
		}
	}
	return;
}

int	chooseOption()
{
	int choice;
	char buffer[256];

	printf("\nChoose an operation:\n");
	printf("1. Insert\n");
	printf("2. Remove\n");
	printf("3. Search Pre-order\n");
	printf("4. Search in order\n");
	printf("5. Search post-order\n");
	printf("0. Close the program\n");
	printf("Enter your choice: ");
	fgets(buffer, sizeof(buffer), stdin);
	choice = atoi(buffer);
	if (choice < CLOSE || choice > SEARCH_POST)
		return 0;
	return choice;
}