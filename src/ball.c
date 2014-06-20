/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <ncurses.h>
#include <stdlib.h>
#include "bricktick.h"

void BounceBallX(struct BALL *const Ball, DirectionX Direction)
{
	Ball->DirX = Direction;
}

void BounceBallY(struct BALL *const Ball, DirectionY Direction)
{
	/*Don't let us keep bouncing in a loop forever up and down.*/
	if (Direction == DOWN && Ball->DirX == X_NEUTRAL)
	{
		Ball->DirX = rand() & 1;
	}
	
	Ball->DirY = Direction;
}
	
void ResetBall(struct BALL *Ball)
{
	Ball->X = COLS / 2;
	Ball->Y = LINES / 2;
	Ball->DirX = X_NEUTRAL;
	Ball->DirY = DOWN;
}

Bool CheckBallHitPaddle(struct BALL *Ball, struct PADDLE *Paddle)
{
	if (Ball->X >= Paddle->X && Ball->X <= (Paddle->X + Paddle->Length)) return true;
	return false;
}


void DrawBall(struct BALL *Ball)
{
	move(Ball->Y, Ball->X);
	attron(A_BOLD);
	addch('*' | COLOR_PAIR(2));
	attroff(A_BOLD);
	refresh();
}

void DeleteBall(struct BALL *Ball)
{
	move(Ball->Y, Ball->X);
	attron(A_NORMAL);
	addch(' ');
	attroff(A_NORMAL);
	refresh();
}

void MoveBall(struct BALL *Ball)
{
	DeleteBall(Ball);
	
	/*X coord movement.*/
	switch (Ball->DirX)
	{
		case RIGHT:
			if (Ball->X + BALL_X_SPEEDMULTIPLIER <= COLS - 1)
			{
				Ball->X += BALL_X_SPEEDMULTIPLIER;
			}
			else
			{
				Ball->X = COLS - 1;
			}
			break;
		case LEFT:
			if (Ball->X - BALL_X_SPEEDMULTIPLIER >= 0)
			{
				Ball->X -= BALL_X_SPEEDMULTIPLIER;
			}
			else
			{
				Ball->X = 0;
			}
			break;
		default:
			break;
	}
		
	/*Y coord movement.*/
	switch (Ball->DirY)
	{
		case DOWN:
			if (Ball->Y + BALL_Y_SPEEDMULTIPLIER < LINES - 1)
			{
				Ball->Y += BALL_Y_SPEEDMULTIPLIER;
			}
			else
			{
				Ball->Y = LINES - 2;
			}	
			break;
		case UP:
			if (Ball->Y - BALL_Y_SPEEDMULTIPLIER > 0)
			{
				Ball->Y -= BALL_Y_SPEEDMULTIPLIER;
			}
			else
			{
				Ball->Y = 1;
			}
			break;
		default:
			break;
	}
	
	DrawBall(Ball);
	
}
	
