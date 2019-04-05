{*===========================================================================
 *  MINI2048.PAS
 *  ------------
 *  The game 2048 (console)
 *  Mini one file version
 *  By Trinh D.D. Nguyen
 *  For Free Pascal only - supports DOS/Win32
 *===========================================================================}
uses    dos, crt;

const   SIZE    = 4;
        CELLW   = 6;
        CELLH   = 3;
        BOARDX  = (80-CELLW*SIZE) shr 1;
        BOARDY  = ((25-CELLH*SIZE) shr 1)+3;
        SCORE   : longint = 0;
        COLORS  : array[0..15] of byte = ($80,$70,$60,$20,$50,$30,$90,$A0,$B0,$C0,$D0,$E0,$F0,$F0,$F0,$F0);

type    VEC4    = array[0..SIZE-1] of integer;
        GAME    = array[0..SIZE-1] of VEC4;

procedure       blinkoff;
var     r       : REGISTERS;
begin   r.ax := $1003; r.bl := 0;  intr($10, r);        end;

function        itoa(n: integer): string;
var     s       : string;
begin	str(n, s); exit(s);	                        end;

function        lpad(s: string; n: integer): string;
begin	while length(s) < n do s:=' '+s; exit(s);	end;

procedure       print(x, y, color: integer; text: string);
begin	gotoxy(x, y); textattr := color; write(text);	end;

procedure       logo;
begin
        print(29, 1, $0F, '   ___  ___  ____ ___ ');
	print(29, 2, $0F, '  |_  |/ _ \/ / /( _ )');
        print(29, 3, $07, ' / __// // /_  _/ _  |');
	print(29, 4, $08, '/____/\___/ /_/ \___/ ');
        print(29, 6, $03, 'WASD: Move * ESC: Quit');
end;

procedure       show(const board: GAME);
var
        x, y, v, c      : integer;
begin
        print(BOARDX, BOARDY-1, $07, 'SCORE: '+itoa(SCORE));
        for y := 0 to SIZE-1 do
        for x := 0 to SIZE-1 do begin
                v := board[x,y]; c := COLORS[v];
                print(BOARDX+x*CELLW, BOARDY+y*CELLH,   c, lpad('', CELLW));
                print(BOARDX+x*CELLW, BOARDY+y*CELLH+2, c, lpad('', CELLW));
                if v <> 0 then
                        print(BOARDX+x*CELLW, BOARDY+y*CELLH+1, c, lpad(itoa(1 shl v), CELLW))
                else    print(BOARDX+x*CELLW, BOARDY+y*CELLH+1, c, lpad('', CELLW));
        end;
end;

function        find(const v: VEC4; x, stop: integer): integer;
var     t: integer;
begin
        if x = 0 then exit(x);
        t := x-1;
        while t >= 0 do begin
                if v[t] <> 0 then begin
                        if v[t] <> v[x] then exit(t+1);
                        exit(t);
                end
                else if t = stop then exit(t);
                dec(t);
        end;
end;

function        slide(var v: VEC4): boolean;
var     next         : boolean = FALSE;
        x, t, stop   : integer;
begin
        stop := 0;
        for x := 0 to SIZE-1 do begin
                if v[x] <> 0 then begin
                        t := find(v, x, stop);
                        if (t <> x) then begin
                                if v[t] = 0 then
                                        v[t] := v[x]
                                else if v[t] = v[x] then begin
                                        inc(v[t]);
                                        inc(SCORE, 1 shl v[t]);
                                        stop := t+1;
                                end;
                                v[x] := 0;
                                next := TRUE;
                        end;
                end;
        end;
        exit(next);
end;

procedure       rotate(var board: GAME);
var     i, j, t, n      : integer;
begin
        n := SIZE;
        for i := 0 to (n shr 1)-1 do for j := i to n-i-2 do begin
                t := board[i,j];
                board[i,j] := board[j,n-i-1];
                board[j,n-i-1] := board[n-i-1,n-j-1];
                board[n-i-1,n-j-1] := board[n-j-1,i];
                board[n-j-1,i] := t;
        end;
