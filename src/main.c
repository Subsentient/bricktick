/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "bricktick.h"

int main(int argc, char **argv)
{
	struct BALL Ball = { 0 };
	struct PADDLE Paddle = { 0 };
	int Key = 0;
	time_t LastRand = 0, Rand = 0;
	
	initscr();
	noecho();
	halfdelay(1);
	keypad(stdscr, true);
	curs_set(0);
	
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
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

		if (Ball.Y == 0)
		{ /*We hit the ceiling.*/
			BounceBallY(&Ball, DOWN);
		}
		else if (Ball.Y == LINES - 2)
		{
			if (!CheckBallHitPaddle(&Ball, &Paddle))
			{
				MoveBall(&Ball);
				move(LINES / 2, (COLS - sizeof "GAME OVER -- Press END to exit" - 1) / 2);
				addstr("GAME OVER -- Press END to exit");
				cbreak();
				while (getch() != KEY_END);
				endwin();
				exit(0);
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
