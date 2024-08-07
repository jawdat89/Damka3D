/* ========================================================================== */
/*                                                                            */
/*   Steps.h                                                                  */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Global functions of steps                                                */
/*   are used in the checkers class						                      */
/* ========================================================================== */
#pragma once
#include <stdlib.h> // for random in easy mode
#include "../multiplayer/multiplayer.h"
#include "checkers.h"


typedef struct step { /* step struct used for calculating and manipulating steps while the game is running */
	step(step *copy) {
		oldcol = copy->oldcol;
		oldrow = copy->oldrow;
		newcol = copy->newcol;
		newrow = copy->newrow;
		attack = copy->attack;
		attackcol = copy->attackcol;
		attackrow = copy->attackrow;
		next = NULL;
	}
	int oldcol;
	int oldrow;
	int newcol;
	int newrow;
	int attack;
	int attackcol;
	int attackrow;
	struct step *next; /* for NODE data structure use */
}Step;

Step *generateMoves(Checkers &checkers, int turn);
void releaseStep(Step* step);
Step *filterAttackMoves(Step *moves);
int isThereAttackMoves(Step *moves);
void applyClick(Checkers &checkers, const int &col, const int &row);
void applyComputerStep(Checkers &checkers);
void check_result(Checkers& checkers);
GLfloat difference(const GLfloat& x, const GLfloat& y);

//MINMAX
int miniMax(Checkers checkers, int depth, int turn);
Checkers applyMove(Checkers checkers, Step *step);
int evaluateBoard(Checkers &checkers);
Step *getBestMove(Checkers checkers, int turn);