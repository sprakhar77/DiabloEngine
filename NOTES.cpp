/*--------------------------------------------------------------------def.h----------------------------------------------------------------*/

	define various enums and macros for easy idexing of all types of pieces

	//enumurating costants for later use in arrays
	enum {FALSE,TRUE};
	enum {WHITE,BLACK,BOTH};
	enum {EMPTY,wP,wN,wB,wR,wQ,wK,bP,bN,bB,bR,bQ,bK};
	enum {FILE_A,FILE_B,FILE_C,FILE_D,FILE_E,FILE_F,FILE_G,FILE_H,FILE_NONE};
	enum {RANK_1,RANK_2,RANK_3,RANK_4,RANK_5,RANK_6,RANK_7,RANK_8,RANK_NONE};

	enum{
		A1=21,B1,C1,D1,E1,F1,G1,H1,
		A2=31,B2,C2,D2,E2,F2,G2,H2,
		A3=41,B3,C3,D3,E3,F3,G3,H3,
		A4=51,B4,C4,D4,E4,F4,G4,H4,
		A5=61,B5,C5,D5,E5,F5,G5,H5,
		A6=71,B6,C6,D6,E6,F6,G6,H6,
		A7=81,B7,C7,D7,E7,F7,G7,H7,
		A8=91,B8,C8,D8,E8,F8,G8,H8,NONE,OFF
	};

	enum {WKCA=1,WQCA=2,BKCA=4,BQCA=8}; //enum weather castleing is possible for the the four types or not...represeted using first 4 bits


	the board which we use is from 0 to 120 which is playable only from 21 to 98...
	which will be equivalent to 0 to 63 for a chess borad
	so we use the function and two arrays
	which interconverts thes boards...

	we then define the board structure wth the contents as 

/*----------------BOARD-----------------------------*/

		int Pieces[SQNO]; //to know which type of piece is present at the respective index	

		ULL Pawns[3];	  //bitbprds for white,black,both pawns

		int PceNum[13];	  //number of each type of piece present on the board
		int BigPce[2];	  //number of all pieces except pawns	
		int MajPce[2];    //number of rooks and queens of white,black 
		int MinPce[2];    //number of knights and bishops of white,black  
		int KingPos[2];	  //position of white and black kings
		int ValPce[2];  //stores the total materialistice value for white and black sides

		int Ply,HisPly;  //number of half moves made and hitory to store previous plys
		int FiftyMov;    //check if 50moves draw is valid 
		int EnPas;       //check for EnPass move
		int Side;        //to know which side has to move

		int CastlePerm;  //if the current king can castle or not rpresented by four bits for the above four enumrations
			
		ULL PosKey;     //unique PosKey generated for every move

		UNDO History[MAXMOVES]; //this array stores all the previous game states with essential things which are stated in the undo
					//structure and also it helps us to check if a move is repeated or not,we can do so by using the
					//HisPly as the index and going back to all the states and see if the PosKey is repeated or not 
		
		int PceList[13][10];
		
		
		PVTABLE PvTable[1];//we keep one PVTABLE in the main structure for the current board position...in the form of
					  // a pointer which is already given memeory...
		int PvArray[MAXDEPTH];//this array store the best line of moves upto certain depth...for the given position				  
	
		int SearchHistory[13][120];//this will be used for move ordering for beta cutoff 
		int SearchKillers[2][MAXDEPTH];//this will also be only used for move ordering...basically these are the 
									//non capture moves which are causing the beta cutoff
	
	
	
		
	then we define out UNDO structure to keep track of all the previous game moves so that we can crosscheck for 50moves and move repetion
	also we can undo back to any move we want to...


