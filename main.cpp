/* ========================================================================== */
/*                                                                            */
/*   main.cpp                                                                 */
/*   (c) 2018 Jawdat Abdullah & Ronnie Moadi                                  */
/*                                                                            */
/*   main file of the project                                                 */
/*   connecting all the pieces of the game project                            */
/*   loading rendering data using OpenGL GLUT window                          */
/*   controlling the game events and actions                                  */
/*                                                                            */
/* ========================================================================== */

/*   -------- Includes ---------   */
//#include <stdlib.h> // for random in easy mode
//#include "graphics/renderer.h" // header for loading and drawing envrionment and table in openGL world
//#include "game/checkers.h" // header for loading checkers(draught) game class, structs, and global functions
#include "game/Steps.h" //header for global functions used for checkers game
#include <ctime> // used for manipulating computer movement speed


/*   -------- Defines ---------   */
/* global variables for calculating screen width and height, used for manipulate actions when mouse is clicked */
#define CELL_WIDTH glutGet(GLUT_WINDOW_WIDTH) / 8   // OpenGL screen width divided by 8
#define CELL_HEIGHT glutGet(GLUT_WINDOW_HEIGHT) / 8 // OpenGL screen height divided by 8

/*   -------- Functions --------   */
// OpenGL
void init();   // initiate openGL functions and openGL componenets that loaded once
void idle();   // loading glutIdleFunc commands
void render(); // loading glutDisplayFunc commands
// Input
void keyboard(unsigned char key, int x, int y);  // loading glutKeyboardFunc commands
void mouse(int button, int state, int x, int y); // loading glutMouseFunc commands
void mouse_passive(int x, int y);                // loading glutPassiveMotionFunc commands
// Game
void print_intro();       // print a hello message on console when openGL window is created successfuly
void animate_cam(E_BoardEventType event); // for changing camera position when checkers game event changed
void idle_menu_values();  // loades texts and colors for rendering idle menu buttons
void hover_menu_values(); // loades texts and colors for rendering idle menu buttons when mouse is hovered
void render_menu();       // render and print idle menu buttons
void print_game_menu();   // print text for pause button when game is started
void change_difficulty(); // to switch between checkers difficulty options (EASY/HARD)
void print_result();      // print result message on the screen when the game is finished
void multiplayer_click();
// Checkers events
void stone_animations();  // animating stones movement, visualizing stone movement

/*   -------- Variables --------   */
/* Screen */
GLfloat SCREEN_WIDTH = 900; // openGL window width
GLfloat SCREEN_HIGHT = 900; // openGL window hight
/* OpenGL camera */
// Rotation
float rotxv = 0.0f; // camera x-axis rotation value
float rotyv = 0.0f; // camera y-axis rotation value
float rotzv = 0.0f; // camera z-axis rotation value
// Position
float camx = 0.2f;   // camera x-axis position value
float camy = -8.0f;  // camera y-axis position value
float camz = -30.0f; // camera z-axis position value
/* OpenGL lightning */
float xp, yp, zp;
/* Environment */
// Room
float room_length = 40.0f, room_height = 30.0f, room_width = 40.0f; // decleration for creating Environment class
Environment room(room_length, room_height, room_width);             // creating Environment class
// Table
float table_length = 8.0f, table_width = 8.0f, table_height = 3.0f; // decleration for creating Table class
Table table(table_length, table_width, table_height, 0.4f);         // creating Table class
/* Checkers(draught) */
int checkers_cells_row_number = 8;                                                     // number of rows and columns for checkers(draught) board
Checkers checkers(table_length, table_height, table_width, checkers_cells_row_number); // creating Checkers class (board 8 x 8 )
/* Mouse cursor */
GLint mouse_cursor_x = 0.0f, mouse_cursor_y = 0.0f; // for allocating the mouse position at the current time, used for mouse clicks manipulations
/* Game */
float count_down = 100;              // count down before switching game event
GLvec3Color start_button_color;      // start background color
GLvec3Color start_text_color;        // start text color
GLvec3Color restart_button_color;    // restart background color
GLvec3Color restart_text_color;      // restart text color
GLvec3Color diffeculty_button_color; // diffeculty background color
GLvec3Color diffeculty_text_color;   // diffeculty text color
GLvec3Color multiplayer_box_color;   // multiplayer background color
GLvec3Color multiplayer_button_color; // diffeculty background color
GLvec3Color multiplayer_text_color;  // multiplayer text color


