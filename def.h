#ifndef DEF
#define DEF
#define SQNO 120
#define NAME "Diablo"    // Name of the engine.
#define MAXMOVES 2048  // Maxium number of half moves that can happen in a game.
#define MAXDEPTH 64
#include <stdlib.h>
#include <stdio.h>
#define NOMOVE 0

//#define DEBUG
#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n)                                                                                                      \
    if (!(n))                                                                                                          \
    {                                                                                                                  \
        printf("%s - FAILED\n", #n);                                                                                   \
        printf("ON - %s\n", __DATE__);                                                                                 \
        printf("AT - %s\n", __TIME__);                                                                                 \
        printf("In File - %s\n", __FILE__);                                                                            \
        printf("At line - %d\n", __LINE__);                                                                            \
        exit(1);                                                                                                       \
    }
#endif

typedef unsigned long long int ULL;

// Enumurating costants for later use in arrays.
enum
{
    FALSE,
    TRUE
};

enum
{
    WHITE,
    BLACK,
    BOTH
};

enum
{
    EMPTY,
    wP,
    wN,
    wB,
    wR,
    wQ,
    wK,
    bP,
    bN,
    bB,
    bR,
    bQ,
    bK
};

enum
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NONE
};

enum
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NONE
};

enum
{
    A1 = 21,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2 = 31,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3 = 41,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4 = 51,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5 = 61,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6 = 71,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7 = 81,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8 = 91,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
    NONE,
    OFF
};

enum
{
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};  // Enum weather castleing is possible for the the four types or not represeted using first 4 bits.

// This structures is to undo any move if we want to to go back to certain previous stage of the game.
typedef struct
{
    ULL PosKey;

    int Move;
    int EnPas;
    int FiftyMove;
    int CastlePerm;
} UNDO;

// This structure is the entries which will be present in the PVTABLE. It is a simple pair of the PosKey and the
// move which will be stored in our principal variation table.
typedef struct
{
    ULL PosKey;
    int Move;
} PVENTRY;

// This is the actual PVTABLE structure which simply stores the entries array. Which is the hash array for this
// PV hash table...and it conatins the count of entries.
typedef struct
{
    PVENTRY PvEntries[131070];
    int Count;
} PVTABLE;

// Chess board structure.
typedef struct
{
    int Pieces[SQNO];  				// To know which type of piece is present at the respective index.

    ULL Pawns[3];  					// Bitbprds for white,black,both pawns.

    int PceNum[13];  				// Number of each type of piece present on the board.
    int BigPce[2];   				// Number of all pieces except pawns.
    int MajPce[2];   				// Number of rooks and queens of white,black.
    int MinPce[2];   				// Number of knights and bishops of white, black.
    int KingPos[2];  				// Position of white and black kings.
    int ValPce[2];   				// Stores the total materialistice value for white and black sides.

    int Ply, HisPly;  				// Number of half moves made and hitory to store previous plys.
    int FiftyMove;    				// Check if 50moves draw is valid.
    int EnPas;        				// Check for EnPass move.
    int Side;         				// To know which side has to move.

    int CastlePerm;  				// If the current king can castle or not rpresented by four bits for the above four enumrations.

    ULL PosKey;  					// Unique PosKey generated for every move.

    UNDO History[MAXMOVES];  		// This array stores all the previous game states with essential things which are stated in
                             		// the undo structure and also it helps us to check if a move is repeated or not,we can do
                             		// so by using the HisPly as the index and going back to all the states and see if the
                             		// PosKey is repeated or not.

    int PceList[13][10]; 			// This is a list of position all types of pieces on the board with their position which are
                          			// index through enumurations eg PceList[wn][0]=E1.

    PVTABLE PvTable[1];  			// We keep one PVTABLE in the main structure for the current board position in the form of
                         			// a pointer which is already given memeory.

    int PvArray[MAXDEPTH];  		// This array store the best line of moves upto certain depth for the given position.

    int SearchHistory[13][120];     // This will be used for move ordering for beta cutoff.

    int SearchKillers[2][MAXDEPTH]; // This will also be only used for move ordering basically these are the
                                    // non capture moves which are causing the beta cutoff.

} BOARD;

// This structure stores the information about the move and in a very intresting way of bitmasking.
typedef struct
{
    int Move;
    int Score;
} MOVE;

