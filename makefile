all:
	gcc Diablo.c init.c bitboards.c hashkey.c board.c data.c io.c attack.c movegenerator.c makemove.c perftest.c search.c misc.c pvtable.c evaluate.c uci.c -o Diablo
