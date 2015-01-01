/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */


/**charms.c, Charms like lives, slowdown, etc.**/
 
#include <curses.h>
#include "bricktick.h"

/*Globals*/
struct CHARM Charms[BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE];

/*Functions.*/
Bool AddCharm(struct BRICK *const Brick)
{ /*Create a charm.*/
	static enum CharmType Type = CHARM_MIN;
	int Inc = 0;
	
	/*Charm type counter/selector thingy.*/
	if (Type == CHARM_MAX) Type = CHARM_MIN;
	
	for (; Inc < BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE && Charms[Inc].Type != CHARM_NONE; ++Inc);
	
	if (Inc == BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE) return false; /*No space left.*/
	
	Charms[Inc].BrickX1 = Brick->X1;
	Charms[Inc].BrickX2 = Brick->X2;
	Charms[Inc].BrickY = Brick->Y;
	Charms[Inc].Dropped = false;
	
	Charms[Inc].Type = Type; /*Our non-randomized charm type, e.g. score, life, slow, etc.*/
	
	Charms[Inc].X = Brick->X2 - (Brick->X2 - Brick->X1) / 2;
	Charms[Inc].Y = Brick->Y + 1;
	
	++Type; /*For next iteration.*/
	
	return true;
}
	

void MoveCharm(struct CHARM *const Charm)
{ /*Only move one out of two times. In other words, half speed.*/
	if (!Charm || !Charm->Dropped) return;
	
	DeleteCharm(Charm);
	
	if (Charm->Y == BRICKTICK_MAX_Y - 2) return;
	
	++Charm->Y;

	DrawCharm(Charm);
}
	
void DrawCharm(struct CHARM *const Charm)
{
	int Color = 0;
	char Character = 0;

	if (!Charm || !Charm->Dropped) return;
	
	/*Special exception on long drops with holes between bricks etc.*/
	while (BrickOnLocation(Charm->X, Charm->Y)) ++Charm->Y;
	
	switch (Charm->Type)
	{
		case CHARM_SCORE:
			Color = UseColor ? COLOR_PAIR(5) : 0;
			Character = '%';
			break;
		case CHARM_LIFE:
			Color = UseColor ? COLOR_PAIR(6) : 0;
			Character = '@';
			break;
		case CHARM_SLOW: 
			Color = UseColor ? COLOR_PAIR(7) : 0;
			Character = '#';
			break;
		default:
			break;
	}
	
	move(Charm->Y, Charm->X);
	attron(Color);
	addch(Character | A_BOLD);
	attroff(Color);
	attrset(COLOR_PAIR(1));
	refresh();
}

Bool PerformCharmDrop(struct CHARM *const Charm)
{
	if (!Charm || Charm->Dropped || Charm->Type == CHARM_NONE) return false;
	
	Charm->Dropped = true; /*Set brick to null to signal we dropped it.*/
	
	return true;
}

struct CHARM *GetCharmByBrick(const struct BRICK *const Brick)
{
	int Inc = 0;
	
	if (!Brick) return NULL;
	
	for (; Inc < BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE; ++Inc)
	{
		if (!Charms[Inc].Dropped && Charms[Inc].Type != CHARM_NONE &&
			Charms[Inc].BrickX1 == Brick->X1 && Charms[Inc].BrickX2 == Brick->X2 &&
			Charms[Inc].BrickY == Brick->Y)
		{
			return Charms + Inc;
		}
	}
	
	return NULL;
}

Bool CheckCharmHitPaddle(struct PADDLE *const Paddle, struct CHARM *const Charm)
{
	if (Charm->X >= Paddle->X && Charm->X <= (Paddle->X + Paddle->Length)) return true;
	
	return false;
}

Bool DeleteCharm(struct CHARM *const Charm)
{
	if (!Charm || !Charm->Dropped || Charm->Type == CHARM_NONE) return false;
	move(Charm->Y, Charm->X);
	addch(' ');
	refresh();
	return true;
}

void DeleteAllCharms(void)
{
	int Inc = 0;
	
	for (; Inc < BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE; ++Inc)
	{
		if (Charms[Inc].Type == CHARM_NONE || !Charms[Inc].Dropped) continue;
		DeleteCharm(Charms + Inc);
	}
}

Bool ProcessCharmAction(struct CHARM *const Charm, void *OutStream)
{ /*actually does something.*/
	
	if (!Charm) return false;
	
	switch (Charm->Type)
	{
		case CHARM_SCORE:
		{
			int *Score = OutStream;
			
			*Score += 1000;
			break;
		}
		case CHARM_LIFE:
		{
			int *Lives = OutStream;
			
			++*Lives;
			break;
		}
		case CHARM_SLOW:
		{
			int *SlowBallTicks = OutStream;
			
			*SlowBallTicks = 100; /*Ten seconds.*/
			break;
		}
		default:
			return false;
	}
	
	return true;
}
