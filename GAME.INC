{$ifndef	__GAME_2048__}
{$define	__GAME_2048__}

{ ////////////////////////////////////////////////////////////////////////// }

{ exposed API }
procedure       gameInit;		forward;
procedure	gameDone;		forward;
function        gameDemo: boolean;	forward;
procedure       gamePlay;		forward;

{ private API }
procedure	gameLogo;		forward;
procedure       gameUpdate;		forward;

{ ////////////////////////////////////////////////////////////////////////// }

const
	INFO_X  	= 63;
	INFO_SCORE	= 14;
	INFO_HISCORE	= 17;
	INFO_MOVES	= 20;

var
	board	: BOARD_TYPE;

procedure       gameInit;
begin
	textmode(CO80);
	randomize;

	cursor(CURSOR_OFF);
	blinkoff;

	scoreLoad;
	stateInit(board);
end;

procedure	gameDone;
begin
	textmode(CO80);
	writeln(' ______ ______ _____  ______ ');
	writeln('|__    |      |  |  ||  __  |');
	writeln('|    __|  --  |__    |  __  |');
	writeln('|______|______|  |__||______|');
	write  ('               v');
	write  (hi(GAME_VERSION),'.',lo(GAME_VERSION),'.',GAME_BUILD);
	writeln(' [', COMPILER, ']');
	writeln('          Thanks for playing!');
	writeln;
end;

procedure	gameLogo;
var     x       : longint;
begin
	for x := 0 to 3999 do MEM[VIDEO_SEG:x] := ord(IMAGEDATA[x+1]);
	prints(72, 25, $01, 'v' + ltoa(hi(GAME_VERSION)) + '.' +
				 ltoa(lo(GAME_VERSION)) + '.' +
				 GAME_BUILD);
end;

procedure       gameUpdate;
begin
	prints(INFO_X, INFO_SCORE,   $07, lpad(ltoa(GAME_SCORE), 12));
	prints(INFO_X, INFO_HISCORE, $07, lpad(ltoa(GAME_HISCORE), 12));
	prints(INFO_X, INFO_MOVES,   $07, lpad(ltoa(GAME_MOVES), 12));
	boardShow(board);
end;

function        gameDemo: boolean;
var     x, y, c : longint;
	tmp	: BOARD_TYPE;
	stop	: boolean;
	ch	: char;
begin
	gameLogo;
	gameUpdate;
	fillchar(tmp, sizeof(BOARD_TYPE), 0);
	c := 0;
	stop := FALSE;
	center(24, '~09ENTER~01:START  ~08-  ~09ESC~01:QUIT');
	repeat
		x := c mod BOARD_DIM; y := c div BOARD_DIM;
		tmp[x, y] := (c mod 11)+1;
		boardShow(tmp);
		sleep(3);
		tmp[x, y] := 0;
		c := (c + 1) mod BOARD_SIZE;
		if keypressed then
		begin
			ch := readkey;
			if ch in [#13, #27, #32] then stop := TRUE;
		end;
	until stop;
	gameDemo := ch in [#13, #32];
end;

procedure       gamePlay;
var	c       : char;
	quit,
	next	: boolean;
begin
	quit := FALSE; next := FALSE;
	gameLogo;
	if stateLoad(board) then
	begin
		gameUpdate;
	end
	else	stateRestart(board);
	repeat
		gameUpdate;
		c := wait(['W','A','S','D','R',#27]);
		gameUpdate;
		case c of
		#27:	begin
				if query('~0CQUIT AND SAVE GAME (~0FY~0C/~0FN~0C) ?') then
					quit := TRUE;
				next := FALSE;
			end;
		'R':	if query('~03START A NEW GAME (~0FY~03/~0FN~03) ?') then
			begin
				stateRestart(board);
				scoreSave;
				next := FALSE;
			end;
		'A':	next := boardSlideLeft (board);
		'D':	next := boardSlideRight(board);
		'W':	next := boardSlideUp   (board);
		'S':	next := boardSlideDown (board);
		end;
		if next then
		begin
			inc(GAME_MOVES);
			boardPlace(board);
			boardShow(board);
			if stateCheckOver(board) then
			begin
				info('~06!! ~0EGAME OVER~06 !!');
				stateRestart(board);
				next := FALSE;
			end;
		end;
	until quit;
	scoreSave;
	stateSave(board);
end;

{$endif}
