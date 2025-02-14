/* ========================================================================== */
/*                                                                            */
/*   Steps.cpp                                                                */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Global functions implementation of steps                                 */
/*   used in the checkers class			    			                      */
/* ========================================================================== */

#include "Steps.h"


/**
 * MiniMax algorithm implementation.
 * @param checkers - The current state of the checkers game.
 * @param depth - The depth to which the algorithm should explore.
 * @param turn - The current player's turn (1 for maximizing player, 0 for minimizing player).
 * @return The evaluation score of the board.
 */
int miniMax(Checkers checkers, int depth, int turn) {
	// Base case: if we've reached the maximum depth, evaluate the board
	if (depth == 0)
		return evaluateBoard(checkers);

	if (turn) {
		// Maximizing player (assumed to be player with 'turn == 1')

		int bestValue = -_CRT_INT_MAX; // Initialize to the smallest possible value
		Step *stepRoot = generateMoves(checkers, turn); // Generate all possible moves
		if (isThereAttackMoves(stepRoot)) // Filter attack moves if available
			stepRoot = filterAttackMoves(stepRoot);
		Step *step = stepRoot;

		// Iterate through all possible moves
		while (step) {
			Checkers check = applyMove(checkers, step); // Apply the move and get the new board state
			int value = miniMax(check, depth - 1, PLAYER); // Recursively call miniMax for the opponent
			if (value > bestValue) { // Choose the maximum value
				bestValue = value;
			}
			step = step->next;
		}
		releaseStep(stepRoot); // Free the memory allocated for moves
		return bestValue; // Return the best value found
	} else {
		// Minimizing player (assumed to be player with 'turn == 0')

		int bestValue = _CRT_INT_MAX; // Initialize to the largest possible value
		Step *stepRoot = generateMoves(checkers, turn); // Generate all possible moves
		if (isThereAttackMoves(stepRoot)) // Filter attack moves if available
			stepRoot = filterAttackMoves(stepRoot);
		Step *step = stepRoot;

		// Iterate through all possible moves
		while (step) {
			Checkers check = applyMove(checkers, step); // Apply the move and get the new board state
			int value = miniMax(check, depth - 1, COMPUTER); // Choose the minimum value
			if (value < bestValue) {
				bestValue = value;
			}
			step = step->next;
		}
		releaseStep(stepRoot); // Free the memory allocated for moves
		return bestValue; // Return the best value found
	}
}

/**
 * Applies a move to the checkers board and returns the new board state.
 * @param checkers - The current state of the checkers game.
 * @param step - The move to apply.
 * @return The new state of the checkers game after applying the move.
 */
Checkers applyMove(Checkers checkers, Step *step) {

	Checkers temp(checkers); // Create a copy of the current board state

	// Move the stone to the new position
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone = temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->stone;

	// Set the animation parameters
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->isAnimating = true;
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->animx = temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->x + temp.stones_length;
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->animy = temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->y + temp.stones_height;
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->animz = temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->z + temp.stones_width;

	// Update the board state
	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->isEmpty = temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->isEmpty;
	temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->isEmpty = true;;

	temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->turn = temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->turn;
	temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->turn = EMPTY;

	temp.block[(step->oldrow)* temp.event.cells_per_row + step->oldcol]->stone = nullptr;

	// Handle promotion to king
	if (step->newrow == 0 && temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->type == WHITE)
		temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->state = STONE_KING;
	if (step->newrow == 7 && temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->type == BLACK)
		temp.block[(step->newrow)* temp.event.cells_per_row + step->newcol]->stone->state = STONE_KING;


	// Handle attack moves	
	if (step->attack) {
		temp.block[(step->attackrow) * temp.event.cells_per_row + step->attackcol]->stone->y = temp.event.y - 1;
		temp.block[(step->attackrow) * temp.event.cells_per_row + step->attackcol]->turn = EMPTY;
		temp.block[(step->attackrow) * temp.event.cells_per_row + step->attackcol]->isEmpty = true;
		temp.block[(step->attackrow) * temp.event.cells_per_row + step->attackcol]->stone = nullptr;
	}
	return temp; // Return the new board state
}

