/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <ncurses.h>
#include "bricktick.h"

void ResetPaddle(struct PADDLE *Paddle)
{
	Paddle->Length = DEFAULT_PADDLE_LENGTH;
	Paddle->X = (COLS - Paddle->Length) / 2;
}

void DrawPaddle(struct PADDLE *Paddle)
{
	int Inc = 0;
	
	move(LINES - 1, Paddle->X);

	for (; Inc < Paddle->Length; ++Inc)
	{
		addch('#' | A_BOLD);
	}
	refresh();
}

void DeletePaddle(struct PADDLE *Paddle)
{
	int Inc = 0;
	move(LINES - 1, Paddle->X);
	
	for (; Inc < Paddle->Length; ++Inc)
	{
		addch(' ');
	}
	refresh();
}

Bool MovePaddle(struct PADDLE *Paddle, DirectionX Direction)
{
	Bool RetVal = true;
	
	DeletePaddle(Paddle);

	if (Direction == LEFT)
	{
		if (DEFAULT_PADDLE_MOVE <= Paddle->X) Paddle->X -= DEFAULT_PADDLE_MOVE;
		else RetVal = false;
	}
	else
	{
		if (COLS - Paddle->X - Paddle->Length >= DEFAULT_PADDLE_MOVE) Paddle->X += DEFAULT_PADDLE_MOVE;
		else RetVal = false;
	}
	
	DrawPaddle(Paddle);
	
	return RetVal;
}
