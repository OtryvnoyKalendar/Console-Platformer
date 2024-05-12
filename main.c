#include <stdlib.h> // для exit()
#include <ncurses.h> // работа с выводом в консоль
#include <time.h> // для time()
#include <string.h> // для sprintf(), strlen()
#include <signal.h> // для signal()
#include <math.h> // для round()

// мои функции
#include "core.h"
#include "keys.h"
// мои объявления
#include "main.h"

void initConsoleMode() {
	OpenNcursesMode();
	OpenKeysMode();
}

void quitConsoleMode() {
	CloseNcursesMode();
	CloseKeysMode();
}

void BeforeCloseGame() {
	quitConsoleMode();
}

// рисование карты

int isMapCellExists(int y, int x) {
	return ( x >= 0 && x < MAP_WIDTH && y >= 0 &&  y < MAP_HEIGHT);
}

void putSymbolToConsole(int y, int x, char ch, int color) {
	if (!isMapCellExists(y, x))
		ErrorCloseGame("выход за границы игрового поля");
	map[y][x] = ch;
	move(y, x);
	if (color != -1)
		attron(COLOR_PAIR(color));
	addch(ch);
}

void showMap() {
	int color;
	for(int i=0; i<MAP_HEIGHT; i++)
		for(int j=0; j<MAP_WIDTH; j++) {
			color = mapColors[i][j];  // был COLOR_STANDARD
			if(j > 0 && color == mapColors[i][j-1])
				color = -1;
				
			putSymbolToConsole(i, j, map[i][j], color);
		}
	refresh();
}



// код Антона

void clearMap() {
	for(int i=0; i<MAP_WIDTH; i++)
		map[0][i] = SYMBOL_NOTHING;
	
	for(int i=1; i<MAP_HEIGHT; i++)
		sprintf(map[i], "%s", map[0]);
}


void setObjectPos(tObject *obj, float xPos, float yPos) {
	(*obj).x = xPos;
	(*obj).y = yPos;
}

void initObject(tObject *obj, float xPos, float yPos, float oWidth, float oHeight, char drawSymbol, int color) {
	setObjectPos(obj, xPos, yPos);
	(*obj).width = oWidth;
	(*obj).height = oHeight;
	(*obj).vertSpeed = 0;
	(*obj).horizSpeed = 0.6;
	(*obj).drawSymbol = drawSymbol;
	(*obj).color = color;
}

int isCollision(tObject obj1, tObject obj2) {
	return ((obj1.x+obj1.width) > obj2.x) && (obj1.x < (obj2.x+obj2.width)) &&
			((obj1.y+obj1.height) > obj2.y) && (obj1.y < (obj2.y+obj2.height));
}

void createLevel();
void showMessage(const char* text, int gameDelayTime, int backgroundColor);

tObject* getNewBrick() {
	brickLenght++;
	return &brick[brickLenght - 1];
}

tObject* getNewMoving() {
	movingLenght++;
	return &moving[movingLenght - 1];
}

void playerDead() {
	score = scorePrev;
	showMessage("You're dead!", GAME_DELAY_PLAYERDEAD, COLOR_RED);
	createLevel();
}

void vertMoveObject(tObject *obj) {
	(*obj).isFly = 1;
	(*obj).vertSpeed += 0.07;
	setObjectPos(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);
	
	for(int i=0; i<brickLenght; i++)
		if (isCollision(*obj, brick[i])) {
			if(obj[0].vertSpeed > 0)
				(*obj).isFly = 0;
			
			if ( brick[i].drawSymbol == SYMBOL_MONEYBLOCK && obj[0].vertSpeed < 0
				&& obj == &mario) {
				brick[i].drawSymbol = SYMBOL_EMPTYBLOCK;
				initObject(getNewMoving(), brick[i].x, brick[i].y-3, 3, 2, SYMBOL_MONEY, COLOR_MONEY);
				moving[movingLenght - 1].vertSpeed = -0.7;
			}
			
			(*obj).y -= (*obj).vertSpeed;
			(*obj).vertSpeed = 0;
			
			if(brick[i].drawSymbol == SYMBOL_NEXTLEVEL
			&& (*obj).drawSymbol == SYMBOL_PLAYER) {
				level++;
				createLevel();
				showMessage("Next level!", GAME_DELAY_NEXTLEVEL, COLOR_GREEN);
			}
			break;
		}
}

