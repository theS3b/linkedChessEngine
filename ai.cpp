#include "ai.h"
#include <algorithm>
#include <vector>
#include <omp.h>


#include <iostream>
using namespace std;

Board select_best_move(Board& state, const unsigned int& depth, const bool& maxPlayer, bool & haswon) {
	haswon = false;
	posTable pt = posTable();
	vector<Board> children = state.get_bitboard_children_cpp();
	double best_value = MAX;
	Board best_move;

	unsigned size = (int)children.size();

	if (size == 0) {
		cout << "Le joueur " << state.get_result(WHITE, true) << " a gagne !" << endl;
		haswon = true;
		return state;
	}

	#pragma omp parallel for
	for (int i = 0; i < size; i++) {
		double value = alphabeta(children[i], depth -1, !maxPlayer, -10000, 10000, pt, depth);

		if (maxPlayer) {
			if (value >= best_value) {
				best_move = children[i];
				best_value = value;
			}
		}
		else {
			if (value <= best_value) {
				best_move = children[i];
				best_value = value;
			}
		}
	}
	return best_move;
}

double alphabeta(Board& state, const unsigned int& depth, const bool& maxPlayer, double a, double b, const posTable& postab, const unsigned & init_depth) {
	double value = 0;
	std::vector<Board> children = state.get_bitboard_children_cpp();
	if (depth == 0 || children.size() == 0) {
		return heuristic_evaluation(state, !(bool)children.size(), postab, depth, init_depth);
	}
	if (maxPlayer) {
		value = MIN;
		for (unsigned i = 0; i < children.size(); i++) {
			value = std::max(value, alphabeta(children[i], depth - 1, false, a, b, postab, init_depth));
			a = std::max(value, a);
			if (b <= a)
				break;
		}
		return value;
	}
	else {
		value = MAX;
		for (unsigned i = 0; i < children.size(); i++) {
			value = std::min(value, alphabeta(children[i], depth - 1, true, a, b, postab, init_depth));
			b = std::min(b, value);
			if (b <= a)
				break;
		}
		return value;
	}

	return value;
}

double heuristic_evaluation(Board& s, const bool& leaf, const posTable& postab, const unsigned & depth, const unsigned& init_depth)
{
	// material evaluation
	double value = (double)(10 * (double)((signed)POPCNT(s.bit_board[wpawn]) - (signed)POPCNT(s.bit_board[bpawn])) );
	value += (double)30 * ((signed)POPCNT(s.bit_board[wknight]) - (signed)POPCNT(s.bit_board[bknight]));
	value += (double)30 * ((signed)POPCNT(s.bit_board[wbishop]) - (signed)POPCNT(s.bit_board[bbishop]));
	value += (double)50 * ((signed)POPCNT(s.bit_board[wrook]) - (signed)POPCNT(s.bit_board[brook]));
	value += (double)90 * ((signed)POPCNT(s.bit_board[wqueen]) - (signed)POPCNT(s.bit_board[bqueen]));
	value += (double)900 * s.get_result(WHITE, leaf) * ((int)depth + 1); // if there's checkmate

	if (POPCNT(get_all_pieces(s, s.player)) <= 5) {

		vector<double> wkingEndGame = {
				-0.5, -0.5, 0.0, -0.5, -0.5, 0.0, -0.5, -0.5,
				-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.0,
				0.0, 0.0, 0.5, 1.5, 1.5, 0.5, 0.0, -0.0,
				0.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -0.0,
				-0.5, 0.0, 0.0, 0.5, 0.5, 0.5, 0.0, -0.5,
				-0.5, -0.5, -0.5, -0.5, -0.5, 0.0, -0.5, -0.5
		};
		vector<double> bkingEndGame = {
				-0.5, -0.5, -0.5, -0.5, -0.5, 0.0, -0.5, -0.5,
				- 0.5, 0.0, 0.0, 0.5, 0.5, 0.5, 0.0, -0.5,
				0.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -0.0,
				0.0, 0.0, 0.5, 1.5, 1.5, 0.5, 0.0, -0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.0,
				0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.0,
				-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
				-0.5, -0.5, 0.0, -0.5, -0.5, 0.0, -0.5, -0.5
		};

		uint64_t splitted[MAX_PIECES_OF_ONE_TYPE];
		unsigned n = split_bits(splitted, s.bit_board[wking]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += 2 * wkingEndGame[pos];
		}

		n = split_bits(splitted, s.bit_board[bking]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= 2 * bkingEndGame[pos];
		}

		value -= 3 * (int)(init_depth - depth);  // we want to checkmate asap

	}
	else {
		// position evaluation
		uint64_t splitted[MAX_PIECES_OF_ONE_TYPE];
		unsigned n = split_bits(splitted, s.bit_board[wpawn]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.pawnPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[bpawn]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.pawnPosB[pos];
		}

		n = split_bits(splitted, s.bit_board[wknight]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.knightPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[bknight]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.knightPosB[pos];
		}

		n = split_bits(splitted, s.bit_board[wbishop]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.bishopPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[bbishop]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.bishopPosB[pos];
		}

		n = split_bits(splitted, s.bit_board[wrook]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.rookPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[brook]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.rookPosB[pos];
		}

		n = split_bits(splitted, s.bit_board[wqueen]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.queenPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[bqueen]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.queenPosB[pos];
		}
		n = split_bits(splitted, s.bit_board[wking]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value += postab.kingPosW[pos];
		}

		n = split_bits(splitted, s.bit_board[bking]);
		for (unsigned i = 0; i < n; i++) {
			unsigned pos = CLZ(splitted[i]);
			value -= postab.kingPosB[pos];
		}
	}
	return value;
}



