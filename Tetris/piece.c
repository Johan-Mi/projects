#include <stdlib.h>

#include "piece.h"

Shape const leftRotatorArray[] = {
	[O] = O,
	[I1] = I4,
	[I2] = I1,
	[I3] = I2,
	[I4] = I3,
	[T1] = T4,
	[T2] = T1,
	[T3] = T2,
	[T4] = T3,
	[J1] = J4,
	[J2] = J1,
	[J3] = J2,
	[J4] = J3,
	[L1] = L4,
	[L2] = L1,
	[L3] = L2,
	[L4] = L3,
	[S1] = S4,
	[S2] = S1,
	[S3] = S2,
	[S4] = S3,
	[Z1] = Z4,
	[Z2] = Z1,
	[Z3] = Z2,
	[Z4] = Z3,
};

Shape const rightRotatorArray[] = {
	[O] = O,
	[I1] = I2,
	[I2] = I3,
	[I3] = I4,
	[I4] = I1,
	[T1] = T2,
	[T2] = T3,
	[T3] = T4,
	[T4] = T1,
	[J1] = J2,
	[J2] = J3,
	[J3] = J4,
	[J4] = J1,
	[L1] = L2,
	[L2] = L3,
	[L3] = L4,
	[L4] = L1,
	[S1] = S2,
	[S2] = S3,
	[S3] = S4,
	[S4] = S1,
	[Z1] = Z2,
	[Z2] = Z3,
	[Z3] = Z4,
	[Z4] = Z1,
};

u8 const tileCoordinates[29][4][2] = {
	[O]  = {{1, 0}, {2, 0}, {1, 1}, {2, 1}},
	[I1] = {{0, 1}, {1, 1}, {2, 1}, {3, 1}},
	[I2] = {{2, 0}, {2, 1}, {2, 2}, {2, 3}},
	[I3] = {{0, 2}, {1, 2}, {2, 2}, {3, 2}},
	[I4] = {{1, 0}, {1, 1}, {1, 2}, {1, 3}},
	[T1] = {{1, 0}, {0, 1}, {1, 1}, {2, 1}},
	[T2] = {{1, 0}, {1, 1}, {2, 1}, {1, 2}},
	[T3] = {{0, 1}, {1, 1}, {2, 1}, {1, 2}},
	[T4] = {{1, 0}, {0, 1}, {1, 1}, {1, 2}},
	[J1] = {{0, 0}, {0, 1}, {1, 1}, {2, 1}},
	[J2] = {{1, 0}, {2, 0}, {1, 1}, {1, 2}},
	[J3] = {{0, 1}, {1, 1}, {2, 1}, {2, 2}},
	[J4] = {{1, 0}, {1, 1}, {0, 2}, {1, 2}},
	[L1] = {{2, 0}, {0, 1}, {1, 1}, {2, 1}},
	[L2] = {{1, 0}, {1, 1}, {1, 2}, {2, 2}},
	[L3] = {{0, 1}, {1, 1}, {2, 1}, {0, 2}},
	[L4] = {{0, 0}, {1, 0}, {1, 1}, {1, 2}},
	[S1] = {{1, 0}, {2, 0}, {0, 1}, {1, 1}},
	[S2] = {{1, 0}, {1, 1}, {2, 1}, {2, 2}},
	[S3] = {{1, 1}, {2, 1}, {0, 2}, {1, 2}},
	[S4] = {{0, 0}, {0, 1}, {1, 1}, {1, 2}},
	[Z1] = {{0, 0}, {1, 0}, {1, 1}, {2, 1}},
	[Z2] = {{2, 0}, {1, 1}, {2, 1}, {1, 2}},
	[Z3] = {{0, 1}, {1, 1}, {1, 2}, {2, 2}},
	[Z4] = {{1, 0}, {0, 1}, {1, 1}, {0, 2}},
};

void tryMoveLeft(Board board, Piece* piece) {
	piece->x--;
	if(pieceCollides(board, *piece))
		piece->x++;
}

void tryMoveRight(Board board, Piece* piece) {
	piece->x++;
	if(pieceCollides(board, *piece))
		piece->x--;
}

void tryMoveDown(Board board, Piece* piece) {
	piece->y++;
	if(pieceCollides(board, *piece))
		piece->y--;
}

void tryMoveUp(Board board, Piece* piece) {
	piece->y--;
	if(pieceCollides(board, *piece))
		piece->y++;
}

void tryRotLeft(Board board, Piece* piece) {
	piece->shape = leftRotatorArray[piece->shape];
	if(pieceCollides(board, *piece))
		piece->shape = rightRotatorArray[piece->shape];
}

void tryRotRight(Board board, Piece* piece) {
	piece->shape = rightRotatorArray[piece->shape];
	if(pieceCollides(board, *piece))
		piece->shape = leftRotatorArray[piece->shape];
}

void tryDrop(Board board, Piece* piece) {
	do {
		piece->y++;
	} while(!pieceCollides(board, *piece));
	piece->y--;
}

bool pieceCollides(Board board, Piece piece) {
	for(u8 i = 0; i < 4; i++) {
		if((u8)(piece.x + tileCoordinates[piece.shape][i][0]) >= BOARD_WIDTH
				|| (u8)(piece.y + tileCoordinates[piece.shape][i][1]) >= BOARD_HEIGHT
				|| board[(u8)(piece.y + tileCoordinates[piece.shape][i][1])]
					[(u8)(piece.x + tileCoordinates[piece.shape][i][0])]) {
			return true;
		}
	}

	return false;
}

u8 colorOfPiece(Piece piece) {
	switch(piece.shape) {
		case O:
		case T1 ... T4:
		case I1 ... I4:
			return 1;
		case J1 ... J4:
		case S1 ... S4:
			return 2;
		case Z1 ... Z4:
		case L1 ... L4:
			return 3;
		default:
			return 0;
	}
}

Piece randomPiece() {
	return (Piece){
		.x = BOARD_WIDTH / 2 - 2,
		.y = 0,
		.shape = (Shape[]){
			O, I1, T1, J1, L1, S1, Z1
		}[rand() % 7]
	};
}