/**
 * Evaluates the board state and returns a score based on the positions of the stones.
 * @param checkers - The current state of the checkers game.
 * @return The evaluation score of the board.
 */
int evaluateBoard(Checkers &checkers) {
	int whiteStones = 0, blackStones = 0;

	// Iterate through the board and calculate the score
	for (int row = 0; row < checkers.event.cells_per_row; row++)
		for (int col = 0; col < checkers.event.cells_per_row; col++) {
			if (!checkers.block[row + col * checkers.event.cells_per_row]->isEmpty) {
				if (checkers.block[row + col * checkers.event.cells_per_row]->stone != nullptr) {
					if (checkers.block[row + col * checkers.event.cells_per_row]->stone->type == WHITE) {
						if (checkers.block[row + col * checkers.event.cells_per_row]->stone->state == STONE_KING) {
							whiteStones += 10; // King stones are more valuable
						} else {
							whiteStones += checkers.event.cells_per_row - row; // Regular stones are valued based on their row
						}
					} else {
						if (checkers.block[row + col * checkers.event.cells_per_row]->stone->state == STONE_KING) {
							blackStones += 10;  // King stones are more valuable
						} else {
							blackStones += row; // Regular stones are valued based on their row
						}
					}
				}
			}
		}
	/*for (auto& i : checkers.block) {
			if (i->stone != NULL)
					if (!i->isEmpty)
							if (i->stone->type == WHITE)
									if (i->state == STONE_KING)
											whiteStones += 2;
									else
											whiteStones++;
							else if (i->stone->type == BLACK)
									if (i->state == STONE_KING)
											blackStones += 2;
									else
											blackStones++;
	}*/
	return whiteStones - blackStones; // Return the difference in scores
}

/**
 * Determines the best move for the computer using the MiniMax algorithm.
 * @param checkers - The current state of the checkers game.
 * @param turn - The turn indicator (0 for PLAYER, 1 for COMPUTER).
 * @return A pointer to the best move.
 */
Step *getBestMove(Checkers checkers, int turn) {
	// Generate all possible moves for the computer
	Step *stepRoot = generateMoves(checkers, COMPUTER);

	// Filter to only attack moves if any exist
	if (isThereAttackMoves(stepRoot))
		stepRoot = filterAttackMoves(stepRoot);

	Step *step = stepRoot;

	// Perform the MiniMax algorithm with a depth of 3
	int minimaxResult = miniMax(checkers, 3, COMPUTER);

	// Iterate through all possible moves to find the best one
	while (step) {
		//printf("POSSIBLE MOVE: oldcol: %d - oldow: %d | newcol: %d - newrow: %d | attack = %d\n", step->oldcol, step->oldrow, step->newcol, step->newrow, step->attack);

		// Apply the current move and evaluate the board state
		Checkers check = applyMove(checkers, step);
		int evalresult = miniMax(check, 2, COMPUTER);

		//printf("MAX: %d EVAL: %d\n", minimaxResult, evalresult);

		// If the evaluated result matches the best MiniMax result, break
		if (evalresult == minimaxResult)
			break;
		step = step->next;
	}

	// If no specific best move was found, default to the first move
	if (step == nullptr)
		step = stepRoot;

	// Allocate memory for the new best move step
	Step *newStep = (Step*)malloc(sizeof(Step));
	newStep = &Step(step);

	// Release the memory allocated for the list of possible moves
	releaseStep(stepRoot);
	
	//printf("BEST MOVE: oldcol: %d - oldow: %d | newcol: %d - newrow: %d | attack = %d\n", newStep->oldcol, newStep->oldrow, newStep->newcol, newStep->newrow, newStep->attack);

	return newStep;
}

/**
 * Generates all possible moves for the given player.
 * @param checkers - The current state of the checkers game.
 * @param turn - The current player's turn (1 for maximizing player, 0 for minimizing player).
 * @return A linked list of possible moves.
 */
