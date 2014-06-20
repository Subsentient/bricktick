/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#define true 1
#define false 0

#define DEFAULT_PADDLE_LENGTH COLS / 10
#define DEFAULT_PADDLE_MOVE (DEFAULT_PADDLE_LENGTH) / 2
#define BRICKS_PER_LINE 10 /*Ten bricks per line of text*/
#define BRICK_LINE_COUNT 4  /*Four lines of bricks.*/
#define BALL_X_SPEEDMULTIPLIER (COLS / 80)
#define BALL_Y_SPEEDMULTIPLIER (LINES / 24)

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
extern Bool MovePaddle(struct PADDLE *Paddle, DirectionX Direction);
extern Bool CheckBallHitPaddle(struct BALL *Ball, struct PADDLE *Paddle);

extern void DrawMessage(const char *const Message);
extern void DeleteMessage(void);
extern void DrawLives(int Lives);
extern void DrawScore(unsigned long Score);

/*Globals*/
extern int Lives;
extern unsigned long Score;
extern Bool UseColor;
