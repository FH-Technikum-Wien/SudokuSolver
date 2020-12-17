#include <iostream>
#include <vector>
#include <set>

#define SUDOKU_SIZE 81


class Cell
{
public:
	bool isLocked = false;
	int index = 0;

	Cell() {};
	Cell(std::set<int> possibleNumbers, int index);
	void lockNumber(int number);
	bool removeNumber(int number);
	int getLockedNumber();
	int possibleNumberCount();
	bool IsNumberPossible(int number);
	int getPossibleNumberAtIndex(int index);
	std::set<int> getPossibleNumbers();

private:
	std::set<int> possibleNumbers{ 1,2,3,4,5,6,7,8,9 };
};

Cell::Cell(std::set<int> possibleNumbers, int index)
{
	this->possibleNumbers = possibleNumbers;
	this->index = index;
}

void Cell::lockNumber(int number)
{
	isLocked = true;
	possibleNumbers = { number };
}

bool Cell::removeNumber(int number)
{
	if (isLocked || (possibleNumbers.find(number) == possibleNumbers.end()))
		return false;
	possibleNumbers.erase(number);
	return true;
}

int Cell::getLockedNumber() {
	if (possibleNumberCount() == 0)
		return -1;
	if (!isLocked)
		return 0;

	return *possibleNumbers.begin();
}

inline int Cell::possibleNumberCount() { return possibleNumbers.size(); }
inline bool Cell::IsNumberPossible(int number) { return possibleNumbers.find(number) != possibleNumbers.end(); }

inline int Cell::getPossibleNumberAtIndex(int index) { return *std::next(possibleNumbers.begin(), index); }

inline std::set<int> Cell::getPossibleNumbers() { return possibleNumbers; }

Cell* solveSudoku(Cell* sudokuCells);
int checkSudokuStatus(Cell* sudokuCells);
void printSudoku(Cell* sudoku);
void updatePossibleCellNumbers(Cell* sudokuCells);
bool setLockableNumbers(Cell* sudokuCells, int cellIndex);
int countInvolvments(Cell* sudokuCells, int cellIndex, int number);

/// <summary>
/// MAX -> Choose cells with max possibilities
/// </summary>
std::vector<Cell> getCellsWithMaxPossibleNumbers(Cell* sudokuCells);

/// <summary>
/// DEGREE -> Choose max involvements
/// </summary>
Cell* getCellWithMaxInvolvments(Cell* sudokuCells, std::vector<Cell> possibleCells);

/// <summary>
/// MIN -> Choose number with min involvements
/// </summary>
int getMinInvolvedNumber(Cell* sudokuCells, Cell* cell);


int main()
{
	Cell sudokuCells[SUDOKU_SIZE];
	// 81 + 1 for 'ending null'
	char input[SUDOKU_SIZE + 1];
	std::cin >> input;

	// Convert chars to ints
	for (int i = 0; i < SUDOKU_SIZE; i++) {
		int number = input[i] - '0';
		if (number != 0) {
			sudokuCells[i].lockNumber(number);
		}
		sudokuCells[i].index = i;
	}
	updatePossibleCellNumbers(sudokuCells);
	Cell* solvedSudoku = solveSudoku(sudokuCells);
	if (solvedSudoku == nullptr) {
		std::cerr << "COULDN'T SOLVE SUDOKU!" << std::endl;
		std::exit(0);
	}

	std::cout << "done!" << std::endl;
	printSudoku(solvedSudoku);
	return 0;
}



Cell* solveSudoku(Cell* sudokuCells)
{
	int status = checkSudokuStatus(sudokuCells);
	if (status == 0)
		return nullptr;

	if (status == 2)
		return sudokuCells;

	while (true)
	{
		std::vector<Cell> cells = getCellsWithMaxPossibleNumbers(sudokuCells);
		Cell* cell = getCellWithMaxInvolvments(sudokuCells, cells);
		int number = getMinInvolvedNumber(sudokuCells, cell);

		// Clone sudoku
		Cell* sudokuClone = new Cell[SUDOKU_SIZE];
		for (int i = 0; i < SUDOKU_SIZE; i++)
			sudokuClone[i] = Cell(sudokuCells[i]);

		// Apply change and update sudoku
		sudokuClone[cell->index].lockNumber(number);
		updatePossibleCellNumbers(sudokuClone);

		// Go deeper
		Cell* result = solveSudoku(sudokuClone);
		// If reached dead end, remove last added number from possibilities
		if (result == nullptr) {
			sudokuCells[cell->index].removeNumber(number);
			updatePossibleCellNumbers(sudokuCells);
			// If depth has no possibilities, go one up
			if (checkSudokuStatus(sudokuCells) == 0)
				return nullptr;
		}
		else {
			// If finished, stop
			return result;
		}
	}
}