Step *generateMoves(Checkers &checkers, int turn) {

	Step *root = (Step*)malloc(sizeof(Step));
	Step *current = root;
	root->oldcol = NULL;
	root->oldrow = NULL;
	root->newcol = NULL;
	root->newrow = NULL;
	root->attack = NULL;
	root->attackcol = NULL;
	root->attackrow = NULL;
	root->next = NULL;

	// Iterate through the board to generate moves
	if (turn == COMPUTER) {
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (checkers.block[row * checkers.event.cells_per_row + col]->turn == COMPUTER) {
					if (checkers.block[row * checkers.event.cells_per_row + col]->stone->state == STONE_KING) { //if king
						if (col == 7 || col == 0 || row == 0 || row == 7) { //if on edge
							if (col == 0 && row == 0) { // can move right down only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 0 && row == 7) { //can move right up only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7 && row == 0) { //can move left up only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7 && row == 7) { //can move left down only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else {
								if (col == 0) { //can move right only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								} else if (col == 7) { //can move left only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
								if (row == 0) { // can move down only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								} else if (row == 7) { //can move up only
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							}
						} else { //if not on edge
							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}
							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}
						}
					} else { //if not king
						if (col == 7 || col == 0) { //if on edge
							if (col == 0) { //can move right only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7) { //can move left only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							}
						} else { //if not on edge
							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}
						}
					}
				}
			}
		}
	} else if (turn == PLAYER) {
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (checkers.block[row * checkers.event.cells_per_row + col]->turn == PLAYER) {
					if (checkers.block[row * checkers.event.cells_per_row + col]->stone != NULL && checkers.block[row * checkers.event.cells_per_row + col]->stone->state == STONE_KING) { //if king
						if (col == 7 || col == 0 || row == 0 || row == 7) { //if on edge
							if (col == 0 && row == 0) { // can move right down only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 0 && row == 7) { //can move right up only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7 && row == 0) { //can move left up only
								if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row + 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row + 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row + 1 <= 7) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row + 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7 && row == 7) { //can move left down only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else {
								if (col == 0) { //can move right only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								} else if (col == 7) { //can move left only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								} else if (row == 0) { // can move down only
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row + 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row + 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row + 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								} else if (row == 7) { //can move up only
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col - 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col - 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row - 1 >= 0) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
									if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
										if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
											if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
												if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
													Step *step = (Step*)malloc(sizeof(Step));
													step->oldcol = col;
													step->oldrow = row;
													step->newcol = col + 2;
													step->newrow = row - 2;
													step->attack = 1;
													step->attackcol = col + 1;
													step->attackrow = row - 1;
													step->next = NULL;
													current->next = step;
													current = current->next;
												}
											}
										}
									} else { //if not blocked
										if (row + 1 <= 7) { //doesn't go out of bounds
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 1;
											step->newrow = row - 1;
											step->attack = 0;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							}
						} else { //if not on edge
							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row + 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row + 2 <= 7 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row + 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row + 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row + 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row + 1 <= 7) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row + 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}
						}
					} else { //if not king
						if (col == 7 || col == 0) { //if on edge
							if (col == 0) { //can move right only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col + 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col + 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col + 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							} else if (col == 7) { //can move left only
								if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
									if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
										if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
											if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
												Step *step = (Step*)malloc(sizeof(Step));
												step->oldcol = col;
												step->oldrow = row;
												step->newcol = col - 2;
												step->newrow = row - 2;
												step->attack = 1;
												step->attackcol = col - 1;
												step->attackrow = row - 1;
												step->next = NULL;
												current->next = step;
												current = current->next;
											}
										}
									}
								} else { //if not blocked
									if (row - 1 >= 0) { //doesn't go out of bounds
										Step *step = (Step*)malloc(sizeof(Step));
										step->oldcol = col;
										step->oldrow = row;
										step->newcol = col - 1;
										step->newrow = row - 1;
										step->attack = 0;
										step->next = NULL;
										current->next = step;
										current = current->next;
									}
								}
							}
						} else { //if not on edge
							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col + 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col + 2 <= 7) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col + 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col + 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col + 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col + 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}

							if (!checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->isEmpty) { //if blocked
								if (checkers.block[(row - 1) * checkers.event.cells_per_row + col - 1]->turn != checkers.block[row * checkers.event.cells_per_row + col]->turn) { //if not same type
									if (row - 2 >= 0 && col - 2 >= 0) { //doesn't go out of bounds
										if (checkers.block[(row - 2) * checkers.event.cells_per_row + col - 2]->isEmpty) {//if the one after it is not occupied
											Step *step = (Step*)malloc(sizeof(Step));
											step->oldcol = col;
											step->oldrow = row;
											step->newcol = col - 2;
											step->newrow = row - 2;
											step->attack = 1;
											step->attackcol = col - 1;
											step->attackrow = row - 1;
											step->next = NULL;
											current->next = step;
											current = current->next;
										}
									}
								}
							} else { //if not blocked
								if (row - 1 >= 0) { //doesn't go out of bounds
									Step *step = (Step*)malloc(sizeof(Step));
									step->oldcol = col;
									step->oldrow = row;
									step->newcol = col - 1;
									step->newrow = row - 1;
									step->attack = 0;
									step->next = NULL;
									current->next = step;
									current = current->next;
								}
							}
						}
					}
				}
			}
		}
	}

	root = root->next; // Skip the initial dummy node
	return root; // Return the generated moves

}

