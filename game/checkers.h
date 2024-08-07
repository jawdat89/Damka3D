/* ========================================================================== */
/*                                                                            */
/*   checkers.h                                                               */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Checkers game                                                            */
/*   class for declaring a game                                               */
/*   structs and enums that used in the code                                  */
/*                                                                            */
/* ========================================================================== */
#pragma once
#include "../graphics/renderer.h"

typedef enum { WHITE, BLACK } E_CellType; /* this enum is characterizes the checkers part (block or stone)  */

typedef enum
{
    BOARD_GAME_START,
    BOARD_GAME_IDLE
} E_BoardEventType; /* to determine what is the status of the game */

typedef enum
{
	MP_OFFLINE,
	MP_WAITING,
	MP_PLAYING
}E_MultiplayerStatus;

typedef enum
{
    EMPTY = -1, // used when checkers class is created, and no changes yet occured
    PLAYER = 0,
    COMPUTER = 1
} E_MoveTurn; /* tells who can make action in the game, when game is started */

typedef enum
{
    EASY = 0, // randomize computer movements
    HARD = 1,  // minimax algorithm computer movements
	MULTIPLAYER = 2
} E_Difficulty; /* for setting computer's algorithm of taking actions in the game */

struct S_BoardEvent
{
	S_BoardEvent() {}
	S_BoardEvent(S_BoardEvent &copy) {
		type = copy.type;
		turn = copy.turn;
		difficulty = copy.difficulty;
		x = copy.x;
		y = copy.y;
		z = copy.z;
		cells_per_row = copy.cells_per_row;
	}
    E_BoardEventType type;
    E_MoveTurn turn;
    E_Difficulty difficulty;
    GLfloat x, z; // coordination
    GLfloat y;
    int cells_per_row;
}; /* struct to collect game components for manipulate them during the game is still running */

typedef enum
{
    RESULT_NOTYET,
    RESULT_WON,
    RESULT_LOST
} E_ResultType; /* game result */

typedef enum
{
    INPUT_ILLEGAL = -1,
    INPUT_NOTYET = 0,
    INPUT_LEGAL = 1
} E_InputType; /* type TODO: check if this enum is usable */


#define ISINBOARD(x, y)  ((x)>=0 && (y)>=0 && (x)<board_length && (y)< board_width) /* tells if stone is in blocks grid TODO: check if this define is usable */
#define BLOCK_CELLS 64 /* total number of blocks */
#define STONES_COUNT 12 /* total number of stones of each type */

struct GLvec3Color /* this struct is for making a variables of colors for checkers game components */
{
    GLfloat r, g, b;
    GLvec3Color()
    {
        this->r = 0.0f;
        this->g = 0.0f;
        this->b = 0.0f;
    }
    GLvec3Color(const GLfloat& r, const GLfloat& g, const GLfloat& b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    void setGLvec3Color(const GLfloat& r, const GLfloat& g, const GLfloat& b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }
};

typedef enum
{
    STONE_SELECTED,
    STONE_REGULAR,
    STONE_KING,
    STONE_INACTIVE
}E_StoneState; /* this enum determines the current stone state */

struct S_CheckersStone /* struct used for creating and manipulating stone in checkers game */
{
    S_CheckersStone(E_CellType type) // constructor gives the stone its color by its type
    {
        this->type = type;
        if (type == WHITE)
            color = GLvec3Color(0.83, 0.8, 0.71);
        else if (type == BLACK)
            color = GLvec3Color(0.22f, 0.09f, 0.09f);
    }
	S_CheckersStone(S_CheckersStone *copy) {
		this->type = type;
		if (type == WHITE)
			color = GLvec3Color(0.83, 0.8, 0.71);
		else if (type == BLACK)
			color = GLvec3Color(0.22f, 0.09f, 0.09f);
		if (copy != nullptr) {
			x = copy->x;
			y = copy->y;
			z = copy->z;
			length = copy->length;
			width = copy->width;
			height = copy->height;
			animx = copy->animx;
			animy = copy->animy;
			animz = copy->animz;
			type = copy->type;
			state = copy->state;
			isSelected = copy->isSelected;
			isAnimating = copy->isAnimating;
		}
	}