/* ---------- main ---------- */
int main(int argc, char *argv[])
{
	/* ----------- Launch openGL graphics ----------- */
	// Initialize GLUT
	glutInit(&argc, argv);
	// Set up some memory buffers for our display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	// Set the window size
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_WIDTH);
	// Create the window with the title
	glutCreateWindow("Damka 3D");
	/* ----------- Load game ----------- */
	print_intro();
	// Set the display function
	glutDisplayFunc(render);
	// Set the keyboard functions
	glutKeyboardFunc(keyboard);
	// Set the mouse functions
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mouse_passive); // used for handling mouse cursor clicks
	// Set idle function
	glutIdleFunc(idle);
	// Initialiate openGL components
	init();
	// Enters the GLUT event processing loop
	glutMainLoop();
	// When GLUT window closed
	return 0;
}

/* loading glutIdleFunc commands */
void idle()
{
	if (checkers.event.type == BOARD_GAME_START) // if the game is started
		animate_cam(BOARD_GAME_START); // loading the data for camera position
	if (checkers.event.type == BOARD_GAME_IDLE)  // if the game is stopped
		animate_cam(BOARD_GAME_IDLE); // loading the data for camera position

	if (checkers.event.type == BOARD_GAME_START) {
		checkers.update();      // updates checkers class, when actions and events are happened
		check_result(checkers); // check what is the result of the game at the moment
	}
	/* Game Result
	 * print a message when the game is finished
	 * count down before perform an action
	 * change the result to RESULT_NOTYET allowing player for further viewing current game state
	 * pause the game returning to the idle menu
	 */
	if (checkers.result == RESULT_WON) // if player is the winner and the opponent (computer) is the loser
	{
		checkers.event.turn = EMPTY;
		if (count_down <= 0)
		{
			count_down = 100;
			checkers.result = RESULT_NOTYET;
			checkers.stop_game();
			if (checkers.event.difficulty == MULTIPLAYER) {
				closeConnection();
				checkers.reset_game();
			}
		}
		count_down -= 0.1;
	} else if (checkers.result == RESULT_LOST) // if the opponent (computer) is the winner and player is the loser
	{
		checkers.event.turn = EMPTY;
		if (count_down <= 0)
		{
			count_down = 100;
			checkers.result = RESULT_NOTYET;
			checkers.stop_game();
			if (checkers.event.difficulty == MULTIPLAYER) {
				closeConnection();
				checkers.reset_game();
			}
		}
		count_down -= 0.1;
	} else {
		//if RESULT_NOTYET
		stone_animations();
	}

	/* Game event change
	 * when starting the game
	 * when stopping the game
	 */

	if (checkers.event.difficulty == MULTIPLAYER && checkers.MPSTATUS == MP_WAITING && checkers.doneAnimatingCam) {

		//send request to server asking for turn
		sendToServer("turn.");
		printf("waiting\n");
		char *buffer = (char*)malloc(sizeof(char) * MAXLINE + 1);
		recieveFromServer(buffer);
		printf("%s\n", buffer);
		if (buffer[0] == '1') {
			checkers.MPSTATUS = MP_WAITING;
			checkers.event.turn = COMPUTER;
			checkers.doneAnimatingCam = 0;
			printf("turn computer\n");
		} else if (buffer[0] == '0') {
			checkers.MPSTATUS = MP_PLAYING;
			checkers.event.turn = PLAYER;
			checkers.doneAnimatingCam = 0;
			printf("turn player\n");

		}
	}
	render(); // call openGL render function
}

