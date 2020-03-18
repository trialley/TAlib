#ifndef TA_EXAMPLES_SUDOKU_SUDOKU_H
#define TA_EXAMPLES_SUDOKU_SUDOKU_H


#include "TA/base/Types.h"
#include "TA/base/StringPiece.h"

TA::string solveSudoku(const TA::StringPiece& puzzle);
const int kCells = 81;
extern const char kNoSolution[];

#endif  // TA_EXAMPLES_SUDOKU_SUDOKU_H