// This structure is the move list and it contains an array of movess and a counter for these moves
// to count how many moves are present in this array...256 is a MAX number of moves at any step for a
// given piece as we can see any particular piece can never have more number of moves than this at a given state.
typedef struct
{
    MOVE MoveNum[256];
    int Count;
} MOVELIST;

// This structure is simply for the use f GUI when connecting it to arena.
typedef struct
{
    int starttime;
    int stoptime;
    int depth;
    int depthset;
    int timeset;
    int movestogo;
    int infinite;

    long nodes;

    int quit;
    int stopped;
    float fh, fhf;

} SEARCHINFO;

/*
We can see that we any square on the playable board i.e 21 to 98 can be represented using a 7bit number
becaue the max value is 64+32+4 which can be made by setting some bits in a 7 digit number
so we can see that all the aspects of a move can be stored in a number which has 32 bit.

0000 0000 0000 0000 0000 0111 1111 ->from (the masked value of first 7 bits will we be the index from where we start the
move) 

0000 0000 0000 0011 1111 1000 0000 ->to(the masked value of next 7 bits sotre the index where we want to go)

0000 0000 0011 1100 0000 0000 0000 ->captured(the masked value of next four bit ranges from 0 to 15 which will give us the
piece (wP to bK) or value (0 to 12) which is being captured)

0000 0000 0100 0000 0000 0000 0000 ->EnPas (seting of this bit will tell that this move is an EnPas move)

0000 0000 1000 0000 0000 0000 0000 ->PawnSt(if this bit is set that means that this is a starting move of a pawn) 

0000 1111 0000 0000 0000 0000 0000 ->Promoted(these 4 bits indicate that this move can promote to a Knight,Bishop,Queen or Rook each for respective bit) 

0001 0000 0000 0000 0000 0000 0000 ->Castle(setting of this bit tells us that the current move will be a castling move)

So as we can see we can easly store all the information about a move in just a single 32 bit number so
we dont need to maintain seperate variables to store all these.

*/
/*    MACROS    */

#define fr2sq(f, r)                                                                                                    \
    ((21 + f) + (r * 10))  // Takes in input as file and rank and return the square number in the 120 size board.

// Remember FILE_A is index 0 and RANK_1 is index 0
#define SetBit(b, sq) (b |= (1ULL << sq))
#define ClearBit(b, sq) (b &= (~(1ULL << sq)))

// Macros for simple shifting and anding to find the masked value which will give us the index of movement
// and captured type index and promoted type index.
#define FromSq(m) (m & (0x7F))
#define ToSq(m) ((m >> 7) & (0x7F))
#define Captured(m) ((m >> 14) & (0xF))
#define Promoted(m) ((m >> 20) & (0xF))

// These macros are simply the values which we will & with our move to see if the corrsponding bits are set or not.
#define FlagEnPas 0x40000
#define FlagPawnSt 0x80000
#define FlagCastle 0x1000000

// These flag are flag to see if the capturing is hapenning or not so for that we need to any one bit from
// EnPas bit to 4 capyured bit must be on so the on coresponding hexadecimal number will be 0x7C000.
#define FlagCap 0x7C000

// To see if the promotion in this move promotion is hapenning or not so for that any of the 4 bits representing
// the promotion must be on so the coressponding on bit structure in hexadecimal is 0xF00000.
#define FlagPromoted 0xF00000

/*    GLOBALS   */
extern int sq120to64[SQNO];   	// Since we will use these arrays in ever program so we define them globally using extern.
extern int sq64to120[64];
extern ULL HashKey[13][120];  	// Stores the random 64bit values for each piece at each position which are then xored for
                              	// PosKey value.S
extern ULL SideKey;
extern ULL CastleKey[16];

extern char PceChar[];   		// Used for easy indexing and printing the coressponding characters.
extern char SideChar[];  		// Useful as when we pass sat wP as an index it refers to characer P.
extern char RankChar[];  		// Which when printed means a white pawn...and similaarly for the other arrays.
extern char FileChar[];
extern char Fen[];  			// The starting configuration of chessboard.

extern int PceBig[13];  		// True false array which takes the index as enum values of types of peiecs and return true if
                        		// it is a Bigpce.

extern int PceMaj[13];  		// True false array which takes the index as enum values of types of peiecs and return true if
                        		// it is a Majpce.