/* loading glutDisplayFunc commands */
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear screen
	glLoadIdentity(); // resets the current matrix

	/******************* Camera positioning **********************/
	glTranslatef(camx, camy, camz);

	/* make changes according to game event state*/
	if (checkers.event.type == BOARD_GAME_START)
	{
		if (rotxv >= 90.0f)    // print text on the screen, for the pause(stop) game button
			print_game_menu();
		print_result();        // print message  on the screen, when the game is finished
	}
	if (checkers.event.type == BOARD_GAME_IDLE)
	{
		if (rotxv <= 0.0f) // render menu buttons
		{
			idle_menu_values();
			hover_menu_values();
			render_menu();
		}
	}

	/******************* Camera rotation **********************/
	/* rotation camera between BOARD_GAME_START/BOARD_GAME_IDLE */
	glRotatef(rotxv, 1, 0, 0);
	glRotatef(rotyv, 0, 1, 0);
	glRotatef(rotzv, 0, 0, 1);

	/****************** Draw room elements ***********************/
	room.draw();
	/********************* Draw table ****************************/
	table.draw();
	/********************* Draw checkers *************************/
	checkers.draw();
	glutSwapBuffers();	// display the output
}

/* loading glutMouseFunc commands */
void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {


	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		if (checkers.event.type == BOARD_GAME_IDLE)
		{
			if (mouse_cursor_x >= CELL_WIDTH + (CELL_WIDTH * 3 + CELL_WIDTH / 5) && mouse_cursor_x <= CELL_WIDTH + (CELL_WIDTH * 4 + CELL_WIDTH / 2))
			{
				// Play
				if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 2 + CELL_HEIGHT / 4) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 2 + CELL_HEIGHT / 2))
				{
					checkers.init_game();
				}
				// Restart
				else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 3 - CELL_HEIGHT / 6) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 3 + CELL_HEIGHT / 8))
				{
					checkers.reset_game();
				}
				// Multiplayer
				else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 4 - CELL_HEIGHT / 2) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 4 - CELL_HEIGHT / 3))
				{
					multiplayer_click();
				}
				// Change difficulty
				else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT / 10 * 3) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT / 10 * 3) + CELL_HEIGHT / 2)
				{
					change_difficulty();
				}
			}
		} else if (checkers.event.type == BOARD_GAME_START)
		{
			// Pause
			if (mouse_cursor_x >= 1 && mouse_cursor_x <= CELL_WIDTH)
			{
				if (mouse_cursor_y >= CELL_HEIGHT * 7 && mouse_cursor_y <= CELL_HEIGHT * 8)
					checkers.stop_game();
			}
			int col, row;
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 8 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 7) // col 0
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 8 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7) // row 1 block[0]
				{
					col = 0;
					row = 0;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5) // row 3 block[16]
				{
					col = 0;
					row = 2;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3) // row 5 block[32]
				{
					col = 0;
					row = 4;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT) // row 7 block[48]
				{
					col = 0;
					row = 6;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 7 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 6) // col 1
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6) // row 1 block[0]
				{
					col = 1;
					row = 1;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4) // row 3 block[16]
				{
					col = 1;
					row = 3;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2) // row 5 block[32]
				{
					col = 1;
					row = 5;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 1 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 0) // row 7 block[48]
				{
					col = 1;
					row = 7;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 6 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 5) // col 2
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 8 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7) // row 1 block[0]
				{
					col = 2;
					row = 0;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5) // row 3 block[16]
				{
					col = 2;
					row = 2;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3) // row 5 block[32]
				{
					col = 2;
					row = 4;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT) // row 7 block[48]
				{
					col = 2;
					row = 6;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 5 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 4) // col 3
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6) // row 1 block[0]
				{
					col = 3;
					row = 1;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4) // row 3 block[16]
				{
					col = 3;
					row = 3;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2) // row 5 block[32]
				{
					col = 3;
					row = 5;
					applyClick(checkers, col, row);

				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 1 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 0) // row 7 block[48]
				{
					col = 3;
					row = 7;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 4 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 3) // col 4
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 8 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7) // row 1 block[0]
				{
					col = 4;
					row = 0;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5) // row 3 block[16]
				{
					col = 4;
					row = 2;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3) // row 5 block[32]
				{
					col = 4;
					row = 4;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT) // row 7 block[48]
				{
					col = 4;
					row = 6;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 3 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 2) // col 5
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6) // row 1 block[0]
				{
					col = 5;
					row = 1;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4) // row 3 block[16]
				{
					col = 5;
					row = 3;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2) // row 5 block[32]
				{
					col = 5;
					row = 5;
					applyClick(checkers, col, row);

				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 1 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 0) // row 7 block[48]
				{
					col = 5;
					row = 7;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 2 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 1) // col 6
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 8 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7) // row 1 block[0]
				{
					col = 6;
					row = 0;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5) // row 3 block[16]
				{
					col = 6;
					row = 2;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3) // row 5 block[32]
				{
					col = 6;
					row = 4;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT) // row 7 block[48]
				{
					col = 6;
					row = 6;
					applyClick(checkers, col, row);
				}
			}
			if (mouse_cursor_x >= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 1 && mouse_cursor_x <= glutGet(GLUT_WINDOW_WIDTH) - CELL_WIDTH * 0) // col 7
			{
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 7 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 6) // row 1 block[0]
				{
					col = 7;
					row = 1;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 5 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 4) // row 3 block[16]
				{
					col = 7;
					row = 3;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 3 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 2) // row 5 block[32]
				{
					col = 7;
					row = 5;
					applyClick(checkers, col, row);
				}
				if (mouse_cursor_y >= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 1 && mouse_cursor_y <= glutGet(GLUT_WINDOW_HEIGHT) - CELL_HEIGHT * 0) // row 7 block[48]
				{
					col = 7;
					row = 7;
					applyClick(checkers, col, row);
				}
			}
		}
	}
}

