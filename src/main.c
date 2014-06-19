/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "bricktick.h"

int main(int argc, char **argv)
{
	struct BALL Ball = { 0 };
	struct PADDLE Paddle = { 0 };
	int Key = 0;
	time_t LastRand = 0, Rand = 0;
	int Lives = 3;
	
	initscr();
	noecho();
	halfdelay(1);
	keypad(stdscr, true);
	curs_set(0);
	
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
	
	
	/*Show our initial lives count.*/
	DrawLives(Lives);
	attrset(COLOR_PAIR(1));

	refresh();

	/*Fix our color so we needn't create a window.*/
	
	ResetBall(&Ball);
	ResetPaddle(&Paddle);
	
	DrawBall(&Ball);
	DrawPaddle(&Paddle);
	
	while ((Key = getch()) != KEY_END)
	{
		if ((Rand = time(NULL)) != LastRand)
		{
			srand(Rand);
			LastRand = Rand;
		}

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
					DrawMessage("GAME OVER -- Press END to exit.");
					cbreak();
					while (getch() != KEY_END);
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
			}
		}
		
		/*Bounce off left and right walls.*/
		if (Ball.X == COLS - 1) BounceBallX(&Ball, LEFT);
		else if (Ball.X == 0) BounceBallX(&Ball, RIGHT);
		
		switch (Key)
		{ /*Paddle movement.*/
			case KEY_LEFT:
				MovePaddle(&Paddle, LEFT);
				break;
			case KEY_RIGHT:
				MovePaddle(&Paddle, RIGHT);
				break;
			default:
				break;
		}
		
		MoveBall(&Ball);
	}
	
	DeleteBall(&Ball);
	
	
	endwin();
	
	return 0;
}

void DrawMessage(const char *const Message)
{
	move(LINES / 2, (COLS - strlen(Message)) / 2);
	addstr(Message);
}

void DeleteMessage(void)
{
	int Inc = 0;
	move(LINES / 2, 0);
	
	for (; Inc < COLS - 1; ++Inc)
	{
		addch(' ');
	}
}

void DrawLives(int Lives)
{
	move(0, 2);
	attron(COLOR_PAIR(3));
	printw("Lives: %d", Lives);
	attroff(COLOR_PAIR(3));
	refresh();
}
