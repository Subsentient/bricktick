/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <stdlib.h>
#include <ncurses.h>
#include "bricktick.h"

struct BRICK Bricks[BRICK_LINE_COUNT][BRICKS_PER_LINE];

void DrawBrick(struct BRICK *Brick)
{ /*Draw a single brick.*/
	int Inc = 0;
	int Color = UseColor ? COLOR_PAIR(Brick->CF ? 4 : 2) : 0;
	
	if (!Brick->Visible) return;
	
	move(Brick->Y, Brick->X1);
	
	if (UseColor) attron(Color);
	
	addch('[');
	for (; Inc < Brick->X2 - Brick->X1 - 2; ++Inc)
	{
		addch('=');
	}
	addch(']');
	
	if (UseColor)
	{
		attroff(Color);
		attrset(COLOR_PAIR(1)); /*Set it back for everything else so it doesn't muck up.*/
	}
	
	refresh();
}

void ResetBricks(void)
{ /*Set up parameters for every brick.*/
	int Width = BRICKTICK_MAX_X / BRICKS_PER_LINE;
	const int StartY = 4;
	int CWidth = 0;
	int Inc1 = 0, Inc2 = 0;
	Bool Flip = false;
	struct BRICK *B2 = NULL;
	
	/*Don't let us run off the screen.*/
	for (; Width * BRICKS_PER_LINE > BRICKTICK_MAX_X; --Width);
	
	for (; Inc1 < BRICK_LINE_COUNT; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BRICKS_PER_LINE; ++Inc2)
		{
			CWidth = Width * Inc2;
			
			B2 = &Bricks[Inc1][Inc2];
			
			B2->CF = (Flip = !Flip);
			B2->X1 = CWidth;
			B2->X2 = B2->X1 + Width;
			B2->Y = StartY + Inc1;
			B2->Visible = true;
		}
		Flip = !Flip; /*Alternate colors.*/
	}
}
	
void DrawAllBricks(void)
{
	int Inc1 = 0, Inc2 = 0;
	
	for (; Inc1 < BRICK_LINE_COUNT; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BRICKS_PER_LINE; ++Inc2)
		{
			if (Bricks[Inc1][Inc2].Visible)
			{
				DrawBrick(&Bricks[Inc1][Inc2]);
			}
		}
	}
}

void DeleteBrick(struct BRICK *Brick)
{
	const int Length = Brick->X2 - Brick->X1;
	int Inc = 0;
	
	move(Brick->Y, Brick->X1);
	
	attron(A_NORMAL);
	
	for (; Inc < Length; ++Inc)
	{
		addch(' ');
	}
	
	attroff(A_NORMAL);
	
	attrset(COLOR_PAIR(1));
	
	Brick->Visible = false;
	refresh();
}
	
void DeleteAllBricks(void)
{
	int Inc1 = 0, Inc2 = 0;
	
	for (; Inc1 < BRICK_LINE_COUNT; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BRICKS_PER_LINE; ++Inc2)
		{
			DeleteBrick(&Bricks[Inc1][Inc2]);
		}
	}
}

int BricksLeft(void)
{
	int Inc1 = 0, Inc2 = 0;
	int BrickCount = 0;
	
	for (; Inc1 < BRICK_LINE_COUNT; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BRICKS_PER_LINE; ++Inc2)
		{
			if (Bricks[Inc1][Inc2].Visible) ++BrickCount;
		}
	}
	
	return BrickCount;
}


Bool BallStruckBrick(const struct BALL *const Ball, struct BRICKSTRIKE *const Strike)
{
	int Inc1 = 0, Inc2 = 0;
	
	for (; Inc1 < BRICK_LINE_COUNT; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BRICKS_PER_LINE; ++Inc2)
		{
			if (Bricks[Inc1][Inc2].Visible &&
				Ball->X >= Bricks[Inc1][Inc2].X1 - 1 && Ball->X <= Bricks[Inc1][Inc2].X2 + 1 &&
				Ball->Y <= Bricks[Inc1][Inc2].Y + 1 && Ball->Y >= Bricks[Inc1][Inc2].Y - 1)
			{
				/*Process the different vertical directions first.*/
				if (Ball->Y == Bricks[Inc1][Inc2].Y - 1) /*Struck the top of the brick.*/
				{
					Strike->StrikeV = STRIKE_TOP;
				}
				else if (Ball->Y == Bricks[Inc1][Inc2].Y + 1) /*Struck the bottom of the brick.*/
				{
					Strike->StrikeV  = STRIKE_BOTTOM;
				}
				else
				{
					Strike->StrikeV = STRIKE_VNONE;
				}
				
				/*Process the different horizontal directions now.*/
				if (Ball->X == Bricks[Inc1][Inc2].X1 - 1) /*Struck the left side of the brick.*/
				{
					Strike->StrikeH = STRIKE_LEFT;
				}
				else if (Ball->X == Bricks[Inc1][Inc2].X2 + 1)
				{
					Strike->StrikeH = STRIKE_RIGHT;
				}
				else
				{
					Strike->StrikeH = STRIKE_HNONE;
				}
				
				Strike->Brick = &Bricks[Inc1][Inc2];
				
				return true;
			}
		}
	}
	
	return false;
}