/*-------------------------------UNDO----------------------------*/
	//this structures is to undo any move if we want to to go back to certain previous stage of the game
	typedef struct{

		ULL PosKey;	
		
		int move;
		int EnPas;
		int FiftyMov;
		int CastlePerm;
	}UNDO;

	ClearBit(b,sq)
	this Macro usets the bit at numbersq from the bitboard

	SetBit(b,sq)
	this Macro sets the bit at numbersq from the bitboard

	we also define all the global arrays and function
	with extern keyword so that the can be accessed globally
	
	
	/*--------------------------------------MOVE-----------------------------------------------------------*/
	
	typedef struct{
		int Move;
		int Score;
	}MOVE;
	//this structure stores the information about the move and in a very intresting wa of bitmasking...


	we can see that we any square on the playable board i.e 21 to 98 can be represented using a 7bit number
	becaue the max value is 64+32+4 which can be made by setting some bits in a 7 digit number 
	so we can see that all the aspects of a move can be stored in a number which has 32 bit...

	0000 0000 0000 0000 0000 0111 1111 ->from (the masked value of first 7 bits will we be the index from where we start the move)
	0000 0000 0000 0011 1111 1000 0000 ->to(the masked value of next 7 bits sotre the index where we want to go)
	0000 0000 0011 1100 0000 0000 0000 ->captured(the masked value of next four bit ranges from 0 to 15 which will
												  give us the piece (wP to bK) or value (0 to 12) which is being captured)
	0000 0000 0100 0000 0000 0000 0000 ->EnPas (seting of this bit will tell that this move is an EnPas move) 
	0000 0000 1000 0000 0000 0000 0000 ->PawnSt(if this bit is set that means that this is a starting move of a pawn)
	0000 1111 0000 0000 0000 0000 0000 ->Promoted(these 4 bits indicate that this move can promote to a Knight,Bishop,Queen or Rook each for respective bit)
	0001 0000 0000 0000 0000 0000 0000 ->Castle(setting of this bit tells us that the current move will be a castling move)

	so as we can see we can easly store all the information about a move in just a single 32 bit number so
	we dont need to maintain seperate variables to store all these
	
	//macros for simple shifting and anding to find the masked value which will give us the index of movement
	//and captured type index and promoted type index
	#define FromSq(m) (m & (0x7F))
	#define ToSq(m) ((m>>7) & (0x7F))
	#define Captured(m) ((m>>14) & (0xF))
	#define Promoted(m) ((m>>20) & (0xF))

	//thes macros are simply the values which we will & with our move to see if the corrsponding bits are set or not
	#define FlagEnPas 0x40000
	#define FlagPawnSt 0x80000
	#define FlagCastle 0x1000000

	//these flag are flag to see if the capturing is hapenning or not so for that we need to any one bit from 
	//EnPas bit to 4 capyured bit must be on so the on coresponding hexadecimal number will be 0x7C000
	#define FlagCap 0x7C000

	//to see if the promotion in this move promotion is hapenning or not so for that any of the 4 bits representing
	//the promotion must be on so the coressponding on bit structure in hexadecimal is 0xF00000
	#define FlagPromoted 0xF00000
	
	
	//this structure is the entries which will be present in the PVTABLE...it is a simple pair of the PosKey and the
	//move which will be stored in our principal variation table
	typedef struct{
		ULL PosKey;
		int Move;
	}PVENTRY;


	//this is the actual PVTABLE structure which simply stores the entries array...which is the hash array for this
	//PV hash table...and it conatins the count of entries
	typedef struct{
		PVENTRY PvEntries[131070];
		int Count;
	}PVTABLE;
	
	
	//this structure is simply for the use f GUI....when connecting it to arena
	typedef struct{
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
		float fh,fhf;
		
		
	}SEARCHINFO;


