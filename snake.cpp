#include <iostream>
#include <cstdlib>
#include <Windows.h>
#include <winuser.h>
#include <list>
#include <thread>
#include <mmsystem.h>
using namespace std;
#define sndPlaySoundW
int nScreenWidth = 120;
int nScreenHeight = 30;
int messageWidth = 30;
int messageHeight = 10;
struct SnakeSegment {
	int x;
	int y;
};


int main() {
	//screen buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwbyteswritten = 0;
	list<SnakeSegment> snake = { {60, 15}, {61,15} };
	//variables
	int FoodX = 30;
	int FoodY = 15;
	int Score = 0;
	int SnakeDirection = 3;
	bool Dead = false;
	bool keyleft = false, keyright = false, keyleftOld = false, keyrightOld = false;
	bool keyup = false, keydown = false, keyupOld = false, keydownOld = false;
	bool ESCkey = false;
	while (!Dead) {

		//this_thread::sleep_for(200ms);

		//input
		auto t1 = chrono::system_clock::now();
		while ((chrono::system_clock::now() - t1) < ((SnakeDirection % 2 == 1) ? 120ms : 200ms))
		{
			ESCkey = (0x8000 & GetAsyncKeyState(VK_ESCAPE));
			keyright = (0x8000 & GetAsyncKeyState(VK_RIGHT));
			keyleft = (0x8000 & GetAsyncKeyState(VK_LEFT));
			keydown = (0x8000 & GetAsyncKeyState(VK_DOWN));
			keyup = (0x8000 & GetAsyncKeyState(VK_UP));

			if (ESCkey) {
				int PAUSEID = MessageBox(NULL, &screen[nScreenWidth], L"Pause", MB_OK);
				if (MB_OK) {
					continue;
				}
			}

			if (keyright && !keyrightOld && SnakeDirection != 3) {
				SnakeDirection = 1;
			}
			if (keyleft && !keyleftOld && SnakeDirection != 1) {
				SnakeDirection = 3;
			}
			if (keydown && !keydownOld && SnakeDirection != 0) {
				SnakeDirection = 2;
			}
			if (keyup && !keyupOld && SnakeDirection != 2) {
				SnakeDirection = 0;
			};
			keyrightOld = keyright;
			keyleftOld = keyleft;
		}
		switch (SnakeDirection) {
		case 0://up
			snake.push_front({ snake.front().x, snake.front().y - 1 });
			break;
		case 1://right
			snake.push_front({ snake.front().x + 1, snake.front().y });
			break;
		case 2://down
			snake.push_front({ snake.front().x, snake.front().y + 1 });
			break;
		case 3://left
			snake.push_front({ snake.front().x - 1, snake.front().y });
			break;
		};

		//Collision
		if (snake.front().y < 3 || snake.front().y >= nScreenHeight - 1) { Dead = true; }
		if (snake.front().x <= 1 || snake.front().x >= nScreenWidth) { Dead = true; }

		if (snake.front().x == FoodX && snake.front().y == FoodY) {
			Score++;
			while (screen[FoodY * nScreenWidth + FoodX] != L' ') {
				FoodX = rand() % nScreenWidth;
				FoodY = (rand() % (nScreenHeight - 3)) + 6;
			};
			for (int i = 0; i < 1; i++) {
				snake.push_back({ snake.back().x, snake.back().y });
			};
		}

		for (list<SnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
			if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
				Dead = true;


		snake.pop_back();
		//clear screen
		for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

		//stats & borders
		for (int i = 0; i < nScreenWidth; i++) {
			screen[i] = L'=';
			screen[2 * nScreenWidth + i] = L'=';
			screen[29 * nScreenWidth + i] = L'=';
		};
		for (int i = 0; i < nScreenWidth; i++) {
			for (int i = 3; i < nScreenHeight - 1; i++) {
				screen[i * nScreenWidth] = L'||';
				screen[i * nScreenWidth + 119] = L'||';
			}
		};

		wsprintf(&screen[nScreenWidth + 4], L"SCORE: %d", Score);
		//draw SNAKE
		//draw BODY
		for (auto s : snake)
			screen[s.y * nScreenWidth + s.x] = Dead ? L'+' : L'O';
		//draw HEAD
		screen[snake.front().y * nScreenWidth + snake.front().x] = Dead ? L'X' : L'@';

		//draw FOOD
		screen[FoodY * nScreenWidth + FoodX] = L'&';

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwbyteswritten);
	};
	while (Dead) {
		//message lose
		int msgboxID = MessageBox(NULL, &screen[nScreenWidth], L"You lost", MB_RETRYCANCEL);
		switch (msgboxID) {
		case IDCANCEL:
			return 0;
		case IDRETRY:
			main();
		};
		return 0;
	};
};