/* animating stones movement, visualizing stone movement */
void stone_animations()
{
	// Stones animations
	for (auto& i : checkers.block) {
		int isAnimatingX = 0, isAnimatingY = 0, isAnimatingZ = 0;
		if (i->stone != NULL) {
			if (i->stone->isAnimating) {
				checkers.isAnimating = true;
				if (difference(i->stone->x, i->stone->animx) > 0.0005) {
					printf("(%f, %f)\n", i->stone->x, i->stone->animx);
					isAnimatingX = 1;
					if (i->stone->animx > i->stone->x)
						i->stone->x += 0.1f;
					else
						i->stone->x -= 0.1f;
				} else
					isAnimatingX = 0;

				if (difference(i->stone->y, i->stone->animy) > 0.0005) {
					isAnimatingY = 1;
					if (i->stone->animy > i->stone->y)
						i->stone->y += 0.1;
					else
						i->stone->y -= 0.1;
				} else
					isAnimatingY = 0;

				if (difference(i->stone->z, i->stone->animz) > 0.0005) {
					isAnimatingZ = 1;
					if (i->stone->animz > i->stone->z)
						i->stone->z += 0.1;
					else
						i->stone->z -= 0.1;
				} else
					isAnimatingZ = 0;

			}
			if (isAnimatingX + isAnimatingY + isAnimatingZ == 0)
				i->stone->isAnimating = false;
		}
	}
	bool isStillAnimating = false;
	for (auto& i : checkers.block) {
		if (i->stone != NULL)
			if (i->stone->isAnimating) {
				isStillAnimating = true;
				break;
			}
	}
	if (!isStillAnimating) {
		checkers.isAnimating = false;
		if (checkers.event.turn == COMPUTER) {
			if (checkers.doneAnimatingCam) {
				Sleep(1000);
				applyComputerStep(checkers);
			}

		}
	}
}

/* loading glutPassiveMotionFunc commands */
void mouse_passive(int x, int y)
{
	mouse_cursor_x = x; //place current mouse pos in mouse_x
	mouse_cursor_y = y; //place current mouse pos in mouse_y
}