/*-----------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------------------------INIT.cpp--------------------------------------------------------------*/
	this file does all our initialization task.....which are

	Convert();
	this function interconverts the two types of boards
	using arrays
	sq120to64[120];
	sq64to120[64];


	InitHashKeys();
	this function initializes our hashkey values from our arrays
	ULL HashKey[13][120];  //to store a random 64bit no which states that that type of element is present on that sq no
	ULL SideKey;           //to store a random numbeer if it is the turn of white in the game
	ULL CastleKey[16];     //to store 64  bits random numbers which represent each possible type of castlepositions which are 0 to 15
	which are later xored according to the position of piece to genereate a unique HashKey value for
	current board condtion...repetition of a HashKey value 3 times in a games indicates 3xrepetition and the game is draw


	void InitializeFileAndRanks();
	this function initializes our RankNo and FileNo array wihch for any given 12 based square as an index
	will give its rank and file
	uses the arrays 
	FileNo[]
	RankNo[];

	AllInit();
	this function just act to call the remaning functions.....

	#define rand_64 ((ULL)rand() | \
			(ULL)rand()<<15 | \
			(ULL)rand()<<30 | \
			((ULL)rand()<<45 | \
			((ULL)rand() & 0xf)<<60))
	this is a modified rand function which generates 64 bit random numbers for our hashkey values 

/*----------------------------------------------------------------------------------------------------------------------------------------*/






/*------------------------------------------------------------------bitboard.cpp----------------------------------------------------------*/
	 this file is just about our pawn bitboards and has the functions
	 
	 void PrintBitBoard(ULL b);
	 this function is simply printing out a bit board by representing set bits as 'X' and unset as '-'
	 we print the ranks from 8 to 1....remember in our program we represet the ranks opposite to what
	 the appear in a game so while printing we simply reverse them....see the bitboard video for any confusion.....keep in mind that
	 all the calulation and working is done on the structure where ranks ar 1 to 8 froms tops down....but only to show it to the user
	 while printing we reverse the ranks....so that ranks become 8 to 1 as they appear in a normal chess board
	 
	 
	int POP(ULL *bb);
	return the first index of the set bit int the bit boards and sets it to 0 code taken from chessprogramming.wkik
	and also the bitboard is used only for this function....we can also use the simple brute force method but it may be slower
	it use this talble--
	const int BitTable[64] = {
	  63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
	  51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
	  26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
	  58, 20, 37, 17, 36, 8
	};


	int Count(ULL b)
	counts the nuber of set bits in a a bitboard




/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*-----------------------------------------------------------hashkey.cpp------------------------------------------------------------------*/

	this file is just to generate the hashkeys

	ULL GenerateHashKeys(const BOARD *board);
	this function takes in the current board status and according to the positions of pieces,Side to play,CastlePerm and the EnPas position
	generates the unique PosKey value by xoring  all the random 64 bit numbers for those states of piece for that board and returns it....

/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*-----------------------------------------------------------board.cpp------------------------------------------------------------------*/

	this file has all the necessary functions that concern our chess board...

	void ResetBoard(BOARD *b);
	this function resets the entire board by seting all positions to empty or offboard and the count of all other 
	this to 0 or their respective starting value in the boaard structure


	int CalcFen(char *arr,BOARD *board);
	this function is the most important one as it takes in a FEN notation character array....and a point er to BOARD structure
	and then process that FEN notation to initialize the members of board structure with respect to the configuration in the
	FEN array....except for PceList,and maj,min and Big pieces....because as we will se they are initialized in other function
	as they help us to initialize the PceList effectively


	void PrintBoard(BOARD *board);
	simply funtion that prints the board when it has been set up by an FEN notation....it uses
	char PceChar[]=".PNBRQKpnbrqk";   
	char SideChar[]="wb-";
	char RankChar[]="12345678";
	char FileChar[]="abcdefgh";
	these arrays for easy indexing and printing using the type of pieces in the ENUM as the indexes we can effectively print them


	void UpdateMaterial(BOARD *board);
	function to update the remaning items i.e PceList,MajPce,MinPce...pawn bitboards etc... 
	once we have the board structure ready....we can easly count the tnumber of big,maj,min,material value of pieces
	by simply running a loop from 0 to 120...and store the position or counts....we use various boolean array which
	are define in data.cpp which fasten our approach to see what type of a piece the piece is


	extern int CheckBoard(BOARD *board);
	this function crosscheks everthing we have done to our board so far with various assert statemnts...it ensure
	that what we have stored on the board structutre is actually present in the board...it is not that necessary if
	everything is coreect but we need to be sure in such a big program...so we make a temporary arrays system
	which are same as those present in te board structure then using various value of board structure it cross checks
	other values in the board stucture

