#include "chess_attacked.h"

unsigned short get_type_from_pos(const Board b, const unsigned short pos) {
	for (unsigned short i = wpawn; i <= bking; i++)
		if (b.bit_board[i] & ((uint64_t)1 << pos)) return i;
	return notype;
}

uint64_t get_attacked_by_knights(const Board b, const Color c)
{
	uint64_t knights_pos = get_knights(b, c);
	uint64_t left = knights_pos & ~(bit_columnH << 7);
	uint64_t Left = knights_pos & (~(bit_columnH << 7) & ~(bit_columnH << 6));
	uint64_t right = knights_pos & ~bit_columnH;
	uint64_t Right = knights_pos & (~bit_columnH & ~(bit_columnH << 1));
	uint64_t up = knights_pos & ~((uint64_t)bit_line << 56);
	uint64_t Up = knights_pos & (~bit_columnH & ~((uint64_t)bit_line << 48));
	uint64_t down = knights_pos & ~bit_line;
	uint64_t Down = knights_pos & (~bit_line & ~(bit_line << 8));
	return ((up & Right) << 6) | ((Up & right) << 15) | ((up & Left) << 10) | ((Up & left) << 17) |
		((down & Right) >> 10) | ((Down & right) >> 17) | ((Left & down) >> 6) | ((Down & left) >> 15);
}

uint64_t get_attacked_by_bishops(const Board b, const Color c, const uint64_t all_pieces) {
	uint64_t bishops[MAX_PIECES_OF_ONE_TYPE];
	uint64_t attacked = 0;
	const unsigned short nbr = split_bits(bishops, get_bishops(b, c));

	for (unsigned short i = 0; i < nbr; i++) {
		unsigned short pos = LOG2(bishops[i]);
		// NORTH EST
		uint64_t tmp = attacked_by_bishops_NE[pos];
		attacked |= tmp ^ attacked_by_bishops_NE[CTZ_ARR(tmp & all_pieces)];
		// NORTH WEST
		tmp = attacked_by_bishops_NW[pos];
		attacked |= tmp ^ attacked_by_bishops_NW[CTZ_ARR(tmp & all_pieces)];
		// SOUTH EST
		tmp = attacked_by_bishops_SE[pos];
		attacked |= tmp ^ attacked_by_bishops_SE[LOG2(tmp & all_pieces)];
		// SOUTH WEST
		tmp = attacked_by_bishops_SW[pos];
		attacked |= tmp ^ attacked_by_bishops_SW[LOG2(tmp & all_pieces)];
	}
	return attacked;
}

uint64_t get_attacked_by_rooks(const Board b, const Color c, const uint64_t all_pieces)
{
	uint64_t rooks[MAX_PIECES_OF_ONE_TYPE];
	uint64_t attacked = 0;
	unsigned short nbr = split_bits(rooks, get_rooks(b, c));
	for (unsigned short i = 0; i < nbr; i++) {
		unsigned int pos = LOG2(rooks[i]);
		// NORTH
		uint64_t tmp = attacked_by_rooks_N[pos];
		attacked |= tmp ^ attacked_by_rooks_N[CTZ_ARR(tmp & all_pieces)];
		// WEST
		tmp = attacked_by_rooks_W[pos];
		attacked |= tmp ^ attacked_by_rooks_W[CTZ_ARR(tmp & all_pieces)];
		// EST
		tmp = attacked_by_rooks_E[pos];
		attacked |= tmp ^ attacked_by_rooks_E[LOG2(tmp & all_pieces)];
		// SOUTH
		tmp = attacked_by_rooks_S[pos];
		attacked |= tmp ^ attacked_by_rooks_S[LOG2(tmp & all_pieces)];
	}
	return attacked;
}

uint64_t get_attacked_by_queens(const Board b, const Color c, const uint64_t all_pieces) {
	uint64_t queens[MAX_PIECES_OF_ONE_TYPE];
	uint64_t attacked = 0;
	unsigned short nbr = split_bits(queens, get_queens(b, c));
	for (unsigned short i = 0; i < nbr; i++) {
		unsigned int pos = LOG2(queens[i]);
		// NORTH
		uint64_t tmp = attacked_by_rooks_N[pos];
		attacked |= tmp ^ attacked_by_rooks_N[CTZ_ARR(tmp & all_pieces)];
		// WEST
		tmp = attacked_by_rooks_W[pos];
		attacked |= tmp ^ attacked_by_rooks_W[CTZ_ARR(tmp & all_pieces)];
		// EST
		tmp = attacked_by_rooks_E[pos];
		attacked |= tmp ^ attacked_by_rooks_E[LOG2(tmp & all_pieces)];
		// SOUTH
		tmp = attacked_by_rooks_S[pos];
		attacked |= tmp ^ attacked_by_rooks_S[LOG2(tmp & all_pieces)];
		// NORTH EST
		tmp = attacked_by_bishops_NE[pos];
		attacked |= tmp ^ attacked_by_bishops_NE[CTZ_ARR(tmp & all_pieces)];
		// NORTH WEST
		tmp = attacked_by_bishops_NW[pos];
		attacked |= tmp ^ attacked_by_bishops_NW[CTZ_ARR(tmp & all_pieces)];
		// SOUTH EST
		tmp = attacked_by_bishops_SE[pos];
		attacked |= tmp ^ attacked_by_bishops_SE[LOG2(tmp & all_pieces)];
		// SOUTH WEST
		tmp = attacked_by_bishops_SW[pos];
		attacked |= tmp ^ attacked_by_bishops_SW[LOG2(tmp & all_pieces)];
	}
	return attacked;
}

uint64_t get_attacked_by_king(const Board b, const Color c) {
	const uint64_t king = get_king(b, c);
	const uint64_t north = king & ~((uint64_t)bit_line << 56);
	const uint64_t south = king & ~bit_line;
	const uint64_t est = king & ~bit_columnH;
	const uint64_t west = king & ~(bit_columnH << 7);
	return (north << 8) | (south >> 8) | (est >> 1) | (west << 1) | ((north & est) << 7) | ((north & west) << 9) | ((south & est) >> 9) | ((south & west) >> 7);
}