/* loading glutKeyboardFunc commands */
void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) // Exit OpenGL
		exit(1);
	// move camera axis
	if (key == 'a')
		camx++;
	if (key == 'd')
		camx--;
	if (key == 's')
		camy++;
	if (key == 'w')
		camy--;
	if (key == 'z')
		camz++;
	if (key == 'x')
		camz--;
	glutPostRedisplay();
}

/* for changing camera position when checkers game event changed */
void animate_cam(E_BoardEventType event)
{

	if (event == BOARD_GAME_START) {
		if (checkers.event.turn == PLAYER)
		{
			int flag = 0;

			if (rotxv <= 90.0f)
			{
				rotxv += 0.5;
			} else
				flag++;
			if (rotyv > 0.0f)
			{
				rotyv -= 0.8;
			} else 
				flag++;


			if (camy <= -0.19f)
			{
				camy += 0.05;
			} else
				flag++;

			if (camz <= -22.9f)
			{
				camz += 0.05;
			} else
				flag++;

			if (camx <= 0.2f)
			{
				camx += 0.05;
			} else
				flag++;
			if (flag == 5)
				checkers.doneAnimatingCam = 1;
			else
				checkers.doneAnimatingCam = 0;

		} else if (checkers.event.turn == COMPUTER)
		{
			int flag = 0;
			if (rotxv >= 50.0f)
			{
				rotxv -= 0.3;
			} else
				flag++;

			if (rotyv <= 180.0f)
			{
				rotyv += 0.8;
			} else
				flag++;

			if (camy >= -0.19f)
			{
				camy -= 0.05;
			} else
				flag++;

			if (camz >= -27.9f)
			{
				camz -= 0.05;
			} else
				flag++;

			if (camx >= 0.2f)
			{
				camx -= 0.05;
			} else
				flag++;

			if (flag == 5)
				checkers.doneAnimatingCam = 1;
			else
				checkers.doneAnimatingCam = 0;

		}
	} else {
		int flag = 0;
		if (rotxv >= 0.0f)
		{
			rotxv -= 0.5;
		} else
			flag++;

		if (camy >= -8.0f)
		{
			camy -= 0.05;
		} else
			flag++;

		if (camz >= -30.0f)
		{
			camz -= 0.05;
		} else
			flag++;

		if (camx >= 0.2f)
		{
			camx -= 0.05;
		} else
			flag++;

		if (flag == 4)
			checkers.doneAnimatingCam = 1;
		else
			checkers.doneAnimatingCam = 0;
	}


}

/* initiate openGL functions and openGL componenets that loaded once */
void init()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, SCREEN_WIDTH / SCREEN_HIGHT, 0.1f, 100000.0f);
	glMatrixMode(GL_MODELVIEW);
	// Create light components
	GLfloat ambientLight[] = { 0.87f, 0.76f, 0.35f, 1.0f };
	GLfloat diffuseLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat specularLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat mat_shininess[] = { 128.0f };
	//x,y,z,light
	GLfloat position0[] = { xp, yp, zp, 1.0f };
	// Assign created components to GL_LIGHT0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glMaterialfv(GL_MAX_LIGHTS, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, position0);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
}

/* loades texts and colors for rendering idle menu buttons */
void idle_menu_values()
{
	/* Menu buttons */
	/* for drawing background*/
	start_button_color = GLvec3Color(0.0f, 0.0f, 0.0f);
	restart_button_color = GLvec3Color(0.0f, 0.0f, 0.0f);
	multiplayer_button_color = GLvec3Color(0.0f, 0.0f, 0.0f);
	/* for drawing text*/
	start_text_color = GLvec3Color(1.0, 1.0, 1.0);
	restart_text_color = GLvec3Color(1.0, 1.0, 1.0);
	multiplayer_text_color = GLvec3Color(1.0, 1.0, 1.0);

	/* Diffeculty buttons */
	if (checkers.event.difficulty == EASY)
	{
		/* for drawing background*/
		diffeculty_button_color = GLvec3Color(0.0f, 0.51f, 0.6f);
		/* for drawing text*/
		diffeculty_text_color = GLvec3Color(1.0, 1.0, 1.0);
	} else if (checkers.event.difficulty == HARD)
	{
		/* for drawing background*/
		diffeculty_button_color = GLvec3Color(0.16f, 0.0f, 0.28f);
		/* for drawing text*/
		diffeculty_text_color = GLvec3Color(1.0, 1.0, 1.0);
	} else
	{
		/* for drawing background*/
		diffeculty_button_color = GLvec3Color(0.65f, 0.6f, 0.44f);
		/* for drawing text*/
		diffeculty_text_color = GLvec3Color(0.65f, 0.6f, 0.44f);

		multiplayer_box_color = GLvec3Color(0.21, 0.8, 0.82);
		multiplayer_text_color = GLvec3Color(1.0, 1.0, 1.0);
	}
}

