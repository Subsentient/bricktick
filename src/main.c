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
#include <time.h>
#include "bricktick.h"

int Lives = 3;
unsigned long Score = 0;
Bool UseColor = true;

int main(int argc, char **argv)
{
	struct BALL Ball = { 0 };
	struct PADDLE Paddle = { 0 };
	int Key = 0;
	int SecTick = 0;
	Bool PaddleMovedLastTick;
	DirectionX PaddleMoveDir;
	struct BRICKSTRIKE Strike;
	
	int Inc = 1;
	
	for (; Inc < argc; ++Inc)
	{
		if (!strcmp("--nocolor", argv[Inc]))
		{
			UseColor = false;
		}
		else
		{
			fprintf(stderr, "Bad command line argument \"%s\".\n", argv[Inc]);
			exit(1);
		}
	}
	
	
	initscr();
	
	if (BRICKTICK_MAX_X < 80 || BRICKTICK_MAX_Y < 24)
	{
		endwin();
		fprintf(stderr, "Please use a console with a resolution of at least 80x24.\n");
		exit(1);
		
	}
	
	if (!has_colors() && UseColor)
	{
		endwin();
		fprintf(stderr, "Color is not supported. Pass --nocolor to play without it.\n");
		exit(1);
	}
	
	noecho();
	halfdelay(1);
	keypad(stdscr, true);
	set_escdelay(25);
	curs_set(0);
	
	if (UseColor)
	{
		start_color();
		init_pair(1, COLOR_CYAN, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_BLACK, COLOR_WHITE);
		init_pair(4, COLOR_BLUE, COLOR_BLACK);
	}
	
	
	/*Show our initial lives count.*/
	DrawLives(Lives);
	DrawScore(Score);

	refresh();

	/*Fix our color so we needn't create a window.*/
	
	ResetBall(&Ball);
	ResetPaddle(&Paddle);
	
	DrawBall(&Ball);
	DrawPaddle(&Paddle);
	ResetBricks();
	DrawAllBricks();
	/*Wait for L key.*/
	WaitForUserLaunch();
	
	/*Set up the number generator.*/
	srand(time(NULL));
MainLoop:
	while ((Key = getch()) != 27) /*27 is ESC*/
	{		
		if (SecTick == 10)
		{ /*We get score every second for just surviving.*/
			DrawScore((Score += 2));
			SecTick = 0;
		}
		++SecTick;

		if (Ball.Y == 1)
		{ /*We hit the ceiling.*/
			BounceBallY(&Ball, DOWN);
		}
		else if (Ball.Y >= BRICKTICK_MAX_Y - 2)
		{ /*More happens when we hit the floor.*/
			if (!CheckBallHitPaddle(&Ball, &Paddle))
			{
				DeleteBall(&Ball);
				Ball.Y = BRICKTICK_MAX_Y - 1;
				DrawBall(&Ball);
				
				if (Lives == 1)
				{
					DrawLives(0);
					
					Lives = 3;
					Score = 0;
					DrawMessage("GAME OVER -- Press ESC to exit or any key to play again");
					cbreak();
					if (getch() == 27)
					{
						endwin();
						exit(0);
					}
					halfdelay(1);
					DeleteMessage();
					
					DeleteBall(&Ball);
					DeletePaddle(&Paddle);
					DeleteAllBricks();
					
					ResetPaddle(&Paddle);
					ResetBricks();
					
					DrawPaddle(&Paddle);
					DrawAllBricks();
					
					DrawLives(Lives);
					
					/*Assume they want to play again.*/
					WaitForUserLaunch();
					ResetBall(&Ball);
					DrawBall(&Ball);
				}
				else
				{
					DrawLives(--Lives);
					
					WaitForUserLaunch();
							
					DeleteBall(&Ball);
					DeletePaddle(&Paddle);
					
					ResetBall(&Ball);
					ResetPaddle(&Paddle);
					
					DrawPaddle(&Paddle);
					DrawBall(&Ball);
					
					/*Redraw but don't reset.*/
					DrawAllBricks();
				}
			}
			else
			{
				BounceBallY(&Ball, UP);
				
				if (PaddleMovedLastTick)
				{ /*We can "whack" the ball with our paddle.*/
					Ball.DirX = PaddleMoveDir;
				}
				else
				{ /*We cut the paddle into thirds for the X direction after bounce.*/
#define PADDLE_THIRD (Paddle.Length / 3)
					if (Ball.X <= Paddle.X + PADDLE_THIRD)
					{
						Ball.DirX = LEFT;
					}
					else if (Ball.X  > Paddle.X + PADDLE_THIRD && Ball.X <= Paddle.X + (PADDLE_THIRD * 2))
					{
						/*Make whether we hit up or not as a chance.*/
						Bool StraightUp = rand() & 1;
						if (StraightUp) Ball.DirX = X_NEUTRAL;
					}
					else
					{
						Ball.DirX = RIGHT;
					}
				}
			}
		}
		PaddleMovedLastTick = false;
		/*Bounce off left and right walls.*/
		if (Ball.X >= BRICKTICK_MAX_X - 1)
		{
			Ball.X = BRICKTICK_MAX_X - 1;
			BounceBallX(&Ball, LEFT);
		}
		else if (Ball.X <= 0)
		{
			Ball.X = 0;
			BounceBallX(&Ball, RIGHT);
		}
			
	
		/*We hit a brick.*/
		if (BallStruckBrick(&Ball, &Strike))
		{
			switch (Strike.StrikeV)
			{
				case STRIKE_TOP:
					Ball.DirY = UP;
					break;
				case STRIKE_BOTTOM:
					Ball.DirY = DOWN;
					break;
				default:
					break;
			}
			
			switch (Strike.StrikeH)
			{
				case STRIKE_LEFT:
					Ball.DirX = LEFT;
					break;
				case STRIKE_RIGHT:
					Ball.DirX = RIGHT;
					break;
				default:
				{
					if (Ball.DirX != X_NEUTRAL) break;
					else
					{
						Bool Dir = rand() & 1;
						Ball.DirX = (DirectionX)Dir;
					}
					break;
				}
			}
			
			DeleteBrick(Strike.Brick);
			DrawScore((Score += 100));
		}
		
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
	move(BRICKTICK_MAX_Y / 2, (BRICKTICK_MAX_X - strlen(Message)) / 2);
	addstr(Message);
	refresh();
}

void DeleteMessage(void)
{
	int Inc = 0;
	move(BRICKTICK_MAX_Y / 2, 0);
	
	for (; Inc < BRICKTICK_MAX_X - 1; ++Inc)
	{
		addch(' ');
	}
	refresh();
}

void DrawLives(int Lives)
{
	move(0, 2);
	if (UseColor) attron(COLOR_PAIR(3));
	printw("Lives: %d", Lives);
	
	if (UseColor)
	{
		/*For us.*/
		attroff(COLOR_PAIR(3));
		/*For everyone else.*/
		attrset(COLOR_PAIR(1));
	}
	refresh();
}

void DrawScore(unsigned long Score)
{
	char ScoreMSG[128];
	snprintf(ScoreMSG, sizeof ScoreMSG, "Score: %lu", Score);
	
	move(0, BRICKTICK_MAX_X - 1 - strlen(ScoreMSG) - 2);
	if (UseColor) attron(COLOR_PAIR(3));
	addstr(ScoreMSG);

	if (UseColor)
	{
		/*For us.*/
		attroff(COLOR_PAIR(3));
		/*For everyone else.*/
		attrset(COLOR_PAIR(1));
	}
	refresh();
}

void WaitForUserLaunch(void)
{
	int Key;
	
	DrawMessage("Hit L to launch.");
	
	while ((Key = getch()) != 'l' && Key != 'L' && Key != 27);
	
	DeleteMessage();
	
	if (Key == 27) /*ESC*/
	{
		endwin();
		exit(0);
	}

}