void deleteMoving(int i) {
	for(int j=i; j<movingLenght-1; j++)
		moving[j] = moving[j+1];
	
	movingLenght--;
}

void setBackgroundColorForTime(int backgroundColor, int timerNew) {
	backgroundEffectTime = timerNew;
	setBackgroundColor(backgroundColor);
}

void marioCollision() {
	for(int i=0; i<movingLenght; i++)
		if(isCollision(mario, moving[i]))
			if(moving[i].drawSymbol == SYMBOL_ENEMY) {
				if(
				(mario.isFly && mario.vertSpeed > 0
				&& (mario.y + mario.height < moving[i].y + moving[i].height*0.5))
				|| CHITS
				) {
					deleteMoving(i);
					i--;
					score += SCORE_INCR_KILL;
					continue;
				}
				else
					playerDead();
			}
			else if(moving[i].drawSymbol == SYMBOL_MONEY) {
				deleteMoving(i);
				i--;
				score += SCORE_INCR_MONEY;
				setBackgroundColorForTime(COLOR_YELLOW,	5);
				continue;
			}
}

void horizonMoveObject(tObject *obj) {
	obj[0].x += obj[0].horizSpeed;
	
	for(int i=0; i<brickLenght; i++)
		if(isCollision(obj[0], brick[i])) {
			obj[0].x -= obj[0].horizSpeed;
			obj[0].horizSpeed = -obj[0].horizSpeed;
			return;
		}
	
	if(obj[0].drawSymbol == SYMBOL_ENEMY) {
		tObject tmp = *obj;
		vertMoveObject(&tmp);
		if(tmp.isFly) {
			obj[0].x -= obj[0].horizSpeed;
			obj[0].horizSpeed = -obj[0].horizSpeed;
		}
	}
}

void PutObjectOnMap(tObject obj) {
	int ix = (int)round(obj.x);
	int iy = (int)round(obj.y);
	
	int iWidth = (int)round(obj.width);
	int iHeight = (int)round(obj.height);
	
	for(int i=ix; i<(ix+iWidth); i++)
		for(int j=iy; j<(iy+iHeight); j++)
			if (isMapCellExists(j, i)) {
				map[j][i] = obj.drawSymbol;
				mapColors[j][i] = obj.color;
			}
}

void horizonMoveMap(float dx) {
	mario.x -= dx;
	for(int i=0; i<brickLenght; i++)
		if(isCollision(mario, brick[i])) {
			mario.x += dx;
			return;
		}
	mario.x += dx;
	
	for(int i=0; i<brickLenght; i++)
		brick[i].x += dx;
	
	for(int i=0; i<movingLenght; i++)
		moving[i].x += dx;
}

// не хотел, но пишу

