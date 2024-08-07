/* ========================================================================== */
/*                                                                            */
/*   checkers.cpp                                                             */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   Checkers class                                                           */
/*   methods of Checkers class                                                */
/*                                                                            */
/* ========================================================================== */


#include "checkers.h"
#include <math.h> // for using cin and cos to draw circles


Checkers::Checkers(GLfloat& x, GLfloat& y, GLfloat& z, int& cellrow) /* constructor takes position, and number of cells */
	:x(x), y(y), z(z), cellrow(cellrow)
{
	initCheckers(); // the variables first settings when the class is created, ispite of x,y,z, and cellrow that are already set
	event.type = BOARD_GAME_IDLE; // checkers state stating as IDLE
}

Checkers::~Checkers() /* destructor takes care of freeing alocated memory from the heap of structs that created */
{

	for (S_CheckersBlock* checkers_block : block)
		delete checkers_block;
	for (S_CheckersStone* white_stone : white)
		delete white_stone;
	for (S_CheckersStone* black_stone : black)
		delete black_stone;
}

Checkers::Checkers(const Checkers &copy) { //Copy constructor
	x = copy.x;
	y = copy.y;
	z = copy.z;
	cellrow = copy.cellrow;

	//change with a copy constructor for S_BoardEvent
	event.cells_per_row = copy.event.cells_per_row;
	event.difficulty = copy.event.difficulty;
	event.turn = copy.event.turn;
	event.type = copy.event.type;
	event.x = copy.event.x;
	event.y = copy.event.y;
	event.z = copy.event.z;

	result = copy.result;
	stone_selected = copy.stone_selected;
	isAnimating = copy.isAnimating;
	for (int i = 0; i < BLOCK_CELLS; i++) {
		block[i] = new S_CheckersBlock(copy.block[i]);
	}
	for (int i = 0; i < STONES_COUNT; i++) {
		black[i] = new S_CheckersStone(copy.black[i]);
		white[i] = new S_CheckersStone(copy.white[i]);
	}
	stones_length = copy.stones_length;
	stones_height = copy.stones_height;
	stones_width = copy.stones_width;
	length = copy.length;
	height = copy.height;
	width = copy.width;
}

void Checkers::draw() /* draw method is drawing the class components with the current data */
{
	for (int i = 0; i < event.cells_per_row * event.cells_per_row; i++) // draw blocks
	{
		drawBlock(*block[i]);
	}

	for (int i = 0; i < STONES_COUNT; i++) // draw stones 
	{
		drawStone(*black[i]);
		drawStone(*white[i]);
	}

}

void Checkers::update() // this method is called in idle's openGL function
{
	for (auto& i : block)
	{
		if (i->state == BLOCK_OPTIONAL_PATH)
		{
			i->color.setGLvec3Color(0.75f, 0.95f, 0.5f); //(0.75f, 0.95f, 0.95f) - LIGHT BLUE
		} else if (i->state == BLOCK_SELECTED)
		{
			i->color.setGLvec3Color(0.21f, 0.64f, 0.85f);
		} else if (i->state == BLOCK_IDLE)
		{
			if (i->type == BLACK)
			{
				i->color.setGLvec3Color(0.0f, 0.0f, 0.0f);
			} else if (i->type == WHITE)
			{
				i->color.setGLvec3Color(1.0f, 1.0f, 1.0f);
			}
		}

		if (i->stone != NULL) {
			if (i->stone->isAnimating)
				isAnimating = true;
		}
	}
}

void Checkers::initCheckers() /* this method is used when the class is created */
{
	event.x = x; // class position
	event.y = y; // class position
	event.z = z; // class position
	length = x / 2; // drawing purpose
	height = y;     // drawing purpose
	width = z / 2;  // drawing purpose
	event.cells_per_row = cellrow; // number of cells per row
	event.difficulty = HARD; // game difficulty default setting
	result = RESULT_NOTYET; // result of the game at the current time
	MPSTATUS = MP_OFFLINE;

	blocks_value(); // initiates blocks positions and parameters
	stones_value(); // initiates stones positions and parameters

	stone_selected = false; // checking if there a stone selected
	isAnimating = false; // checking if there is an animations drawing goes around
	doneAnimatingCam = 0;
}

