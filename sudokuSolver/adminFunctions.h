#pragma once
#include <string>
#include <iostream>
#include "square.h"

bool logicallySolved(square grid[][9]) {
	/* create zero-initialized arrays rowcheck and colcheck of legnth 9 for each row and column.
	row/colcheck[n] is incremented by 1 when the value n + 1 is found in a possibleVals grid.
	Loop through all possibleVals of a row and column, and if by the end col/rowcheck contains
	a zero, there are duplicate numbers in the row/column and thus the sudoku is improperly solved. */
	for (unsigned int i = 0; i < 9; ++i) {
		unsigned int rowcheck[9]{ 0 };
		unsigned int colcheck[9]{ 0 };
		for (unsigned int j = 0; j < 9; ++j) {
			if (grid[i][j].finalVal != 0)
				++rowcheck[grid[i][j].finalVal - 1];
			else
				return false;
			if (grid[j][i].finalVal != 0)
				++colcheck[grid[i][j].finalVal - 1];
			else
				return false;
		}
		for (unsigned int k = 0; k < 9; ++k) {
			if (rowcheck[k] == 0 || colcheck[k] == 0)
				return false;
		}
	}
	// same approach for regions.
	for (unsigned int yMultiplier = 0; yMultiplier < 3; ++yMultiplier) {
		for (unsigned int xMultiplier = 0; xMultiplier < 3; ++xMultiplier) {
			unsigned int blockcheck[9]{ 0 };
			for (unsigned int i = 0; i < 3; ++i) {
				for (unsigned int j = 0; j < 3; ++j) {
					if (grid[yMultiplier * 3 + i][xMultiplier * 3 + j].finalVal != 0)
						++blockcheck[grid[yMultiplier * 3 + i][xMultiplier * 3 + j].finalVal - 1];
					else
						return false;
				}
			}
			for (auto elem : blockcheck) {
				if (!elem) // should only return when elem = 0;
					return false;
			}
		}
	}
	return true;
} // YOU KNOW I MIGHT NOT EVEN NEED TO PROPERLY CHECK, IF I JUST CHECK FOR ZEROES IT SHOULD BE FINE, CUZ MY RULES ARE SOLID

unsigned int convertInput(char input) {
	return input - 48;
}

void printgrid(square grid[][9], int type = 0) {
	if (type == 0) {
		for (unsigned int i = 0; i < 9; ++i) {
			for (unsigned int j = 0; j < 9; ++j)
				std::cout << grid[i][j].finalVal << " ";
			std::cout << "\n";
		}
		std::cout << "\n";
	}
	else {
		for (unsigned int i = 0; i < 9; ++i) {
			for (unsigned int j = 0; j < 9; ++j) {
				if (grid[i][j].finalVal == 0) {
					for (unsigned int k = 0; k < 9; ++k) {
						if (grid[i][j].possibleVals[k] != 0)
							std::cout << grid[i][j].possibleVals[k];
						else
							std::cout << " ";
					}
				}
				else {
					std::cout << "         ";
				}
				std::cout << "|";
			}
			std::cout << "\n";
			std::cout << "__________________________________________________________________________________________\n";
		}
		std::cout << "\n";
	}
}

bool hasRowHiddenSingle(square grid[][9]) {
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
						return true;
				}
			}
		}
	}
	return false;
}

bool hasColHiddenSingle(square grid[][9]) {
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
						return true;
				}
			}
		}
	}
	return false;
}

bool hasBlockHiddenSingle(square grid[][9]) {
	for (unsigned int yMultiplier = 0; yMultiplier < 3; ++yMultiplier) {
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
								return true;
						}
					}
				}

			}
		}
	}
	return false;
}

bool hasSingleBoxRow(square grid[][9]) {
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
							if (grid[yMultiplier * 3 + i][k].possibleVals[j] != 0 && (k < (xMultiplier * 3) || k > (xMultiplier * 3 + 2))) { // make sure not to check the actual row segment in question
								rowSegmentStorage[j] = 0;
								k = 9;
							}
						}
						if (rowSegmentStorage[j])// IF the value doesn't appear outside the segment of the row in the block, remove the value from the rest of the block, excluding row offset i;
							std::cout << "has singleboxrow\n";
							return true;
					}
				}
			}
		}
	}
	return false;
}

/*bool comparativelySolved(square grid[][9], std::string solutionstring) {
	for (auto i = 0; i < 81; ++i) {
		//std::cout << "cheking comparatively, index " << i << ", row " << i / 9 << ", col " << i % 9 << ", finalVal " << grid[i / 9][i % 9].finalVal << ", solutionVal " << convertInput(solutionstring[i]) << "\n";
		if (grid[i / 9][i % 9].finalVal != convertInput(solutionstring[i]))
			return false;
	}
	return true;
}*/ // NO LONGER NEEDED, CHECK WHETHER SOLVED MANUALLY

/*bool Updatable(square grid[][9]) {
	for (auto i = 0; i < 81; ++i)
		if (grid[i / 9][i % 9].updated == true)
			return true;
	return false;
}*/ // DONT THINK I NEED THIS ANYMORE, since scanupdates function keeps going until grid not updatable