/**
 * Frees the memory allocated for a linked list of moves.
 * @param step - The linked list of moves to release.
 */
void releaseStep(Step * step)
{
	while (step) {
		Step *oldStep = step;
		step = step->next;
		free(oldStep);
	}
}

/**
 * Filters a list of moves to include only attack moves.
 * @param moves - The linked list of moves to filter.
 * @return A linked list of attack moves.
 */
Step *filterAttackMoves(Step *moves) {
	Step * temp = moves;
	if (!isThereAttackMoves(temp))
		return NULL;
	Step* root = (Step*)malloc(sizeof(Step));
	Step* current = root;
	while (temp) {
		if (temp->attack) {
			Step *move = (Step*)malloc(sizeof(Step));
			move->attack = temp->attack;
			move->attackcol = temp->attackcol;
			move->attackrow = temp->attackrow;
			move->oldcol = temp->oldcol;
			move->oldrow = temp->oldrow;
			move->newcol = temp->newcol;
			move->newrow = temp->newrow;
			move->next = NULL;

			current->next = move;
			current = current->next;
		}
		temp = temp->next;
	}
	root = root->next; // Skip the initial dummy node
	return root; // Return the filtered moves
}

/**
 * Checks if there are any attack moves in a list of moves.
 * @param moves - The linked list of moves to check.
 * @return 1 if there are attack moves, 0 otherwise.
 */
int isThereAttackMoves(Step *moves) {
	Step * temp = moves;
	while (temp) {
		if (temp->attack)
			return 1;
		temp = temp->next;
	}
	return 0;
}

/**
 * Handles a click event in the checkers game.
 * @param checkers - The current state of the checkers game.
 * @param col - The column of the clicked cell.
 * @param row - The row of the clicked cell.
 */
