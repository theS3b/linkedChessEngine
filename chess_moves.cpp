#pragma warning(disable : 6386) // disable false-positive of buffer overrun
#include "chess_moves.h"
#include "utils.h"

void printBits__(size_t const size, void const* const ptr)
{
	unsigned char* b = (unsigned char*)ptr;
	unsigned char byte;
	short i, j;

	for (i = (int)size - 1; i >= 0; i--)
	{
		for (j = 7; j >= 0; j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
		printf("\n");
	}
	puts("");
}

unsigned int separate_moves(Board* res, const Board b, uint64_t attacked, const unsigned short pos, const unsigned short type, unsigned int index) {
	// if there's only 2 kings left we stop
	if (((get_king(b, WHITE) | get_king(b, BLACK)) ^ get_pieces_2players(b)) == 0)
		return 0;

	attacked ^= attacked & get_all_pieces(b, b.player);
	uint64_t preRes[MAX_POSSIBLE_MOVES];
	unsigned short nbr = split_bits(preRes, attacked);
	const Board tmpb = b;
	const short pawn_is_moved = (type <= bpawn) ? type : -1; 	// coup en passant
	const short opponent_type = (type + 1) % 2;					// idem
	const short arr_c = (b.player == WHITE) ? W_ARRAY : B_ARRAY;
	unsigned int i = 0;
	unsigned int j = 0;

	for (; i < nbr; i++, j++) {
		unsigned int tmp = index + j;
		
		// Promotion
		if (type <= bpawn) {
			const uint64_t last_line = (preRes[i] & ((uint64_t)bit_line << 56));
			const uint64_t first_line = preRes[i] & bit_line;
			if (b.player == WHITE && last_line) {
				for (unsigned short counter = 2; counter < bqueen; counter += 2) {
					res[tmp] = tmpb;
					res[tmp].player = GET_OPPONENT(b.player);
					res[tmp].bit_board[wpawn] ^= ((uint64_t)1 << pos);				// remove pawn on last pos
					res[tmp].bit_board[counter] |= last_line;			// add a other pice type on the next pos
					res[tmp].counter = 0;

					// if the king is eaten or attacked we remove the possibility
					// we test only the first time
					if (!get_king(res[tmp], b.player) || get_king(res[tmp], b.player) & get_attacked_map(res[tmp], GET_OPPONENT(b.player))) {
						i++;	// we'll test the next piece after (if we don't put that it'll test again and again the same piece)
						break; // if the king is attacked the piece type won't make any difference so we just stop
					}

					// remove opponent pieces where you wanna go
					for (unsigned short t = opponent_type; t < notype; t += 2) {
						res[tmp].bit_board[t] ^= (res[tmp].bit_board[t] & last_line);
					}
					j++;
					tmp++;
				}
				// we'll add one already with the "more general" for loop
				j--;
				continue;
			}
			else if (b.player == BLACK && first_line) {
				for (unsigned short counter = 3; counter < wking; counter += 2) {
					res[tmp] = tmpb;
					res[tmp].player = GET_OPPONENT(b.player);
					res[tmp].bit_board[bpawn] ^= ((uint64_t)1 << pos);				// remove pawn on last pos
					res[tmp].bit_board[counter] |= first_line;			// add a other pice type on the next pos
					res[tmp].counter = 0;

					// if the king is eaten or attacked we remove the possibility
					if (counter == 2) { // we test only the first time
						if (!get_king(res[tmp], b.player) || get_king(res[tmp], b.player) & get_attacked_map(res[tmp], GET_OPPONENT(b.player))) {
							i++;
							break; // if the king is attacked the piece type won't make any difference so we just stop
						}
					}

					// remove opponent pieces where you wanna go
					for (unsigned short t = opponent_type; t < notype; t += 2) {
						res[tmp].bit_board[t] ^= (res[tmp].bit_board[t] & first_line);
					}
					j++;
					tmp++;
				}
				// we'll add one already with the "more general" for loop
				j--;
				continue;
			}
		}

		// Ordinary case
		res[tmp] = tmpb;
		res[tmp].player = GET_OPPONENT(b.player);
		res[tmp].bit_board[type] = (b.bit_board[type] ^ ((uint64_t)1 << pos)) | preRes[i];

		// remove opponent pieces where you wanna go
		bool smg_is_eaten = false;
		uint64_t rem = 0;
		for (unsigned short t = opponent_type; t < notype; t += 2) {
			rem = res[tmp].bit_board[t];
			res[tmp].bit_board[t] ^= (res[tmp].bit_board[t] & res[tmp].bit_board[type]);
			if (rem != res[tmp].bit_board[t])
				smg_is_eaten = true;
		}

		// if the king is eaten or attacked we remove the possibility
		const uint64_t kings = get_king(res[tmp], b.player);
		if (!kings || kings & get_attacked_map(res[tmp], GET_OPPONENT(b.player))) {
			j--;
			continue;
		}

		// for preparing "coup en passant"
		if (pawn_is_moved != -1)
			res[tmp].last_pawn_pos[pawn_is_moved] = b.bit_board[pawn_is_moved];
		else
			res[tmp].last_pawn_pos[type] = 0;

		/* PAT RULES */

		// LESS THAN 50
		// if nothing is eaten, then we add 1 to the counter to count how many moves there was without anything happening
		if (!smg_is_eaten)
			res[tmp].counter = b.counter + 1;
		else
			res[tmp].counter = 0;

		// we stop if we didn't do anything the last 50 moves or if there's only the 2 kings left
		if (res[tmp].counter >= MAX_MOVES_WITHOUT_DOING_ANYTHING)
		{
			j--;
			continue;
		}

		// THREEFOLD REPETITION
		append_to_array(res[tmp].last_moves[arr_c], res[tmp].bit_board[type]);
		
		// it is not an option to make the threefold repetition
		if (res[tmp].last_moves[arr_c][0] == res[tmp].last_moves[arr_c][2] && res[tmp].last_moves[arr_c][0] == res[tmp].last_moves[arr_c][4]) {
			j--;
			continue;
		}

		// SPECIAL MOVE
		// optimize roque
		if (b.check_type[arr_c]) {
			bool tmpCheck = true;
			switch (type) {
			case wking:
				res[tmp].king_rook_moved[W_ARRAY][LEFT] = true;
				res[tmp].king_rook_moved[W_ARRAY][RIGHT] = true;
				break;
			case bking:
				res[tmp].king_rook_moved[B_ARRAY][LEFT] = true;
				res[tmp].king_rook_moved[B_ARRAY][RIGHT] = true;
				break;
			case wrook:
				if (pos < 5) res[tmp].king_rook_moved[W_ARRAY][RIGHT] = true;
				else res[tmp].king_rook_moved[W_ARRAY][LEFT] = true;
				break;
			case brook:
				if (pos < 60) res[tmp].king_rook_moved[B_ARRAY][RIGHT] = true;
				else res[tmp].king_rook_moved[B_ARRAY][LEFT] = true;
				break;
			default:
				tmpCheck = false;
				break;
			}
			if (tmpCheck) {
				res[tmp].check_type[arr_c] = !(res[tmp].king_rook_moved[arr_c][RIGHT] && res[tmp].king_rook_moved[arr_c][LEFT]);
			}
		}
	}

	// COUP EN PASSANT
	if (type <= bpawn && b.last_pawn_pos[opponent_type] != 0) {
		// we check if the last move was a 2 case move w/ a pawn
		uint64_t diff = b.last_pawn_pos[opponent_type] ^ b.bit_board[opponent_type];
		const int tz = CTZ(diff);
		if (tz >= 0) {
			if (LOG2(diff >> tz) == 16) { // if there's a difference of 16 cases between the two pawns (before and after)

				// we select the pawn to eat (not the old one)
				unsigned int pos_pawn_to_eat = 0;
				if (b.player == WHITE) // we take the pawn that is the most near the bottom
					pos_pawn_to_eat = tz;
				else // the northest one
					pos_pawn_to_eat = LOG2(diff) - 1;

				// evaluate the pos where the pawn will go
				unsigned int pos_to_shift = 0;
				bool next_to = false;
				if (pos_pawn_to_eat == pos + 1) {
					if (b.player == WHITE)
						pos_to_shift = 9 + pos;
					else
						pos_to_shift = pos - 9;
					next_to = true;
				}
				else if (pos_pawn_to_eat == pos - 1) {
					if (b.player == WHITE)
						pos_to_shift = 7 + pos;
					else
						pos_to_shift = pos - 7;
					next_to = true;
				}
				// we add a children
				if (next_to) {
					i++;
					j++;
					unsigned int tmp = index + j;
					res[tmp] = tmpb;
					res[tmp].player = GET_OPPONENT(b.player);
					res[tmp].bit_board[type] = (b.bit_board[type] ^ ((uint64_t)1 << pos)) | ((uint64_t)1 << pos_to_shift); // we make the POV move
					res[tmp].bit_board[opponent_type] ^= (b.bit_board[opponent_type] & ((uint64_t)1 << pos_pawn_to_eat)); // we remove the pawn to eat
					// if we did the coup en passant, it doesn't change the possibility of doing the roque so we don't have to check everything like above
				}
			}
		}
	}

	return j;
}

unsigned int get_bitboard_children_PAWN(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b) {
	const uint64_t pawn = (uint64_t)1 << pos;
	const uint64_t pawn_not_r_border = pawn ^ (pawn & bit_columnH);
	const uint64_t pawn_not_l_border = pawn ^ (pawn & (bit_columnH << 7));
	const uint64_t op_pieces = get_all_pieces(b, GET_OPPONENT(b.player));
	const uint64_t my_pieces = get_all_pieces(b, b.player);
	const uint64_t all_pieces = op_pieces | my_pieces;
	uint64_t attacked = 0;

	const uint64_t NORTH_S = pawn << 8;
	const uint64_t SOUTH_S = pawn >> 8;
	uint64_t tmp = 0, tmp2 = 0;
	switch (b.player) {
	case WHITE:
		// if the pawn is attacking
		attacked |= ((pawn_not_l_border << 9) | (pawn_not_r_border << 7)) & op_pieces;

		// if the pawn is just going forward
		tmp = NORTH_S ^ (NORTH_S & all_pieces);
		tmp2 = (tmp & (bit_line << 16)) << 8;
		attacked |= tmp | (tmp2 ^ (tmp2 & all_pieces));


		break;
	case BLACK:
		// if the pawn is attacking
		attacked |= ((pawn_not_r_border >> 9) | (pawn_not_l_border >> 7)) & op_pieces;

		// if the pawn is just going forward
		tmp = SOUTH_S ^ (SOUTH_S & all_pieces);
		tmp2 = (tmp & ((uint64_t)bit_line << 40)) >> 8;
		attacked |= tmp | (tmp2 ^ (tmp2 & all_pieces));
		break;
	}
	
	return separate_moves(res, b, attacked, pos, type, index);
}

unsigned int get_bitboard_children_KNIGHT(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b) {
	const uint64_t Kpos = (uint64_t)1 << pos;
	const uint64_t left = Kpos & ~(bit_columnH << 7);
	const uint64_t Left = Kpos & (~(bit_columnH << 7) & ~(bit_columnH << 6));
	const uint64_t right = Kpos & ~bit_columnH;
	const uint64_t Right = Kpos & (~bit_columnH & ~(bit_columnH << 1));
	const uint64_t up = Kpos & ~((uint64_t)bit_line << 56);
	const uint64_t Up = Kpos & (~((uint64_t)bit_line << 56) & ~((uint64_t)bit_line << 48));
	const uint64_t down = Kpos & ~bit_line;
	const uint64_t Down = Kpos & (~bit_line & ~(bit_line << 8));
	uint64_t attacked = ((up & Right) << 6) | ((Up & right) << 15) | ((up & Left) << 10) | ((Up & left) << 17) |
		((down & Right) >> 10) | ((Down & right) >> 17) | ((Left & down) >> 6) | ((Down & left) >> 15);
	return separate_moves(res, b, attacked, pos, type, index);
}

unsigned int get_bitboard_children_BISHOP(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces) {
	uint64_t attacked = 0;
	// NORTH EST
	uint64_t tmp = attacked_by_bishops_NE[pos + 1];
	attacked |= tmp ^ attacked_by_bishops_NE[CTZ_ARR(tmp & all_pieces)];
	// NORTH WEST
	tmp = attacked_by_bishops_NW[pos + 1];
	attacked |= tmp ^ attacked_by_bishops_NW[CTZ_ARR(tmp & all_pieces)];
	// SOUTH EST
	tmp = attacked_by_bishops_SE[pos + 1];
	attacked |= tmp ^ attacked_by_bishops_SE[LOG2(tmp & all_pieces)];
	// SOUTH WEST
	tmp = attacked_by_bishops_SW[pos + 1];
	attacked |= tmp ^ attacked_by_bishops_SW[LOG2(tmp & all_pieces)];
	return separate_moves(res, b, attacked, pos, type, index);
}

unsigned int get_bitboard_children_ROOK(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces) {
	uint64_t attacked = 0;
	// NORTH
	uint64_t tmp = attacked_by_rooks_N[pos + 1];
	attacked |= tmp ^ attacked_by_rooks_N[CTZ_ARR(tmp & all_pieces)];
	// WEST
	tmp = attacked_by_rooks_W[pos + 1];
	attacked |= tmp ^ attacked_by_rooks_W[CTZ_ARR(tmp & all_pieces)];
	// EST
	tmp = attacked_by_rooks_E[pos + 1];
	attacked |= tmp ^ attacked_by_rooks_E[LOG2(tmp & all_pieces)];
	// SOUTH
	tmp = attacked_by_rooks_S[pos + 1];
	attacked |= tmp ^ attacked_by_rooks_S[LOG2(tmp & all_pieces)];
	return separate_moves(res, b, attacked, pos, type, index);
}

unsigned int get_bitboard_children_QUEEN(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b, const uint64_t all_pieces) {
	unsigned int c = get_bitboard_children_BISHOP(res, index, pos, type, b, all_pieces);
	return get_bitboard_children_ROOK(res, index + c, pos, type, b, all_pieces) + c;
}

unsigned int get_bitboard_children_KING(Board* res, unsigned int index, const unsigned short pos, const unsigned short type, const Board b) {
	// There is at most one king so we don't need to make as above (treating only one specific piece)
	uint64_t attacked = get_attacked_by_king(b, b.player);
	attacked ^= (attacked & get_attacked_map(b, GET_OPPONENT(b.player)));
	return separate_moves(res, b, attacked, pos, type, index);
}

unsigned int get_bitboard_children(Board* res, const Board b, const Color c) {
	const unsigned short c_arr = (c == WHITE) ? W_ARRAY : B_ARRAY;
	const uint64_t all_pieces = get_pieces_2players(b);
	unsigned int i = 0;
	int pos = 0;

	for (pos = 0; pos < SIZEOF_U64_TIMES_8; pos++) {
		unsigned int type = get_type_from_pos(b, pos);

		if (type == notype)
			continue;
		else if (type == wpawn + c_arr) {
			i += get_bitboard_children_PAWN(res, i, pos, type, b);
			continue;
		}
		else if (type == wknight + c_arr) {
			i += get_bitboard_children_KNIGHT(res, i, pos, type, b);
			continue;
		}
		else if (type == wrook + c_arr) {
			i += get_bitboard_children_ROOK(res, i, pos, type, b, all_pieces);
			continue;
		}
		else if (type == wbishop + c_arr) {
			i += get_bitboard_children_BISHOP(res, i, pos, type, b, all_pieces);
			continue;
		}
		else if (type == wqueen + c_arr) {
			i += get_bitboard_children_QUEEN(res, i, pos, type, b, all_pieces);
			continue;
		}
		else if (type == wking + c_arr) {
			i += get_bitboard_children_KING(res, i, pos, type, b);
			continue;
		}
	}

	// SPECIAL RULES
	const uint64_t att = get_attacked_map(b, GET_OPPONENT(c));
	if (!(att & get_king(b, b.player))) {

		// Roque
		if (b.check_type[c_arr]) {
			uint64_t tmp = get_pieces_2players(b);
			if (!b.king_rook_moved[c_arr][LEFT] && !(tmp & needs_to_be_empty[c_arr][LEFT])) {
				// LEFT
				if (c == WHITE && !(att & not_be_attacked[W_ARRAY][LEFT]) && (b.bit_board[wrook] & 0x80)) {
					res[i] = b;
					res[i].player = BLACK;
					res[i].king_rook_moved[c_arr][LEFT] = true;
					res[i].king_rook_moved[c_arr][RIGHT] = true;
					res[i].check_type[c_arr] = false;
					res[i].last_pawn_pos[c_arr] = 0;
					res[i].bit_board[wrook] |= 0x10;
					res[i].bit_board[wrook] ^= 0x80;
					res[i].bit_board[wking] <<= 2;
					i++;
				}
				else if (c == BLACK && !(att & not_be_attacked[B_ARRAY][LEFT]) && (b.bit_board[brook] & 0x8000000000000000)) {
					res[i] = b;
					res[i].player = WHITE;
					res[i].king_rook_moved[c_arr][LEFT] = true;
					res[i].king_rook_moved[c_arr][RIGHT] = true;
					res[i].check_type[c_arr] = false;
					res[i].last_pawn_pos[c_arr] = 0;
					res[i].bit_board[brook] |= 0x1000000000000000;
					res[i].bit_board[brook] ^= 0x8000000000000000;
					res[i].bit_board[bking] <<= 2;
					i++;
				}
			}
			if (!b.king_rook_moved[c_arr][RIGHT] && !(tmp & needs_to_be_empty[c_arr][RIGHT])) {
				// RIGHT
				if (c == WHITE && !(att & not_be_attacked[W_ARRAY][RIGHT]) && (b.bit_board[wrook] & 0x1)) {
					res[i] = b;
					res[i].player = BLACK;
					res[i].king_rook_moved[c_arr][LEFT] = true;
					res[i].king_rook_moved[c_arr][RIGHT] = true;
					res[i].check_type[c_arr] = false;
					res[i].last_pawn_pos[c_arr] = 0;
					res[i].bit_board[wrook] |= 0x4;
					res[i].bit_board[wrook] ^= 0x1;
					res[i].bit_board[wking] >>= 2;
					i++;
				}
				else if (c == BLACK && !(att & not_be_attacked[B_ARRAY][RIGHT]) && (b.bit_board[brook] & 0x100000000000000)) {
					res[i] = b;
					res[i].player = WHITE;
					res[i].king_rook_moved[c_arr][LEFT] = true;
					res[i].king_rook_moved[c_arr][RIGHT] = true;
					res[i].check_type[c_arr] = false;
					res[i].last_pawn_pos[c_arr] = 0;
					res[i].bit_board[brook] |= 0x400000000000000;
					res[i].bit_board[brook] ^= 0x100000000000000;
					res[i].bit_board[bking] >>= 2;
					i++;
				}
			}
		}
	}

	return i;
}
