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
#define BRICK_MAX_NUMLINES 8
#define BRICK_MAX_PERLINE 20
#define BRICK_DEFAULT_HEIGHT 3

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
	unsigned int CF : 1; /*Zero, we are green, one, we are blue.*/
};

struct BRICKSTRIKE
{
	enum StrikeV { STRIKE_VNONE, STRIKE_BOTTOM, STRIKE_TOP } StrikeV;
	enum StrikeH { STRIKE_HNONE, STRIKE_LEFT, STRIKE_RIGHT } StrikeH;
	struct BRICK *Brick;
};

struct LEVEL
{
	int BrickNumLines;
	int BricksPerLine;
	int HeightFromPaddle;
};

/*Functions.*/
extern void ResetBall(struct BALL *Ball);
extern void DrawBall(struct BALL *Ball);
extern void DeleteBall(struct BALL *Ball);
extern void MoveBall(struct BALL *Ball);
extern void BounceBallY(struct BALL *const Ball, DirectionY Direction);
extern void BounceBallX(struct BALL *const Ball, DirectionX Direction);

extern void DrawPaddle(struct PADDLE *Paddle);
extern void ResetPaddle(struct PADDLE *Paddle);
extern void DeletePaddle(struct PADDLE *Paddle);
extern void MovePaddle(struct PADDLE *Paddle, DirectionX Direction);
extern Bool CheckBallHitPaddle(struct BALL *Ball, struct PADDLE *Paddle);

extern void DrawMessage(const char *const Message);
extern void DeleteMessage(void);
extern void DrawLives(int Lives);
extern void DrawScore(unsigned long Score);
extern void WaitForUserLaunch(void);

extern void DrawBrick(struct BRICK *Brick);
extern Bool BallStruckBrick(const struct BALL *const Ball, struct BRICKSTRIKE *const Strike);
extern void ResetBricks(void);
extern void DrawAllBricks(void);
extern void DeleteAllBricks(void);
extern void DeleteBrick(struct BRICK *Brick);
extern int BricksLeft(void);

/*Globals*/
extern int Lives;
extern unsigned long Score;
extern Bool UseColor;
extern struct BRICK Bricks[BRICK_MAX_NUMLINES][BRICK_MAX_PERLINE];
extern int BrickNumLines;
extern int BricksPerLine;
extern int HeightFromPaddle;