/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*-----------------------------------------------------------data.cpp------------------------------------------------------------------*/

	this file contains all the data declarations needed for the program

	char PceChar[]=".PNBRQKpnbrqk";   
	char SideChar[]="wb-";
	char RankChar[]="12345678";
	char FileChar[]="abcdefgh";
	char Fen[]="rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	int PceBig[13]={FALSE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE};
	int PceMaj[13]={FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE};
	int PceMin[13]={FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE};
	int PceVal[13]={0,100,325,325,550,1000,50000,100,325,325,550,1000,50000};
	int PceCol[13]={BOTH,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};
	
	int IsKnight[13];
	int IsKing[13];
	int IsRQ[13];   
	int IsBQ[13];
	int IsSlide[13]
	
	we have defined so many array as now when we use a type of piese say wN as in index to these arrays we will get the corresponding
	properties for thattype of pieece...i.e whether it is a Big,Maj,Min piece...its printing value...it color...ad its
	MaterialValue....whetehr it is aknight,bishop,rook or a queen...so its very helpful to do so....



/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*-----------------------------------------------------------ghost.cpp------------------------------------------------------------------*/

	this is our main source file which will be executed and contains the main function.....it will call all the other remaning 
	functions and will be used for testing ....











/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------------attack.cpp------------------------------------------------------------------*/

	int IsSqAttack(int sq,int side,BOARD *board);
	this function here checks if the current given square is attacked by any side which is specified...on the 
	current board...this is fairly easy we just need to set few arrays to mark the directions from this square
	from where certain types of pieces can attack...such as diagnols,vertical,horizontal etc..which are don by
	const int KnDir[8] = {-8,-19,-21,-12,8,19,21,12 };
	const int RkDir[4] = {-1,-10,1,10};
	const int BiDir[4] = {-9,-11,11,9};
	const int KiDir[8] = {-1,-10,1,10,-9,-11,11,9};

	queen will take both the direction of the rook and bishop
	further these arrays make sure the whethe the current pce is of the given type or not
	
	extern int IsPawn[13];
	extern int IsKnight[13];
	extern int IsKing[13];
	extern int IsRQ[13];
	extern int IsBQ[13];
	extern int IsSlide[13]; 




/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------------io.cpp------------------------------------------------------------------*/

	void PrintSq(int sq);
	a simple function to print the file and rank of a current square using the file and rank arrays


	void PrintMove(int move);
	simple funtion to print the from,to and promoted bit values of the move using fille and ranks


	void PrintMoveList(MOVELIST *MoveList);
	again a simple function to print a move list for a given side



