/* Bricktick brick breaker
 * This software is public domain.
 * See the file UNLICENSE.TXT for more information.
 * Created in 2014 by Subsentient
 */

#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include "bricktick.h"

#define BRICKTICK_NUMLEVELS 7
#define BRICKTICK_NUMLIVES 3

#ifdef WIN32
#define BT_MKDIR(a, b) mkdir(a)
#define HOME_ENV "USERPROFILE"
#else
#define BT_MKDIR(a, b) mkdir(a, b)
#define HOME_ENV "HOME"
#endif /*WIN32*/

/*Types*/
struct LEVEL
{
	int BrickNumLines;
	int BricksPerLine;
	int HeightFromPaddle;
};

/*Globals*/
static int Lives = BRICKTICK_NUMLIVES;
static int Level = 1;
static int Score = 0;
Bool UseColor = true;

const struct LEVEL Levels[BRICKTICK_NUMLEVELS] =
					{
						{ BRICK_DEFAULT_NUMLINES - 2, BRICK_DEFAULT_PERLINE, BRICK_DEFAULT_HEIGHT },
						{ BRICK_DEFAULT_NUMLINES, BRICK_DEFAULT_PERLINE, BRICK_DEFAULT_HEIGHT },
						{ BRICK_DEFAULT_NUMLINES + 3, BRICK_DEFAULT_PERLINE, BRICK_DEFAULT_HEIGHT },
						{ BRICK_DEFAULT_NUMLINES + 4, BRICK_DEFAULT_PERLINE, BRICK_DEFAULT_HEIGHT + 2 },
						{ BRICK_MAX_NUMLINES, BRICK_DEFAULT_PERLINE, BRICK_DEFAULT_HEIGHT + 3 },
						{ BRICK_DEFAULT_NUMLINES + 5, BRICK_MAX_PERLINE, BRICK_DEFAULT_HEIGHT + 4 },
						{ BRICK_MAX_NUMLINES, BRICK_MAX_PERLINE, BRICK_DEFAULT_HEIGHT + 5 },
					};
static const unsigned SaveGameVersion = 0x001;
					
/*Prototypes for static functions.*/
static void DrawMessage(const char *const Message);
static void DeleteMessage(void);
static void WaitForUserLaunch(void);
static Bool SetLevel(const int Level_);
static void ProcessGameOver(struct BALL *Ball, struct PADDLE *Paddle);
static void GameLoop(struct BALL *const Ball, struct PADDLE *const Paddle);
static void DrawStats(void);
static void DrawGreeting(void);
static Bool LoadGame(struct BALL *OutBall, struct PADDLE *OutPaddle);
static Bool SaveGame(const struct BALL *Ball, const struct PADDLE *Paddle);
static void DrawBorders(void);

/*Functions*/