void applyClick(Checkers &checkers, const int & col, const int & row)
{
	if (!checkers.block[row * checkers.event.cells_per_row + col]->isEmpty && checkers.block[row * checkers.event.cells_per_row + col]->turn == PLAYER) {
		// Handle the selection of a stone
		
		if (checkers.stone_selected = true) {
			for (auto& i : checkers.block) {
				if (i->isSelected) {
					i->isSelected = false;
					i->state = BLOCK_IDLE;
					checkers.stone_selected = false;
				}
				if (i->state == BLOCK_OPTIONAL_PATH)
					i->state = BLOCK_IDLE;
			}
			checkers.block[row * checkers.event.cells_per_row + col]->isSelected = true;
			checkers.block[row * checkers.event.cells_per_row + col]->state = BLOCK_SELECTED;
			checkers.stone_selected = true;
		}

		Step *stepRoot = generateMoves(checkers, PLAYER);
		Step *step = stepRoot;
		if (isThereAttackMoves(step))
			step = filterAttackMoves(step);
		//printf("\n");
		while (step) {
			//printf("POSSIBLE MOVE: oldcol: %d - oldow: %d | newcol: %d - newrow: %d | attack = %d\n", step->oldcol, step->oldrow, step->newcol, step->newrow, step->attack);
			if (step->oldcol == col && step->oldrow == row) {
				checkers.block[step->newrow * checkers.event.cells_per_row + step->newcol]->state = BLOCK_OPTIONAL_PATH;
			}
			step = step->next;
		}
		releaseStep(stepRoot);

	} else {
		// Handle the movement of a selected stone

		if (checkers.stone_selected) {
			if (checkers.block[(row)* checkers.event.cells_per_row + col]->state == BLOCK_OPTIONAL_PATH) {
				for (int row1 = 0; row1 < checkers.event.cells_per_row; row1++) {
					for (int col1 = 0; col1 < checkers.event.cells_per_row; col1++) {
						if (checkers.block[(row1)* checkers.event.cells_per_row + col1]->isSelected) {
							Step *stepRoot = generateMoves(checkers, PLAYER); //released
							Step *step = stepRoot;
							if (isThereAttackMoves(step)) {
								Step *temp = step;
								int flag = 0;
								while (temp) {
									if (temp->newcol == col && temp->newrow == row && temp->attack) {
										flag = 1;
										break;
									}
									temp = temp->next;
								}
								if (flag) {
									step = filterAttackMoves(step);
									while (step) {
										if (step->newcol == col && step->newrow == row) {
											checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone->y = checkers.event.y - 1;
											checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->turn = EMPTY;
											checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->isEmpty = true;
											checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone = nullptr;
											break;
										}
										step = step->next;
									}
									releaseStep(stepRoot);
								}
							}

							int soldcol = col1,
								soldrow = row1,
								scol = col,
								srow = row,
								sattack = 0,
								sattackrow = 0,
								sattackcol = 0;
							if (step->attack) {
								sattack = step->attack;
								sattackrow = step->attackrow;
								sattackcol = step->attackcol;
							}

							checkers.block[(row)* checkers.event.cells_per_row + col]->stone = checkers.block[(row1)* checkers.event.cells_per_row + col1]->stone;

							checkers.block[(row)* checkers.event.cells_per_row + col]->stone->isAnimating = true;
							checkers.block[(row)* checkers.event.cells_per_row + col]->stone->animx = checkers.block[(row)* checkers.event.cells_per_row + col]->x + checkers.stones_length;
							checkers.block[(row)* checkers.event.cells_per_row + col]->stone->animy = checkers.block[(row)* checkers.event.cells_per_row + col]->y + checkers.stones_height;
							checkers.block[(row)* checkers.event.cells_per_row + col]->stone->animz = checkers.block[(row)* checkers.event.cells_per_row + col]->z + checkers.stones_width;

							checkers.block[(row)* checkers.event.cells_per_row + col]->isEmpty = checkers.block[(row1)* checkers.event.cells_per_row + col1]->isEmpty;
							checkers.block[(row1)* checkers.event.cells_per_row + col1]->isEmpty = true;;

							checkers.block[(row)* checkers.event.cells_per_row + col]->turn = checkers.block[(row1)* checkers.event.cells_per_row + col1]->turn;
							checkers.block[(row1)* checkers.event.cells_per_row + col1]->turn = EMPTY;

							checkers.block[(row1)* checkers.event.cells_per_row + col1]->stone = nullptr;


							for (auto& i : checkers.block) {
								if (i->isSelected)
									i->isSelected = false;
								if (i->state != BLOCK_IDLE)
									i->state = BLOCK_IDLE;
							}

							checkers.event.turn = checkers.event.turn == PLAYER ? COMPUTER : PLAYER;
							
							if (row == 0)
								checkers.block[(row)* checkers.event.cells_per_row + col]->stone->state = STONE_KING;


							if (step->attack) {
								Step *stepRoot = generateMoves(checkers, PLAYER); //released
								Step *step1 = stepRoot;
								if (isThereAttackMoves(step1)) {
									step1 = filterAttackMoves(step1);
									while (step1) {
										if (step1->oldcol == step->newcol && step1->oldrow == step->newrow) {
											checkers.event.turn = PLAYER;
											if (checkers.event.difficulty != MULTIPLAYER) {
												checkers.block[step->newrow * checkers.event.cells_per_row + step->newcol]->isSelected = true;
												checkers.block[step->newrow * checkers.event.cells_per_row + step->newcol]->state = BLOCK_SELECTED;
												checkers.stone_selected = true;
												checkers.block[step1->newrow * checkers.event.cells_per_row + step1->newcol]->state = BLOCK_OPTIONAL_PATH;
											}
										}
										step1 = step1->next;
									}
								}
								releaseStep(stepRoot);
							}
							if (checkers.event.difficulty == MULTIPLAYER) {
								char *buffer = (char*)malloc(sizeof(char) * 20);
								sprintf(buffer, "#%d,%d,%d,%d,%d,%d,%d,%d.", checkers.event.turn, soldcol, soldrow, scol, srow, sattack, sattackcol, sattackrow);
								sendToServer(buffer);
								if (checkers.event.turn != PLAYER)
									checkers.MPSTATUS = MP_WAITING;
							}
							break;

						}
					}
				}
			}
		}
	}
}

