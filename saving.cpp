#include "saving.h"
#include "consts.h"
#include <fstream>

using namespace std;

void export_board(const Board& b) {
	ofstream file("board.s3b");
	for (int i = wpawn; i < notype; i++)
		file << b.bit_board[i] << " ";

	file << b.player << " ";
	file << b.check_type[0] << " ";
	file << b.check_type[1] << " ";
	file << b.last_pawn_pos[0] << " ";
	file << b.last_pawn_pos[1] << " ";
	file << b.king_rook_moved[0][0] << " ";
	file << b.king_rook_moved[0][1] << " ";
	file << b.king_rook_moved[1][0] << " ";
	file << b.king_rook_moved[1][1] << " ";
	file << b.counter << " ";

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++)
			file << b.last_moves[i][j] << " ";
	}
	file.close();
}

Board import_board() {
	ifstream file("board.s3b");
	Board b;
	for (int i = wpawn; i < notype; i++)
		file >> b.bit_board[i];

	file >> b.player;
	file >> b.check_type[0];
	file >> b.check_type[1];
	file >> b.last_pawn_pos[0];
	file >> b.last_pawn_pos[1];
	file >> b.king_rook_moved[0][0];
	file >> b.king_rook_moved[0][1];
	file >> b.king_rook_moved[1][0];
	file >> b.king_rook_moved[1][1];
	file >> b.counter;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++)
			file >> b.last_moves[i][j];
	}
	file.close();
	return b;
}