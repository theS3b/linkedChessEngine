#pragma once
#include "consts.h"
#include "board.h"

inline uint64_t get_pawns(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wpawn];	default: return b.bit_board[bpawn]; }	return 0; }
inline uint64_t get_rooks(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wrook];	default: return b.bit_board[brook]; }	return 0; }
inline uint64_t get_bishops(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wbishop];	default: return b.bit_board[bbishop]; }	return 0; }
inline uint64_t get_knights(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wknight];	default: return b.bit_board[bknight]; }	return 0; }
inline uint64_t get_queens(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wqueen];	default: return b.bit_board[bqueen]; }	return 0; }
inline uint64_t get_king(const Board b, const Color c) { switch (c) { case WHITE: return b.bit_board[wking];	default: return b.bit_board[bking]; }	return 0; }

inline uint64_t get_all_pieces(const Board b, const Color c) {
	switch (c) {
	case WHITE:
		return b.bit_board[wpawn] | b.bit_board[wrook] | b.bit_board[wbishop] | b.bit_board[wknight] | b.bit_board[wqueen] | b.bit_board[wking];
	default:
		return b.bit_board[bpawn] | b.bit_board[brook] | b.bit_board[bbishop] | b.bit_board[bknight] | b.bit_board[bqueen] | b.bit_board[bking];
	}
	return 0;
}
inline uint64_t get_pieces_2players(const Board b) {
	return	b.bit_board[wpawn] | b.bit_board[wrook] | b.bit_board[wbishop] | b.bit_board[wknight] | b.bit_board[wqueen] | b.bit_board[wking] |
		b.bit_board[bpawn] | b.bit_board[brook] | b.bit_board[bbishop] | b.bit_board[bknight] | b.bit_board[bqueen] | b.bit_board[bking];
}
inline uint64_t get_bit_from_pos(const uint64_t pos) { return (uint64_t)1 << pos; }
unsigned short get_type_from_pos(const Board b, const unsigned short pos);

// split_bits : seperate all the bits set in a number in different bits with only one bit set
// exp : 0110010 --> 0100000, 0010000, 0000010
inline unsigned short split_bits(uint64_t* splitted, uint64_t bits) {
	unsigned short i = 0;
	for (; i < MAX_POSSIBLE_MOVES; i++) {
		if (!bits) return i;
		splitted[i] = (uint64_t)(LAST_BIT >> CLZ(bits));
		bits ^= splitted[i];
	}
	return i;
}

// return a U64 where every "1"s represent an attacked case by either a pawn, knights..
inline uint64_t get_attacked_by_pawns(const Board b, const Color c) {
	const uint64_t wpawns = b.bit_board[wpawn];
	const uint64_t bpawns = b.bit_board[bpawn];
	switch (c)
	{
	case WHITE:
		return ((wpawns & ~bit_columnH) << 7) | ((wpawns & ~(bit_columnH << 7)) << 9);
	default:
		return ((bpawns & ~bit_columnH) >> 9) | ((bpawns & ~(bit_columnH << 7)) >> 7);
	}
}
uint64_t get_attacked_by_knights(const Board b, const Color c);
uint64_t get_attacked_by_bishops(const Board b, const Color c, const uint64_t all_pieces);
uint64_t get_attacked_by_rooks(const Board b, const Color c, const uint64_t all_pieces);
uint64_t get_attacked_by_queens(const Board b, const Color c, const uint64_t all_pieces);
uint64_t get_attacked_by_king(const Board b, const Color c);

// addition (inclusive or) of all attacked functions
inline uint64_t get_attacked_map(const Board b, const Color c) {
	const uint64_t all_pieces = get_pieces_2players(b);
	return	get_attacked_by_pawns(b, c) | get_attacked_by_knights(b, c) | get_attacked_by_bishops(b, c, all_pieces) |
		get_attacked_by_rooks(b, c, all_pieces) | get_attacked_by_queens(b, c, all_pieces) | get_attacked_by_king(b, c);
}