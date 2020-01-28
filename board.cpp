#include "board.h"
#include "consts.h"
#include "chess_moves.h"
#include <iostream>
#include <string>

Board::Board(void) {
	player = WHITE;
	bit_board[wpawn] = (uint64_t)bit_line << (uint64_t)8;
	bit_board[bpawn] = (uint64_t)bit_line << (uint64_t)48;
	bool white = true;
	for (unsigned short i = wrook; i <= bking; i++, white = !white)
		bit_board[i] = white ? w_start[i / 2] : bit_board[i - 1] << (uint64_t)56;

	last_pawn_pos[wpawn] = 0;
	last_pawn_pos[bpawn] = 0;
	king_rook_moved[W_ARRAY][LEFT] = false;
	king_rook_moved[W_ARRAY][RIGHT] = false;
	king_rook_moved[B_ARRAY][LEFT] = false;
	king_rook_moved[B_ARRAY][RIGHT] = false;
	check_type[W_ARRAY] = true;
	check_type[B_ARRAY] = true;
	counter = 0;

	last_moves[0][0] = 0;
	last_moves[1][0] = 0;
	last_moves[0][1] = 1;
	last_moves[1][1] = 1;
	last_moves[0][2] = 2;
	last_moves[1][2] = 2;
	last_moves[0][3] = 3;
	last_moves[1][3] = 3;
	last_moves[0][4] = 4;
	last_moves[1][4] = 4;
	
	return;
}

std::vector<Board> Board::get_bitboard_children_cpp() {
	Board f[MAX_POSSIBLE_MOVES];
	int i = get_bitboard_children(f, *this, player);
	return std::vector<Board>(std::begin(f), std::begin(f)+i);
}


short Board::get_result(const short _player, const bool & leaf) {

	// if you can't do anything and the king is attacked
	Board f[MAX_POSSIBLE_MOVES];
	if (leaf && (get_king(*this, player) & get_attacked_map(*this, GET_OPPONENT(player))) != 0) {
		if (_player == player) {
			return -1;
		}
		else
			return 1;
	}
	if (get_king(*this, _player) == 0)
		return -1;

	// if no one wins, it's a pat
	return 0;
}

std::string Board::string_board() {
	std::string str = "";
	for (int pos = SIZEOF_U64_TIMES_8 - 1; pos >= 0; pos--) {
		unsigned int type = get_type_from_pos(*this, pos);
		switch (type) {
		case notype:
			str += ".";
			break;
		case wpawn:
			str += "P";
			break;
		case bpawn:
			str += "p";
			break;
		case wrook:
			str += "T";
			break;
		case brook:
			str += "t";
			break;
		case wbishop:
			str += "F";
			break;
		case bbishop:
			str += "f";
			break;
		case wknight:
			str += "C";
			break;
		case bknight:
			str += "c";
			break;
		case wqueen:
			str += "Q";
			break;
		case bqueen:
			str += "q";
			break;
		case wking:
			str += "K";
			break;
		case bking:
			str += "k";
			break;
		}
	}
	return str;
}

void Board::print() {
	std::string str = string_board();
	std::cout << std::endl << "  ABCDEFGH" << std::endl;
	unsigned c = 0;
	for (unsigned i = 0; i < 8; i++) {
		std::cout << std::to_string(8 - i) << "|";
		for (unsigned j = 0; j < 8; j++) {
			std::cout << str[c];
			c++;
		}
		std::cout << "|" << std::to_string(8 - i) << std::endl;
	}
	std::cout << "  ABCDEFGH" << std::endl << std::endl;
}

