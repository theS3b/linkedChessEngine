#pragma once
#include <array>
#include <vector>
#include <string>
#include <stdint.h>
#include "utils.h"

struct Board {
	std::array<uint64_t, 12> bit_board; 	// 12 bitboards representing each one the positions of a certain type of pieces
	short player; 							// WHITE / BLACK, who is the current player

	Board();								// set state to initial state
	std::vector<Board> get_bitboard_children_cpp();	// same as get_bitboard_children but returns a vector
	short get_result(const short _player, const bool & leaf);			// returns +1/-1/0 in function of the player
	std::string string_board();		
	void print();					// easily display a board

	std::array<bool, 2> check_type; 					// WHITE / BLACK, boolean to speed up the program : true for a color if the king has moved or the 2 rooks a had moved
	std::array<uint64_t, 2> last_pawn_pos; 				// used for the "coup en passant", 0 if the last move wasn't a pawn move and the last bitboard in the other case
	std::array<std::array<bool, 2>, 2> king_rook_moved; // WHITE / BLACK, LEFT / RIGHT
	unsigned counter;									// to count if nothing happend the last 50 moves (then it'd be null)
	std::array<std::array<uint64_t, 5>, 2> last_moves;						// to count how many same state there was in a row (needs to be less than 3)
};

bool move_piece(Board & b, const std::string& pos);
std::string transform_chessboard_to_move(const Board& from, const Board& to, const short & player);