void Checkers::blocks_value() /* this method is sets blocks parametes when the class is created */
{
	for (int i = 0; i < event.cells_per_row; i++)
	{
		for (int j = 0; j < event.cells_per_row; j++)
		{ // for each block in the block array set the block type to match checkers board
			if (i % 2)
			{
				if (j % 2)
					block[i + j * event.cells_per_row] = new S_CheckersBlock(WHITE);
				else
					block[i + j * event.cells_per_row] = new S_CheckersBlock(BLACK);
			} else
			{
				if (j % 2)
					block[i + j * event.cells_per_row] = new S_CheckersBlock(BLACK);
				else
					block[i + j * event.cells_per_row] = new S_CheckersBlock(WHITE);
			}
		}
	}

	for (int i = 0; i < event.cells_per_row * event.cells_per_row; i++)
	{ // set all blocks in the block array
		block[i]->state = BLOCK_IDLE;
		block[i]->isEmpty = true;
		block[i]->isSelected = false;
	}
	/* giving positions and parameters of blocks to drawing with openGL */
	const GLfloat divider = 4.4; // the padding between the blocks
	for (int col = 0; col < event.cells_per_row; col++)
	{
		for (int row = 0; row < event.cells_per_row; row++)
		{
			block[row + col * event.cells_per_row]->x = (row - x + divider) * 2;
			block[row + col * event.cells_per_row]->y = y;
			block[row + col * event.cells_per_row]->z = (col - z + divider) * 2;
			block[row + col * event.cells_per_row]->length = 1.0f;
			block[row + col * event.cells_per_row]->width = 1.0f;
			block[row + col * event.cells_per_row]->height = 0.5f;
		}
	}

	update(); // call update method to load the new settings
}

