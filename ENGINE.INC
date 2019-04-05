{$ifndef	__ENGINE_2048__}
{$define	__ENGINE_2048__}

{ ////////////////////////////////////////////////////////////////////////// }

{ Board constants }
const   BOARD_DIM	= 4;
	BOARD_SIZE	= BOARD_DIM*BOARD_DIM;
	BOARD_W		= 6;
	BOARD_H		= BOARD_W shr 1;
	BOARD_X		= (SCR_WIDTH-BOARD_DIM * BOARD_W) shr 1;
	BOARD_Y 	= 11;
	BOARD_FILLER	= #177#177#177#177#177#177;

type    TILE_TYPE	= longint;
	VEC4_TYPE    	= array[0..BOARD_DIM-1] of TILE_TYPE;
	BOARD_TYPE	= array[0..BOARD_DIM-1] of VEC4_TYPE;

const	GAME_SCORE   	: longint = 0;
	GAME_HISCORE	: longint = 500;
	GAME_MOVES	: longint = 0;
	FILE_SCORE	: string[13] = 'HISCORE.DAT';
	FILE_SAVE	: string[13] = 'SAVEGAME.DAT';
	CELL_COLORS  	: array[0..16] of byte =
			($80, $70,
			 $10, $20, $30, $40, $50, $90, $A0,
			 $B0, $C0, $D0, $E0, $F0, $F0, $F0, $F0);

{ ////////////////////////////////////////////////////////////////////////// }

{ exposed game engine API }
procedure       boardShow(const board: BOARD_TYPE); 				forward;
function        boardFind(const v: VEC4_TYPE; x, stop: longint): longint;	forward;
function        boardSlide(var v: VEC4_TYPE): boolean;				forward;
procedure       boardRotate(var board: BOARD_TYPE);				forward;
function        boardSlideUp(var board: BOARD_TYPE): boolean;			forward;
function        boardSlideLeft(var board: BOARD_TYPE): boolean;			forward;
function        boardSlideRight(var board: BOARD_TYPE): boolean;		forward;
function        boardSlideDown(var board: BOARD_TYPE): boolean;			forward;
function        boardCheckPair(board: BOARD_TYPE): boolean;			forward;
function        boardCountBlank(board: BOARD_TYPE): longint;			forward;
function        boardPlace(var board: BOARD_TYPE): boolean;			forward;

procedure	stateInit(var board: BOARD_TYPE);				forward;
procedure	stateRestart(var board: BOARD_TYPE);				forward;
function        stateSave(var board: BOARD_TYPE): boolean;			forward;
function        stateLoad(var board: BOARD_TYPE): boolean;			forward;

function        scoreLoad: boolean;						forward;
function	scoreSave: boolean;						forward;

{ ////////////////////////////////////////////////////////////////////////// }

{ Displays the game board }
procedure       boardShow(const board: BOARD_TYPE);
var
	x, y, v,
	i, j, c	: longint;
begin
	for y := 0 to BOARD_DIM-1 do
	for x := 0 to BOARD_DIM-1 do
	begin
		i := BOARD_X+x*BOARD_W; j := BOARD_Y+y*BOARD_H;
		v := board[x, y];
		if v <> 0 then
		begin
			c := CELL_COLORS[v+1];
			prints(i, j  , c, BOARD_FILLER);
			prints(i, j+1, c, make_center(ltoa(1 shl longint(v)), BOARD_W));
			prints(i, j+2, c, BOARD_FILLER);
		end
		else
		begin
			c := CELL_COLORS[(x+y) and 1];
			prints(i, j  , c, BOARD_FILLER);
			prints(i, j+1, c, BOARD_FILLER);
			prints(i, j+2, c, BOARD_FILLER);
		end;
	end;
end;

{ Finds the proper cell to slide to }
function        boardFind(const v: VEC4_TYPE; x, stop: longint): longint;
var	t: longint;
begin
	if x = 0 then
	begin
		boardFind := x;
		exit;
	end;
        t := x-1;
        while t >= 0 do
        begin
                if v[t] <> 0 then
                begin
			if v[t] <> v[x] then
			begin
				boardFind := t+1;
				exit;
			end;
			boardFind := t;
			exit;
		end
                else
		if t = stop then
		begin
			boardFind := t;
			exit;
		end;
                dec(t);
        end;
end;

{ Slides the board }
function        boardSlide(var v: VEC4_TYPE): boolean;
var     success : boolean;
	x, t,
	stop    : longint;
begin
	stop := 0;
	success := FALSE;
	for x := 0 to BOARD_DIM-1 do
        begin
                if v[x] <> 0 then
                begin
			t := boardFind(v, x, stop);
                        if (t <> x) then
                        begin
                                if v[t] = 0 then
                                        v[t] := v[x]
                                else
                                if v[t] = v[x] then
                                begin
                                        inc(v[t]);
					inc(GAME_SCORE, 1 shl v[t]);
					if GAME_HISCORE < GAME_SCORE then
						GAME_HISCORE := GAME_SCORE;
                                        stop := t+1;
                                end;
                                v[x] := 0;
                                success := TRUE;
                        end;
                end;
        end;
	boardSlide := success;
end;

{ Rotates the board CW }
procedure       boardRotate(var board: BOARD_TYPE);
var     i, j,
	t, n : longint;
begin
	{ inspace square CW matrix rotation, from geeksforgeeks.com }
	n := BOARD_DIM;
	for i := 0 to (n shr 1)-1 do
	for j := i to (n-i-1)-1 do
	begin
				  t := board[j    , i    ];
		board[j    , i    ] := board[i    , n-1-j];
		board[i    , n-1-j] := board[n-1-j, n-1-i];
		board[n-1-j, n-1-i] := board[n-1-i, j    ];
		board[n-1-i, j    ] := t;
	end;
end;

{ Slides the board up }
function        boardSlideUp(var board: BOARD_TYPE): boolean;
var     x       : longint;
	ok, v	: boolean;
begin
	ok := FALSE;
	for x := 0 to BOARD_DIM-1 do
	begin
		{ in case compiler option 'Complete boolean eval' is off }
		v  := boardSlide(board[x]);
		ok := ok or v;
	end;
	boardSlideUp := ok;
end;

{ Slides the board left }
function        boardSlideLeft(var board: BOARD_TYPE): boolean;
var	ok	: boolean;
begin
	boardRotate(board);
	ok := boardSlideUp(board);
	boardRotate(board);
	boardRotate(board);
	boardRotate(board);
	boardSlideLeft := ok;
end;

{ Slides the board down }
function        boardSlideDown(var board: BOARD_TYPE): boolean;
var	ok	: boolean;
begin
	boardRotate(board);
	boardRotate(board);
	ok := boardSlideUp(board);
	boardRotate(board);
	boardRotate(board);
	boardSlideDown := ok;
end;

{ Slides the board right }
function        boardSlideRight(var board: BOARD_TYPE): boolean;
var	ok	: boolean;
begin
	boardRotate(board);
	boardRotate(board);
	boardRotate(board);
	ok := boardSlideUp(board);
	boardRotate(board);
	boardSlideRight := ok;
end;

{ Checks for mergeable pair }
function        boardCheckPair(board: BOARD_TYPE): boolean;
var     x, y    : longint;
begin
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-2 do
		if board[x, y] = board[x, y+1] then
		begin
			boardCheckPair := TRUE;
			exit;
		end;
	boardCheckPair := FALSE;
end;

{ Counts the number of empty cells }
function        boardCountBlank(board: BOARD_TYPE): longint;
var     x, y, c	: longint;
begin
	c := 0;
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-1 do
		if board[x, y] = 0 then inc(c);
	boardCountBlank := c;
end;

{ Place a random cell into the board }
function        boardPlace(var board: BOARD_TYPE): boolean;
var     x, y,
	r, n,
	len	: longint;
	list    : array[0..BOARD_SIZE-1] of record
			x, y: longint;
		  end;
begin
        len := 0;
	for x := 0 to BOARD_DIM-1 do
	for y := 0 to BOARD_DIM-1 do
		if board[x, y] = 0 then
		begin
			list[len].x := x;
			list[len].y := y;
                        inc(len);
                end;
	if (len > 0) then
	begin
                r := random(len);
		n := (random(10) div 9)+1;
		board[list[r].x, list[r].y] := n;
		boardPlace := TRUE;
        end
	else    boardPlace := FALSE;
end;

{ Check for game over state }
function        stateCheckOver(board: BOARD_TYPE): boolean;
var     ended   : boolean;
begin
	ended := TRUE;
	if boardCountBlank(board) > 0 then
	begin
		stateCheckOver := FALSE;
		exit;
	end;
	if boardCheckPair(board) then
	begin
		stateCheckOver := FALSE;
		exit;
	end;

	boardRotate(board);
	if boardCheckPair(board) then
		ended := FALSE;
	boardRotate(board);
	boardRotate(board);
	boardRotate(board);
	stateCheckOver := ended;
end;

{ Puts the game into initial state }
procedure	stateInit(var board: BOARD_TYPE);
begin
	fillchar(board, sizeof(BOARD_TYPE), 0);
	GAME_MOVES := 0;
	GAME_SCORE := 0;
end;

{ Restarts the game }
procedure	stateRestart(var board: BOARD_TYPE);
begin
	stateInit(board);
	boardPlace(board);
	boardPlace(board);
	boardShow(board);
end;

{ Saves current game state to the save game file, returns TRUE if succeed }
function        stateSave(var board: BOARD_TYPE): boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SAVE);
	{$i-}
	rewrite(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockwrite(f, GAME_SCORE, sizeof(longint));
		blockwrite(f, GAME_MOVES, sizeof(longint));
		blockwrite(f, board, sizeof(BOARD_TYPE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	stateSave := ret;
end;

{ Loads the game state from the save game file, returns TRUE if succeed }
function        stateLoad(var board: BOARD_TYPE): boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SAVE);
	{$i-}
	reset(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockread(f, GAME_SCORE, sizeof(longint));
		blockread(f, GAME_MOVES, sizeof(longint));
		blockread(f, board, sizeof(BOARD_TYPE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	stateLoad := ret;
end;

{ Loads the hi-score value from the score file, returns TRUE if succeed }
function        scoreLoad: boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SCORE);
	{$i-}
	reset(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockread(f, GAME_HISCORE, sizeof(GAME_HISCORE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	scoreLoad := ret;
end;

{ Saves the hi-score value into the score file, returns TRUE if succeed }
function	scoreSave: boolean;
var	f	: FILE;
	ret	: boolean;
begin
	assign(f, FILE_SCORE);
	{$i-}
	rewrite(f, 1);
	{$i+}
	if ioresult = 0 then
	begin
		blockwrite(f, GAME_HISCORE, sizeof(GAME_HISCORE));
		close(f);
		ret := TRUE;
	end
	else	ret := FALSE;
	scoreSave := ret;
end;

{$endif}
