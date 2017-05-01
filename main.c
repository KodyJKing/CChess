#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// ---- Declarations ---- //

int charToType(char c);

// ---- Representation ---- //

#define EMPTY  0
#define KING   1
#define QUEEN  2
#define BISHOP 3
#define KNIGHT 4
#define ROOK   5
#define PAWN   6

#define WHITE  0
#define BLACK  1

#define NULL_PIECE (Piece){0, 0, 0}

#define DIR_N  (Vec){ 0,   1}
#define DIR_NE (Vec){ 1,   1}
#define DIR_E  (Vec){ 1,   0}
#define DIR_SE (Vec){ 1,  -1}
#define DIR_S  (Vec){ 0,  -1}
#define DIR_SW (Vec){-1,  -1}
#define DIR_W  (Vec){-1,   0}
#define DIR_NW (Vec){-1,   1}

// knight movement
#define DIR_NNE (Vec){ 1,  2}
#define DIR_NEE (Vec){ 2,  1}
#define DIR_SEE (Vec){ 2, -1}
#define DIR_SSE (Vec){ 1, -2}
#define DIR_SSW (Vec){-1, -2}
#define DIR_SWW (Vec){-2, -1}
#define DIR_NWW (Vec){-2,  1}
#define DIR_NNW (Vec){-1,  2}

typedef struct { unsigned int type:3, color:1; bool moved:1; } Piece;
typedef struct { unsigned int x:4, y:4; }                      Vec;
typedef struct { Vec from, to; Piece captured; }               Move;
typedef Piece                                                  Board[64];
typedef struct { Vec kings[2]; int turn; Board board; }        Game;

bool vecEqual(Vec a, Vec b) { return a.x == b.x &&  a.y == b.y; }

int vecIndex(Vec pos) { return pos.x + pos.y * 8; }
Vec indexVec(int index) {
    Vec pos;
    pos.x = index * 8;
    pos.y = (index - pos.x) / 8;
    return pos;
}

int indexOf(int x, int y) { return x + y * 8; }

int xOf(int index) { return index % 8; }
int yOf(int index) { return (index - (index % 8)) / 8; }

bool inBoard(Vec pos) { return pos.x >= 0 && pos.x < 8 && pos.y >= 0 && pos.y < 8; }
Piece getPiece(Board board, Vec pos) { return board[vecIndex(pos)]; }
void setPiece(Board board, Vec pos, Piece piece) { board[vecIndex(pos)] = piece; }
bool occupied(Board board, Vec pos) { return inBoard(pos) && getPiece(board, pos).type != EMPTY; }

void initBoard(Board board) {
    int i;
    Piece p = (Piece){EMPTY, WHITE};
    for ( i = indexOf(0, 2); i < indexOf(0, 6); i++) board[i] = p;

    for ( i = 0; i < 8; i++ ) {
        int type = charToType("rnbqkbnr"[i]);
        board[indexOf(i, 0)] = (Piece){type, BLACK};
        board[indexOf(i, 1)] = (Piece){PAWN, BLACK};
        board[indexOf(i, 7)] = (Piece){type, WHITE};
        board[indexOf(i, 6)] = (Piece){PAWN, WHITE};
    }
}

// ---- Game Logic ---- //

//Returns the max distance slid. Adds possible positions to moves array.
int slide(Vec pos, Vec dir, int maxSlide, int color, Board board, Vec *moves) {
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

int slideCardinals(Vec pos, int maxSlide, int color, Board board, Vec *moves) {
    int count = 0;
    count += slide(pos, DIR_N, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_E, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_S, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_W, maxSlide, color, board, moves + count );
    return count;
}

int slideDiagonals(Vec pos, int maxSlide, int color, Board board, Vec *moves) {
    int count = 0;
    count += slide(pos, DIR_NE, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_SE, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_SW, maxSlide, color, board, moves + count );
    count += slide(pos, DIR_NW, maxSlide, color, board, moves + count );
    return count;
}

int getMoves(Vec pos, int type, bool moved, int color, Board board, Vec *moves) {
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
            int slides;

            slides = slide(pos, color == BLACK ? DIR_N : DIR_S, moved ? 1 : 2, color, board, moves + count);
            count += slides;
            if (slides > 0 && occupied(board, moves[count - 1]) ) count--; //Pawns can't capture forward.

            slides = slide(pos, color == BLACK ? DIR_NE : DIR_SE, 1, color, board, moves + count);
            count += slides;
            if (slides > 0 && !occupied(board, moves[count - 1]) ) count--; //Pawns MUST capture diagonal.

            slides = slide(pos, color == BLACK ? DIR_NW : DIR_SW, 1, color, board, moves + count);
            count += slides;
            if (slides > 0 && !occupied(board, moves[count - 1]) ) count--; //Pawns MUST capture diagonal.
            break;}
    }
    return count;
}

