#include <iostream>


// 81 + 1 for 'ending null'
#define SUDOKU_SIZE 82

void printSudoku(int* sudoku);

int main()
{
	int sudoku[SUDOKU_SIZE];
	char input[SUDOKU_SIZE];
	std::cin >> input;

	// Convert chars to ints
	for (unsigned int i = 0; i < SUDOKU_SIZE; i++)
		sudoku[i] = input[i] - '0';

	printSudoku(sudoku);

	return 0;
}

void printSudoku(int* sudoku)
{
	std::cout << "done!\n";
	unsigned int index = 0;
	/*	Print sections:
		8 5 9 | 6 1 2 | 4 3 7
		7 2 3 | 8 5 4 | 1 6 9
		1 6 4 | 3 7 9 | 5 2 8
		------+-------+------
		9 8 6 | 1 4 7 | 3 5 2
		3 7 5 | 2 6 8 | 9 1 4
		2 4 1 | 5 9 3 | 7 8 6
		------+-------+------
		4 3 2 | 9 8 1 | 6 7 5
		6 1 7 | 4 2 5 | 8 9 3
		5 9 8 | 7 3 6 | 2 4 1
	*/
	for (unsigned int i = 0; i < 3; i++)
	{
		/*	Print section-rows
			8 5 9 | 6 1 2 | 4 3 7
			7 2 3 | 8 5 4 | 1 6 9
			1 6 4 | 3 7 9 | 5 2 8
		*/
		for (unsigned int j = 0; j < 3; j++)
		{
			// Print row-numbers: 8 5 9 | 6 1 2 | 4 3 7
			for (unsigned int k = 0; k < 3; k++)
			{
				// Print numbers: 8 5 9
				for (unsigned l = 0; l < 3; l++)
				{
					if (l != 0)
						std::cout << " ";
					std::cout << sudoku[index];
					index++;
				}
				if (k != 2)
					std::cout << " | ";
			}
			std::cout << "\n";
		}
		if (i != 2)
			std::cout << "------+-------+------\n";

	}
}