static void GameLoop(struct BALL *const Ball, struct PADDLE *const Paddle)
{ /*Primary loop where most events get processed.*/
	
	struct BRICKSTRIKE Strike;
	int Key = 0;
	int SecTick = 0;
	Bool PaddleMovedLastTick;
	DirectionX PaddleMoveDir;
	int Inc = 0;
	Bool Flip = false;
	int SlowBallTicks = 0, BallNukeTicks = 0;
	
	while ((Key = getch()) != 27) /*27 is ESC*/
	{		
		if (SecTick == 10)
		{ /*We get score every second for just surviving.*/
			Score += 2;
			DrawStats();
			SecTick = 0;
		}
		++SecTick;

		if (Ball->Y == 1)
		{ /*We hit the ceiling.*/
			BounceBallY(Ball, DOWN);
		}
		else if (Ball->Y >= BRICKTICK_MAX_Y - 2)
		{ /*More happens when we hit the floor.*/
			if (!CheckBallHitPaddle(Ball, Paddle))
			{
				DeleteBall(Ball);
				Ball->Y = BRICKTICK_MAX_Y - 1;
				DrawBall(Ball);
				
				if (Lives == 1)
				{ /*We ran out of lives.*/
					ProcessGameOver(Ball, Paddle);
				}
				else
				{
					--Lives;
					DrawStats();
					
					WaitForUserLaunch();
							
					DeleteBall(Ball);
					DeletePaddle(Paddle);
					
					ResetBall(Ball);
					ResetPaddle(Paddle);
					
					DrawPaddle(Paddle);
					DrawBall(Ball);
					
					/*Redraw but don't reset.*/
					DrawAllBricks();
				}
			}
			else
			{
				BounceBallY(Ball, UP);
				
				if (PaddleMovedLastTick)
				{ /*We can "whack" the ball with our Paddle->*/
					Ball->DirX = PaddleMoveDir;
				}
				else
				{ /*We cut the paddle into thirds for the X direction after bounce.*/
#define PADDLE_THIRD (Paddle->Length / 3)
					if (Ball->X <= Paddle->X + PADDLE_THIRD)
					{
						Ball->DirX = LEFT;
					}
					else if (Ball->X  > Paddle->X + PADDLE_THIRD && Ball->X <= Paddle->X + (PADDLE_THIRD * 2))
					{
						/*Make whether we hit up or not as a chance.*/
						Bool StraightUp = rand() & 1;
						if (StraightUp) Ball->DirX = X_NEUTRAL;
					}
					else
					{
						Ball->DirX = RIGHT;
					}
				}
			}
		}
		PaddleMovedLastTick = false;
		/*Bounce off left and right walls.*/
		if (Ball->X >= BRICKTICK_MAX_X - 1)
		{
			Ball->X = BRICKTICK_MAX_X - 1;
			BounceBallX(Ball, LEFT);
		}
		else if (Ball->X <= 0)
		{
			Ball->X = 0;
			BounceBallX(Ball, RIGHT);
		}
			
	
		/*Check if a charm hit the paddle.*/
		for (Inc = 0; Inc < BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE; ++Inc)
		{
			if (Charms[Inc].Type == CHARM_NONE || !Charms[Inc].Dropped || Charms[Inc].Y != BRICKTICK_MAX_Y - 2) continue;
			
			if (CheckCharmHitPaddle(Paddle, Charms + Inc))
			{
				void *Ptr = NULL;
				const char *const Strings[] = { "+1,000 Score", "+1 Lives",
											"10 Second Slow Ball", "3 second nuke mode" };
					
				switch (Charms[Inc].Type)
				{
					case CHARM_SCORE:
						Ptr = &Score;
						break;
					case CHARM_LIFE:
						Ptr = &Lives;
						break;
					case CHARM_SLOW:
						Ptr = &SlowBallTicks;
						break;
					case CHARM_NUKE:
						Ptr = &BallNukeTicks;
						break;
					default:
						break;
				}
				
				if (Charms[Inc].Type != CHARM_NONE)
				{ /*Show a message on what type of charm we have here.*/
					DrawMessage(Strings[Charms[Inc].Type - 1]); fflush(NULL);
					usleep(500000);
					DeleteMessage();
					DrawAllBricks();
					DrawStats();
				}
				
				/*Do the thing the charm does.*/
				ProcessCharmAction(Charms + Inc, Ptr);
			}
			
			/*In any case, we're done with it.*/
			DeleteCharm(Charms + Inc);
			Charms[Inc].Type = CHARM_NONE;
		}
		
		/*We hit a brick.*/
		if (BallStruckBrick(Ball, &Strike))
		{
			
			if (BallNukeTicks == 0) /*Nuclear ball passes through.*/
			{
				switch (Strike.StrikeV)
				{
					case STRIKE_TOP:
						Ball->DirY = UP;
						break;
					case STRIKE_BOTTOM:
						Ball->DirY = DOWN;
						break;
					default:
						break;
				}
				
				switch (Strike.StrikeH)
				{
					case STRIKE_LEFT:
						Ball->DirX = LEFT;
						break;
					case STRIKE_RIGHT:
						Ball->DirX = RIGHT;
						break;
					default:
					{
						if (Ball->DirX != X_NEUTRAL) break;
						else
						{
							Bool Dir = rand() & 1;
							Ball->DirX = (DirectionX)Dir;
						}
						break;
					}
				}
			}
			
			DeleteBrick(Strike.Brick);
			Score += 100;
			DrawStats();
			
			if (!BricksLeft())
			{ /*Move to next level.*/

				if (SetLevel(Level + 1))
				{ /*We have more levels to go before we win.*/
					DeleteAllBricks();
					DeleteAllCharms();
					DeleteBall(Ball);
					DeletePaddle(Paddle);
					DrawStats();
					
					Score += 1000; /*Reward for making it this far.*/
					Lives = BRICKTICK_NUMLIVES;
					
					ResetBall(Ball);
					ResetPaddle(Paddle);
					ResetBricks();
					
					DrawAllBricks();
					DrawPaddle(Paddle);
					WaitForUserLaunch();
					DrawBall(Ball);
					DrawAllBricks(); /*Redraw to fix WaitForUserLaunch() goofing.*/
				}
				else
				{ /*WE WON!!!!*/
					char WonBuf[256];
					
					snprintf(WonBuf, sizeof WonBuf, "You Won! Score is %d! Hit ESC to exit or space to play again.", Score);
					DrawMessage(WonBuf);
					
				WinRegetch:
					switch (getch())
					{
						case 27: /*27 is ESC*/
							endwin();
							exit(0);
							break;
						case ' ':
						{
							DeleteMessage();
							
							SetLevel(1);
							
							Lives = BRICKTICK_NUMLIVES;
							Score = 0;
							
							DeleteAllBricks();
							DeleteAllCharms();
							DeleteBall(Ball);
							DeletePaddle(Paddle);
							DrawStats();
							
							ResetBall(Ball);
							ResetPaddle(Paddle);
							ResetBricks();
							
							DrawAllBricks();
							DrawPaddle(Paddle);
							WaitForUserLaunch();
							DrawBall(Ball);
							DrawAllBricks(); /*Redraw to fix WaitForUserLaunch() goofing.*/

							continue; /*For the loop we are in.*/
						}
						default:
							goto WinRegetch;
					}
				}

				continue;
			}
			else
			{ /*Charm drops.*/
				struct CHARM *Charm = GetCharmByBrick(Strike.Brick);
				
				if (Charm)
				{ /*We DO have a charm for this brick.*/
					PerformCharmDrop(Charm); /*Mark it dropped.*/

					/*Now draw the charm.*/
					DrawCharm(Charm);
				}
			}
		}
		
		switch (Key)
		{ /*Paddle movement.*/
			case KEY_LEFT:
				MovePaddle(Paddle, LEFT);
				PaddleMovedLastTick = true;
				PaddleMoveDir = LEFT;
				break;
			case KEY_RIGHT:
				MovePaddle(Paddle, RIGHT);
				PaddleMovedLastTick = true;
				PaddleMoveDir = RIGHT;
				break;
			case 's': /*They want to save the game.*/
				if (SaveGame(Ball, Paddle))
				{
					DrawMessage("Game saved.");
				}
				else
				{
					DrawMessage("Failed to save game.");
				}
				fflush(NULL);
				usleep(500000);
				
				DeleteMessage();
				DrawAllBricks(); /*Redraw bricks if damaged.*/
				
				break;
			case 'o': /*They want us to load a game.*/
			{
				const Bool LoadedOk = LoadGame(Ball, Paddle);
				
				if (LoadedOk)
				{
					/*Restore the state.*/
					clear();
					DrawBorders(); /*Need to redraw these after clearing the screen.*/
					
					DrawBall(Ball);
					DrawPaddle(Paddle);
					DrawStats();
					DrawAllBricks();
					
					DrawMessage("Game loaded.");
				}
				else
				{
					DrawMessage("Failed to load game.");
				}

				fflush(NULL);
				usleep(500000);
				
				DeleteMessage();
				DrawAllBricks();
				break;
			}
			case ' ':
			{
				DrawMessage("PAUSED");
				cbreak();
				
			PauseRegetch:
				switch (getch())
				{
					case ' ':
						DeleteMessage();
						DrawAllBricks(); /*Redraw to fix what deleting the message messed up.*/
						halfdelay(1);
						break;
					case 27: /*27 is ESC*/
						endwin();
						exit(0);
						break;
					default:
						goto PauseRegetch;
				}
			}
			default:
				break;
		}
		
		Flip = !Flip;

		/*Ball movement, obviously. Flip is used to keep it at half speed if we got a 'slow' charm.*/
		if (Flip || SlowBallTicks == 0) MoveBall(Ball);
		
		/*Decrement slow ball ticks until zero, then the ball goes fast again.*/
		if (SlowBallTicks > 0) --SlowBallTicks;
		
		/*Decrement nuclear ball until ticks hit zero.*/
		if (BallNukeTicks > 0) --BallNukeTicks;
		
		/*Charm movement.*/		
		for (Inc = 0; Inc < BRICK_MAX_NUMLINES * BRICK_MAX_PERLINE; ++Inc)
		{
			if (Charms[Inc].Type == CHARM_NONE || !Charms[Inc].Dropped) continue;
			
			if (Flip) MoveCharm(Charms + Inc);
		}
		
		/*Redraw borders in case of terminal resize.*/
		DrawBorders();
	}
}

