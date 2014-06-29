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
	Paddle->X = (BRICKTICK_MAX_X - Paddle->Length) / 2;
}

void DrawPaddle(struct PADDLE *Paddle)
{
	int Inc = 0;
	
	move(BRICKTICK_MAX_Y - 1, Paddle->X);

	addch('<' | A_BOLD);
	for (; Inc < Paddle->Length - 2; ++Inc)
	{
		addch('#' | A_BOLD);
	}
	addch('>' | A_BOLD);
	refresh();
}

void DeletePaddle(struct PADDLE *Paddle)
{
	int Inc = 0;
	move(BRICKTICK_MAX_Y - 1, Paddle->X);
	
	for (; Inc < Paddle->Length; ++Inc)
	{
		addch(' ');
	}
	refresh();
}

void MovePaddle(struct PADDLE *Paddle, DirectionX Direction)
{
	DeletePaddle(Paddle);

	if (Direction == LEFT)
	{
		if (DEFAULT_PADDLE_MOVE <= Paddle->X) Paddle->X -= DEFAULT_PADDLE_MOVE;
		else Paddle->X = 0;
	}
	else
	{
		if (BRICKTICK_MAX_X - Paddle->X - Paddle->Length >= DEFAULT_PADDLE_MOVE) Paddle->X += DEFAULT_PADDLE_MOVE;
		else Paddle->X = BRICKTICK_MAX_X  - Paddle->Length;
	}
	
	DrawPaddle(Paddle);
}
