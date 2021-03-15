#include <stdio.h>
#include <Windows.h>
#include <time.h>

#define SETCURSORXY(x,y) SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),(COORD){x-1,y-1});
#define PRINTBOARDCHARACTER(c) printf("%c",c);
#define CLEARSCREEN() system("cls");

struct SnakeNode {
	struct SnakeNode* parent;
	struct SnakeNode* child;
	int x;
	int y;
};

void DrawBoard(void);
void HideCusor(void);
int RunGame(void);
struct SnakeNode *MakeSnake(struct SnakeNode* parent,int x, int y);
int UpdateAndDrawSnake(struct SnakeNode* snake,struct SnakeNode* tail,int mx, int my);
void CleanSnake(struct SnakeNode* snake);
void CreateCollectible(int* x, int* y, struct SnakeNode* snake);

const char BOARD_SYMBOL_TOP_LEFT = '\xc9';
const char BOARD_SYMBOL_GROUND = '\xcd';
const char BOARD_SYMBOL_TOP_RIGHT = '\xbb';
const char BOARD_SYMBOL_SIDE = '\xba';
const char BOARD_SYMBOL_BOTTOM_LEFT = '\xc8';
const char BOARD_SYMBOL_BOTTOM_rIGHT = '\xbc';

const char BOARD_SYMBOL_SNAKE_BODY = '\xdb';
const char BOARD_SYMBOL_COLLECTIBLE = '\x0f';

const char BOARD_SYMBOL_CONTROLS_UP = '\x1e';
const char BOARD_SYMBOL_CONTROLS_DOWN = '\x1f';
const char BOARD_SYMBOL_CONTROLS_LEFT = '\x11';
const char BOARD_SYMBOL_CONTROLS_RIGHT = '\x10';

const int BOARD_WIDTH = 60;
const int BOARD_HEIGHT = 24;

int main() {
	RunGame();
}

int RunGame(void) {
	HideCusor();
	DrawBoard();

	struct SnakeNode* snake;
	snake = MakeSnake(NULL, BOARD_WIDTH / 2, BOARD_HEIGHT / 2);
	if (snake == NULL) return EXIT_FAILURE;

	struct SnakeNode* tail = MakeSnake(snake,BOARD_WIDTH / 2 -1, BOARD_HEIGHT / 2);
	tail = MakeSnake(tail,BOARD_WIDTH / 2 -1, BOARD_HEIGHT / 2);

	srand((unsigned)time(NULL));
	int score = 0;
	int sleepTime = 200;
	int movex = 1;
	int movey = 0;
	int run = 1;
	int collectibleX = rand() % (BOARD_WIDTH  - 3) + 2;
	int collectibleY = rand() % (BOARD_HEIGHT - 3) + 2;

	while (run) {
		char c = ' ';
		if (kbhit()) c = getch();			//gets User input if button was pressed

		switch (c) {
			case 'q': run = 0; break;		// QUIT
			case 'w': movex = 0; movey = -1; break; // UP
			case 'd': movex = 1; movey = 0; break;  // RIGHT
			case 's': movex = 0; movey = 1; break;  // DOWN
			case 'a': movex = -1; movey = 0; break; // LEFT
			default:break;
		}

		SETCURSORXY(collectibleX, collectibleY);
		PRINTBOARDCHARACTER(BOARD_SYMBOL_COLLECTIBLE);

		if (UpdateAndDrawSnake(snake, tail, movex, movey) != 0) run = 0;	//if player collides with himself end game

		if (snake->x == collectibleX && snake->y == collectibleY) {		//if player collects collectible, make him larger and create a new collectible
			tail->child = MakeSnake(tail, tail->x, tail->y);
			tail = tail->child;

			score++;							//increase the score and print it to the screen
			SETCURSORXY(BOARD_WIDTH + 12, 3);
			printf("%d", score);

			CreateCollectible(&collectibleX,&collectibleY, snake);

			if (sleepTime > 10) sleepTime-=5;				//decrease the sleep time to make the game more difficult
		}

		SETCURSORXY(0, BOARD_HEIGHT + 2);
		Sleep(sleepTime);
	}
	
	CleanSnake(snake);
	system("cls");
	SETCURSORXY(0, 0);
	printf("Score: %d\nThank you for playing.\n Press any Key to Exit", score);
	getch();

	return EXIT_SUCCESS;
}