/* loades texts and colors for rendering idle menu buttons when mouse is hovered */
void hover_menu_values()
{
	if (mouse_cursor_x >= CELL_WIDTH + (CELL_WIDTH * 3 + CELL_WIDTH / 5) && mouse_cursor_x <= CELL_WIDTH + (CELL_WIDTH * 4 + CELL_WIDTH / 2))
	{
		// Play
		if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 2 + CELL_HEIGHT / 4) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 2 + CELL_HEIGHT / 2))
		{
			start_button_color = GLvec3Color(1.0f, 1.0f, 1.0f);
			start_text_color = GLvec3Color(0.0, 0.0, 0.0);
		}
		// Restart
		else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 3 - CELL_HEIGHT / 6) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 3 + CELL_HEIGHT / 8))
		{
			restart_button_color = GLvec3Color(1.0f, 1.0f, 1.0f);
			restart_text_color = GLvec3Color(0.0, 0.0, 0.0);
		}
		//mulitplayer
		else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT * 4 - CELL_HEIGHT / 2) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT * 4 - CELL_HEIGHT / 3))
		{
			multiplayer_button_color = GLvec3Color(1.0f, 1.0f, 1.0f);
			multiplayer_text_color = GLvec3Color(0.0, 0.0, 0.0);
		}
		// Change difficulty
		else if (mouse_cursor_y >= CELL_HEIGHT + (CELL_HEIGHT / 10 * 3) && mouse_cursor_y <= CELL_HEIGHT + (CELL_HEIGHT / 10 * 3) + CELL_HEIGHT / 2)
		{
			if (checkers.event.difficulty == EASY)
			{
				diffeculty_button_color = GLvec3Color(1.0f, 0.49f, 0.4f);
				diffeculty_text_color = GLvec3Color(0.0, 0.0, 0.0);
			} else
			{
				diffeculty_button_color = GLvec3Color(0.84f, 1.0f, 0.72f);
				diffeculty_text_color = GLvec3Color(0.0, 0.0, 0.0);
			}
		}
	}
}