extern int PceMin[13];  		// True false array which takes the index as enum values of types of peiecs and return true if
                        		// it is a Minpce.

extern int PceVal[13];  		// Gives the matirialistice value of the type of piece accoring to the index base on enum values.
extern int PceCol[13];  		// Gives the colour of the type of Piece index based on enum values of Pieces.
extern int FileNo[SQNO];  		// To store the file of a particualar square no in 120 based indeing.
extern int RankNo[SQNO];  		// To store the rank of a particular square no in 120 based indexing.

extern int IsPawn[13];
extern int IsKnight[13];
extern int IsKing[13];
extern int IsRQ[13];
extern int IsBQ[13];
extern int IsSlide[13];

/*   FUNCTIONS   */
// init.c
extern void AllInit();

// bitboard.c
extern void PrintBitBoard(ULL b);
extern int POP(ULL* b);
extern int COUNT(ULL b);

// hashkey.c
extern ULL GenerateHashKeys(const BOARD* board);

// board.c
extern void ResetBoard(BOARD* board);         				// Reset the board to all 0 and initial states of emptiness.
extern int CalcFen(char* arr, BOARD* board);  				// To set the board according to the given FEN state.
extern void PrintBoard(BOARD* board);
extern void UpdateMaterial(BOARD* board);  					// Update the big,min,maj and pieceList.
extern int CheckBoard(BOARD* board);       					// This function crosscheks everthing we have done to our board so far.

// attack.c
int IsSqAttack(int sq,
               int side,
               BOARD* board);  								// This functin checks weather a given square is attacked by a given side.

// io.c
extern void PrintSq(int sq);                        		// To print the file and rank of a given square.
extern char* PrintMove(int move);                   		// To print the from,to and promoted move.
extern void PrintMoveList(MOVELIST* MoveList);      		// Again a simple function to print a move list for a given piece.

extern int ParseMove(char* ptrChar, BOARD* board);  		// Takes in the move as files and rank and return the move in
                                                    		// integer.

// movegenerator.c
extern void GenMoves(BOARD* board, MOVELIST* MoveList);  	// Generates all the possible move for a given side movelist.
extern int MoveExists(BOARD* board, int Move);           	// Checks if a current move existis on the board or not.
extern void InitMvvLva();  									// This is to initialize the most valuable victim leas valuable attacker array.
extern void GenCapMoves(BOARD* board, MOVELIST* MoveList);  // Generates all the capture moves.

// makemove.c
extern void RemovePiece(BOARD* board, int sq);  			// Change the values of the board structure when a piece is removed.
extern void AddPiece(BOARD* board, int sq, int Pce);        // Opposite to removePiece.
extern void MovePiece(BOARD* board, int from, int to);  	// Call first remove the addpiece.
extern int MakeMove(BOARD* board,
                    int Move);  							// Actually make a move on the board and do the necessary changes to board structure
extern void TakeMove(BOARD* board);  						// Takes back the last move made.

// perfttest.c
extern void PerftTest(int depth, BOARD* pos);  				// These functions are used for perft testing the correctness of
extern void Perft(int depth, BOARD* pos);      				// The move generateor the makemove and takemove functions.

// search.c
extern int IsRepeat(BOARD* board);                      	// Checks if the position was repeated.
extern void SearchPos(BOARD* board, SEARCHINFO* info);  	// Out iterative deepening function.

// misc.c
extern int GetTime();  										// This function is to get the current time from the system clock.
extern void ReadInput(SEARCHINFO* info);

// pvtable.c
extern void ClearPvTable(PVTABLE* PvTable);  				// Resets the pvtable marking all to 0.
extern void InitPvTable(PVTABLE* PvTable);   				// Initilizes the pvtable by giving it memory and clearing it initiallly.
extern void StorePvMove(BOARD* board, int Move);  			// To store the PvMove in the hashtable.
extern int ProbePvMove(BOARD* board);  						// To retrive the move if present for the current poskey value of the board.
extern int GetPvLine(BOARD* board, int depth);  			// This functions give us the best set of move upto a certain depth.

// evaluate.c
extern int EvalPos(BOARD* board);  							// This function give the evaluation value of the current board position from the
                                   							// sides point of view.

// uci.c
extern void Uci_Loop(); 									// The UCI protocol infinite loop used to run the program.

#endif