void Checkers::stones_value() /* this method is sets stones parametes when the class is created */
{
	for (int i = 0; i < STONES_COUNT; i++)
	{
		/*-------Blacks--------*/
		black[i] = new S_CheckersStone(BLACK);
		black[i]->state = STONE_REGULAR;
		black[i]->isSelected = false;
		black[i]->isAnimating = false;
		black[i]->length = 0.5f;
		black[i]->width = 0.5f;
		black[i]->height = 0.6f;
		/*-------Whites--------*/
		white[i] = new S_CheckersStone(WHITE);
		white[i]->state = STONE_REGULAR;
		white[i]->isSelected = false;
		white[i]->isAnimating = false;
		white[i]->length = 0.5f;
		white[i]->width = 0.5f;
		white[i]->height = 0.6f;

	}
	// initiate positions
	// position of stones is placed at the matched block of checkers(draughts) roles


	// Blacks                                                                   Original checkers position
	black[0]->x = block[0]->x + stones_length; // for drawing purpose                  block[0]  
	black[0]->y = block[0]->y + stones_height; // for drawing purpose
	black[0]->z = block[0]->z + stones_width;  // for drawing purpose
	block[0]->isEmpty = false;  // occupy the block
	block[0]->stone = black[0]; // change the blocks pointer to the stone  
	block[0]->turn = COMPUTER;  // tell who is occupying the block

	black[1]->x = block[2]->x + stones_length; // for drawing purpose                 block[2]
	black[1]->y = block[2]->y + stones_height; // for drawing purpose
	black[1]->z = block[2]->z + stones_width;  // for drawing purpose
	block[2]->isEmpty = false;  // occupy the block
	block[2]->stone = black[1]; // change the blocks pointer to the stone 
	block[2]->turn = COMPUTER;  // tell who is occupying the block

	black[2]->x = block[4]->x + stones_length; // for drawing purpose                  block[4] 
	black[2]->y = block[4]->y + stones_height; // for drawing purpose                  
	black[2]->z = block[4]->z + stones_width;  // for drawing purpose                  
	block[4]->isEmpty = false;  // occupy the block                                    
	block[4]->stone = black[2]; // change the blocks pointer to the stone              
	block[4]->turn = COMPUTER;  // tell who is occupying the block                     

	black[3]->x = block[6]->x + stones_length; // for drawing purpose                  block[6]
	black[3]->y = block[6]->y + stones_height; // for drawing purpose
	black[3]->z = block[6]->z + stones_width;  // for drawing purpose
	block[6]->isEmpty = false;  // occupy the block
	block[6]->stone = black[3]; // change the blocks pointer to the stone 
	block[6]->turn = COMPUTER;  // tell who is occupying the block

	black[4]->x = block[9]->x + stones_length; // for drawing purpose                  block[9]                
	black[4]->y = block[9]->y + stones_height; // for drawing purpose
	black[4]->z = block[9]->z + stones_width;  // for drawing purpose
	block[9]->isEmpty = false;  // occupy the block
	block[9]->stone = black[4]; // change the blocks pointer to the stone 
	block[9]->turn = COMPUTER;  // tell who is occupying the block

	black[5]->x = block[11]->x + stones_length; // for drawing purpose                 block[11]
	black[5]->y = block[11]->y + stones_height; // for drawing purpose
	black[5]->z = block[11]->z + stones_width;  // for drawing purpose
	block[11]->isEmpty = false;  // occupy the block
	block[11]->stone = black[5]; // change the blocks pointer to the stone 
	block[11]->turn = COMPUTER;  // tell who is occupying the block

	black[6]->x = block[13]->x + stones_length; // for drawing purpose                 block[13] 
	black[6]->y = block[13]->y + stones_height; // for drawing purpose                 
	black[6]->z = block[13]->z + stones_width;  // for drawing purpose                 
	block[13]->isEmpty = false;  // occupy the block                                   
	block[13]->stone = black[6]; // change the blocks pointer to the stone             
	block[13]->turn = COMPUTER;  // tell who is occupying the block                    

	black[7]->x = block[15]->x + stones_length; // for drawing purpose                 block[15]
	black[7]->y = block[15]->y + stones_height; // for drawing purpose                 
	black[7]->z = block[15]->z + stones_width;  // for drawing purpose                 
	block[15]->isEmpty = false;  // occupy the block                                   
	block[15]->stone = black[7]; // change the blocks pointer to the stone             
	block[15]->turn = COMPUTER;  // tell who is occupying the block                    

	black[8]->x = block[16]->x + stones_length; // for drawing purpose                 block[16] 
	black[8]->y = block[16]->y + stones_height; // for drawing purpose                 
	black[8]->z = block[16]->z + stones_width;  // for drawing purpose                 
	block[16]->isEmpty = false;  // occupy the block                                   
	block[16]->stone = black[8]; // change the blocks pointer to the stone             
	block[16]->turn = COMPUTER;  // tell who is occupying the block                    

	black[9]->x = block[18]->x + stones_length; // for drawing purpose                 block[18]
	black[9]->y = block[18]->y + stones_height; // for drawing purpose
	black[9]->z = block[18]->z + stones_width;  // for drawing purpose
	block[18]->isEmpty = false;  // occupy the block
	block[18]->stone = black[9]; // change the blocks pointer to the stone 
	block[18]->turn = COMPUTER;  // tell who is occupying the block

	black[10]->x = block[20]->x + stones_length; // for drawing purpose                block[20]
	black[10]->y = block[20]->y + stones_height; // for drawing purpose                
	black[10]->z = block[20]->z + stones_width;  // for drawing purpose                
	block[20]->isEmpty = false;   // occupy the block                                  
	block[20]->stone = black[10]; // change the blocks pointer to the stone            
	block[20]->turn = COMPUTER;   // tell who is occupying the block                   

	black[11]->x = block[22]->x + stones_length; // for drawing purpose                block[22]
	black[11]->y = block[22]->y + stones_height; // for drawing purpose
	black[11]->z = block[22]->z + stones_width;  // for drawing purpose
	block[22]->isEmpty = false;   // occupy the block
	block[22]->stone = black[11]; // change the blocks pointer to the stone 
	block[22]->turn = COMPUTER;   // tell who is occupying the block

								  // Whites                                                                   Original checkers position
	white[0]->x = block[63]->x + stones_length; // for drawing purpose                 block[63]  
	white[0]->y = block[63]->y + stones_height; // for drawing purpose
	white[0]->z = block[63]->z + stones_width;  // for drawing purpose
	block[63]->isEmpty = false;  // occupy the block
	block[63]->stone = white[0]; // change the blocks pointer to the stone 
	block[63]->turn = PLAYER;    // tell who is occupying the block

	white[1]->x = block[61]->x + stones_length; // for drawing purpose                 block[61]
	white[1]->y = block[61]->y + stones_height; // for drawing purpose
	white[1]->z = block[61]->z + stones_width;  // for drawing purpose
	block[61]->isEmpty = false;  // occupy the block
	block[61]->stone = white[1]; // change the blocks pointer to the stone 
	block[61]->turn = PLAYER;    // tell who is occupying the block

	white[2]->x = block[59]->x + stones_length; // for drawing purpose                 block[59] 
	white[2]->y = block[59]->y + stones_height; // for drawing purpose
	white[2]->z = block[59]->z + stones_width;  // for drawing purpose
	block[59]->isEmpty = false;  // occupy the block
	block[59]->stone = white[2]; // change the blocks pointer to the stone 
	block[59]->turn = PLAYER;    // tell who is occupying the block

	white[3]->x = block[57]->x + stones_length; // for drawing purpose                 block[57]
	white[3]->y = block[57]->y + stones_height; // for drawing purpose
	white[3]->z = block[57]->z + stones_width;  // for drawing purpose
	block[57]->isEmpty = false;  // occupy the block
	block[57]->stone = white[3]; // change the blocks pointer to the stone 
	block[57]->turn = PLAYER;    // tell who is occupying the block

	white[4]->x = block[54]->x + stones_length; // for drawing purpose                 block[54]  
	white[4]->y = block[54]->y + stones_height; // for drawing purpose
	white[4]->z = block[54]->z + stones_width;  // for drawing purpose
	block[54]->isEmpty = false;  // occupy the block
	block[54]->stone = white[4]; // change the blocks pointer to the stone 
	block[54]->turn = PLAYER;    // tell who is occupying the block

	white[5]->x = block[52]->x + stones_length; // for drawing purpose                 block[52]
	white[5]->y = block[52]->y + stones_height; // for drawing purpose
	white[5]->z = block[52]->z + stones_width;  // for drawing purpose
	block[52]->isEmpty = false;  // occupy the block
	block[52]->stone = white[5]; // change the blocks pointer to the stone 
	block[52]->turn = PLAYER;    // tell who is occupying the block

	white[6]->x = block[50]->x + stones_length; // for drawing purpose                 block[50] 
	white[6]->y = block[50]->y + stones_height; // for drawing purpose
	white[6]->z = block[50]->z + stones_width;  // for drawing purpose
	block[50]->isEmpty = false;  // occupy the block
	block[50]->stone = white[6]; // change the blocks pointer to the stone 
	block[50]->turn = PLAYER;    // tell who is occupying the block

	white[7]->x = block[48]->x + stones_length; // for drawing purpose                 block[48]
	white[7]->y = block[48]->y + stones_height; // for drawing purpose
	white[7]->z = block[48]->z + stones_width;  // for drawing purpose
	block[48]->isEmpty = false;  // occupy the block
	block[48]->stone = white[7]; // change the blocks pointer to the stone 
	block[48]->turn = PLAYER;    // tell who is occupying the block

	white[8]->x = block[47]->x + stones_length; // for drawing purpose                 block[47] 
	white[8]->y = block[47]->y + stones_height; // for drawing purpose
	white[8]->z = block[47]->z + stones_width;  // for drawing purpose
	block[47]->isEmpty = false;  // occupy the block
	block[47]->stone = white[8]; // change the blocks pointer to the stone 
	block[47]->turn = PLAYER;    // tell who is occupying the block

	white[9]->x = block[45]->x + stones_length; // for drawing purpose                 block[45]
	white[9]->y = block[45]->y + stones_height; // for drawing purpose
	white[9]->z = block[45]->z + stones_width;  // for drawing purpose
	block[45]->isEmpty = false;  // occupy the block
	block[45]->stone = white[9]; // change the blocks pointer to the stone 
	block[45]->turn = PLAYER;    // tell who is occupying the block

	white[10]->x = block[43]->x + stones_length; // for drawing purpose                block[43]
	white[10]->y = block[43]->y + stones_height; // for drawing purpose
	white[10]->z = block[43]->z + stones_width;  // for drawing purpose
	block[43]->isEmpty = false;   // occupy the block
	block[43]->stone = white[10]; // change the blocks pointer to the stone 
	block[43]->turn = PLAYER;     // tell who is occupying the block

	white[11]->x = block[41]->x + stones_length; // for drawing purpose                block[41]
	white[11]->y = block[41]->y + stones_height; // for drawing purpose
	white[11]->z = block[41]->z + stones_width;  // for drawing purpose
	block[41]->isEmpty = false;   // occupy the block
	block[41]->stone = white[11]; // change the blocks pointer to the stone 
	block[41]->turn = PLAYER;     // tell who is occupying the block

}

