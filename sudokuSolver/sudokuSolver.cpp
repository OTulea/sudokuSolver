#include <iostream>
#include <fstream>
#include <queue>
#include <thread>
#include <vector>
#include <string>
#include <cmath>
#include <assert.h>
#include "square.h"
#include "adminFunctions.h"
#include "threadPool.h"
#include "inputBuffer.h"

/* propagate sets the finalVal of the square at (row,col) to val,
sets square.updated to false, and removes val from possibleVals
of local row, column and region.
It can ONLY be called on squares that have no finalVal (=0).
It calls NO OTHER FUNCTIONS*/
void propagate(square grid[][9], unsigned int row, unsigned int col, unsigned int val) {
	assert(grid[row][col].finalVal == 0);
	assert(val != 0);
	// set target square values.
	grid[row][col].updated = false;
	grid[row][col].finalVal = val;
	for (auto elem : grid[row][col].possibleVals)
		elem = 0;
	grid[row][col].possibleVals[val - 1] = val; // val will never equal zero so index error is fine
	// remove val from possibleVals of local region.
	for (unsigned int i = 0; i < 3; ++i) {
		for (unsigned int j = 0; j < 3; ++j) {
			unsigned int nuRow = 3 * (row / 3) + i;
			unsigned int nuCol = 3 * (col / 3) + j;
			if (grid[nuRow][nuCol].finalVal == 0 && grid[nuRow][nuCol].possibleVals[val - 1] != 0) { // if the square is set to a finalVal or is already set to zero for that possibleval, don't update
				grid[nuRow][nuCol].possibleVals[val - 1] = 0;
				grid[nuRow][nuCol].updated = true;
			}
		}
	}
	// remove val from possibleVals of row and column.
	for (unsigned int i = 0; i < 9; ++i) {
		if (grid[i][col].finalVal == 0 && grid[i][col].possibleVals[val - 1] != 0) {
			grid[i][col].possibleVals[val - 1] = 0;
			grid[i][col].updated = true;
		}
		if (grid[row][i].finalVal == 0 && grid[row][i].possibleVals[val - 1] != 0) {
			grid[row][i].possibleVals[val - 1] = 0;
			grid[row][i].updated = true;
		}
	}
}

/* possibleValsCheck checks the possibleVals of a square and calls propagate
if only one value remains, and outputs an error if zero remain.
possibleValsCheck can ONLY be called on updated squares with finalVal = 0.*/
void possibleValsCheck(square grid[][9], unsigned int row, unsigned int col) {
	assert(grid[row][col].finalVal == 0);
	auto valStorage = 0, numofVals = 0;
	for (auto elem : grid[row][col].possibleVals) { // RANGE BASED FOR LOOP
		if (elem != 0) {
			valStorage = elem;
			++numofVals;
		}
	}
	if (numofVals == 0) {
		std::cout << col << "," << row << "has no possible value \n";
		printgrid(grid);
		printgrid(grid, 1);
		exit(EXIT_FAILURE);
	}
	else if (numofVals == 1) {
		propagate(grid, row, col, valStorage);
	}
}

//scanForUpdates calls possibleValsCheck on every updated square until none are left
void scanForUpdates(square grid[][9]) {
	int i = 0;
	do {
		if (grid[i / 9][i % 9].updated == true) {
			grid[i / 9][i % 9].updated = false;
			possibleValsCheck(grid, i / 9, i % 9);
			i = 0;
		}
		else
			++i;
	} while (i < 81);
}

void rowHiddenSingle(square grid[][9]) {
	for (auto i = 0; i < 9; ++i) {
		unsigned int rowStorage[9]{ 0 };
		for (auto j = 0; j < 9; ++j) {
			for (auto k = 0; k < 9; ++k) {
				if (grid[i][j].possibleVals[k] != 0 && grid[i][j].finalVal == 0)
					++rowStorage[k];
			}
		}
		for (auto j = 0; j < 9; ++j) {
			if (rowStorage[j] == 1) {
				for (auto k = 0; k < 9; ++k) {
					if (grid[i][k].possibleVals[j] != 0 && grid[i][k].finalVal == 0) // finalval check is since they could call propagate on each others squares
						propagate(grid, i, k, j + 1);
				}
			}
		}
	}
}

void colHiddenSingle(square grid[][9]) {
	for (auto i = 0; i < 9; ++i) {
		unsigned int colStorage[9]{ 0 };
		for (auto j = 0; j < 9; ++j) {
			for (auto k = 0; k < 9; ++k) {
				if (grid[j][i].possibleVals[k] != 0 && grid[j][i].finalVal == 0)
					++colStorage[k];
			}
		}
		for (auto j = 0; j < 9; ++j) {
			if (colStorage[j] == 1) {
				for (auto k = 0; k < 9; ++k) {
					if (grid[k][i].possibleVals[j] != 0 && grid[k][i].finalVal == 0)
						propagate(grid, k, i, j + 1);
				}
			}
		}
	}
}

