#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ---- Representation ---- //

#define KING   0
#define QUEEN  1
#define BISHOP 2
#define KNIGHT 3
#define ROOK   4
#define PAWN   5
#define EMPTY  6

#define WHITE  0
#define BLACK  1

#define DIR_N  (Pos){ 0,   1}
#define DIR_NE (Pos){ 1,   1}
#define DIR_E  (Pos){ 1,   0}
#define DIR_SE (Pos){ 1,  -1}
#define DIR_S  (Pos){ 0,  -1}
#define DIR_SW (Pos){-1,  -1}
#define DIR_W  (Pos){-1,   0}
#define DIR_NW (Pos){-1,   1}

// knight movement
#define DIR_NNE (Pos){ 1,  2}
#define DIR_NEE (Pos){ 2,  1}
#define DIR_SEE (Pos){ 2, -1}
#define DIR_SSE (Pos){ 1, -2}
#define DIR_SSW (Pos){-1, -2}
#define DIR_SWW (Pos){-2, -1}
#define DIR_NWW (Pos){-2,  1}
#define DIR_NNW (Pos){-1,  2}

typedef struct { unsigned int type:3, color:1, moved:1; }  Piece;
typedef struct { unsigned int x:4, y:4; }                  Pos;
typedef struct { Pos start, end; }                         Move;
typedef Piece                                              Board[64];

int posIndex(Pos pos) { return pos.x + pos.y * 8; }
Pos indexPos(int index) {
    Pos pos;
    pos.x = index * 8;
    pos.y = (index - pos.x) / 8;
    return pos;
}

int indexOf(int x, int y) { return x + y * 8; }

int xOf(int index) { return index % 8; }
int yOf(int index) { return (index - (index % 8)) / 8; }

bool inBoard(Pos pos) { return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8; }
Piece getPiece(Board board, Pos pos) { return board[posIndex(pos)]; }
void setPiece(Board board, Pos pos, Piece piece) { board[posIndex(pos)] = piece; }
bool occupied(Board board, Pos pos) { return getPiece(board, pos).type != EMPTY; }

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

char pieceToChar(Piece p, bool tileParity) {
    if ( p.type == EMPTY ) return tileParity ? '_' : '#';
    char result = "kqbnrp"[p.type];
    return p.color == WHITE ? result : toupper(result);
}

char xChar(int x) { return 'A' + x; }
char yChar(int y) { return '0' + y; }

void printPos(Pos pos) { printf("%c%c", xChar(pos.x), yChar(pos.y)); }
void printMove(Move move) { printPos(move.start); putchar('-'); printPos(move.end); }

void printBoard(Board board) {
    int i;
    printf("\n    ABCDEFGH\n    ________");
    for( i = 0; i < 64; i++ ) {
        if ( xOf(i) == 0 ) printf("\n %d |", yOf(i));
        putchar(pieceToChar( board[i], (xOf(i) + yOf(i)) % 2 ));
    }
    putchar('\n');
}

// ---- Logic ---- //

void initBoard(Board board) {
    int i;
    Piece p = (Piece) {EMPTY, BLACK};
    for ( i = indexOf(0, 2); i < indexOf(0, 6); i++) board[i] = p;

    for ( i = 0; i < 8; i++ ) {
        int type = charToType("rnbqkbnr"[i]);
        board[indexOf(i, 0)] = (Piece) {type, BLACK};
        board[indexOf(i, 1)] = (Piece) {PAWN, BLACK};
        board[indexOf(i, 7)] = (Piece) {type, WHITE};
        board[indexOf(i, 6)] = (Piece) {PAWN, WHITE};
    }
}

//Returns the max distance slid. Adds possible positions to moves array.
int slide(Pos pos, Pos dir, int maxSlide, int color, Board board, Pos *moves) {
    int count = 0;
    while (count < maxSlide) {
        pos.x += dir.x;
        pos.y += dir.y;
        if (inBoard(pos)) {
            Piece obstacle = getPiece(board, pos);
            if ( obstacle.type != EMPTY ) {
                if( obstacle.color != color ) moves[count++] = pos;
                break;
            }
            moves[count++] = pos;
        } else {
            break;
        }
    }
    return count;
}

int slideDiagonals(Pos pos, int maxSlide, int color, Board board, Pos *moves) {
    int count = 0;
    count += slide(pos, DIR_NE, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_SE, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_SW, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_NW, maxSlide, color, board, moves + count );
    return count;
}

int slideCardinals(Pos pos, int maxSlide, int color, Board board, Pos *moves) {
    int count = 0;
    count += slide(pos, DIR_N, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_E, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_S, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_W, maxSlide, color, board, moves + count );
    return count;
}

int getMoves(Pos pos, int type, bool moved, int color, Board board, Pos *moves) {
    int count = 0;
    switch(type) {
        case KING: {
            count += slideCardinals(pos, 1, color, board, moves + count );
            count += slideDiagonals(pos, 1, color, board, moves + count );
            break; }
        case QUEEN: {
            count += slideCardinals(pos, 7, color, board, moves + count);
            count += slideDiagonals(pos, 7, color, board, moves + count);
            break; }
        case BISHOP: {
            count += slideDiagonals(pos, 7, color, board, moves + count);
            break; }
        case KNIGHT: {
            count += slide(pos, DIR_NNE, 1, color, board, moves + count );
            count += slide(pos, DIR_NEE, 1, color, board, moves + count );
            count += slide(pos, DIR_SEE, 1, color, board, moves + count );
            count += slide(pos, DIR_SSE, 1, color, board, moves + count );
            count += slide(pos, DIR_SSW, 1, color, board, moves + count );
            count += slide(pos, DIR_SWW, 1, color, board, moves + count );
            count += slide(pos, DIR_NWW, 1, color, board, moves + count );
            count += slide(pos, DIR_NNW, 1, color, board, moves + count );
            break; }
        case ROOK: {
            count += slideCardinals(pos, 7, color, board, moves + count);
            break; }
        case PAWN: {
            count += slide(pos, color == BLACK ? DIR_N : DIR_S, moved ? 1 : 2, color, board, moves + count);
            break;}
    }
    return count;
}

int getMovesForPiece(Pos pos, Board board, Pos *moves){
    Piece p = getPiece(board, pos);
    return getMoves(pos, p.type, p.moved, p.color, board, moves);
}

int main()
{
    Board board;
    initBoard(board);
    printBoard(board);

    Pos moves[64];
    Pos pos = (Pos){1, 0};

    int count = getMovesForPiece(pos, board, moves);

    printf("\n%c\n", pieceToChar(getPiece(board, pos), true));
    printf("\nResults count: %i\n", count);

    int i;
    for ( i = 0; i < count; i++ ) {
        printPos(moves[i]);
        putchar('\n');
    }

    return 0;
}
