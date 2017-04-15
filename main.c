#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
} Piece;

typedef struct {
    unsigned int x:3, y:3;
} Pos;

typedef struct {
    Pos start, end;
} Move;

int posIndex(Pos pos) {
    return pos.x + pos.y * 8;
}

Pos indexPos(int index) {
    Pos pos;
    pos.x = index * 8;
    pos.y = (index - pos.x) / 8;
    return pos;
}

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
    Piece p;
    p.type = charToType(c);
    p.color = (c == tolower(c)) ? WHITE : BLACK;
}

char pieceToChar(Piece p) {
    char result = "kqbnrp."[p.type];
    return p.color == WHITE ? result : toupper(result);
}

char xChar(int x) { return 'a' + x; }
char yChar(int y) { return '0' + y; }

void printMove(Move move) {
    printf("%c%c-%c%c", xChar(move.start.x), yChar(move.start.y), xChar(move.end.x), yChar(move.end.y));
}

void printBoard(Piece board[64]) {
    int i;
    printf("\n   abcdefgh\n");
    for( i = 0; i < 64; i++ ) {
        if (xOf(i) == 0) printf("\n %d ", yOf(i));
        putchar(pieceToChar(board[i]));
    }
    putchar('\n');
}

// ---- Logic ---- //

void initBoard(Piece board[64]) {
    int i;
    Piece p = (Piece) {EMPTY, BLACK};
    for ( i = 16; i < 48; i++) board[i] = p;

    for ( i = 0; i < 8; i++) {
        int type = charToType("rnbqkbnr"[i]);
        board[indexOf(i, 0)] = (Piece) {type, BLACK};
        board[indexOf(i, 1)] = (Piece) {PAWN, BLACK};
        board[indexOf(i, 7)] = (Piece) {type, WHITE};
        board[indexOf(i, 6)] = (Piece) {PAWN, WHITE};
    }
}

int main()
{
    Piece board[64];
    initBoard(&board);
    printBoard(&board);
    return 0;
}
