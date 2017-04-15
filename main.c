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
} Piece;

typedef struct {
    unsigned int x:4, y:4;
} coord;

int indexOf(coord pos) { return pos.x + pos.y * 8; }
coord posOf(int index) {
    coord pos;
    pos.x = index % 8;
    pos.y = ( index - pos.x ) / 8;
    return pos;
}
char xChar(int x) {return 'a' + x; }
char yChar(int y) {return '0' + y; }

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

char backrow[] = "rnbqkbnr";
void initBoard(Piece board[64]) {
    int i;
    coord pos;
    Piece p;

    p.type = EMPTY;
    p.color = BLACK;
    for ( i = 0; i < 64; i++) board[i] = p;

    for ( pos.x = 0; pos.x < 8; pos.x++) {
        p.color = BLACK;

        pos.y = 0;
        p.type = charToType(backrow[pos.x]);
        board[indexOf(pos)] = p;

        pos.y = 1;
        p.type = PAWN;
        board[indexOf(pos)] = p;

        p.color = WHITE;

        pos.y = 7;
        p.type = charToType(backrow[pos.x]);
        board[indexOf(pos)] = p;

        pos.y = 6;
        p.type = PAWN;
        board[indexOf(pos)] = p;
    }
}

// ---- Implementation ---- //

int main()
{
    Piece board[64];
    initBoard(&board);
    printBoard(&board);
    return 0;
}