bool move_piece(Board & b, const std::string & pos) {

	std::vector<Board> children = b.get_bitboard_children_cpp();

	unsigned from = (7 - pos[0] + 'a') + ((pos[1] - '1') * 8);
	unsigned to = (7 - pos[2] + 'a') + ((pos[3] - '1') * 8);

	unsigned type = get_type_from_pos(b, from);

	if (type == notype)
		return false;


	if (type == wpawn && pos[3] == '8') {  // promotion 

		unsigned type_to_promote_to = notype;
		std::cout << "What kind of piece would you want to promote your pawn to ? (0 for knight, 1 for bishop, 2 for rook, 3 for queen) : ";
		std::string piece_type = "";
		std::getline(std::cin, piece_type);

		if (piece_type == "0")
			type_to_promote_to = wknight;
		else if (piece_type == "1")
			type_to_promote_to = wbishop;
		else if (piece_type == "2")
			type_to_promote_to = wrook;
		else if (piece_type == "3")
			type_to_promote_to = wqueen;
		else
			type_to_promote_to = wqueen;

		uint64_t boardpos = b.bit_board[type_to_promote_to] | ((unsigned long long int)1 << to); // adding promoted piece
		uint64_t secboardpos = b.bit_board[wpawn] ^ (((unsigned long long int)1 << from) & b.bit_board[type]); // removing pawn

		// test if legal move
		for (int i = 0; i < (int)children.size(); i++) {
			if (children[i].bit_board[type_to_promote_to] == boardpos && children[i].bit_board[wpawn] == secboardpos) {
				b = children[i];
				return true;
			}
		}
	}
	else if (type == bpawn && pos[3] == '1') {
		unsigned type_to_promote_to = notype;
		std::cout << "What kind of piece would you want to promote your pawn to ? (0 for knight, 1 for bishop, 2 for rook, 3 for queen) : ";
		std::string piece_type = "";
		std::getline(std::cin, piece_type);

		if (piece_type == "0")
			type_to_promote_to = bknight;
		else if (piece_type == "1")
			type_to_promote_to = bbishop;
		else if (piece_type == "2")
			type_to_promote_to = brook;
		else if (piece_type == "3")
			type_to_promote_to = bqueen;
		else
			type_to_promote_to = bqueen;

		uint64_t boardpos = b.bit_board[type_to_promote_to] | ((unsigned long long int)1 << to); // adding promoted piece
		uint64_t secboardpos = b.bit_board[bpawn] ^ (((unsigned long long int)1 << from) & b.bit_board[type]); // removing pawn
		for (int i = 0; i < (int)children.size(); i++) {
			if (children[i].bit_board[type_to_promote_to] == boardpos && children[i].bit_board[bpawn] == secboardpos) {
				b = children[i];
				return true;
			}
		}
	}
	else {
		uint64_t boardpos = b.bit_board[type] ^ (((unsigned long long int)1 << from) & b.bit_board[type]);  // removing piece
		boardpos |= ((unsigned long long int)1 << to);  // adding piece
		for (int i = 0; i < (int)children.size(); i++) {
			if (children[i].bit_board[type] == boardpos) {
				b = children[i];
				return true;
			}
		}
	}
	return false;
}

std::string transform_chessboard_to_move(const Board& from, const Board& to, const short & player)
{
	std::string ret = "";

	if (player == WHITE) {
		// Right roque
		if ((from.bit_board[wking] & 0x8) != 0 && (to.bit_board[wking] & 0x2) != 0) {
			char ret[] = "e1g1#h1f1";
			return ret;
		}
		// Left roque
		else if ((from.bit_board[bking] & 0x8) != 0 && (to.bit_board[bking] & 0x20) != 0) {
			char ret[] = "e1c1#a1d1";
			return ret;
		}

		// Promotion
		if (POPCNT(from.bit_board[wpawn]) != POPCNT(to.bit_board[wpawn])) {
			ret = "PR";
		}

		// Normal move
		short i = 0;
		for (i = wpawn; i < notype; i += 2) {
			if (from.bit_board[i] != to.bit_board[i])
				break;
		}
		uint64_t before = from.bit_board[i];
		uint64_t after = to.bit_board[i];
		before &= before ^ after;
		after &= from.bit_board[i] ^ after;

		short before_case = LOG2(before);
		short after_case = LOG2(after);

		std::cout << "move case :" << before_case << ", " << after_case << std::endl;

		if (POPCNT(get_all_pieces(from, BLACK)) != POPCNT(get_all_pieces(to, BLACK))) {
			uint64_t captured = get_all_pieces(from, BLACK) ^ get_all_pieces(to, BLACK);
			ret += casenb_to_coo(LOG2(captured)) + "CB#";
		}

		ret += casenb_to_coo(before_case) + casenb_to_coo(after_case);
	}
	else {
		// Right roque
		if ((from.bit_board[bking] & 0x800000000000000) != 0 && (to.bit_board[bking] & 0x200000000000000) != 0) {
			char ret[] = "e8g8#h8f8";
			return ret;
		}
		// Left roque
		else if ((from.bit_board[bking] & 0x800000000000000) != 0 && (to.bit_board[bking] & 0x2000000000000000) != 0) {
			char ret[] = "e8c8#a8d8";
			return ret;
		}
		// Promotion
		if (POPCNT(from.bit_board[bpawn]) != POPCNT(to.bit_board[bpawn])) {
			ret = "PR";
		}

		// Normal move
		short i = 0;
		for (i = bpawn; i < notype; i += 2) {
			if (from.bit_board[i] != to.bit_board[i])
				break;
		}
		uint64_t before = from.bit_board[i];
		uint64_t after = to.bit_board[i];
		before &= before ^ after;
		after &= from.bit_board[i] ^ after;

		short before_case = LOG2(before);
		short after_case = LOG2(after);

		std::cout << "move case :" << before_case << ", " << after_case << std::endl;

		if (POPCNT(get_all_pieces(from, WHITE)) != POPCNT(get_all_pieces(to, WHITE))) {
			uint64_t captured = get_all_pieces(from, WHITE) ^ get_all_pieces(to, WHITE);
			ret += casenb_to_coo(LOG2(captured)) + "CW#";
		}

		ret += casenb_to_coo(before_case) + casenb_to_coo(after_case);

	}
	
	std::cout << ret << std::endl;
	return ret;
}