void Checkers::drawBlock(S_CheckersBlock& block) /* method for drawing block with openGL */
{
	/* set block color */
	glColor3f(block.color.r, block.color.g, block.color.b);
	/* start drawing block */
	glTranslatef(block.x, block.y, block.z);
	glBegin(GL_QUADS);
	// Front
	glNormal3b(0, 0, 1);
	glVertex3f(-block.length, 0.0f, block.width);
	glVertex3f(-block.length, block.height, block.width);
	glVertex3f(block.length, block.height, block.width);
	glVertex3f(block.length, 0.0f, block.width);
	//// Back
	glNormal3f(0, 0, -1);
	glVertex3f(-block.length, 0.0f, -block.width);
	glVertex3f(-block.length, block.height, -block.width);
	glVertex3f(block.length, block.height, -block.width);
	glVertex3f(block.length, 0.0f, -block.width);
	// Left
	glNormal3b(-1, 0, 0);
	glVertex3f(-block.length, 0.0f, -block.width);
	glVertex3f(-block.length, block.height, -block.width);
	glVertex3f(-block.length, block.height, block.width);
	glVertex3f(-block.length, 0.0f, block.width);
	// Right
	glNormal3b(1, 0, 0);
	glVertex3f(block.length, 0.0f, -block.width);
	glVertex3f(block.length, block.height, -block.width);
	glVertex3f(block.length, block.height, block.width);
	glVertex3f(block.length, 0.0f, block.width);
	// Top
	glNormal3b(0, 1, 0);
	glVertex3f(-block.length, block.height, -block.width);
	glVertex3f(-block.length, block.height, block.width);
	glVertex3f(block.length, block.height, block.width);
	glVertex3f(block.length, block.height, -block.width);
	glEnd();
	glTranslatef(-block.x, -block.y, -block.z);
	/* end drawing block */
}

