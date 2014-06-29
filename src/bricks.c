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
	
	if (!Brick->Show) return;
	
	move(Brick->Y, Brick->X1);
	
	if (UseColor) attron(Color);
	
	addch('[');
	for (; Inc < Brick->X2 - Brick->X1 - 2; ++Inc)
	{
		addch('=');
	}
	addch(']');
	
	if (UseColor) attroff(Color);
	refresh();
}

void InitBricks(void)
{ /*Set up parameters for every brick.*/
	int Width = COLS / 10;
	const int StartY = 4;
	int CWidth = 0;
	int Inc1 = 0, Inc2 = 0;
	Bool Flip = false;
	struct BRICK *B2 = NULL;
	
	/*Don't let us run off the screen.*/
	for (; Width * 10 > COLS - 1; --Width);
	
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
			B2->Show = true;
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
			DrawBrick(&Bricks[Inc1][Inc2]);
		}
	}
}