/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------movegenerator.cpp------------------------------------------------------------------*/

	//remember there are only 2 movelists for a each side and not individually for each piece

	#define MkMove(f,t,cap,prom,flag) (f|(t<<7)|(cap<<14)|(prom<<20)|flag)
	a macro define to reduce our task to type everytime to make a move from the values


	void AddMove(BOARD *board, int Move,MOVELIST *MoveList);
	function that takes in a movelist say for WHITE and a possible move and add its to the movelist of WHITE
	it takes in all the three types of moves CaptureMove,SimpleMove...and special EnPas move for pawns


	void GenMoves(BOARD *board,MOVELIST MoveList);
	generates all the the possible moves to be added to a particular side whose turn it is...first it
	checks if its white side...if it is...it starts adding by pawnmoves
	now to add moves from pawns there can be 4 types...
	1. simle one square ahed move for wP it is +10 s check if its empty and in board...if it is then 
	   addwpmove is called because it can lead to a promotion if a wP is present on rank 6..
	2. starting move if the sq+20 is empty and the pawn is at rank2...then we also add a PawnSt flag
	3. diagonal moves if it is a capture move then we check sq+9 and sq+11...it can also lead to promotion
	   so we call the addwpMove insted of normal Addmove...because it takes care of promotions...
	4. and the last one is that it can be an EnPAs move..if that square on the board is marked as EnPas
	   then we add flag with EnPas...in it
	   
	void AddwPMove(BOARD *board ,int from,int to,int cap,int flag,MOVELIST *MoveList);
	adds moves of white pawns using the addmove function...it takes into account that if the from of 
	any pawn is from rank 2 then in its next simple or capturing move it will promote..else it will not
	
	void AddbPMove(BOARD *board ,int from,int to,int cap,int flag,MOVELIST *MoveList);
	similar funtion for black pawns....just the promtion rank check change to rank7
	
	
	//these arrays easly help us to generate moves for sliding pieces of both sides as they define the starting
	//index of white and black sliding pieces and when we reach a 0 we have completed one sides sliding pieces
	int SlidePce[]={ wB,wR,wQ ,0,bB,bR,bQ,0};
	int SlideIndex[]={0,4};


	//similar arrays for non sliding pieces...
	int NonSlidePce[]={wN,wK,  0 , bN,bK,0};
	int NonSlideIndex[]={0,3};

	//these store the direction for each piece which need to be added to the current square...to get all the squares
	//where a type of piecce indexed by rows can move 
	int PceDir[13][8] = {
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ -8, -19,	-21, -12, 8, 19, 21, 12 },
		{ -9, -11, 11, 9, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ -8, -19,	-21, -12, 8, 19, 21, 12 },
		{ -9, -11, 11, 9, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, 0, 0, 0, 0 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 },
		{ -1, -10,	1, 10, -9, -11, 11, 9 }
	};

	//these are the number of directions for each piece type for queen=8,bishp and rook=4
	int NumDir[13] = {
	 0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8
	};
	
	for sliding and nonsliding pieces we add move in 2 parts....
	1. for non sliding pieces....we first get the boar->Side in the variable Side...then with that
	   side we get the starting index for non sliding pieces...which will be 0 when side i white
	   and 4 when side is black....and then we run a wile loop till we get and EMPTY(0) piece
	   which tells us that non sliding pieces for the current side are over...we loop ecah of them
	   and inside this loop we loop till the number of direction of that piece...and then we test
	   each direction by adding that may squares...if the new square is not offboard we check if
	   it is empy...if it is then it is a normal move eles if its not epty and has a piece of 
	   opposite side (which we simply check by xoring with that pieces colour)..then it is a capture
	   move....we do this for all direction for all non sliding pieces...
	   
	2. all is same for sliding pieces just wile checking directions we reun an iside loop to move
	   to all posiible squares in that direction (sliding)...until we reach an opposite color square
	   or which will be capture move or and EMPTY move which will be normal move or we get offboard..
	   after that we test the next direction...and do this for all the sliding pieces of that colour 

	int MoveExists(BOARD *board,int Move);
	it simply checks if a given move is possible on the board or not
	
	
	int VictimScore[13]={0,100,200,300,400,500,600,100,200,300,400,500,600};
	static int MvvLvaScores[13][13]={0};
	these arrays are for the move ordering method of mostvaluable victim least valuabe attacker
	like a pawn captures queen...we initially assign everthing to zero...so that the score
	of non capture moves remain zero


	void InitMvvLva();
	this function is the actual function to initialize the MvvLvaScores arary for every combination of 
	2 pieces
	
	void GenCapMoves(BOARD *board,MOVELIST *MoveList);
	this is 95% same fucntion as GenMove...but it only generates the capture move which are required
	in the Quiescence search function