int getMovesForPiece(Vec pos, Board board, Vec *moves){
    Piece p = getPiece(board, pos);
    return getMoves(pos, p.type, p.moved, p.color, board, moves);
}

//Capturing moves are direction reversible so it's OK to scan from the friendly piece to the enemy pieces.
//If we do this for each type, we can be sure no pieces threaten our piece.
//If we can 'capture' them, they can capture us.
bool isSafe(Board board, Vec pos) {
    int type, color, count, i;
    color = getPiece(board, pos).color;
    Vec moves[64];
    for ( type = 0; type < 6; type++ ) {
        //It's safe to assume the enemy piece has moved since there are no capturing moves which are only first moves.
        //(Pawn opening and castling are non-capturing.)
        count = getMoves(pos, type, true, color, board, moves);
        for ( i = 0; i < count; i++ ) {
                if ( getPiece(board, moves[i]).type == type )
                    return false;
        }
    }
    return true;
}

Move movePiece(Board board, Vec from, Vec to) {
    Piece p = getPiece(board, from);
    Piece captured = getPiece(board, to);
    p.moved = true;
    setPiece(board, from, NULL_PIECE);
    setPiece(board, to, p);
    return (Move){from, to, captured};
}

void undoMove(Board board, Move move) {
    Piece p = getPiece(board, move.to);
    setPiece(board, move.to, move.captured);
    setPiece(board, move.from, p);
}

bool isLegal(Board board, Vec from, Vec to) {
    Vec moves[64];
    int count = getMovesForPiece(from, board, moves);
    int i;
    for ( i = 0; i < count; i++ ) {
            if ( vecEqual(moves[i], to) ) {
                if (getPiece(board, from).type != KING) return true;
                Move move = movePiece(board, from, to);
                bool safe = isSafe(board, to);
                undoMove(board, move);
                return safe;
            }
    }
    return false;
}

// ---- Input / Output ---- //

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

char pieceToChar(Piece p, Vec pos) {
    if ( p.type == EMPTY ) return ",."[(pos.x + pos.y) % 2];
    char result = "kqbnrp"[p.type - 1];
    return p.color == WHITE ? result : toupper(result);
}

char xChar(int x) { return 'a' + x; }
char yChar(int y) { return '0' + y; }

int charX(char c) { return (int)(c - 'a'); }
int charY(char c) { return (int)(c - '0'); }

void printVec(Vec pos) { printf("%c%c", xChar(pos.x), yChar(pos.y)); }

void printBoard(Board board) {
    printf("\n    abcdefgh\n    ________");
    int i;
    for( i = 0; i < 64; i++ ) {
        if ( xOf(i) == 0 ) printf("\n %d |", yOf(i));
        putchar(pieceToChar( board[i], indexVec(i)));
    }
    printf("\n");
}

void parseVec(char *input, Vec *v) {
    while (true) {
        if (input[2] != '\0') {
            printf("\nInput is too long! Use form xy:\n");
            continue;
        }

        v -> x = charX(input[0]);
        v -> y = charY(input[1]);
        if ( inBoard(*v) ) break;
        else printf("\nThat's not on the board!\nx must be in a-h\ny must be in 0-7\n");
        scanf("%s", input);
    }
}

void requestMove(Board board, Vec *from, Vec *to) {
    char input[6];

    while (true) {

        printf("\nEnter start:\n");
        scanf("%s", input);
        parseVec(input, from);

        Vec moves[64];
        int count = getMovesForPiece(*from, board, moves);

        if ( count <= 0 ) {
            printf("\nNo possible moves!\n");
            continue;
        }

        printf("\nPossible moves: ");
        int i;
        for ( i = 0; i < count; i++ ) {
            printVec(moves[i]);
            if (i != count - 1) printf(", ");
        }

        printf("\nEnter destination or type 'back' to go back:\n");
        scanf("%s", input);

        if (strcmp(input, "back") == 0) continue;

        parseVec(input, to);
        return;
    }
}

// ---- Program ---- //

int main()
{
    Board board;
    initBoard(board);
    printBoard(board);

    Vec from, to;
    while (true) {
        requestMove(board, &from, &to);
        printf("\n"); printVec(from); printf("-"); printVec(to); printf("\n");
        if (isLegal(board, from, to)) {
            movePiece(board, from, to);

            printBoard(board);
        } else {
            printf("\nThat move is illegal!\n");
        }
    }

    return 0;
}
