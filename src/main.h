#ifndef MAIN_H
#define MAIN_H

#define GAME_DELAY_CONSTANT		30 // в видеоуроке 10
#define GAME_DELAY_NEXTLEVEL	600 // в видеоуроке 1000
#define GAME_DELAY_PLAYERDEAD	200 // в видеоуроке 500
#define GAME_DELAY_WIN 			2000

#define SYMBOL_NOTHING 		' '
#define SYMBOL_BARRIER 		'#'
#define SYMBOL_PLAYER 		'@'
#define SYMBOL_NEXTLEVEL	'+'
#define SYMBOL_ENEMY 		'o'
#define SYMBOL_MONEYBLOCK	'?'
#define SYMBOL_EMPTYBLOCK	'-'
#define SYMBOL_MONEY		'$'

// чёрный цвет иногда не работает
#define COLOR_BACKGROUND COLOR_BLACK
#define COLOR_STANDARD COLOR_GREEN

#define COLOR_MENUTEXT COLOR_WHITE
#define COLOR_PLAYER COLOR_RED
#define COLOR_ENEMY COLOR_GREEN
#define COLOR_BARRIER COLOR_CYAN
#define COLOR_MONEYBLOCK COLOR_MAGENTA
#define COLOR_NEXTLEVEL COLOR_YELLOW
#define COLOR_MONEY COLOR_YELLOW

#define CHITS 0
#define OBJECT_MAX_NUMBER 50

#define SCORE_INCR_MONEY 50
#define SCORE_INCR_KILL 100

#define GAMEMODE_MENU_START 0
#define GAMEMODE_PLAY 		1
#define GAMEMODE_MENU_PAUSE 2

void BeforeCloseGame();

char logo[7][35];

void initLogo() {
	sprintf(("%s", logo[0]), " 000       0         0    ");
	sprintf(("%s", logo[1]), " 0  0      0         0    ");
	sprintf(("%s", logo[2]), " 0  0  000 000   00  000  ");
	sprintf(("%s", logo[3]), " 000  0  0 0    0  0 0    ");
	sprintf(("%s", logo[4]), " 0  0 0  0 0    0000 0    ");
	sprintf(("%s", logo[5]), " 0  0 0  0 0    0    0    ");
	sprintf(("%s", logo[6]), " 000   000  000  000  000 ");
}

char map[MAP_HEIGHT][MAP_WIDTH+1];
int mapColors[MAP_HEIGHT][MAP_WIDTH+1];
int keypress = 0;
int level = 1;
int score = 0;
int scorePrev = 0;
int scoreSecretLevel = 2000;
char nickname[] = "changeme";
int backgroundEffectTime = 0;
int menu = GAMEMODE_MENU_START;
int openSecretLevel = 0;

typedef struct sObject {
	float x, y;
	float width, height;
	
	float vertSpeed;
	float horizSpeed;
	int isFly;
	
	char drawSymbol;
	int color;
} tObject;

tObject mario;
tObject brick[OBJECT_MAX_NUMBER];
int brickLenght;

tObject moving[OBJECT_MAX_NUMBER];
int movingLenght;

#endif // MAIN_H
