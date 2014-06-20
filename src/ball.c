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
	Ball->DirY = Direction;
}
	
void ResetBall(struct BALL *Ball)
{
	Ball->X = COLS / 2;
	Ball->Y = LINES / 2;
	Ball->DirX = LEFT;
	Ball->DirY = UP;
	
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
			++Ball->X;
			break;
		case LEFT:
			--Ball->X;
			break;
		default:
			break;
	}
		
	/*Y coord movement.*/
	switch (Ball->DirY)
	{
		case DOWN:
			++Ball->Y;
			break;
		case UP:
			--Ball->Y;
			break;
		default:
			break;
	}
	
	DrawBall(Ball);
	
}
	