/* render and print idle menu buttons */
void render_menu()
{
	/* Menu Buttons */
	// Start
	// background
	glColor3f(start_button_color.r, start_button_color.g, start_button_color.b);
	glBegin(GL_QUADS);
	glVertex3f(-0.7f, 9.8f, -1.2f);
	glVertex3f(5.9f, 9.8f, -1.2f);
	glVertex3f(5.9f, 10.8f, -1.2f);
	glVertex3f(-0.7f, 10.8f, -1.2f);
	glEnd();
	// text
	char text2[20];
	sprintf_s(text2, "Click here to start");
	char *str2 = text2;
	glColor3f(start_text_color.r, start_text_color.g, start_text_color.b);
	glRasterPos3f(0.0f, 10.0f, 0.0f);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str2); while (*(++str2));
	// Restart
	// background
	glColor3f(restart_button_color.r, restart_button_color.g, restart_button_color.b);
	glBegin(GL_QUADS);
	glVertex3f(-0.7f, 7.7f, -1.2f);
	glVertex3f(5.9f, 7.7f, -1.2f);
	glVertex3f(5.9f, 8.7f, -1.2f);
	glVertex3f(-0.7f, 8.7f, -1.2f);
	glEnd();
	// text
	char text3[22];
	glColor3f(restart_text_color.r, restart_text_color.g, restart_text_color.b);
	sprintf_s(text3, "Click here to restart");
	char *str3 = text3;
	glRasterPos3f(0.0f, 8.0f, 0.0f);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str3); while (*(++str3));
	// Muliplayer
	// background
	glColor3f(multiplayer_button_color.r, multiplayer_button_color.g, multiplayer_button_color.b);
	glBegin(GL_QUADS);
	glVertex3f(-0.7f, 5.7f, -1.2f);
	glVertex3f(7.3f, 5.7f, -1.2f);
	glVertex3f(7.3f, 6.7f, -1.2f);
	glVertex3f(-0.7f, 6.7f, -1.2f);
	glEnd();
	// text
	char text4[27];
	glColor3f(multiplayer_text_color.r, multiplayer_text_color.g, multiplayer_text_color.b);
	sprintf_s(text4, "Click here for Multiplayer");
	char *str4 = text4;
	glRasterPos3f(0.0f, 6.0f, 0.0f);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str4); while (*(++str4));
	/* Diffeculty Buttons*/
	if (checkers.event.difficulty == EASY)
	{
		// background
		glColor3f(diffeculty_button_color.r, diffeculty_button_color.g, diffeculty_button_color.b);
		glBegin(GL_QUADS);
		glVertex3f(-0.5f, 16.0f, -1.2f);
		glVertex3f(11.2f, 16.0f, -1.2f);
		glVertex3f(11.2f, 17.0f, -1.2f);
		glVertex3f(-0.5f, 17.0f, -1.2f);
		glEnd();
		// text
		char text4[38];
		glColor3f(diffeculty_text_color.r, diffeculty_text_color.g, diffeculty_text_color.b);
		sprintf_s(text4, "Click here to change difficulty: EASY");
		char *str4 = text4;
		glRasterPos3f(0.0f, 16.0f, 0.0f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str4); while (*(++str4));
	} else if (checkers.event.difficulty == HARD)
	{
		// background
		glColor3f(diffeculty_button_color.r, diffeculty_button_color.g, diffeculty_button_color.b);
		glBegin(GL_QUADS);
		glVertex3f(-0.5f, 16.0f, -1.2f);
		glVertex3f(11.2f, 16.0f, -1.2f);
		glVertex3f(11.2f, 17.0f, -1.2f);
		glVertex3f(-0.5f, 17.0f, -1.2f);
		glEnd();
		// text
		char text4[38];
		glColor3f(diffeculty_text_color.r, diffeculty_text_color.g, diffeculty_text_color.b);
		sprintf_s(text4, "Click here to change difficulty: HARD");
		char *str4 = text4;
		glRasterPos3f(0.0f, 16.0f, 0.0f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str4); while (*(++str4));
	}
	/*Multiplayer Mode*/
	if (checkers.event.difficulty == MULTIPLAYER)
	{
		// background
		glColor3f(multiplayer_box_color.r, multiplayer_box_color.g, multiplayer_box_color.b);
		glBegin(GL_QUADS);
		glVertex3f(-0.5f, 16.0f, -1.2f);
		glVertex3f(11.2f, 16.0f, -1.2f);
		glVertex3f(11.2f, 17.0f, -1.2f);
		glVertex3f(-0.5f, 17.0f, -1.2f);
		glEnd();
		// text
		char text5[17];
		glColor3f(multiplayer_text_color.r, multiplayer_text_color.g, multiplayer_text_color.b);
		sprintf_s(text5, "Multiplayer Mode");
		char *str5 = text5;
		glRasterPos3f(0.0f, 16.0f, 0.0f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str5); while (*(++str5));
	}
}