void DrawBoard(void) {
	// print the board
	SETCURSORXY(0, 0);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_TOP_LEFT);
	for (int i = 0; i < BOARD_WIDTH; i++)
		PRINTBOARDCHARACTER(BOARD_SYMBOL_GROUND);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_TOP_RIGHT);
	for (int i = 0; i < BOARD_HEIGHT - 1; i++) {
		printf("\n");
		PRINTBOARDCHARACTER(BOARD_SYMBOL_SIDE);
		SETCURSORXY(BOARD_WIDTH + 2, i + 2);
		PRINTBOARDCHARACTER(BOARD_SYMBOL_SIDE);
	} 
	printf("\n");
	PRINTBOARDCHARACTER(BOARD_SYMBOL_BOTTOM_LEFT);
	for (int i = 0; i < BOARD_WIDTH; i++)
		PRINTBOARDCHARACTER(BOARD_SYMBOL_GROUND);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_BOTTOM_rIGHT);

	//print the controls
	const int controlsHeightPosition = 4;
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition - 1);
	printf(" Score: 0");
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_CONTROLS_UP);
	printf(" W");
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition + 1);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_CONTROLS_DOWN);
	printf(" S");
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition + 2);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_CONTROLS_LEFT);
	printf(" A");
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition + 3);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_CONTROLS_RIGHT);
	printf(" D");
	SETCURSORXY(BOARD_WIDTH + 4, controlsHeightPosition + 4);
	printf("Press Q to quit");
}

void HideCusor(void) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(handle, &info);
}

struct SnakeNode *MakeSnake(struct SnakeNode *parent,int x, int y) {
	struct SnakeNode *node = malloc(sizeof(struct SnakeNode));
	if (node == NULL) return NULL;
	node->x = x;
	node->y = y;
	node->parent = parent;
	if (parent != NULL)
		parent->child = node;
	node->child = NULL;
	return node;
}

int UpdateAndDrawSnake(struct SnakeNode* snake, struct SnakeNode* tail, int mx, int my) {
	snake->x += mx;
	snake->y += my;
	if (snake->x > BOARD_WIDTH) snake->x = 2;
	if (snake->y > BOARD_HEIGHT) snake->y = 2;
	if (snake->x < 2) snake->x = BOARD_WIDTH;
	if (snake->y < 2) snake->y = BOARD_HEIGHT;

	SETCURSORXY(snake->x, snake->y);
	PRINTBOARDCHARACTER(BOARD_SYMBOL_SNAKE_BODY);
	SETCURSORXY(tail->x, tail->y);
	printf(" ");

	struct SnakeNode* tmp = tail;
	while (tmp->parent != NULL) {
		if (tmp->x == snake->x && tmp->y == snake->y) return 1; 	//if the snake collides with itself return 1 and end the game
		tmp->x = tmp->parent->x;
		tmp->y = tmp->parent->y;
		tmp = tmp->parent;
	}
	return 0;
}

void CleanSnake(struct SnakeNode* snake) {
	struct SnakeNode* tmp = snake;
	while (tmp != NULL) {
		if (tmp->child != NULL) {
			tmp = tmp->child;
			free(tmp->parent);
		} else {
			free(tmp);
			return;
		}
	}
}

void CreateCollectible(int* x, int* y, struct SnakeNode* snake) {
	for(;;) {
		*x = rand() % (BOARD_WIDTH - 3) + 2;
		*y = rand() % (BOARD_HEIGHT - 3) + 2;
		struct SnakeNode* tmp = snake;
		while (tmp != NULL) {						//if part of the snake is on the collectible place the collectible at a differnet place
			if (tmp->x == x && tmp->y == y) break;
			tmp = tmp->child;
			if (tmp == NULL) return; 				//if the collectible is not on a snake part return			
		}
	}
}
