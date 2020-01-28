#pragma once
#include "chess_moves.h"
#include <vector>
#define MIN -9999
#define MAX 9999

class posTable {
public:
	posTable();
	std::vector<double> pawnPosW;
	std::vector<double> pawnPosB;
	std::vector<double> knightPosW;
	std::vector<double> knightPosB;
	std::vector<double> bishopPosW;
	std::vector<double> bishopPosB;
	std::vector<double> rookPosW;
	std::vector<double> rookPosB;
	std::vector<double> queenPosW;
	std::vector<double> queenPosB;
	std::vector<double> kingPosW;
	std::vector<double> kingPosB;
};

Board select_best_move(Board& state, const unsigned int& depth, const bool& maxPlayer, bool & haswon);
double alphabeta(Board& state, const unsigned int& depth, const bool& maxPlayer, double a, double b, const posTable& postab, const unsigned& init_depth);
double heuristic_evaluation(Board& s, const bool& leaf, const posTable& postab, const unsigned& depth, const unsigned& init_depth);