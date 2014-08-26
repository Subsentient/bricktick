/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "bricktick.h"

struct BRICK Bricks[BRICK_MAX_NUMLINES][BRICK_MAX_PERLINE];
int BricksPerLine = BRICK_DEFAULT_PERLINE; /*These are changeable for levelling up, etc.*/
int BrickNumLines = BRICK_DEFAULT_NUMLINES;
int HeightFromPaddle = BRICK_DEFAULT_HEIGHT; /*How far down from the ceiling the first line of bricks gets drawn. 4 is further down than 3.*/

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
	int Width = BRICKTICK_MAX_X / BricksPerLine;
	const int StartY = HeightFromPaddle;
	int CWidth = 0;
	int Inc1 = 0, Inc2 = 0;
	int CharmCounter = 0;
	Bool Flip = false;
	struct BRICK *B2 = NULL;
	
	/*Wipe the charms and bricks.*/
	memset(Charms, 0, sizeof Charms);
	memset(Bricks, 0, sizeof Bricks);
	
	/*Don't let us run off the screen.*/
	for (; Width * BricksPerLine > BRICKTICK_MAX_X; --Width);
	
	for (; Inc1 < BrickNumLines; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BricksPerLine; ++Inc2)
		{
			CWidth = Width * Inc2;
			
			B2 = &Bricks[Inc1][Inc2];
			
			B2->CF = (Flip = !Flip);
			B2->X1 = CWidth;
			B2->X2 = B2->X1 + Width;
			B2->Y = StartY + Inc1;
			B2->Visible = true;
			
			/*Add charms every few bricks.*/
			if (CharmCounter == 3)
			{
				AddCharm(B2);
				CharmCounter = 0;
			}
			++CharmCounter;		
		}
		
		if (BricksPerLine % 2 == 0)
		{ /*Combat repetitiveness from even-numbered brick lines.*/
			Flip = !Flip; /*Alternate colors.*/
		
			/*Vary charm drops.*/
			if (CharmCounter != 3) ++CharmCounter;
		}
	}
}
	
void DrawAllBricks(void)
{
	int Inc1 = 0, Inc2 = 0;
	
	for (; Inc1 < BrickNumLines; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BricksPerLine; ++Inc2)
		{
			if (Bricks[Inc1][Inc2].Visible)
			{
				DrawBrick(&Bricks[Inc1][Inc2]);
			}
		}
	}
}

Bool BrickOnLocation(const int X, const int Y)
{ /*Check if we have a brick at specific coordinates*/
	int Inc = 0, Inc2 = 0;
	struct BRICK *Brick = NULL;
	
	for (; Inc < BRICK_MAX_NUMLINES; ++Inc)
	{
		for (Inc2 = 0; Inc2 < BRICK_MAX_PERLINE; ++Inc2)
		{
			Brick = Bricks[Inc] + Inc2;
			
			if (X >= Brick->X1 && Y <= Brick->X2 && Y == Brick->Y)
			{
				return true;
			}
			
		}
	}
	
	return false;
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
	
	for (; Inc1 < BrickNumLines; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BricksPerLine; ++Inc2)
		{
			DeleteBrick(&Bricks[Inc1][Inc2]);
		}
	}
}

int BricksLeft(void)
{
	int Inc1 = 0, Inc2 = 0;
	int BrickCount = 0;
	
	for (; Inc1 < BrickNumLines; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BricksPerLine; ++Inc2)
		{
			if (Bricks[Inc1][Inc2].Visible) ++BrickCount;
		}
	}
	
	return BrickCount;
}


Bool BallStruckBrick(const struct BALL *const Ball, struct BRICKSTRIKE *const Strike)
{
	int Inc1 = 0, Inc2 = 0;
	
	for (; Inc1 < BrickNumLines; ++Inc1)
	{
		for (Inc2 = 0; Inc2 < BricksPerLine; ++Inc2)
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