void Checkers::drawStone(S_CheckersStone& stone) /* method for drawing stone with openGL */
{
	if (stone.state == STONE_REGULAR) // check if the stone state is regular
	{
		GLfloat radius = 0.8f; // for drawing circles
		GLfloat sides = 0.2f;  // for drawing circles
		/* set stone color */
		glColor3f(stone.color.r, stone.color.g, stone.color.b);
		/* start drawing stone */
		glTranslatef(stone.x, stone.y, stone.z);
		GLfloat dawing_radius = radius;
		while (dawing_radius > 0)
		{
			draw_circle(0.5f, 0.2f, dawing_radius, 8); // call drawing circle private method for drawing circle with 8 segments 
			dawing_radius -= 0.01f; // used for giving fill to the circles that is drawn
			if (sides > 0) // making sides for the stone
			{
				sides -= 0.01f;
				glTranslatef(0.0f, -sides, 0.0f);
				draw_circle(0.5f, 0.2f, radius, 8);
				glTranslatef(0.0f, sides, 0.0f);
			}
		}
		glTranslatef(-stone.x, -stone.y, -stone.z);
		/* end drawing stone */
	} else if (stone.state == STONE_KING) // check if the stone state is king
	{
		GLfloat radius = 0.8f; // for drawing circles
		GLfloat sides = 0.5f;  // for drawing circles
		/* set stone color */
		glColor3f(stone.color.r, stone.color.g, stone.color.b);
		/* start drawing stone */
		glTranslatef(stone.x, stone.y, stone.z);
		GLfloat dawing_radius = radius;
		while (dawing_radius > 0)
		{
			draw_circle(0.5f, 0.2f, dawing_radius, 16); // call drawing circle private method for drawing circle with 16 segments
			dawing_radius -= 0.01f; // used for giving fill to the circles that is drawn
			if (sides > 0) // making sides for the stone
			{
				sides -= 0.01f;
				glTranslatef(0.0f, -sides, 0.0f);
				draw_circle(0.5f, 0.2f, radius, 16);
				glTranslatef(0.0f, sides, 0.0f);
			}
		}
		glTranslatef(-stone.x, -stone.y, -stone.z);
	}
}

#define M_PI 3.14159265358979323846  // for using radians
void Checkers::draw_circle(GLfloat cx, GLfloat cy, GLfloat r, int num_segments) // private method used for drawing circle in openGL using math.h
{
	/* start drawing circle */
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < num_segments; i++)
	{
		const GLfloat theta = 2.0f * (GLfloat)M_PI * GLfloat(i) / GLfloat(num_segments); //get the current angle
		const GLfloat x = r * cosf(theta); //calculate the x component
		const GLfloat z = r * sinf(theta); //calculate the z component
		glVertex3f(x + cx, 0, z + cy); //output 
	}
	glEnd();
	/* end drawing circle */
}

void Checkers::init_game() /* this method used for stating the game */
{
	event.type = BOARD_GAME_START;
	event.turn = PLAYER;
	doneAnimatingCam = 0;
}

void Checkers::stop_game()  /* this method used for pausing the game */
{
	event.type = BOARD_GAME_IDLE;
	doneAnimatingCam = 0;
}

void Checkers::reset_game()  /* this method used for resetting the game */
{
	for (S_CheckersBlock* checkers_block : block)
		delete checkers_block;
	for (S_CheckersStone* white_stone : white)
		delete white_stone;
	for (S_CheckersStone* black_stone : black)
		delete black_stone;
	initCheckers();
	event.type = BOARD_GAME_IDLE;
	event.turn = PLAYER;
	doneAnimatingCam = 0;
}