/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------makemove.cpp------------------------------------------------------------------*/


	extern void RemovePiece(BOARD *board,int sq);
	change the values of the board structure when a piece is removed
	
	
	extern void AddPiece(BOARD *board,int sq,int Pce);
	opposite to removePiece
	
	extern void MovePiece(BOARD *board,int from,int to);
	call first remove the addpiece for the given squares
	
	extern int MakeMove(BOARD *board,int Move);
	now this function is the most important one....it makes actual changes to the board structure and 
	actually makes a move....to do so...we us our history array which stored the unique information 
	about all our previous moves....when we make a move we firstly sotre all the unique values to the
	hitory structure indexed by HisPly..
	
	board->History[board->HisPly].Move=Move;
	board->History[board->HisPly].PosKey=board->PosKey;
	board->History[board->HisPly].EnPas=board->EnPas;
	board->History[board->HisPly].FiftyMove=board->FiftyMove;
	board->History[board->HisPly].CastlePerm=board->CastlePerm;
	
	we then increment the hisply and ply as the move has been made so we need to increae the index
	
	now we first check if the moves are special moves liek EnPas or catling..if so 
	we remove the pawns of other color from the respective position..or if it is castling
	we move the rooks to the respective position decided by the move to where they are going
	if there is any EnPas psoition...it sholud now be removed so we unhash it from the hashkey
	and also the castlePerm we unhash it...it may remain same or get changed...we then update the
	castlepem and hash it back and we marks EnPas as NONE..we use the array CastlePerm for the updation
	
	
	const int CastlePerm[120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
	};
	
	now if it is acapturing move we need to remove the captured piece..and increase the fifty moves counter
	
	now we check if it was a starting move for a pawn...if it was then we need to add the enpas position
	for that pawns...and also hash it back to the positionkey
	
	then we finally move a piece from 2 to
	
	after this we see if there was a promotion...if so then we remove the piece moved from the 
	previous step and add the promoted piece...finally we change the side and hash it to the unique key
	now only we check if this move has made the kng into check position..if so we take back the move
	as it will be illegal and return false stating that this mive cannot be made
	
	so if the king is in a mate the entire movelist will be traveresed but every move will be taken 
	back and returned false if even after that move king still remains in mate...so only those
	moves will be evaluated in which either the king moves or any other piece blocks the mate
	
	
	extern void TakeMove();
	this function is the exact mirror of the the makemove function and 
	we firstly restore all the unique value back from the historty array after decreasing its size
	
	board->EnPas=board->History[board->HisPly].EnPas;
	board->CastlePerm=board->History[board->HisPly].CastlePerm;
	board->FiftyMove=board->History[board->HisPly].FiftyMove;
	
	we the nreverse the sides
	if it was an EnPas capture we add back the respective pawn pieces to thir places or the rooks back to 
	their orignal position...we finllay movev back the piece from (to 2 from)..we update the kingpos and
	see if it was a capture move...if it was we add the captured piece back to the board...if the pieces
	was promoted we remove theat promoted piece which is now stading at the from square with and add a 
	pawn of the same colour over there
	
	finally we restore back the unique PosKey value....we do it at the last because if we did it before
	then the AddPieceand RemovePiece functions would have altered its value which we didint wanted...
	either we can do that or we can let it be and and get back to its history value by manually xoring
	and with the help of add and remove functions 
	
/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------perftest.cpp------------------------------------------------------------------*/

	/*code if this file is taken from the videos*/
	void PerftTest(int depth, BOARD *pos);
	this function is basically backtracing to a given depth to calculate all the valid move that can be
	made from a given board position...it first makes one move ...the ngenerate a new move list for that
	position and then again makes a move and makes a new list ...and so on till depth is 0...then it
	comes back one step and makes the second moves and goes on...just like dfs...and count the number
	of leaf nodes in the moves tree
	
	void Perft(int depth,BOARD *board);
	this is just a helper function for the above function
	
/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------misc.cpp------------------------------------------------------------------*/

	this file contains all the misc functions
	
	if running the program in windows them WIN32 is defined and we get the time buy th library windows.h
	else if running on lunix then we include sys/time.h to get the time...
	#ifdef WIN32
	#include "windows.h"
	#else
	#include "sys/time.h"
	#endif
	int GetTime();
	this function return the value of current time....it does not matter which system it is lunix or windows
	we will use this when we enable the engine with the gui...then it will sent us a stop interrupt
	then we stop the serching and make the move
	
 