static void ProcessGameOver(struct BALL *Ball, struct PADDLE *Paddle)
{
	char OutBuf[255];
	
	snprintf(OutBuf, sizeof OutBuf, "GAME OVER! Score is %d. Hit ESC to exit or space to play again.", Score);

	Lives = 0;
	
	DrawStats();
	
	Lives = 3;
	Score = 0;
	
	DrawMessage(OutBuf);
	
	cbreak();
	
AskAgain:
	switch (getch())
	{
		case 27: /*27 is ESC.*/
			endwin();
			exit(0);
			break;
		case ' ':
			break;
		default:
			goto AskAgain;
	}
	
	halfdelay(1);
	DeleteMessage();
	
	DeleteBall(Ball);
	DeletePaddle(Paddle);
	DeleteAllBricks();
	DeleteAllCharms();
	SetLevel(1); /*Set back to the default level again.*/
	
	ResetPaddle(Paddle);
	ResetBricks();
	
	DrawPaddle(Paddle);
	DrawAllBricks();
	
	DrawStats();
	
	/*Assume they want to play again.*/
	WaitForUserLaunch();
	ResetBall(Ball);
	DrawBall(Ball);
	DrawAllBricks(); /*Redraw to fix WaitForUserLaunch() goofing.*/

}
						
static Bool SetLevel(const int Level_)
{
	
	if (Level_ > BRICKTICK_NUMLEVELS) return false;
	
	Level = Level_;
	
	HeightFromPaddle = Levels[Level - 1].HeightFromPaddle;
	BrickNumLines = Levels[Level - 1].BrickNumLines;
	BricksPerLine = Levels[Level - 1].BricksPerLine;
	
	return true;
}