void createLevel() {	
	initObject(&mario, 39, 10, 3, 3, SYMBOL_PLAYER, COLOR_PLAYER);
	brickLenght = 0;
	movingLenght = 0;
	scorePrev = score;
	
	if(score >= scoreSecretLevel) {
		showMessage("Is this the end?!", GAME_DELAY_WIN, COLOR_CYAN);
		
		initObject(getNewBrick(), 15, 20, 45, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewMoving(), 20, 10, 8, 5, SYMBOL_ENEMY, COLOR_PLAYER);
		
		score = 0;
		scorePrev = 0;
	}
	else if (level == 1) {		
		initObject(getNewBrick(), 20, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 30, 10, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 50, 10, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		
		initObject(getNewBrick(), 60, 15, 40, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 60, 5, 10, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 70, 5, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 75, 5, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 80, 5, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 85, 5, 10, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		
		initObject(getNewBrick(), 100, 20, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 120, 15, 10, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 150, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 210, 15, 10, 2, SYMBOL_NEXTLEVEL, COLOR_NEXTLEVEL);
		initObject(getNewBrick(), 210, 17, 10, 8, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewMoving(), 25, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 80, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
	}
	else if (level == 2) {		
		initObject(getNewBrick(), 20, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 60, 15, 10, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 80, 20, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 120, 15, 10, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 150, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 210, 15, 10, 2, SYMBOL_NEXTLEVEL, COLOR_NEXTLEVEL);
		initObject(getNewBrick(), 210, 17, 10, 8, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 163, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 168, 10, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), 173, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		
		initObject(getNewMoving(), 25, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 80, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 150, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 178, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 163, 5, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 167, 5, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
	}
	else if (level == 3) {
		initObject(getNewBrick(), 20, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 80, 20, 15, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 120, 15, 15, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 164, 10, 8, 2, SYMBOL_NEXTLEVEL, COLOR_NEXTLEVEL);
		initObject(getNewBrick(), 160, 12, 15, 13, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewMoving(), 25, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 50, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 80, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 90, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		//initObject(getNewMoving(), 120, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 130, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
	}
	else if (level == 4) {
		initObject(getNewBrick(), 10, 20, 40, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 85, 15, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 135, 15, 15, 10, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 170, 10, 20, 7, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 170, 0, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 210, 10, 20, 7, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 210, 0, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 250, 15, 20, 7, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 250, 5, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 290, 10, 20, 7, SYMBOL_BARRIER, COLOR_BARRIER);
		initObject(getNewBrick(), 290, 0, 20, 5, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewBrick(), 370, 10, 15, 2, SYMBOL_NEXTLEVEL, COLOR_NEXTLEVEL);
		initObject(getNewBrick(), 370, 12, 15, 13, SYMBOL_BARRIER, COLOR_BARRIER);
		
		initObject(getNewMoving(), 85, 10, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		initObject(getNewMoving(), 170, 5, 3, 2, SYMBOL_ENEMY, COLOR_ENEMY);
		
		initObject(getNewBrick(), -230, 17, 12, 10, SYMBOL_BARRIER, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -220, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewMoving(), -230, 10, 3, 3, SYMBOL_ENEMY, COLOR_MONEY);
		
		initObject(getNewBrick(), -175, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -180, 10, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -185, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		
		initObject(getNewBrick(), -180, 22, 25, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -120, 22, 25, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -60, 22, 25, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -55, 10, 5, 3, SYMBOL_EMPTYBLOCK, COLOR_MONEYBLOCK);
		initObject(getNewBrick(), -60, 10, 5, 3, SYMBOL_MONEYBLOCK, COLOR_MONEYBLOCK);
	}
	else {
		showMessage("You win!", GAME_DELAY_WIN, COLOR_CYAN);
		level = 1;
		createLevel();
	}
}

// добавляю цвета
void clearMapColor() {
	for(int i=0; i<MAP_WIDTH; i++)
		for(int j=0; j<MAP_HEIGHT; j++)
			mapColors[j][i] = COLOR_STANDARD;
}
// добавляю цвета

void putText(const char *message, int ystart, int xstart, int color) {
	for(int i=0; message[i]!='\0'; i++)
		if (isMapCellExists(ystart, xstart+i)) {
			map[ystart][xstart+i] = message[i];
			mapColors[ystart][xstart+i] = color;
		}
}

void showMessage(const char* text, int gameDelayTime, int backgroundColor) {
	if(backgroundColor > 0 && backgroundColor < 8)
		setBackgroundColor(backgroundColor);
	
	clearMapColor();
	clearMap();
	
	putText(("%s", text), (int)(MAP_HEIGHT/2-1), (int)(MAP_WIDTH/2-strlen(text)/2)
	,COLOR_MENUTEXT);
	showMap();
	refresh();
	
	napms(gameDelayTime);
	
	if(backgroundColor > 0 && backgroundColor < 8)
		setBackgroundColor(COLOR_BACKGROUND);
	
	clearMapColor();
	clearMap();
}

void GameControl(tObject *obj) {
	RefreshKeyboardStatus();
	
	if(menu == GAMEMODE_PLAY) {
		float horizonSpeed = 1;
		if(GetKeyState(KEY_A))
			horizonMoveMap(horizonSpeed);
		if(GetKeyState(KEY_D))
			horizonMoveMap(-horizonSpeed);
		if(GetKeyState(KEY_SPACE))
			if ((*obj).isFly == 0)
				(*obj).vertSpeed = -1;
		
		if(GetKeyState(KEY_Q))
			menu = GAMEMODE_MENU_PAUSE;
	}
	else {
		if(GetKeyState(KEY_ESC))
			CloseGame();
		
		if(GetKeyState(KEY_E))
			menu = GAMEMODE_PLAY;
	}
}

void showMenu() {
	char formattedMessage[100];
	if(menu == GAMEMODE_MENU_START) {
		for(int i=0; i<7; i++) {
			putText(logo[i], 2+i, MAP_WIDTH/2-strlen(logo[i])/2+1, COLOR_MENUTEXT);
		}
		
		sprintf(formattedMessage, "Console platformer");
		putText(formattedMessage, MAP_HEIGHT/2, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
		
		sprintf(formattedMessage, "Press [E] to play");
		putText(formattedMessage, MAP_HEIGHT/2+2, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
		
		sprintf(formattedMessage, "Press [Q] to pause");
		putText(formattedMessage, MAP_HEIGHT/2+3, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
		
		sprintf(formattedMessage, "Press [Esc] to exit");
		putText(formattedMessage, MAP_HEIGHT/2+4, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
	}
	else if(menu == GAMEMODE_MENU_PAUSE) {
		sprintf(formattedMessage, "Paused");
		putText(formattedMessage, MAP_HEIGHT/3, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
		
		sprintf(formattedMessage, "Press [E] to return to the game");
		putText(formattedMessage, MAP_HEIGHT/3+2, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
		
		sprintf(formattedMessage, "Press [Esc] to exit.");
		putText(formattedMessage, MAP_HEIGHT/3+3, MAP_WIDTH/2-strlen(formattedMessage)/2+1, COLOR_MENUTEXT);
	}
}

void doObjectsActions() {
	if(mario.y > MAP_HEIGHT)
		playerDead();
	
	vertMoveObject(&mario);
	marioCollision();
	PutObjectOnMap(mario);
	
	for(int i=0; i<brickLenght; i++)
		PutObjectOnMap(brick[i]);
	
	for(int i=0; i<movingLenght; i++) {
		vertMoveObject(moving + i);
		horizonMoveObject(moving + i);
		if(moving[i].y > MAP_HEIGHT) {
			deleteMoving(i);
			i--;
			continue;
		}
		PutObjectOnMap(moving[i]);
	}
}

void showGameplayInformation() {
	char formattedMessage[100];
	
	sprintf(formattedMessage, "level: %d", level);
	putText(formattedMessage, 1, 1, COLOR_MENUTEXT);
	
	sprintf(formattedMessage, "score: %d", score);
	putText(formattedMessage, 2, 1, COLOR_MENUTEXT);
	
	putText(("%s", nickname), (int)round(mario.y-2.0), (int)(mario.x-strlen(nickname)/2+1), COLOR_MENUTEXT);
}

int main() {
	srand((unsigned) time(NULL));
	setConsoleSize(MAP_WIDTH, MAP_HEIGHT);
	
	initConsoleMode();
	initLogo();
	
	clearMap();
	createLevel();
	
	while(1) {
		clearMapColor();
		clearMap();
		
		if(backgroundEffectTime > 0) {
			backgroundEffectTime--;
			if (backgroundEffectTime == 0)
				setBackgroundColor(COLOR_BACKGROUND);
		}
		
		if(menu == GAMEMODE_PLAY) {
			doObjectsActions();
			showGameplayInformation();
		}
		else
			showMenu();
		
		CheckSignals();
		GameControl(&mario);
		napms(GAME_DELAY_CONSTANT);
		GameControl(&mario);
		
		showMap();
	}
	
	CloseGame();
	return 0;
}
