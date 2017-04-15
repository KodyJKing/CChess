#include <stdio.h>
#include <stdlib.h>

// ---- Representation ---- //

const int KING = 0;
const int QUEEN = 1;
const int BISHOP = 2;
const int KNIGHT = 3;
const int ROOK = 4;
const int PAWN = 5;
const int EMPTY = 6;

const int WHITE = 0;
const int BLACK = 1;

typedef struct {
    unsigned int type:3, color:1, moved:1;
} piece;

int indexOf(int x, int y) {
    return x + y * 8;
}

int xOf(int index) {
    return index % 8;
}

int yOf(int index) {
    return (index - (index % 8)) / 8;
}

// ---- Printing ---- //

int charToType(char c) {
    c = tolower(c);
    switch(c) {
        case 'k': return KING;
        case 'q': return QUEEN;
        case 'b': return BISHOP;
        case 'n': return KNIGHT;
        case 'r': return ROOK;
        case 'p': return PAWN;
        case '.': return EMPTY;
    }
}

int charToPiece(char c) {
    piece p;
    p.type = charToType(c);
    p.color = (c == tolower(c)) ? WHITE : BLACK;
}

char pieceToChar(piece p) {
    char result = "kqbnrp."[p.type];
    return p.color == WHITE ? result : toupper(result);
}

void printBoard(piece board[64]) {
    int i;
    printf("\n   abcdefgh\n");
    for( i = 0; i < 64; i++ ) {
        if (xOf(i) == 0) printf("\n %d ", yOf(i));
        putchar(pieceToChar(board[i]));
    }
    putchar('\n');
}

// ---- Logic ---- //

char backrow[] = "rnbqkbnr";

void initBoard(piece board[64]) {
    int i;
    piece p;

    p.type = EMPTY;
    p.color = BLACK;
    for ( i = 0; i < 64; i++) board[i] = p;

    for ( i = 0; i < 8; i++) {
        p.type = charToType(backrow[i]);
        board[indexOf(i, 0)] = p;
        p.type = PAWN;
        board[indexOf(i, 1)] = p;
    }

    p.color = WHITE;
    for ( i = 0; i < 8; i++) {
        p.type = charToType(backrow[i]);
        board[indexOf(i, 7)] = p;
        p.type = PAWN;
        board[indexOf(i, 6)] = p;
    }
}

int main()
{
    piece board[64];
    initBoard(&board);
    printBoard(&board);
    return 0;
}