void BlockHiddenSingle(square grid[][9]) {
	for (unsigned int yMultiplier = 0; yMultiplier < 3; ++yMultiplier) { // set it to just go up by 3 lul, instead of multiplying by 3 everywhere
		for (unsigned int xMultiplier = 0; xMultiplier < 3; ++xMultiplier) {
			unsigned int blockstorage[9]{ 0 };
			for (unsigned int i = 0; i < 3; ++i) {
				for (unsigned int j = 0; j < 3; ++j) {
					for (unsigned int k = 0; k < 9; ++k) {
						if (grid[yMultiplier * 3 + i][xMultiplier * 3 + j].possibleVals[k] != 0 && grid[yMultiplier * 3 + i][xMultiplier * 3 + j].finalVal == 0)
							++blockstorage[k];
					}
				}
			}
			for (unsigned int k = 0; k < 9; ++k) {
				if (blockstorage[k] == 1) {
					for (unsigned int i = 0; i < 3; ++i) {
						for (unsigned int j = 0; j < 3; ++j) {
							if (grid[yMultiplier * 3 + i][xMultiplier * 3 + j].possibleVals[k] != 0 && grid[yMultiplier * 3 + i][xMultiplier * 3 + j].finalVal == 0)
								propagate(grid, yMultiplier * 3 + i, xMultiplier * 3 + j, k + 1);
						}
					}
				}

			}
		}
	}
}

void SingleBoxRow(square grid[][9]) { // THIS DOESNT TEST IT OUTPUTS LOL
	for (unsigned int yMultiplier = 0; yMultiplier < 3; ++yMultiplier) {
		for (unsigned int xMultiplier = 0; xMultiplier < 3; ++xMultiplier) { // this is code for a single block;
			for (unsigned int i = 0; i < 3; ++i) { // FOR EACH ROW SEGMENT IN THE BLOCK
				unsigned int rowSegmentStorage[9]{ 0 };
				for (unsigned int j = 0; j < 3; ++j) { //GO THROUGH THE ROW (iterate through the cols)
					for (unsigned int k = 0; k < 9; ++k) { // go through the possiblevals
						if (grid[yMultiplier * 3 + i][xMultiplier * 3 + j].possibleVals[k] != 0 && grid[yMultiplier * 3 + i][xMultiplier * 3 + j].finalVal == 0)
							++rowSegmentStorage[k];
					}
				} // AFTER ROW FINISHES CHECK if ANY OCCUR 2 or 3 times
				for (unsigned int j = 0; j < 9; ++j) {
					if (rowSegmentStorage[j] > 1) { // values can only be 0 1 2 3, since only 3 squares gone through.
						for (unsigned int k = 0; k < 9; ++k) { // go through the rest of the row
							if (grid[yMultiplier * 3 + i][k].possibleVals[j] != 0 && (k < (xMultiplier * 3) || k >(xMultiplier * 3 + 2))) { // make sure not to check the actual row segment in question
								rowSegmentStorage[j] = 0;
								k = 9;
							}
						}
						if (rowSegmentStorage[j]) {// IF the value doesn't appear outside the segment of the row in the block, remove the value from the rest of the block, excluding row offset i;
							for (unsigned int k = 0; k < 3; ++k) {
								if (k != i) {
									for (unsigned int m = 0; m < 3; ++m)
										grid[yMultiplier * 3 + k][xMultiplier * 3 + m].possibleVals[j] = 0;

								}
							}
						}

					}
				}
			}
		}
	}
}

unsigned int counter = 0;
std::string sol;

bool attemptSolution(std::string input) {
	square grid[9][9]; // c++ arrays decay to pointers when passed to functions
	for (unsigned int i = 0; i < 81; ++i) {
		if (convertInput(input[i]) != 0)
			propagate(grid, i / 9, i % 9, convertInput(input[i]));
	}
	// make an array of functions going up in complexity, then iterate through them if the function remains unsolved, until you run out of functions;
	scanForUpdates(grid);
	while (hasRowHiddenSingle(grid) || hasColHiddenSingle(grid) || hasBlockHiddenSingle(grid)){// am i really smar by doing it in reverse order? so short circuit makes it more efficient
		BlockHiddenSingle(grid);
		colHiddenSingle(grid);
		rowHiddenSingle(grid);
		scanForUpdates(grid);
	}
	scanForUpdates(grid);
	if (logicallySolved(grid)) {
		std::cout << "solved!\n";
		return true;
	}
	else {
		printgrid(grid, 1);
		printgrid(grid);
		return false;
	}
}

int main() { // 8773934 solved; // 8795011 solved with scanforupdates in hiddensingle loop
	//std::string in = "000000000000468000063907480034600810010000090097004320046309170000145000000000000";
	//std::string sol = "478231659925468731163957482534692817612873594897514326246389175789145263351726948";
	//attemptSolution(in);

	//auto threadCount = std::thread::hardware_concurrency();
	//threadPool pool{threadCount};

	std::ifstream inFile;
	inFile.open("C:\\Users\\Amadeus\\Desktop\\sudoku2.csv");
	if (!inFile) {
		std::cerr << "Unable to open file\n";
		exit(1);
	}
	std::string input;
	while (getline(inFile, input, ',')) {
		inFile.ignore(82);
		if (attemptSolution(input))
			++counter;
	}
	std::cout << counter << " solved!\n";
	/*inputBuffer::inputBuffer();
	while (!inFile.eof()) {
		for (unsigned int i = 0; i < threadCount; ++i) {
			getline(inFile, input, ',');
			inFile.ignore(82);
			inputBuffer::push(input);
		}
		for (unsigned int i = 0; i < threadCount; ++i) {
			pool.enqueue([=] {
				attemptSolution(buffer.front());
				});
			buffer.pop();
		}
	}*/
	inFile.close();
}