/* print text for pause button when game is started */
void print_game_menu()
{
	char text1[11];
	sprintf_s(text1, "Click here");
	char *str1 = text1;
	glColor3f(1.0, 1.0, 1.0);
	glRasterPos3f(-8.0f, -6.5f, 3.5f);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *str1); while (*(++str1));

	char text2[9];
	sprintf_s(text2, "To Pause");
	char *str2 = text2;
	glRasterPos3f(-8.0f, -7.5f, 3.5f);
	do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, *str2); while (*(++str2));
	if (checkers.event.difficulty == MULTIPLAYER)
	{
		if (checkers.MPSTATUS == MP_WAITING)
		{
			char text3[8];
			sprintf_s(text3, "Waiting");
			char *str3 = text3;
			glColor3f(0.0, 0.41, 0.65);
			glRasterPos3f(-6.0f, -3.5f, 3.7f);
			do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str3); while (*(++str3));
		}
	}
}

/* to switch between checkers difficulty options (EASY/HARD) */
void change_difficulty()
{
	if (checkers.event.difficulty == EASY)
		checkers.event.difficulty = HARD;
	else if (checkers.event.difficulty == HARD)
		checkers.event.difficulty = EASY;
	else {
		//Multiplayer Mode
	}


}

/* print result message on the screen when the game is finished */
void print_result()
{
	if (checkers.result == RESULT_WON)
	{
		/*draw background*/
		glColor3f(0.8f, 0.4f, 0.4f);
		glBegin(GL_QUADS);
		glVertex3f(-1.9f, -0.2f, 4.2f);
		glVertex3f(1.5f, -0.2f, 4.2f);
		glVertex3f(1.5f, -1.4f, 4.2f);
		glVertex3f(-1.9f, -1.4f, 4.2f);
		glEnd();
		/*draw text*/
		char text5[19];
		sprintf_s(text5, "Congratulations!!!");
		char *str5 = text5;
		glColor3f(0.2, 1.0, 0.6);
		glRasterPos3f(-1.7f, -0.7f, 4.5f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str5); while (*(++str5));
		char text6[15];
		sprintf_s(text6, "       You Won");
		char *str6 = text6;
		glRasterPos3f(-1.7f, -1.2f, 4.5f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str6); while (*(++str6));
	} else if (checkers.result == RESULT_LOST)
	{
		/*draw background*/
		glColor3f(0.5f, 0.5f, 0.4f);
		glBegin(GL_QUADS);
		glVertex3f(-0.2f, 0.0f, 4.2f);
		glVertex3f(1.7f, 0.0f, 4.2f);
		glVertex3f(1.7f, -1.3f, 4.2f);
		glVertex3f(-0.2f, -1.3f, 4.2f);
		glEnd();
		/*draw text*/
		char text5[9];
		sprintf_s(text5, "    Lost");
		char *str5 = text5;
		glColor3f(0.6, 0.9, 1.0);
		glRasterPos3f(0.0f, -0.5f, 4.5f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str5); while (*(++str5));
		char text6[10];
		sprintf_s(text6, "Try again");
		char *str6 = text6;
		glRasterPos3f(0.0f, -1.0f, 4.5f);
		do glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *str6); while (*(++str6));
	}
}

/* print a hello message on console when openGL window is created successfuly */
void print_intro()
{
	printf("\nDamka3D Game\n");
	printf("By: Jawdat Abdullah & Ronny Moadi \n");
	printf("Checkers game (draughts)  \n");
	printf("\n");
}

void multiplayer_click()
{
	if (checkers.event.difficulty != MULTIPLAYER)
	{
		if (checkers.MPSTATUS == MP_OFFLINE) {
			if (initConnection()) {
				checkers.MPSTATUS = MP_WAITING;
				checkers.event.type = BOARD_GAME_START;
				checkers.event.difficulty = MULTIPLAYER;
				checkers.doneAnimatingCam = 0;
			} else {
				//connection failed
				printf("\nError: could not initiate connection with the server");
			}
		}
	} else
	{
		closeConnection();
		checkers.MPSTATUS = MP_OFFLINE;
		checkers.event.type = BOARD_GAME_IDLE;
		checkers.event.turn = PLAYER;
		checkers.event.difficulty = EASY;
		checkers.doneAnimatingCam = 0;
	}
}