    GLfloat x, y, z; // stone position
    GLfloat animx, animy, animz; // movement animation purpose
    GLfloat length, width; // drawing purpose
    GLfloat height; // drawing purpose
    E_CellType type; // type of stone
    GLvec3Color color; // stone color

    E_StoneState state; // stone state at the current time
    bool isSelected; // checking if the stone is selected
    bool isAnimating; // checking if the stone is animating
};

typedef enum
{
    BLOCK_HOVERED,
    BLOCK_SELECTED,
    BLOCK_OPTIONAL_PATH,
    BLOCK_IDLE
}E_BlockState; /* this enum determines the current block state */

struct S_CheckersBlock
{
    S_CheckersBlock(E_CellType type) // constructor gives the block its color by its type
    {
        this->type = type;
        if (type == BLACK)
            color = GLvec3Color(0.0f, 0.0f, 0.0f);
        else if (type == WHITE)
            color = GLvec3Color(1.0f, 1.0f, 1.0f);
        stone = nullptr;
    }
	S_CheckersBlock(S_CheckersBlock *copy) { //copy->constructor
		this->type = type;
		if (type == BLACK)
			color = GLvec3Color(0.0f, 0.0f, 0.0f);
		else if (type == WHITE)
			color = GLvec3Color(1.0f, 1.0f, 1.0f);
		stone = nullptr;

		x = copy->x;
		y = copy->y;
		z = copy->z;
		length = copy->length;
		width = copy->width;
		height = copy->height;
		type = copy->type;
		turn = copy->turn;
		color = copy->color;
		stone = new S_CheckersStone(copy->stone);
		state = copy->state;
		isEmpty = copy->isEmpty;
		isSelected = copy->isSelected;

		
	}

    GLfloat x, y, z; // block position
    GLfloat length, width; // drawing purpose
    GLfloat height; // drawing purpose
    E_CellType type; // type of stone
    E_MoveTurn turn; // tells who is occupied the block
    GLvec3Color color; // block color

    S_CheckersStone* stone; // pointer to tell which stone is now at the block

    E_BlockState state; // block state at the current time
    bool isEmpty; // checking if the block is occupied
    bool isSelected; // checking if the block is selected
};
/*
 * Game class
 * this class is accually the game it self,
 * creating the game, 
 * updating the game,
 * drawing the game components,
 * telling what is the result of the game
 */
class Checkers
{
public:

    Checkers(GLfloat& x, GLfloat& y, GLfloat& z, int& cellrow);
	Checkers(const Checkers &copy);
    ~Checkers();

    void init_game();
    void stop_game();
    void reset_game();

    void draw(); // this method should be called in render(draw)'s openGL fuction
    void update(); // this method should be called in idle's openGL fuction
private:
    void initCheckers();
    void blocks_value();
    void stones_value();

    static void drawBlock(S_CheckersBlock& block);
    static void drawStone(S_CheckersStone& stone);
    static void draw_circle(GLfloat cx, GLfloat cy, GLfloat r, int num_segments);
public:
    GLfloat x, y, z, cellrow; // origin point of checker board
    S_BoardEvent event;
    E_ResultType result;
	E_MultiplayerStatus MPSTATUS;
    bool stone_selected;
    bool isAnimating;
	int doneAnimatingCam;

    S_CheckersBlock *block[BLOCK_CELLS];
    S_CheckersStone *black[STONES_COUNT];
    S_CheckersStone *white[STONES_COUNT];

    //alignment stone position
    GLfloat stones_length = -0.5;
    GLfloat stones_height = 0.7;
    GLfloat stones_width = -0.2;

private:
    GLfloat length, height, width;
};


