/***********************
* Daniel Knafel
* 316012624
* Group 01 - CS
* Assignment 4
***********************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
//#define assert(ignore)((void) 0)

#define SIZE 8

typedef struct
{
	char srcPiece, destPiece, promotionPiece;
	int srcRow, srcCol, destRow, destCol;
	int isWhite, isCapture, isPromotion, isCheck, isMate, isLegal;
	int forCheck, colWasGiven, rowWasGiven;
} Move;

// A struct representing a point (x,y)
typedef struct
{
	int row, col;
} Point;
// A struct representing the amount of lower-case letters and digits in a PGN
typedef struct
{
	int chars, nums;
} PGNSrcAndDest;

const Point NULL_POINT = { -1, -1 };

// All the game pieces
const char whiteGameCharacters[] = { 'P','R','N','B','Q','K','\0' };
const char blackGameCharacters[] = { 'p','r','n','b','q', 'k','\0' };

// FEN carachters
const char FEN_SEP = '/';

// PGN characters
const char CAPTURE = 'x';
const char PROMOTION = '=';
const char CHECK = '+';
const char MATE = '#';

// Board characters
const char EMPTY = ' ';
const int NUMBER = '0';
const int CHAR = 'a';

Point findSrcPiece(char board[][SIZE], Move m1);

/******************
* Function Name: isWhitePiece
* Input: char c
* Output: int
* Function Operation: returns 1 if the given char is a White piece and 0 if not (black piece or empty).
******************/
int isWhitePiece(char c)
{
	if (strchr(whiteGameCharacters, c) != NULL)
		return 1;
	return 0;
}
/******************
* Function Name: isBlackPiece
* Input: char c
* Output: int
* Function Operation: returns 1 if the given char is a Black piece and 0 if not (White piece or empty).
******************/
int isBlackPiece(char c)
{
	if (strchr(blackGameCharacters, c) != NULL)
		return 1;
	return 0;
}
/******************
* Function Name: isNumber
* Input: char c
* Output: int
* Function Operation: returns 1 if the given char is a number (0-9) and 0 if not (White piece or empty).
******************/
int isNumber(char c)
{
	if (c >= '0' && c <= '9')
		return 1;
	return 0;
}
/******************
* Function Name: arePointsEqual
* Input: Point p1, Point p2
* Output: int
* Function Operation: returns 1 if the given points are equal and 0 if not.
******************/
int arePointsEqual(Point p1, Point p2)
{
	if (p1.row == p2.row && p1.col == p2.col)
		return 1;
	return 0;
}
/******************
* Function Name: isPointLegal
* Input: Point p1
* Output: int
* Function Operation: returns 1 if the given point is withing the playing board, and 0 if not.
******************/
int isPointLegal(Point p1)
{
	if (p1.row >= 0 && p1.col >= 0 && p1.row < SIZE && p1.col < SIZE)
		return 1;
	return 0;
}
/******************
* Function Name: toPoint
* Input: int x, int y
* Output: Point
* Function Operation: returns a point struct from 2 integers
******************/
Point toPoint(int x, int y)
{
	Point p;
	p.row = x;
	p.col = y;
	return p;
}
/******************
* Function Name: createBoard
* Input: char board[][SIZE], char fen[]
* Output: Void
* Function Operation: creates the playing board from a FEN string
******************/
void createBoard(char board[][SIZE], char fen[])
{
	int i = 0, rowCounter = 0, columbCounter = 0;
	char c;

	while (fen[i] != '\0')
	{
		// Temp value for the current char in the FEN code
		c = fen[i];

		if (c != FEN_SEP)
		{
			// If the current chracter in the FEN code is a chess character
			if (isWhitePiece(toupper(c)))
			{
				board[rowCounter][columbCounter] = c;
				columbCounter++;
			}
			// If the current chracter in the FEN code is a digit
			else if (isNumber(c))
			{
				for (int j = 0; j < c - NUMBER; j++)
				{
					board[rowCounter][columbCounter + j] = EMPTY;
				}
				// Move to the next ocupied square
				columbCounter += c - NUMBER;
			}
			i++;
		}
		// If the loop reached a seperator, go to the next row
		else
		{
			rowCounter++;
			columbCounter = 0;
			i++;
		}
	}
}
/******************
* Function Name: printColumns
* Input: Void
* Output: Void
* Function Operation: prints the outline of the playing board
******************/
void printColumns() {
	char columb = 'A';
	printf("* |");
	for (int i = 0; i < SIZE; i++)
	{
		if (i)
			printf(" ");
		printf("%c", columb);
		columb++;
	}
	printf("| *\n");
}
/******************
* Function Name: printSpacers
* Input: Void
* Output: Void
* Function Operation: prints the spacers for the playing board
******************/
void printSpacers()
{
	printf("* -");
	for (int i = 0; i < SIZE; i++)
	{
		printf("--");
	}
	printf(" *\n");
}
/******************
* Function Name: printBoard
* Input: char board[][SIZE]
* Output: Void
* Function Operation: prints the playing board
******************/
void printBoard(char board[][SIZE])
{
	printColumns();
	printSpacers();
	for (int i = 0; i < SIZE; i++)
	{
		// Print the numbers on the board from bottom to top, on both sides 
		printf("%d |", SIZE - i);
		// Print each row from the board 
		for (int j = 0; j < SIZE; j++)
			printf("%c|", board[i][j]);
		printf(" %d\n", SIZE - i);
	}
	printSpacers();
	printColumns();
}
/******************
* Function Name: moveCharacter
* Input: char board[][SIZE], Move m1
* Output: Void
* Function Operation: moves a chess piece from its source to its destination, and sets the source to empty
******************/
void moveCharacter(char board[][SIZE], Move m1)
{
	assert(m1.srcRow >= 0 && m1.srcRow < SIZE && m1.srcCol >= 0 && m1.srcCol < SIZE);
	// Make the move
	board[m1.srcRow][m1.srcCol] = EMPTY;
	board[m1.destRow][m1.destCol] = m1.srcPiece;
}
/******************
* Function Name: checkPGNForSrcAndDest
* Input: char pgn[]
* Output: PGNSrcAndDest
* Function Operation: checks the PGN for the amount of lower-case letters and digits
******************/
PGNSrcAndDest checkPGNForSrcAndDest(char pgn[])
{
	int i = 0;
	char c = pgn[i++];
	PGNSrcAndDest p1;

	p1.chars = 0;
	p1.nums = 0;
	// Check the entire PGN
	while (c != '\0')
	{
		// Count the lower-case letters 
		if (islower(c) && c != CAPTURE)
			p1.chars++;
		// Count the digits
		else if (isNumber(c))
			p1.nums++;
		c = pgn[i++];
	}
	return p1;
}
/******************
* Function Name: findKingOnBoard
* Input: char board[][SIZE], int kingColor
* Output: Point
* Function Operation: finds the location of the relevant king on the board
******************/
Point findKingOnBoard(char board[][SIZE], int kingColor)
{
	int row = -1, col = -1;

	// find the king on the board
	for (int i = 0; i < SIZE; i++)
	{

		for (int j = 0; j < SIZE; j++)
		{
			// Search from the top
			if (isBlackPiece(kingColor))
			{
				row = i;
				col = j;
			}
			// Search from the bottom
			else
			{
				row = SIZE - i - 1;
				col = SIZE - j - 1;
			}
			// When the king will be found, return its location on the board
			if (board[row][col] == kingColor)
				return toPoint(row, col);
		}
	}
	// If the king couldn't be found, return NULL
	return NULL_POINT;
}
/******************
* Function Name: setSimulatedMove
* Input: char board[][SIZE], Move m1, Move* m2
* Output: int
* Function Operation: Sets the simulated move's attributes
******************/
void setSimulatedMove(char board[][SIZE], Move m1, Move* m2)
{
	// Set the attributes of the "simulated" move for the current king
	m2->isCapture = 1;
	m2->isCheck = 0;
	m2->srcPiece = EMPTY;
	m2->colWasGiven = 0;
	m2->rowWasGiven = 0;
	// This attribute prevents the search function from valadating self-check on the simulated move
	m2->forCheck = 1;

	// If white is playing, simulate a black move towards the white king
	if (m1.isWhite)
	{
		m2->destPiece = 'K';
		m2->isWhite = 0;
	}
	else
	{
		m2->destPiece = 'k';
		m2->isWhite = 1;
	}
	// if the king is moving, skip finding its position
	if (toupper(m1.srcPiece) == 'K')
	{
		m2->destRow = m1.destRow;
		m2->destCol = m1.destCol;
	}
	else
	{
		// find the king
		Point kingPos = findKingOnBoard(board, m2->destPiece);
		m2->destRow = kingPos.row;
		m2->destCol = kingPos.col;
	}
}
/******************
* Function Name: willMoveCauseCheckOnSelf
* Input: char board[][SIZE], Move m1
* Output: int
* Function Operation: checks if the given move will casuse the current player's king to be in a check position.
*					  it does so by executing the move on the board, and then simulating all possible moves in the next turn.
******************/
int willMoveCauseCheckOnSelf(char board[][SIZE], Move m1)
{
	Move m2 = m1;
	Point foundPos = NULL_POINT;

	setSimulatedMove(board, m1, &m2);

	// Simulate the original move on the board
	moveCharacter(board, m1);
	// Search for the original piece from the king's location
	foundPos = findSrcPiece(board, m2);

	// undo the changes to the board
	board[m1.srcRow][m1.srcCol] = m1.srcPiece;
	board[m1.destRow][m1.destCol] = m1.destPiece;

	// If a piece was found to be threatening the king, the move is ilegal
	if (isPointLegal(foundPos))
		return 1;
	else
		return 0;
}
/******************
* Function Name: willMoveCauseCheckOnOpp
* Input: char board[][SIZE], Move m1
* Output: int
* Function Operation: checks if the given move will casuse the oppoent's king to be in a check position.
*					  return 1 if it does and 0 if it doesn't
******************/
int willMoveCauseCheckOnOpp(char board[][SIZE], Move m1)
{
	Move m2 = m1;
	Point kingPos, foundPos = toPoint(-1, -1);

	// if white is playing, find black king
	if (m2.isWhite)
		m2.destPiece = 'k';
	else
		m2.destPiece = 'K';

	// find the king
	kingPos = findKingOnBoard(board, m2.destPiece);
	m2.destRow = kingPos.row;
	m2.destCol = kingPos.col;
	m2.forCheck = 1;
	m2.srcPiece = EMPTY;
	m2.colWasGiven = 0;
	m2.rowWasGiven = 0;
	m2.isCapture = 1;

	// Simulate the move
	moveCharacter(board, m1);

	// Search for the original piece from the king's location
	foundPos = findSrcPiece(board, m2);
	// undo the changes to the board
	board[m1.srcRow][m1.srcCol] = m1.srcPiece;
	board[m1.destRow][m1.destCol] = m1.destPiece;

	// if the move will cause check on the opponent, return 1
	if (isPointLegal(foundPos))
		return 1;
	else
		return 0;
}
/******************
* Function Name: findPieceOnBoard
* Input: char board[][SIZE], Move m1, int i, int j
* Output: int
* Function Operation: checks if the source piece of the move is at position [destRow + i][destCol + j]
******************/
Point findPieceOnBoard(char board[][SIZE], Move m1, int i, int j)
{
	// Set the off-set to check on the board
	int row = m1.destRow + i, col = m1.destCol + j;
	Point srcPoint = toPoint(row, col);

	if (isPointLegal(srcPoint))
	{
		// if the srcPiece was found in the given off-set
		if (board[row][col] == m1.srcPiece)
		{
			// If this is not a self-check search
			if (!m1.forCheck)
			{
				m1.srcRow = row;
				m1.srcCol = col;
				if (!willMoveCauseCheckOnSelf(board, m1))
					return srcPoint;
			}
			else
				return srcPoint;
		}
		// The path to the destination from this location is blocked
		else if (board[row][col] != EMPTY)
			return NULL_POINT;
	}
	// Reached end of the board
	return toPoint(SIZE, SIZE);
}
/******************
* Function Name: canPawnReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: finds the source piece of the move if it is a Pawn
******************/
Point canPawnReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;
	Point src;
	src.row = -1;
	src.col = -1;

	if (m1.isCapture)
	{
		if (m1.isWhite)
		{
			if (destRow + 1 < SIZE)
			{
				if (m1.colWasGiven)
				{
					if (board[destRow + 1][m1.srcCol] == m1.srcPiece)
					{
						src.row = destRow + 1;
						src.col = m1.srcCol;
					}
				}
				else
				{
					if (board[destRow + 1][m1.destCol + 1] == m1.srcPiece)
					{
						src.row = destRow + 1;
						src.col = m1.destCol + 1;
					}
					if (board[destRow + 1][m1.destCol - 1] == m1.srcPiece)
					{
						src.row = destRow + 1;
						src.col = m1.destCol - 1;
					}
				}
			}
		}
		else
		{
			if (destRow - 1 >= 0)
			{
				if (m1.colWasGiven)
				{
					if (board[destRow - 1][m1.srcCol] == m1.srcPiece)
					{
						src.row = destRow - 1;
						src.col = m1.srcCol;
					}
				}
				else
				{
					if (board[destRow - 1][m1.destCol + 1] == m1.srcPiece)
					{
						src.row = destRow - 1;
						src.col = m1.destCol - 1;
					}
					if (board[destRow - 1][m1.destCol - 1] == m1.srcPiece)
					{
						src.row = destRow - 1;
						src.col = m1.destCol - 1;
					}
				}
			}
		}
	}
	else
	{
		if (m1.isWhite)
		{
			if (destRow == SIZE - 4 && destRow + 2 < SIZE)
			{
				if (board[destRow + 2][destCol] == m1.srcPiece)
				{
					src.row = destRow + 2;
					src.col = destCol;
				}
			}
			if (destRow + 1 < SIZE)
			{
				if (board[destRow + 1][destCol] == m1.srcPiece)
				{
					src.row = destRow + 1;
					src.col = destCol;
				}
			}
		}
		else
		{
			if (destRow == 3 && destRow - 2 >= 0)
			{
				if (board[destRow - 2][destCol] == m1.srcPiece)
				{
					src.row = destRow - 2;
					src.col = destCol;
				}
			}
			if (destRow - 1 >= 0)
			{
				if (board[destRow - 1][destCol] == m1.srcPiece)
				{
					src.row = destRow - 1;
					src.col = destCol;
				}
			}
		}
	}
	return src;
}
/******************
* Function Name: canRookReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: finds the source piece of the move if it is a Rook
******************/
Point canRookReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;
	// Booleans to keep track of blocked paths
	int isNorth = 1, isSouth = 1, isEast = 1, isWest = 1;
	Point src = NULL_POINT;

	for (int i = 1; i < SIZE; i++)
	{
		if (isNorth)
		{
			src = findPieceOnBoard(board, m1, -i, 0);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorth = 0;
		}
		if (isSouth)
		{
			src = findPieceOnBoard(board, m1, i, 0);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouth = 0;
		}
		if (isEast)
		{
			src = findPieceOnBoard(board, m1, 0, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isEast = 0;
		}
		if (isWest)
		{
			src = findPieceOnBoard(board, m1, 0, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isWest = 0;
		}
	}
	return toPoint(-1, -1);
}
/******************
* Function Name: canKnightReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: finds the source piece of the move if it is a Knight
******************/
Point canKnightReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;
	Point src = NULL_POINT;

	src = findPieceOnBoard(board, m1, 1, 2);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, 1, -2);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, 2, 1);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, 2, -1);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, -1, 2);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, -1, -2);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, -2, 1);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}

	src = findPieceOnBoard(board, m1, -2, -1);
	if (isPointLegal(src))
	{
		if (m1.colWasGiven)
		{
			if (m1.srcCol == src.col)
				return src;
		}
		else
			return src;
	}
	return NULL_POINT;
}
/******************
* Function Name: canBishopReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: finds the source piece of the move if it is a Bishop
******************/
Point canBishopReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;
	// Booleans to keep track of blocked paths
	int isNorthEast = 1, isNorthWest = 1, isSouthEast = 1, isSouthWest = 1;
	Point src = NULL_POINT;

	for (int i = 1; i < SIZE; i++)
	{
		if (isNorthEast)
		{
			src = findPieceOnBoard(board, m1, -i, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorthEast = 0;
		}
		if (isNorthWest)
		{
			src = findPieceOnBoard(board, m1, -i, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorthWest = 0;
		}
		if (isSouthEast)
		{
			src = findPieceOnBoard(board, m1, i, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouthEast = 0;
		}
		if (isSouthWest)
		{
			src = findPieceOnBoard(board, m1, i, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouthWest = 0;
		}
	}
	return NULL_POINT;
}
/******************
* Function Name: canQueenReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: finds the source piece of the move if it is a Queen
******************/
Point canQueenReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;
	// Booleans to keep track of blocked paths
	int isNorth = 1, isSouth = 1, isEast = 1, isWest = 1, isNorthEast = 1, isNorthWest = 1, isSouthEast = 1, isSouthWest = 1;
	Point src = NULL_POINT;

	for (int i = 1; i < SIZE; i++)
	{
		if (isNorth)
		{
			src = findPieceOnBoard(board, m1, -i, 0);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorth = 0;
		}
		if (isSouth)
		{
			src = findPieceOnBoard(board, m1, i, 0);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouth = 0;
		}
		if (isEast)
		{
			src = findPieceOnBoard(board, m1, 0, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isEast = 0;
		}
		if (isWest)
		{
			src = findPieceOnBoard(board, m1, 0, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isWest = 0;
		}
		if (isNorthEast)
		{
			src = findPieceOnBoard(board, m1, -i, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorthEast = 0;
		}
		if (isNorthWest)
		{
			src = findPieceOnBoard(board, m1, -i, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isNorthWest = 0;
		}
		if (isSouthEast)
		{
			src = findPieceOnBoard(board, m1, i, i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouthEast = 0;
		}
		if (isSouthWest)
		{
			src = findPieceOnBoard(board, m1, i, -i);
			if (isPointLegal(src))
			{
				if (m1.colWasGiven)
				{
					if (m1.srcCol == src.col)
						return src;
				}
				else
					return src;
			}
			else if (arePointsEqual(src, NULL_POINT))
				isSouthWest = 0;
		}
	}
	return NULL_POINT;
}
/******************
* Function Name: canKingReach
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: Finds the source piece of the move if it is a King
******************/
Point canKingReach(char board[][SIZE], Move m1)
{
	int destRow = m1.destRow, destCol = m1.destCol;

	if (isPointLegal(findPieceOnBoard(board, m1, 1, 0)))
		return toPoint(destRow + 1, destCol);
	if (isPointLegal(findPieceOnBoard(board, m1, 0, 1)))
		return toPoint(destRow, destCol + 1);
	if (isPointLegal(findPieceOnBoard(board, m1, -1, 0)))
		return toPoint(destRow - 1, destCol);
	if (isPointLegal(findPieceOnBoard(board, m1, 0, -1)))
		return toPoint(destRow, destCol - 1);
	if (isPointLegal(findPieceOnBoard(board, m1, 1, 1)))
		return toPoint(destRow + 1, destCol + 1);
	if (isPointLegal(findPieceOnBoard(board, m1, 1, -1)))
		return toPoint(destRow + 1, destCol - 1);
	if (isPointLegal(findPieceOnBoard(board, m1, -1, 1)))
		return toPoint(destRow - 1, destCol + 1);
	if (isPointLegal(findPieceOnBoard(board, m1, -1, -1)))
		return toPoint(destRow - 1, destCol - 1);

	return NULL_POINT;
}
/******************
* Function Name: isPieceFoundFromKing
* Input: char board[][SIZE], Move m1
* Output: int
* Function Operation: Checks if the king is under check by any piece on the board. return 1 if it is and 0 if it is not
******************/
Point isPieceFoundFromKing(char board[][SIZE], Move m1)
{
	Point src = toPoint(-1, -1);

	// an array of pointer to all of the "find piece's" functions
	Point(*func_ptr[])(char[][SIZE], Move) = { &canPawnReach, &canRookReach, &canKnightReach, &canBishopReach, &canQueenReach, &canKingReach };
	int size = sizeof(func_ptr) / sizeof(Point*);

	// check all possible characters with their appropriate movement pattern
	for (int i = 0; i < size; i++)
	{
		if (m1.isWhite)
			m1.srcPiece = whiteGameCharacters[i];
		else
			m1.srcPiece = blackGameCharacters[i];

		src = (*func_ptr[i])(board, m1);
		if (arePointsEqual(src, toPoint(-1, -1)) != 1)
			return src;
	}
	return toPoint(-1, -1);
}
/******************
* Function Name: findSrcPiece
* Input: char board[][SIZE], Move m1
* Output: Point
* Function Operation: Calls the relevant function to the source piece of the move, and returns it's location
******************/
Point findSrcPiece(char board[][SIZE], Move m1)
{
	Point src = toPoint(-1, -1);

	// Choose the correct search function for the character
	switch (toupper(m1.srcPiece))
	{
	case 'P':
		src = canPawnReach(board, m1);
		break;
	case 'R':
		src = canRookReach(board, m1);
		break;
	case 'N':
		src = canKnightReach(board, m1);
		break;
	case 'B':
		src = canBishopReach(board, m1);
		break;
	case 'Q':
		src = canQueenReach(board, m1);
		break;
	case 'K':
		src = canKingReach(board, m1);
		break;
		// wildcard for check assertion
	case ' ':
		src = isPieceFoundFromKing(board, m1);
		break;
	default:
		src.row = -1;
		src.col = -1;
		break;

	}
	return src;
}
/******************
* Function Name: setMove
* Input: Move* m1, char pgn[]
* Output: Void
* Function Operation: Decode the PGN and store to Move m1
******************/
void setMove(Move* m1, char pgn[])
{
	int i = 0;
	PGNSrcAndDest p1 = checkPGNForSrcAndDest(pgn);

	m1->forCheck = 0;
	m1->srcRow = -1;
	m1->srcCol = -1;

	// Set the srcPiece from PGN
	if (isWhitePiece(pgn[i]))
		m1->srcPiece = pgn[i++];
	else
		m1->srcPiece = 'P';

	// Set the correct color for the srcPiece 
	if (!m1->isWhite)
		m1->srcPiece = tolower(m1->srcPiece);

	// if source colomn or row was given in the PGN
	if (p1.chars > 1)
	{
		m1->srcCol = pgn[i++] - CHAR;
		m1->colWasGiven = 1;
		if (p1.nums > 1)
		{
			m1->srcRow = pgn[i++] - NUMBER;
			m1->rowWasGiven = 1;
		}
		else
		{
			m1->srcRow = -1;
			m1->rowWasGiven = 0;
		}
	}

	if (pgn[i] == CAPTURE)
	{
		m1->isCapture = 1;
		i++;
	}

	// set destination and convert to indexes
	m1->destCol = pgn[i++] - CHAR;
	m1->destRow = SIZE - (pgn[i++] - NUMBER);

	assert(m1->destRow >= 0 && m1->destRow < SIZE && m1->destCol >= 0 && m1->destCol < SIZE);
	if (isPointLegal(toPoint(m1->destRow, m1->destCol)))
		m1->isLegal = 1;
	else
		m1->isLegal = 0;

	// if the PGN has ended
	if (i >= strlen(pgn))
		return;

	if (pgn[i] == PROMOTION)
	{
		// if apromotion is indicated, make sure the piece is a Pawn and the destination of the move is the last row of the board
		if (toupper(m1->srcPiece) == 'P' && (m1->destRow == 0 || m1->destRow == SIZE - 1))
		{
			m1->isPromotion = 1;
			i++;

			if (isWhitePiece(pgn[i]) && pgn[i] != 'K' && pgn[i] != 'P')
			{
				m1->promotionPiece = pgn[i++];
				if (!m1->isWhite)
					m1->promotionPiece = tolower(m1->promotionPiece);
			}
			// promotion indicated with no piece to promote to
			else
				m1->isLegal = 0;
		}
		else
			m1->isLegal = 0;
	}

	if (pgn[i] == CHECK)
		m1->isCheck = 1;

	if (pgn[i] == MATE)
		m1->isMate = 1;
}
/******************
* Function Name: isColorMatching
* Input: Move* m1
* Output: int
* Function Operation: Checks if the current player is allowed to move the source piece
******************/
int isColorMatching(Move m1)
{
	// if the character being moved mathces the player's color
	if (isWhitePiece(m1.srcPiece) != m1.isWhite && isBlackPiece(m1.srcPiece) != !m1.isWhite)
		return 0;
	return 1;
}
/******************
* Function Name: isCaptureLegal
* Input: Move m1
* Output: int
* Function Operation: Checks if the capture is legal (not capturing own piece, not capturing with no capture indicate etc.)
******************/
int isCaptureLegal(Move m1)
{
	// if the character being captured is not the same color
	if (m1.isCapture)
	{
		// if the player tries to capture his own piece
		if (m1.isWhite == isWhitePiece(m1.destPiece) && !m1.isWhite == isBlackPiece(m1.destPiece))
			return 0;
		// if the player tries to capture an empty spot
		if (m1.destPiece == EMPTY)
			return 0;
	}
	return 1;
}
/******************
* Function Name: isDestLegal
* Input: Move m1
* Output: int
* Function Operation: Checks if the destination of the move is legal
******************/
int isDestLegal(Move m1)
{
	// if the player is trying to move to an occupied spot with no Capture indicated
	if (!m1.isCapture && m1.destPiece != EMPTY)
		return 0;
	return 1;
}
/******************
* Function Name: isSrcLegal
* Input: Move m1
* Output: int
* Function Operation: Checks if the source of the move is legal
******************/
int isSrcLegal(Move m1)
{
	Point src = toPoint(m1.srcRow, m1.srcCol);
	Point dest = toPoint(m1.destRow, m1.destCol);

	if (m1.srcRow < 0 || m1.srcRow >= SIZE || m1.srcCol < 0 || m1.srcCol >= SIZE)
		return 0;
	// if the detination is the same as the source 
	if (arePointsEqual(dest, src))
		return 0;
	return 1;
}
/******************
* Function Name: checkLegal
* Input: char board[][SIZE], Move m1
* Output: int
* Function Operation: performs all the valitidy checks
******************/
int checkLegal(char board[][SIZE], Move m1)
{
	// an array of pointer to functions, to perform validity checks
	int(*func_ptr[])(Move) = { &isColorMatching, &isDestLegal, &isCaptureLegal, &isSrcLegal };
	int size = sizeof(func_ptr) / sizeof(int*);

	int isLegal = 1;

	int willCausecheckOnOpp = willMoveCauseCheckOnOpp(board, m1);
	// confirm valid check declaration
	if ((willCausecheckOnOpp && (m1.isCheck || m1.isMate)) || (!willCausecheckOnOpp && !m1.isCheck))
		isLegal = 1;
	else
		isLegal = 0;

	// performe all the validity checks 
	for (int i = 0; i < size && isLegal; i++)
		isLegal = (*func_ptr[i])(m1);

	return isLegal;
}
/******************
* Function Name: setPromotion
* Input: Move* m1
* Output: void
* Function Operation: changes the source piece of a Pawn to its promotion piece, if it reaches the end of the board
******************/
void setPromotion(Move* m1)
{
	if (toupper(m1->srcPiece) == 'P')
	{
		if (m1->isPromotion)
		{
			if (m1->destRow == 0 || m1->destRow == SIZE - 1)
			{
				if (m1->isWhite)
					m1->srcPiece = toupper(m1->promotionPiece);
				else
					m1->srcPiece = tolower(m1->promotionPiece);
			}
			else
				m1->isLegal = 0;
		}
	}
}
/******************
* Function Name: makeMove
* Input: char board[][SIZE], char pgn[], int isWhiteTurn
* Output: int
* Function Operation: Creates a Move, set's its members from the PGN and its color, checks valitidy of the move,
					  makes promotion if needed, and performs the move if it's legal. returns 1 if the move was made and 0 if not.
******************/
int makeMove(char board[][SIZE], char pgn[], int isWhiteTurn)
{
	// set the deafult values of the move
	Move m1 = { 0,0,0,-1,-1,0,0,-1,0,0,0,0,0,0,0,1 };
	Point src = NULL_POINT;

	m1.isWhite = isWhiteTurn;
	// decode the PGN and set the Move struct
	setMove(&m1, pgn);
	// set the destination piece of the move (may be empty)
	m1.destPiece = board[m1.destRow][m1.destCol];

	// if setMove didn't detect ilegal information in the PGN
	if (m1.isLegal)
	{
		//if not provided in the PGN, locate the piece to move
		if (m1.srcCol < 0 || m1.srcRow < 0)
		{
			src = findSrcPiece(board, m1);
			m1.srcRow = src.row;
			m1.srcCol = src.col;
		}
	}
	assert(m1.isLegal);
	if (!isPointLegal(src))
		return 0;

	// set the promotion if needed
	setPromotion(&m1);
	m1.isLegal = checkLegal(board, m1);

	if (m1.isLegal)
	{
		moveCharacter(board, m1);
		return 1;
	}
	return 0;
}