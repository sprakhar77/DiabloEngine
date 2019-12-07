#include "def.h"
#include <stdio.h>

int cmp2(MOVE a, MOVE b)
{
    return a.Score > b.Score;
}

// Some start positions to play With.
char Ffen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
char Fen2[] = "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1";
char Fen3[] = "rnbqkbnr/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b KQkq e3 0 1";
char Fen4[] = "5k2/1n6/4n3/6N1/8/3N4/8/5K2 w - - 0 1";
char ROOKS[] = "6k1/8/5r2/8/1nR5/5N2/8/6K1 b - - 0 1";
char QUEENS[] = "6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 b - - 0 1";
char BIS[] = "6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1";
char CAS1[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1";
char CAS2[] = "3rk2r/8/8/8/8/8/6p1/R3K2R b KQk - 0 1";
char perft[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 ";
char WAC1[] = "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1";

char problem[] = "rnb1r1k1/pppp1ppp/8/2q5/2P1P3/4KQ2/PPP3PP/1RB2B1R w - - 1 14 ";
char problem2[] = "3rk2r/1R1n1ppp/4p3/4q3/8/1PN5/1PP1QPPP/4K2R b Kk - 2 18 ";

int main()
{
    AllInit();
    Uci_Loop();
    return 0;
}
