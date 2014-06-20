/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bricktick.h"

int Lives = 3;
unsigned long Score = 0;

int main(int argc, char **argv)
{
	struct BALL Ball = { 0 };
	struct PADDLE Paddle = { 0 };
	int Key = 0;
	int SecTick = 0;
	Bool PaddleMovedLastTick;
	DirectionX PaddleMoveDir;
	
	initscr();
	
	if (COLS < 80 || LINES < 24)
	{
		endwin();
		fprintf(stderr, "Please use a console with a resolution of at least 80x24.\n");
		exit(1);
		
	}
	
	if (!has_colors())
	{
		endwin();
		fprintf(stderr, "Bricktick requires color.\n");
		exit(1);
	}
	
	noecho();
	halfdelay(1);
	keypad(stdscr, true);
	set_escdelay(25);
	curs_set(0);
	
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	
	
	/*Show our initial lives count.*/
	DrawLives(Lives);
	DrawScore(Score);
	attrset(COLOR_PAIR(1));

	refresh();

	/*Fix our color so we needn't create a window.*/
	
	ResetBall(&Ball);
	ResetPaddle(&Paddle);
	
	DrawBall(&Ball);
	DrawPaddle(&Paddle);
	
MainLoop:
	while ((Key = getch()) != 27) /*27 is ESC*/
	{
		if (SecTick == 10)
		{ /*We get score every second for just surviving.*/
			DrawScore((Score += 2));
			attrset(COLOR_PAIR(1));
			SecTick = 0;
		}
		++SecTick;

		if (Ball.Y == 1)
		{ /*We hit the ceiling.*/
			BounceBallY(&Ball, DOWN);
		}
		else if (Ball.Y == LINES - 2)
		{ /*More happens when we hit the floor.*/
			if (!CheckBallHitPaddle(&Ball, &Paddle))
			{ 
				MoveBall(&Ball);
				
				if (Lives == 1)
				{
					DrawMessage("GAME OVER -- Press ESC to exit.");
					cbreak();
					while (getch() != 27); /*ESC*/
					endwin();
					exit(0);
				}
				else
				{
					DrawLives(--Lives);
					
					 /*Fix wonky colors after DrawLives() that I'm certain are arising simply from misuse of ncurses.*/
					attrset(COLOR_PAIR(1));
					
					DrawMessage("Ready?");
					refresh();
					fflush(NULL);
					sleep(2);
					DeleteBall(&Ball);
					DeleteMessage();
					ResetBall(&Ball);
					DrawBall(&Ball);
				}
			}
			else
			{
				BounceBallY(&Ball, UP);
				
				if (PaddleMovedLastTick)
				{
					Ball.DirX = PaddleMoveDir;
				}
			}
		}
		PaddleMovedLastTick = false;
		/*Bounce off left and right walls.*/
		if (Ball.X == COLS - 1) BounceBallX(&Ball, LEFT);
		else if (Ball.X == 0) BounceBallX(&Ball, RIGHT);
		
		switch (Key)
		{ /*Paddle movement.*/
			case KEY_LEFT:
				MovePaddle(&Paddle, LEFT);
				PaddleMovedLastTick = true;
				PaddleMoveDir = LEFT;
				break;
			case KEY_RIGHT:
				MovePaddle(&Paddle, RIGHT);
				PaddleMovedLastTick = true;
				PaddleMoveDir = RIGHT;
				break;
			case ' ':
				DrawMessage("PAUSED");
				cbreak();
				while (getch() != ' ');
				DeleteMessage();
				halfdelay(1);
				break;
			default:
				break;
		}
		
		MoveBall(&Ball);
	}
	
	DrawMessage("Really quit Bricktick? y/n");
	cbreak();
	switch (getch())
	{
		case 'y':
		case 'Y':
			break;
		default:
			DeleteMessage();
			refresh();
			halfdelay(1);
			goto MainLoop;
			break;
	}
	DeleteBall(&Ball);
	DeletePaddle(&Paddle);
	
	endwin();
	
	return 0;
}

void DrawMessage(const char *const Message)
{
	move(LINES / 2, (COLS - strlen(Message)) / 2);
	addstr(Message);
	refresh();
}

void DeleteMessage(void)
{
	int Inc = 0;
	move(LINES / 2, 0);
	
	for (; Inc < COLS - 1; ++Inc)
	{
		addch(' ');
	}
	refresh();
}

void DrawLives(int Lives)
{
	move(0, 2);
	attron(COLOR_PAIR(3));
	printw("Lives: %d", Lives);
	attroff(COLOR_PAIR(3));
	refresh();
}

void DrawScore(unsigned long Score)
{
	char ScoreMSG[128];
	snprintf(ScoreMSG, sizeof ScoreMSG, "Score: %lu", Score);
	
	move(0, COLS - 1 - strlen(ScoreMSG) - 2);
	attron(COLOR_PAIR(3));
	addstr(ScoreMSG);
	attroff(COLOR_PAIR(3));
	refresh();
}