int main(int argc, char **argv)
{
	int Inc = 1;
	struct BALL Ball = { 0 };
	struct PADDLE Paddle = { 0 };
	Bool DoLoadGame = false;
	int GotoLevel = 0;
	
	for (; Inc < argc; ++Inc)
	{ /*Argument parsing.*/
		if (!strcmp("--nocolor", argv[Inc]))
		{
			UseColor = false;
		}
		else if (!strcmp("--version", argv[Inc]))
		{
			printf("Bricktick brick breaker v" BRICKTICK_VERSION "\n"
					"By Subsentient.\n");
			fflush(NULL);
			exit(0);
		}
		else if (!strcmp("--help", argv[Inc]))
		{
			printf("Command line options:\n\t"
					"--help: Show this help.\n\t"
					"--nocolor: Run Bricktick with no color.\n\t"
					"--version: Display version and exit.\n");
			fflush(NULL);
			exit(0);
		}
		else if (!strncmp("--level=", argv[Inc], sizeof "--level=" - 1))
		{
			GotoLevel = atoi(argv[Inc] + sizeof "--level=" - 1);
			
			if (GotoLevel > sizeof Levels / sizeof *Levels)
			{
				fprintf(stderr, "The maximum level is %u.\n", (unsigned)(sizeof Levels / sizeof *Levels)); fflush(NULL);
				exit(1);
			}
		}
		else
		{
			fprintf(stderr, "Bad command line argument \"%s\".\n"
					"Use --help for command line options.\n", argv[Inc]);
			exit(1);
		}
	}
	
	
	initscr(); /*Fire up ncurses.*/
	
	if (COLS < BRICKTICK_MAX_X || LINES < BRICKTICK_MAX_Y)
	{
		endwin();
		fprintf(stderr, "Please use a console with a resolution of at least %dx%d.\n", BRICKTICK_MAX_X, BRICKTICK_MAX_Y);
		exit(1);
		
	}
	
	/*Color not supported.*/
	if (!has_colors()) UseColor = false;
	
	/*Various ncurses things.*/
	noecho();
	
	if (UseColor)
	{ /*Color fireup.*/
		start_color();
		init_pair(1, COLOR_CYAN, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_BLACK, COLOR_WHITE);
		init_pair(4, COLOR_BLUE, COLOR_BLACK);
		init_pair(5, COLOR_GREEN, COLOR_BLACK);
		init_pair(6, COLOR_YELLOW, COLOR_BLACK);
		init_pair(7, COLOR_RED, COLOR_BLACK);
		
		
		/*Fire up colors for the bricks.*/
		init_pair(BRICK_COLORS_START, COLOR_GREEN, COLOR_BLACK);
		init_pair(BRICK_COLORS_START + 1, COLOR_BLUE, COLOR_BLACK);
		init_pair(BRICK_COLORS_START + 2, COLOR_YELLOW, COLOR_BLACK);
		init_pair(BRICK_COLORS_START + 3, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(BRICK_COLORS_START + 4, COLOR_CYAN, COLOR_BLACK);
		init_pair(BRICK_COLORS_END, COLOR_RED, COLOR_BLACK);
	}
	
	/*Draw borders for consoles bigger than our 80x24 playing area.*/
	DrawBorders();
	
	/*Greeting.*/
GreetAsk:

	if (!GotoLevel)
	{ /*If they specified a level let them go straigth to game start.*/
		DrawGreeting();
		switch (getch())
		{
			case 27: /*27 is ESC key*/
				endwin();
				exit(0);
				break;
			case ' ':
				clear(); /*Wipe the message from the screen.*/
				DrawBorders(); /*Redraw borders.*/
				break;
			case 'o':
				clear();
				DrawBorders();
				DoLoadGame = true;
				LoadGame(&Ball, &Paddle);
				break;
			default:
				goto GreetAsk;
		}
	}
	else
	{
		clear();
		DrawBorders();
	}
	
	halfdelay(1);
	keypad(stdscr, true);
#if NCURSES_VERSION_MAJOR >= 5 && NCURSES_VERSION_MINOR >=4
	set_escdelay(25);
#endif
	curs_set(0);
	

	if (DoLoadGame)
	{
		DrawStats();
		DrawBall(&Ball);
		DrawPaddle(&Paddle);
		DrawAllBricks();
	}
	else
	{
		/*Show our initial lives count.*/
		DrawStats();
	
		/*Reset to level 1 if we haven't specified a level.*/
		SetLevel(GotoLevel ? GotoLevel : 1);
		
		ResetBall(&Ball);
		ResetPaddle(&Paddle);
		
		DrawBall(&Ball);
		DrawPaddle(&Paddle);
		ResetBricks();
		DrawAllBricks();
	}
	
	/*Wait for L key.*/
	WaitForUserLaunch();
	DrawAllBricks(); /*Redraw to fix goofing by WaitForUserLaunch().*/
	
	/*Set up the number generator.*/
	srand(time(NULL));

	/**
	 * Main loop for the game!
	 **/
Reloop:
	GameLoop(&Ball, &Paddle);
	
	DrawMessage("Really quit Bricktick? y/n");
	cbreak();
	
	/*When the game loop exits.*/
	switch (getch())
	{
		case 'y':
		case 'Y':
			break;
		default:
			DeleteMessage();
			refresh();
			halfdelay(1);
			DrawAllBricks(); /*Fix damage to brick display resulting from the message.*/
			goto Reloop;
			break;
	}
	DeleteBall(&Ball);
	DeletePaddle(&Paddle);
	
	endwin();
	
	return 0;
}

static void DrawMessage(const char *const Message)
{
	move(BRICKTICK_MAX_Y / 2, (BRICKTICK_MAX_X - strlen(Message)) / 2);
	addstr(Message);
	refresh();
}

static void DrawGreeting(void)
{
	int Inc = 0;
	const struct
	{
		const char *Msg;
		int Color;
	} Greeting[] = {
				{ "Welcome to Bricktick v" BRICKTICK_VERSION "!", COLOR_PAIR(1) },
				{ "Charms:", COLOR_PAIR(1) },
				{ "% is +1,000 score.", COLOR_PAIR(5) },
				{ "@ is +1 lives,", COLOR_PAIR(6) },
				{ "# is 10 second slow ball.", COLOR_PAIR(7) },
				{ "^ is 3 second nuke mode.", COLOR_PAIR(4) },
				{ "Ingame, press 's' to save a game and 'o' to load it.", COLOR_PAIR(1) },
				{ "Press space to start a game, 'o' to load one, or ESC to exit.", COLOR_PAIR(1) },
				{ NULL }
					};
					
	for (; Greeting[Inc].Msg != NULL; ++Inc)
	{
		move(BRICKTICK_MAX_Y / 2 + Inc - (sizeof Greeting / sizeof *Greeting / 2),
			(BRICKTICK_MAX_X - strlen(Greeting[Inc].Msg)) / 2);
		
		if (UseColor) attron(Greeting[Inc].Color);
		addch(*Greeting[Inc].Msg); /*We do the first character so only the first charm is colored.*/
		if (UseColor) attroff(Greeting[Inc].Color), attrset(COLOR_PAIR(1));
		
		addstr(Greeting[Inc].Msg + 1);
	}
	refresh();
}

static void DeleteMessage(void)
{
	int Inc = 0;
	move(BRICKTICK_MAX_Y / 2, 0);
	
	for (; Inc < BRICKTICK_MAX_X - 1; ++Inc)
	{
		addch(' ');
	}
	refresh();
}

static void DrawStats(void)
{
	int Inc = 0;
	const char *const Formats[] = { "Lives: %d", "Score: %d", "Level: %d" };
	const int *const Ptrs[sizeof Formats / sizeof *Formats] = { &Lives, &Score, &Level };

	/*Move to our starting position.*/
	move(0, 2);
	
	/*Clear any old.*/
	for (; Inc < BRICKTICK_MAX_X - 1; ++Inc)
	{
		addch(' ');
	}
	
	/*Print the new.*/
	move(0, 2);
	
	for (Inc = 0; Inc < sizeof Formats / sizeof *Formats; ++Inc)
	{
		if (UseColor) attron(COLOR_PAIR(3));
		
		printw(Formats[Inc], *Ptrs[Inc]);
		
		if (UseColor)
		{
			/*For us.*/
			attroff(COLOR_PAIR(3));
			/*For everyone else.*/
			attrset(COLOR_PAIR(1));
		}
		
		/*Padding.*/
		if (Inc + 1 != sizeof Formats / sizeof *Formats) addch('\t'), addch('\t'), addch('\t');
	}
	refresh();
}

static void WaitForUserLaunch(void)
{
	int Key;
	
	DrawMessage("Hit L to launch.");
	
	while ((Key = getch()) != 'l' && Key != 'L' && Key != 27);
	
	DeleteMessage();
	
	if (Key == 27) /*ESC*/
	{
		endwin();
		exit(0);
	}

}

static Bool SaveGame(const struct BALL *Ball, const struct PADDLE *Paddle)
{ /*Writes the current game, score, etc to our config dir.*/
	FILE *Desc = NULL;
	char SaveFile[2048];
	struct stat FileStat;
	char ConfigDir[1024];
	
	snprintf(ConfigDir, sizeof SaveFile, "%s/.bricktick", getenv(HOME_ENV));
	snprintf(SaveFile, sizeof SaveFile, "%s/savegame.bin", ConfigDir);
	
	/*Create config directory if it does not exist.*/
	if (stat(ConfigDir, &FileStat) != 0) BT_MKDIR(ConfigDir, 0755);
	
	if (!(Desc = fopen(SaveFile, "wb")))
	{
		return false;
	}
	
	/*write savegame version.*/
	fwrite(&SaveGameVersion, 1, sizeof(int), Desc);
	
	/*write bricks.*/
	fwrite(&Bricks, 1, sizeof Bricks, Desc);
	
	/*write charms.*/
	fwrite(&Charms, 1, sizeof Charms, Desc);
	
	/*write paddle.*/
	fwrite(Paddle, 1, sizeof(struct PADDLE), Desc);
	
	/*write ball.*/
	fwrite(Ball, 1, sizeof(struct BALL), Desc);
	
	/*write some extra brick data.*/
	fwrite(&BrickNumLines, 1, sizeof(int), Desc);
	fwrite(&BricksPerLine, 1, sizeof(int), Desc);
	fwrite(&HeightFromPaddle, 1, sizeof(int), Desc);
	
	/*write score, lives, and level.*/
	fwrite(&Score, 1, sizeof(int), Desc);
	fwrite(&Lives, 1, sizeof(int), Desc);
	fwrite(&Level, 1, sizeof(int), Desc);
	
	/*We're done.*/
	fclose(Desc);
	
	return true;
}


static Bool LoadGame(struct BALL *OutBall, struct PADDLE *OutPaddle)
{
	FILE *Desc = NULL;
	char SaveFile[1024];
	unsigned Ver;
	
	snprintf(SaveFile, sizeof SaveFile, "%s/.bricktick/savegame.bin", getenv(HOME_ENV));

	if (!(Desc = fopen(SaveFile, "rb")))
	{
		return false;
	}
	
	/**Check savegame version!**/
	fread(&Ver, 1, sizeof Ver, Desc);
	
	if (Ver != SaveGameVersion)
	{
		fclose(Desc);
		return false;
	}
	
	/*restore bricks.*/
	fread(&Bricks, 1, sizeof Bricks, Desc);
	
	/*restore charms.*/
	fread(&Charms, 1, sizeof Charms, Desc);
	
	/*restore paddle.*/
	fread(OutPaddle, 1, sizeof(struct PADDLE), Desc);
	
	/*restore ball.*/
	fread(OutBall, 1, sizeof(struct BALL), Desc);
	
	/*restore some extra brick data.*/
	fread(&BrickNumLines, 1, sizeof(int), Desc);
	fread(&BricksPerLine, 1, sizeof(int), Desc);
	fread(&HeightFromPaddle, 1, sizeof(int), Desc);
	
	/*restore score, lives, and level.*/
	fread(&Score, 1, sizeof(int), Desc);
	fread(&Lives, 1, sizeof(int), Desc);
	fread(&Level, 1, sizeof(int), Desc);
	
	/*We're done.*/
	fclose(Desc);
	
	return true;
}


static void DrawBorders(void)
{ /*If we're greater than 80x24, draw borders for us so it doesn't look like the ball will fly out into nowhere.*/
	unsigned Inc = 0;
	
	if (LINES > BRICKTICK_MAX_Y)
	{
		move(BRICKTICK_MAX_Y, 0);
		
		for (; Inc < BRICKTICK_MAX_X + 1; ++Inc)
		{
			if (UseColor) addch('-' | COLOR_PAIR(1));
			else addch('-');
		}
	}
	
	if (COLS > BRICKTICK_MAX_X)
	{
		for (Inc = 0; Inc < BRICKTICK_MAX_Y + 1; ++Inc)
		{
			move(Inc, BRICKTICK_MAX_X);
			if (UseColor) addch('|' | COLOR_PAIR(1));
			else addch('|');
		}
	}
	
	refresh();
}

