/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <stdlib.h>
#include <ncurses.h>
#include "bricktick.h"

struct BRICK *Bricks;
int BrickLength;


Bool AllocateBricks(void)
{
	if (Bricks) return false;
	
	/*Get the number of equals signs to use.*/
	BrickLength = (COLS - 2) / BRICKS_PER_LINE;
	
	Bricks = calloc((BRICKS_PER_LINE * BRICK_LINE_COUNT), sizeof(struct BRICK));
}

Bool DrawBrick(struct BRICK *Brick)
{
}
