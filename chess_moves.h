#pragma once

#include "consts.h"
#include "chess_attacked.h"
#include "board.h"
#include <stdio.h> // printf

// separate_moves : attacked is a map of all the different cases attacked from one piece
// this function separates all those different cases in new bitboards (you can see it as "children" bitboards)
// it's the same idea as split_bits but this time you get Boards instead of U64
unsigned int separate_moves(Board* res, const Board b, uint64_t attacked, const unsigned short pos, const unsigned short type, unsigned int index);

// return a number that tells how many children boards have been created
// modify res wich is a table containing all the children boards
unsigned int get_bitboard_children_PAWN(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b);
unsigned int get_bitboard_children_KNIGHT(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b);
unsigned int get_bitboard_children_BISHOP(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces);
unsigned int get_bitboard_children_ROOK(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces);
unsigned int get_bitboard_children_QUEEN(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces);
unsigned int get_bitboard_children_KING(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b);

// additions (table of Boards) of all bitboard_children functions + specials chess hits like roque and "upgrade" of a pawn
unsigned int get_bitboard_children(Board* res, const Board b, const Color c);

void printBits__(size_t const size, void const* const ptr);