posTable::posTable(void)
{
	pawnPosW = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ,
		5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
		1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0,
		0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5,
		0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0,
		0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5,
		0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};
	pawnPosB = {
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.5, 1.0, 1.0, -2.0, -2.0, 1.0, 1.0, 0.5,
		0.5, -0.5, -1.0, 0.0, 0.0, -1.0, -0.5, 0.5,
		0.0, 0.0, 0.0, 2.0, 2.0, 0.0, 0.0, 0.0,
		0.5, 0.5, 1.0, 2.5, 2.5, 1.0, 0.5, 0.5,
		1.0, 1.0, 2.0, 3.0, 3.0, 2.0, 1.0, 1.0,
		5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0,
		0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

	knightPosW = {
		-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
		-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0,
		-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0,
		-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0,
		-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0,
		-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0,
		-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0,
		-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0
	};
	knightPosB = {
		-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0,
		-4.0, -2.0, 0.0, 0.5, 0.5, 0.0, -2.0, -4.0,
		-3.0, 0.5, 1.0, 1.5, 1.5, 1.0, 0.5, -3.0,
		-3.0, 0.0, 1.5, 2.0, 2.0, 1.5, 0.0, -3.0,
		-3.0, 0.5, 1.5, 2.0, 2.0, 1.5, 0.5, -3.0,
		-3.0, 0.0, 1.0, 1.5, 1.5, 1.0, 0.0, -3.0,
		-4.0, -2.0, 0.0, 0.0, 0.0, 0.0, -2.0, -4.0,
		-5.0, -4.0, -3.0, -3.0, -3.0, -3.0, -4.0, -5.0
	};

	bishopPosW = {
		 -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
		-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0,
		-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0,
		-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0,
		-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
		-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0,
		-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0
	};
	bishopPosB = {
		-2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0,
		-1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, -1.0,
		-1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, -1.0,
		-1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0,
		-1.0, 0.5, 0.5, 1.0, 1.0, 0.5, 0.5, -1.0,
		-1.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.0, -1.0,
		-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		 -2.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, -2.0
	};

	rookPosW = {
		 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
		0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0
	};
	rookPosB = {
		0.0, 0.0, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		-0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.5,
		0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5,
		 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
	};

	queenPosW = {
		 -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
		-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
		-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
		0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
		-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
		-1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, -1.0,
		-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0
	};
	queenPosB = {
		-2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0,
		-1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, -1.0,
		-1.0, 0.5, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
		0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
		-0.5, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -0.5,
		-1.0, 0.0, 0.5, 0.5, 0.5, 0.5, 0.0, -1.0,
		-1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0,
		 -2.0, -1.0, -1.0, -0.5, -0.5, -1.0, -1.0, -2.0
	};

	kingPosW = {
		 -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
		-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
		2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0,
		2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0
	};
	kingPosB = {
		2.0, 3.0, 1.0, 0.0, 0.0, 1.0, 3.0, 2.0,
		2.0, 2.0, 0.0, 0.0, 0.0, 0.0, 2.0, 2.0,
		-1.0, -2.0, -2.0, -2.0, -2.0, -2.0, -2.0, -1.0,
		-2.0, -3.0, -3.0, -4.0, -4.0, -3.0, -3.0, -2.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		-3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0,
		 -3.0, -4.0, -4.0, -5.0, -5.0, -4.0, -4.0, -3.0
	};

}