int checkSudokuStatus(Cell* sudokuCells)
{
	for (int i = 0; i < SUDOKU_SIZE; i++)
	{
		// Unsolvable
		if (sudokuCells[i].possibleNumberCount() == 0)
			return 0;
		// Not solved
		if (!sudokuCells[i].isLocked)
			return 1;
	}
	// Solved
	return 2;
}

void printSudoku(Cell* sudoku)
{
	int index = 0;
	//	Print region.
	for (int i = 0; i < 3; i++)
	{
		//	Print section.
		for (int j = 0; j < 3; j++)
		{
			// Print row.
			for (int k = 0; k < 3; k++)
			{
				// Print number.
				for (int l = 0; l < 3; l++)
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
	// As long as a possible number gets removed, repeat (new numbers could be excluded).
	bool hasRemoved = true;
	while (hasRemoved)
	{
		hasRemoved = false;

		for (int cellIndex = 0; cellIndex < SUDOKU_SIZE; cellIndex++)
		{
			Cell cell = sudokuCells[cellIndex];

			if (!cell.isLocked)
				continue;

			// CHECK ROWS & COLUMNS
			for (int i = 0; i < 9; i++)
			{
				// Add all cells in row.
				int rowIndex = (cellIndex / 9) * 9 + i;
				if (rowIndex != cellIndex) {
					if (sudokuCells[rowIndex].removeNumber(cell.getLockedNumber())) {
						hasRemoved = true;
					}
				}

				// Add all cells in column.
				int columnIndex = (cellIndex % 9) + (i * 9);
				if (columnIndex != cellIndex) {
					if (sudokuCells[columnIndex].removeNumber(cell.getLockedNumber())) {
						hasRemoved = true;
					}
				}
			}

			// CHECK SECTIONS
			// Get cell-region
			int regionIndex = cellIndex / 27;
			// Get cell-section 
			int sectionIndex = (cellIndex / 3) % 3 + regionIndex * 3;

			// Calculate start index of cell-section.
			int sectionStartIndex = regionIndex * 27 + ((sectionIndex % 3) * 3);
			// Go through columns in cell-section.
			for (int i = 0; i < 3; i++)
			{
				// Go through rows in cell-section.
				for (int j = 0; j < 3; j++)
				{
					int currentCellIndex = j * 9 + i + sectionStartIndex;
					if (currentCellIndex == cellIndex)
						continue;
					if (sudokuCells[currentCellIndex].removeNumber(cell.getLockedNumber())) {
						hasRemoved = true;
					}
				}
			}
		}
	}
	// Go thorugh all cells and check if any can be locked.
	for (int i = 0; i < SUDOKU_SIZE; i++)
	{
		if (sudokuCells[i].isLocked)
			continue;
		// If a number can be locked, update possibleNumbers
		if (setLockableNumbers(sudokuCells, i))
			updatePossibleCellNumbers(sudokuCells);
	}
}

bool setLockableNumbers(Cell* sudokuCells, int cellIndex)
{
	Cell* cell = &sudokuCells[cellIndex];
	// Check if one cell can be a number, that all other can't be
	bool numberLocked = false;
	for (int number : cell->getPossibleNumbers())
	{
		// CHECK ROWS & COLUMNS
		bool onlyNumberInRow = true;
		bool onlyNumberInColumn = true;
		for (int i = 0; i < 9; i++)
		{
			// Add all cells in row.
			int rowIndex = (cellIndex / 9) * 9 + i;
			if (rowIndex != cellIndex) {
				if (sudokuCells[rowIndex].IsNumberPossible(number)) {
					onlyNumberInRow = false;
				}
			}

			// Add all cells in column.
			int columnIndex = (cellIndex % 9) + (i * 9);
			if (columnIndex != cellIndex) {
				if (sudokuCells[columnIndex].IsNumberPossible(number)) {
					onlyNumberInColumn = false;
				}
			}
		}

		// CHECK SECTIONS
		bool onlyNumberInSection = true;
		int regionIndex = cellIndex / 27;
		int sectionIndex = (cellIndex / 3) % 3 + regionIndex * 3;
		int sectionStartIndex = regionIndex * 27 + ((sectionIndex % 3) * 3);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				int currentCellIndex = j * 9 + i + sectionStartIndex;
				if (currentCellIndex == cellIndex)
					continue;

				if (sudokuCells[currentCellIndex].IsNumberPossible(number))
					onlyNumberInSection = false;
			}
		}
		if (onlyNumberInRow || onlyNumberInColumn || onlyNumberInSection) {
			cell->lockNumber(number);
			numberLocked = true;
		}
	}
	return numberLocked;
}

int countInvolvments(Cell* sudokuCells, int cellIndex, int number)
{
	int involvements = 0;
	// CHECK SECTIONS

	// Get cell-region
	int regionIndex = cellIndex / 27;
	// Get cell-section 
	int sectionIndex = (cellIndex / 3) % 3 + regionIndex * 3;

	// Calculate start index of cell-section.
	int sectionStartIndex = regionIndex * 27 + ((sectionIndex % 3) * 3);

	// Go through columns in cell-section.
	for (int i = 0; i < 3; i++)
	{
		// Go through rows in cell-section.
		for (int j = 0; j < 3; j++)
		{
			int currentCellIndex = j * 9 + i + sectionStartIndex;
			if (currentCellIndex == cellIndex)
				continue;
			// If cell could be given number, increase involvements.
			if (sudokuCells[currentCellIndex].IsNumberPossible(number))
				involvements++;
		}
	}

	// CHECK ROWS & COLUMNS
	for (int i = 0; i < 8; i++)
	{
		int rowIndex = (cellIndex / 9) * 9 + i;
		if (rowIndex == cellIndex)
			continue;
		// If cell could be given number, increase involvements.
		if (sudokuCells[rowIndex].IsNumberPossible(number))
			involvements++;

		int columnIndex = (cellIndex % 9) + (i * 9);
		if (columnIndex != cellIndex)
			continue;
		// If cell could be given number, increase involvements.
		if (sudokuCells[columnIndex].IsNumberPossible(number))
			involvements++;
	}
	return involvements;
}

std::vector<Cell> getCellsWithMaxPossibleNumbers(Cell* sudokuCells)
{
	std::vector<Cell> maxCells{};
	int numberOfPossibleNumbers = 0;
	for (int i = 0; i < SUDOKU_SIZE; i++)
	{
		Cell cell = sudokuCells[i];
		int cellNumberCount = cell.possibleNumberCount();
		// If same amount, add.
		if (cellNumberCount == numberOfPossibleNumbers)
			maxCells.push_back(cell);

		// If more, reset list and add.
		if (cellNumberCount > numberOfPossibleNumbers) {
			numberOfPossibleNumbers = cellNumberCount;
			maxCells.clear();
			maxCells.push_back(cell);
		}
	}
	return maxCells;
}

Cell* getCellWithMaxInvolvments(Cell* sudokuCells, std::vector<Cell> possibleCells)
{
	Cell* maxInvolvementCell = nullptr;
	int maxInvolvement = 0;
	int possibleCellsCount = possibleCells.size();
	for (int i = 0; i < possibleCellsCount; i++)
	{
		Cell* currentCell = &sudokuCells[possibleCells[i].index];
		int involvement = 0;
		for (int j = 0; j < currentCell->possibleNumberCount(); j++)
		{
			// Calculate involvement for each possible number
			involvement += countInvolvments(sudokuCells, currentCell->index, currentCell->getPossibleNumberAtIndex(j));
		}
		if (involvement > maxInvolvement) {
			maxInvolvementCell = currentCell;
			maxInvolvement = involvement;
		}
	}
	return maxInvolvementCell;
}

int getMinInvolvedNumber(Cell* sudokuCells, Cell* cell)
{
	int minInvolvedNumber = 0;
	int minInvolvement = 25;
	for (int i = 0; i < cell->possibleNumberCount(); i++)
	{
		int currentNumber = cell->getPossibleNumberAtIndex(i);
		int involvement = countInvolvments(sudokuCells, cell->index, currentNumber);
		if (involvement < minInvolvement) {
			minInvolvedNumber = currentNumber;
			minInvolvement = involvement;
		}
	}
	return minInvolvedNumber;
}