/**
 * Applies the best move for the computer.
 * @param checkers - The current state of the checkers game.
 */
void applyComputerStep(Checkers & checkers)
{
	if (checkers.event.difficulty == MULTIPLAYER) {
		//send request to server asking for step details
		//wait for message from the server and apply step accordignly
		//notify server that movement is done
		sendToServer("step.");
		printf("requested step\n");
		char *buffer = (char*)malloc(sizeof(char) * MAXLINE + 1);
		char *result = recvAll(buffer);
		printf("Recieved step: %s\n", result);
		//apply recieved step
		int oldcol = buffer[0] - '0',
			oldrow = buffer[2] - '0',
			newcol = buffer[4] - '0',
			newrow = buffer[6] - '0',
			attack = buffer[8] - '0',
			attackcol = buffer[10] - '0',
			attackrow = buffer[12] - '0';

		//invert positions
		oldcol = checkers.event.cells_per_row - oldcol - 1;
		oldrow = checkers.event.cells_per_row - oldrow - 1;
		newcol = checkers.event.cells_per_row - newcol - 1;
		newrow = checkers.event.cells_per_row - newrow - 1;
		attackcol = checkers.event.cells_per_row - attackcol - 1;
		attackrow = checkers.event.cells_per_row - attackrow - 1;


		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone = checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->stone;

		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone->isAnimating = true;
		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone->animx = checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->x + checkers.stones_length;
		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone->animy = checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->y + checkers.stones_height;
		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone->animz = checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->z + checkers.stones_width;

		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->isEmpty = checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->isEmpty;
		checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->isEmpty = true;;

		checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->turn = checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->turn;
		checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->turn = EMPTY;

		checkers.block[(oldrow)* checkers.event.cells_per_row + oldcol]->stone = nullptr;

		checkers.event.turn = PLAYER;

		if (newrow == 7)
			checkers.block[(newrow)* checkers.event.cells_per_row + newcol]->stone->state = STONE_KING;

		if (attack) {
			checkers.block[(attackrow) * checkers.event.cells_per_row + attackcol]->stone->y = checkers.event.y - 1;
			checkers.block[(attackrow) * checkers.event.cells_per_row + attackcol]->turn = EMPTY;
			checkers.block[(attackrow) * checkers.event.cells_per_row + attackcol]->isEmpty = true;
			checkers.block[(attackrow) * checkers.event.cells_per_row + attackcol]->stone = nullptr;
		}

	} 
	else if (checkers.event.difficulty == HARD) {
		Step *step = getBestMove(checkers, COMPUTER);

		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->stone;

		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->isAnimating = true;
		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animx = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->x + checkers.stones_length;
		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animy = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->y + checkers.stones_height;
		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animz = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->z + checkers.stones_width;

		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->isEmpty = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->isEmpty;
		checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->isEmpty = true;;

		checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->turn = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->turn;
		checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->turn = EMPTY;

		checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->stone = nullptr;

		checkers.event.turn = PLAYER;

		if (step->newrow == 7)
			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->state = STONE_KING;

		if (step->attack) {
			checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone->y = checkers.event.y - 1;
			checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->turn = EMPTY;
			checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->isEmpty = true;
			checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone = nullptr;
			int newrow = step->newrow;
			int newcol = step->newcol;
			Step *stepRoot1 = generateMoves(checkers, COMPUTER);
			Step *step1 = stepRoot1; //released
			while (step1) {
				if (step1->attack) {
					if (step1->oldrow == newrow && step1->oldcol == newcol) {
						checkers.event.turn = COMPUTER;
						break;
					}
				}
				step1 = step1->next;
			}
			releaseStep(stepRoot1);
		}
	} else {
		Step *stepRoot = generateMoves(checkers, COMPUTER);
		Step *step = stepRoot; //released
		if (isThereAttackMoves(step))
			step = filterAttackMoves(step);
		int stepCount = 0;
		while (step) {
			stepCount++;
			step = step->next;
		}
		step = stepRoot;
		if (isThereAttackMoves(step))
			step = filterAttackMoves(step);
		//randomize step
		int random = rand() % stepCount;
		for (int i = 0; i < random; i++)
			step = step->next;

		while (step) {
			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->stone;

			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->isAnimating = true;
			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animx = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->x + checkers.stones_length;
			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animy = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->y + checkers.stones_height;
			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->animz = checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->z + checkers.stones_width;

			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->isEmpty = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->isEmpty;
			checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->isEmpty = true;;

			checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->turn = checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->turn;
			checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->turn = EMPTY;

			checkers.block[(step->oldrow)* checkers.event.cells_per_row + step->oldcol]->stone = nullptr;

			checkers.event.turn = PLAYER;

			if (step->newrow == 7)
				checkers.block[(step->newrow)* checkers.event.cells_per_row + step->newcol]->stone->state = STONE_KING;

			if (step->attack) {
				checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone->y = checkers.event.y - 1;
				checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->turn = EMPTY;
				checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->isEmpty = true;
				checkers.block[(step->attackrow) * checkers.event.cells_per_row + step->attackcol]->stone = nullptr;

				Step *stepRoot1 = generateMoves(checkers, COMPUTER);
				Step *step1 = stepRoot1; //released
				while (step1) {
					if (step1->attack) {
						if (step1->oldrow == step->newrow && step1->oldcol == step->newcol) {
							checkers.event.turn = COMPUTER;
							break;
						}
					}
					step1 = step1->next;
				}
				releaseStep(stepRoot1);
			}

			break;
		}
		releaseStep(stepRoot);
	}
}

/**
 * Checks the result of the game.
 * @param checkers - The current state of the checkers game.
 */
void check_result(Checkers& checkers)
{
	if (checkers.result != RESULT_NOTYET)
		return;
	bool isThereWhiteSteps = false, isThereBlackSteps = false;
	Step *stepRoot = generateMoves(checkers, WHITE);
	Step* whiteMoves = stepRoot;
	if (whiteMoves)
		isThereWhiteSteps = true;
	if (!isThereWhiteSteps)
	{
		checkers.result = RESULT_LOST;
		return;
	}
	Step *stepRoot1 = generateMoves(checkers, BLACK);
	Step* blackMoves = stepRoot1;
	if (blackMoves)
		isThereBlackSteps = true;
	if (!isThereBlackSteps)
	{
		checkers.result = RESULT_WON;
	}
	releaseStep(stepRoot);
	releaseStep(stepRoot1);
}

/**
 * Returns the absolute difference between two floating-point numbers.
 * @param x - The first number.
 * @param y - The second number.
 * @return The absolute difference between x and y.
 */
GLfloat difference(const GLfloat& x, const GLfloat& y) {
	if (x > y)
		return x - y;
	else
		return y - x;
}

