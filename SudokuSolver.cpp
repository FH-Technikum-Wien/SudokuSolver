#include <iostream>
#include <vector>
#include <set>


// 81 + 1 for 'ending null'
#define SUDOKU_SIZE 82


class Cell
{
public:
	bool isLocked = false;

	Cell() {};
	Cell(std::set<int> possibleNumbers);
	void lockNumber(int number);
	bool removeNumber(int number);
	int getLockedNumber();

private:
	std::set<int> possibleNumbers{ 1,2,3,4,5,6,7,8,9 };

};

Cell::Cell(std::set<int> possibleNumbers)
{
	this->possibleNumbers = possibleNumbers;
}

void Cell::lockNumber(int number)
{
	isLocked = true;
	possibleNumbers = { number };
}

bool Cell::removeNumber(int number)
{
	if (isLocked || possibleNumbers.find(number) == possibleNumbers.end())
		return false;
	possibleNumbers.erase(number);
	if (possibleNumbers.size() == 1)
		isLocked = true;
	return true;
}

int Cell::getLockedNumber() {
	if (!isLocked)
		return 0;
	return *possibleNumbers.begin();
}


void printSudoku(Cell* sudoku);
void updatePossibleCellNumbers(Cell* sudokuCells);

int main()
{
	int sudoku[SUDOKU_SIZE];
	Cell sudokuCells[SUDOKU_SIZE];
	char input[SUDOKU_SIZE];
	std::cin >> input;

	// Convert chars to ints
	for (unsigned int i = 0; i < SUDOKU_SIZE; i++) {
		int number = input[i] - '0';
		sudoku[i] = number;
		if (number != 0) {
			sudokuCells[i].lockNumber(number);
		}
	}
	updatePossibleCellNumbers(sudokuCells);

	printSudoku(sudokuCells);

	return 0;
}



void printSudoku(Cell* sudoku)
{
	std::cout << "done!\n";
	unsigned int index = 0;
	//	Print region.
	for (unsigned int i = 0; i < 3; i++)
	{
		//	Print section.
		for (unsigned int j = 0; j < 3; j++)
		{
			// Print row.
			for (unsigned int k = 0; k < 3; k++)
			{
				// Print number.
				for (unsigned l = 0; l < 3; l++)
				{
					if (l != 0)
						std::cout << " ";
					std::cout << sudoku[index].getLockedNumber();
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

void updatePossibleCellNumbers(Cell* sudokuCells)
{
	bool hasRemoved = true;
	while (hasRemoved)
	{
		hasRemoved = false;
		
		for (unsigned int cellIndex = 0; cellIndex < SUDOKU_SIZE - 1; cellIndex++)
		{
			Cell cell = sudokuCells[cellIndex];
			if (!cell.isLocked)
				continue;

			// CHECK SECTIONS
			// Get region
			int regionIndex = cellIndex / 27;
			// Get section 
			int sectionIndex = (cellIndex / 3) % 3 + regionIndex * 3;

			// Calculate start index of current section.
			unsigned int sectionStartIndex = regionIndex * 27 + ((sectionIndex % 3) * 3);
			// Go through columns in section.
			for (unsigned int i = 0; i < 3; i++)
			{
				// Go through rows in section.
				for (unsigned int j = 0; j < 3; j++)
				{
					unsigned currentCellIndex = j * 9 + i + sectionStartIndex;
					if (currentCellIndex == cellIndex)
						continue;
					if(sudokuCells[currentCellIndex].removeNumber(cell.getLockedNumber()))
						hasRemoved = true;
				}
			}

			// CHECK ROWS & COLUMNS
			for (unsigned int i = 0; i < 9; i++)
			{
				// Check all cells in row.
				unsigned int rowIndex = (cellIndex / 9) * 9 + i;
				if (rowIndex != cellIndex) {
					if (sudokuCells[rowIndex].removeNumber(cell.getLockedNumber()))
						hasRemoved = true;
				}
				// Check all cells in column.
				unsigned int columnIndex = (cellIndex % 9) + (i * 9);
				if (columnIndex != cellIndex) {
					if (sudokuCells[columnIndex].removeNumber(cell.getLockedNumber()))
						hasRemoved = true;
				}
			}
		}
	}
}

/*
8 5 0 | 0 0 2 | 4 0 0
7 2 0 | 0 0 0 | 0 0 9
0 0 4 | 0 0 0 | 0 0 0
------+-------+------
0 0 0 | 1 0 7 | 0 0 2
3 0 5 | 0 0 0 | 9 0 0
0 4 0 | 0 0 0 | 0 0 0
------+-------+------
0 0 0 | 0 8 0 | 0 7 0
0 1 7 | 0 0 0 | 0 0 0
0 0 0 | 0 3 6 | 0 4 0
*/