/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------pvtable.cpp------------------------------------------------------------------*/

	this file has all the functions for our principal variation table
	
	the pvtable is represented using two structure...
	1. a structures used as hashing element..it has a PosKey...and a best move to be hashed for that PosKey
	2. the table structure which which has the array of above structure
	
	typedef struct{
	ULL PosKey;
	int Move;
	}PVENTRY;
	
	typedef struct{
	PVENTRY PvEntries[131070];
	int Count;
	}PVTABLE;
	
	void ClearPvTable(PVTABLE *PvTable);
	this function is used to reset the Pvtable by setting everything to 0 and the clear the pvtable for use..
	before the next search is applied
	
	void InitPvTable(PVTABLE *PvTable);
	this  function is to initillaze the count 
	

	void StorePvMove(BOARD *board,int Move);
	to store a move in the pvtable...usnig hashing function as poskey%count...


	int ProbePvMove(BOARD *board);
	retrive the move if it is present for that given current poskey in the board
	
	WORKING--
	it serves two purpose
	
	1. evertime we search a given depth it may happen that a certain position has alreday been reached
	   by some other way in the same ongoing search before....so when we again encounter the same 
	   position we have a best move for this position stored which will mostlikey give us the best
	   pv line from there on....
	
	2. now at the end of the search we have a cahin of moves saved...called the Pv line which is the 
	   best possible moves upto a given depth...how do we get this...after the search we get the 
	   best move for the current position...then we make that move and get the next position...and
	   we repeat the same step until we reach the maxdepth of we reach a none move..the the pvtable
	   may get change multiple times for the same position..whenever we reach an alpha cuttoff..indicating
	   we have a better move from this position..so we hash this new move tho that position...but at
	   the end of the search we get the best move for all the index tha tare filled
	   
	why do we clear it everytime when we have the best moves for a givne position which may occur...after
	some time in the game the we do not need to search again..?
	
	no, we will search for every move...because although we may get a best move say for a position at 
	a depth 6...in the last search...and after several moves say  we reach that position...but now if
	we use this previous vlaue then we only get the result which is onwe length deep...because remember
	we got back after 6 depth so no further level analysis was done for the 6th depth...so we need
	to clear the pvtable evertime....before a search...but if in an ongoing search a position is repeated
	then we can use it becoz now we only need the first best move...as w e may get maximum beta cutoff
	and the best pvline staring form this move 
	
	int GetPvLine(BOARD *board,int depth);
	as the name suggest we get the best pvline stored in the PvArray...for a given deapth..but we 
	may also get an nswer greater than the depth



 
/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------evaluate.cpp------------------------------------------------------------------*/


	this file gives us the the score which a given move makes....by calculating the sum of values 
	values which every pieces get at a given position for WHITE and the substracting it with BLACk
	it also takes in the material value of the sides
	
	if the side is white it return a +ve score else for black side it return a -ve score
	it is because it is alpha beta with negamax...so we need to return the score with the sides
	point of view...
	
	
	this if when white makes a move and then we get the evaluation value of the current board state
	/*
	 * we are getting the -score from the evaluation function because we want to see the score we gets from the 
	 * points sides of view in the alpha beta pruning ...so lets say from the starting position we mak a move
	 * from a2 to a4 of a pawn....then as soon as we make the move the side is changed..in the makemove function
	 * so now when we evaluate the current position we will get the value from blacks point of view and that is
	 * what we want...because the second node was the node for black or the min node so we want the  value 
	 * from the mins point of view or blacks point of view...and as now black makes a move further in the alpha
	 * beta search the sides again gets changed...and now we reach a max node...so we  need to evaluate the 
	 * board from whites point of view and so on....
	 * */


	see it this way say white has three options which gives the score of 
	10 20 50
	but these values will be returned from blacks point of view giving
	-10 -20 -50
	
	but the -sign during the calling of the alphabeta function again reverse it and takes the max score
	which will be 50
	
	now say in the next depth black makes a move and now the the side is change to white..and in the evaluation function we
	check the change side...so from whites point of view the pints are
	10 20 30
	
	again the -ve sign at the call of the fucntion will change them to
	-10 -20 -30
	and now black returns the maximum value which will be -10
	
	and further back when the final return occurs for alpha this value again chenge to 10 by the function
	call by that node
	
	so we get a final value of 10 by this path which is correct as black minimizes and white tries to maximize
	
	