end;

function        up(var board: GAME): boolean;
var     x       : integer;
        r, v    : boolean;
begin
        r := FALSE;
        for x := 0 to SIZE-1 do begin
                v := slide(board[x]);
                r := r or v;
        end;
        exit(r);
end;

function        left(var board: GAME): boolean;
var     v       : boolean;
begin
        rotate(board);
        v := up(board);
        rotate(board); rotate(board); rotate(board);
        exit(v);
end;

function        down(var board: GAME): boolean;
var     v       : boolean;
begin
        rotate(board); rotate(board);
        v := up(board);
        rotate(board); rotate(board);
        exit(v);
end;

function        right(var board: GAME): boolean;
var     v       : boolean;
begin
        rotate(board); rotate(board); rotate(board);
        v := up(board);
        rotate(board);
        exit(v);
end;

function        mergeable(board: GAME): boolean;
var     x, y    : integer;
begin
        for x := 0 to SIZE-1 do for y := 0 to SIZE-2 do
                if board[x,y] = board[x,y+1] then exit(TRUE);
        exit(FALSE);
end;

function        blank(board: GAME): integer;
var     x, y    : integer;
        c       : integer = 0;
begin
        for x := 0 to SIZE-1 do
        for y := 0 to SIZE-1 do if board[x,y] = 0 then inc(c);
        exit(c);
end;

function        over(board: GAME): boolean;
var     tmp     : GAME;
begin
        move(board, tmp, sizeof(GAME));
        if (blank(tmp) > 0) or mergeable(tmp) then exit(FALSE);
        rotate(tmp);
        if mergeable(tmp) then exit(FALSE);
        exit(TRUE);
end;

procedure       place(var board: GAME);
var     x, y, r, cnt    : integer;
        tmp             : array[0..SIZE*SIZE-1] of record x, y: integer; end;
begin
        cnt := 0;
        for x := 0 to SIZE-1 do for y := 0 to SIZE-1 do
                if board[x,y] = 0 then begin
                        tmp[cnt].x := x; tmp[cnt].y := y;
                        inc(cnt);
                end;
        if (cnt > 0) then begin
                r := random(cnt);
                x := tmp[r].x; y := tmp[r].y;
                board[x, y] := (random(10) div 9)+1;
        end;
end;

procedure       init(var board: GAME);
var     x, y    : integer;
begin
        randomize;
        for x := 0 to SIZE-1 do for y := 0 to SIZE-1 do board[x,y] := 0;
        place(board);
        place(board);
        show(board);
end;

function        demo(var board: GAME): boolean;
var     x, y, c : integer;
begin
        randomize; clrscr; logo;
        print(29, 22, $0A, 'PRESS ANY KEY TO START');
        fillchar(board, sizeof(board), 0);
        c := 0;
        repeat
                x := c mod 4; y := c div 4;
                board[x, y] := random(11)+1;
                show(board);
                delay(400);
                board[x, y] := 0;
                c := (c + 1) mod (SIZE*SIZE);
        until keypressed;
        if readkey = #27 then exit(FALSE) else exit(TRUE);
end;

var
        c       : char;
        quit    : boolean = FALSE;
        next    : boolean = FALSE;
        board   : GAME;

begin
        cursoroff; blinkoff;
        if not demo(board) then exit;
        textattr := $07; clrscr; logo;
        init(board);
        repeat
                c := upcase(readkey);
                case c of
                #27: begin quit := TRUE; next := FALSE; end;
                'A': next := left (board);
                'D': next := right(board);
                'W': next := up   (board);
                'S': next := down (board);
                else next := FALSE;
                end;
                if next then begin
                        place(board); show(board);
                        if over(board) then quit := TRUE;
                end;
        until quit;
        print(36, 22, $0C, 'GAME OVER');
        readkey;
        writeln;
        cursoron;
end.
