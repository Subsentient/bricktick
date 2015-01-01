/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#define true 1
#define false 0

#define DEFAULT_PADDLE_LENGTH (BRICKTICK_MAX_X / 10) /*How long the paddle is.*/
#define DEFAULT_PADDLE_MOVE (DEFAULT_PADDLE_LENGTH / 2)
#define BRICKTICK_MAX_X 80
#define BRICKTICK_MAX_Y 24
#define BRICK_DEFAULT_NUMLINES 5
#define BRICK_DEFAULT_PERLINE 10
#define BRICK_MAX_NUMLINES 12
#define BRICK_MAX_PERLINE 20
#define BRICK_DEFAULT_HEIGHT 3
#define BRICK_COLORS_START 10
#define BRICK_COLORS_END 15
#define BRICKTICK_VERSION "0.7"

typedef signed char Bool;

typedef enum { LEFT, RIGHT, X_NEUTRAL } DirectionX;
typedef enum { DOWN, UP, Y_NEUTRAL } DirectionY;

struct BALL
{
	int X, Y;
	DirectionX DirX;
	DirectionY DirY;
};

struct PADDLE
{
	int X;
	int Length;
};

struct BRICK
{
	int X1, X2, Y;
	unsigned int Visible : 1;
	unsigned int Color;
};

struct BRICKSTRIKE
{
	enum StrikeV { STRIKE_VNONE, STRIKE_BOTTOM, STRIKE_TOP } StrikeV;
	enum StrikeH { STRIKE_HNONE, STRIKE_LEFT, STRIKE_RIGHT } StrikeH;
	struct BRICK *Brick;
};

struct CHARM
{ /*Things that drop that we catch with the paddle when we break a brick, etc.*/
	enum CharmType { CHARM_NONE, CHARM_MIN = 1, CHARM_SCORE = 1, CHARM_LIFE, CHARM_SLOW, CHARM_MAX } Type;
	int BrickX1, BrickX2, BrickY; /*Brick locations.*/
	Bool Dropped;
	int X, Y;
};

/*Functions.*/
void ResetBall(struct BALL *Ball);
void DrawBall(struct BALL *Ball);
void DeleteBall(struct BALL *Ball);
void MoveBall(struct BALL *Ball);
void BounceBallY(struct BALL *const Ball, DirectionY Direction);
void BounceBallX(struct BALL *const Ball, DirectionX Direction);

void DrawPaddle(struct PADDLE *Paddle);
void ResetPaddle(struct PADDLE *Paddle);
void DeletePaddle(struct PADDLE *Paddle);
void MovePaddle(struct PADDLE *Paddle, DirectionX Direction);
Bool CheckBallHitPaddle(struct BALL *Ball, struct PADDLE *Paddle);

void DrawBrick(struct BRICK *Brick);
Bool BallStruckBrick(const struct BALL *const Ball, struct BRICKSTRIKE *const Strike);
void ResetBricks(void);
void DrawAllBricks(void);
void DeleteAllBricks(void);
void DeleteBrick(struct BRICK *Brick);
int BricksLeft(void);
Bool BrickOnLocation(int X, int Y);

Bool AddCharm(struct BRICK *const Brick);
void DrawCharm(struct CHARM *const Charm);
Bool DeleteCharm(struct CHARM *const Charm);
void MoveCharm(struct CHARM *const Charm);
Bool CheckCharmHitPaddle(struct PADDLE *const Paddle, struct CHARM *const Charm);
struct CHARM *GetCharmByBrick(const struct BRICK *const Brick);
Bool PerformCharmDrop(struct CHARM *const Charm);
Bool ProcessCharmAction(struct CHARM *const Charm, void *OutStream);
void DeleteAllCharms(void);

/*Globals*/
extern Bool UseColor;
extern struct BRICK Bricks[BRICK_MAX_NUMLINES][BRICK_MAX_PERLINE];
extern int BrickNumLines;
extern int BricksPerLine;
extern int HeightFromPaddle;
extern struct CHARM Charms[BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE]; /*Capable of accomodating all bricks if necessary.*/