/*---------------------------------------------------------------------------------------------------------------------------------------*/





/*------------------------------------------------search.cpp------------------------------------------------------------------*/


	the actual search function for iterative deepening and alpha beta and quiescence search
	
	
	#define INF 30000
	#define MATE 29000

 
	int IsRepeat(BOARD *board);
	this functions return true if a ceratin board position is repeated...and ofcoures if a pawn is moved or
	a piece is captured...then the fiftymoves resets...because we cannot move a pawn back or bring back
	the captured piece....so from there on we all the positions before that cannot be repeated so we start
	the loop from the last time when the fifty moves was reset



	static void ClearForSearch(BOARD *board, SEARCHINFO *info);
	this functions clears all the previous data beforn starting a search..it clears our searchkillers for
	beta cutoff and searchjistory for alphacutoff..also it clears the pvtalbe
	
	
	void SearchPos(BOARD *board,SEARCHINFO *info)
	this function is out iterative deepening function...iterative deepenig has an advantage that we already have
	a best move even if the time runs out...because searching for the current depth we have already searched 
	and updated the table for the previous depth search...and we have stored the bestmove for that depth
	which will now help us in our pvmove cutoff disscused before....which will cutoff the mmaximum node
	and lead us to the best pvline
	
	static int AlphaBeta(int Alpha,int Beta,int Depth,BOARD *board,SEARCHINFO *info,int Flag);
	this is the final alphabeta function in negamax fashion...to get the best pvline
	inside this we sort our moves in decreasing order of their score...this is called moveordering
	and is a crucial part of alphabeta search as it cutoffs the maximum nodes...inside this we update
	pur pvtable everytime whenver alpha is improved....say in one path alpha was 10 but from the second
	path the score returned was 20...so the new best moves has change to that path an we need to update it
	in the pvtable
	if we get a mate we send a very large value so that this path is taken above all..that is there is no
	other legal move for the other side we are getting a mate so we have to take this path...so we send a large negetive
	value added with ply which gives us after how many moves we can get a mate
	
	for eg say white can give a mte to black after 1 move then it must take this path
	say we move this path...now the side is changed...now it s blacks turn..since it is a mate
	black has now legal moves...so it return say -2900 value..which gets change to 2900 which is
	the maximum score white will ever get...so it will follow this path...
	
	now say white is getting a mate after 3 steps...so it must avoid this path at all cost...so 
	at the last depth when white also return -2900 value to the black nod it gets chengedd to 2900
	since it is negamax fashion we alwas take the maximum fo all the nodes...both alpha and beta
	and it automatically gets minimized for beta and maximized for alpha but in this case
	when maximized black get the value of 2900 so it return the value of -2900 value to the first alpha
	node...so it knows that it must not take this path...whereas if it takes this path...now black has
	the value 2900 stored for this position which tell it to take this path...so its correct
	
	
	
	
	static int Quiescence(int Alpha,int Beta,BOARD *board,SEARCHINFO *info);
	this search is added to improve the way the engine plays...to improve its game
	
	
	
/*---------------------------------------------------------------------------------------------------------------------------------------*/


	the chess engine is now complete all that is left is to link this engine to the GUI using the protocls...so
	there is no need for notes for those....they can bee seen on the internet


/*------------------------------------------------END------------------------------------------------------------------*/









