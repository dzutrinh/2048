{$ifndef	__HELPERS_2048__}
{$define	__HELPERS_2048__}

{ ////////////////////////////////////////////////////////////////////////// }

const
	SCR_WIDTH	= 80;
	SCR_HEIGHT	= 25;
	CO80		= 3;

	CURSOR_OFF	= $2000;
	CURSOR_ON	= $0607;

	VIDEO_SEG	= $B800;	{ VGA text mode VRAM segment }

type
	KEYS	= set of char;

var
	ticks	: longint absolute $0040:$006C;

{ exposed APIs }
procedure       textmode(mode: byte);					forward;
procedure	sleep(duration: longint);				forward;
function	readkey: char;						forward;
function	keypressed: boolean; 					forward;

procedure       blinkoff;						forward;
procedure	blinkon;						forward;
procedure	cursor(size: word);					forward;
procedure	printc(x, y, attr: longint; c: char);			forward;
procedure       prints(x, y, attr: longint; text: string);		forward;

procedure       print(x, y: longint; st: string);			forward;
procedure       center(y: longint; text: string);			forward;

function	wait(k: KEYS): char;					forward;
function	query(mesg: string): boolean;				forward;
procedure	info(mesg: string);					forward;

function        lpad(text: string; width: longint): string;		forward;
function        ltoa(v: longint): string;				forward;
function        make_center(text: string; width: longint): string;	forward;

{ ////////////////////////////////////////////////////////////////////////// }

procedure	textmode(mode: byte); assembler;
asm
	mov	ah, 00
	mov	al, mode
	int	$10
end;

procedure	sleep(duration: longint);
var	timer	: longint;
begin
	timer := ticks;
	repeat until abs(ticks-timer)=duration;
end;

function	readkey: char; assembler;
asm
	mov  ah, $07
	int  $21
end;

function	keypressed: boolean; assembler;
asm
	mov  ah, $0B
	int  $21
end;

procedure       blinkoff; assembler;
asm
	mov	ax, $1003
	mov	bl, 0
	int	$10
end;

procedure	blinkon; assembler;
asm
	mov	ax, $1003
	mov	bl, 1
	int	$10
end;

procedure	cursor(size: word); assembler;
asm
	mov	ah, $01
	mov	bh, 0
	mov	cx, size
	int	$10
end;

procedure       printc(x, y, attr: longint; c: char);
begin
	MEMW[VIDEO_SEG:((y-1)*SCR_WIDTH+(x-1)) shl 1] := attr shl 8 + ord(c);
end;

procedure       prints(x, y, attr: longint; text: string);
var	i	: longint;
begin
	for i := 1 to length(text) do
		MEMW[VIDEO_SEG:((y-1)*SCR_WIDTH+(x+i-1)) shl 1] := attr shl 8 + ord(text[i]);
end;

procedure       print(x, y: longint; st: string);
var     i, attr,
	fg, bk	: longint;
	v	: char;
begin
	attr := $07; i := 1;
	i := 1;
	while i <= length(st) do
	begin
		v := st[i];
		if v = '~' then
		begin
			inc(i); v := st[i];
			case v of
			'0'..'9': bk := ord(v)-48;
			'A'..'F': bk := ord(v)-55;
			end;
			inc(i); v := st[i];
			case v of
			'0'..'9': fg := ord(v)-48;
			'A'..'F': fg := ord(v)-55;
			end;
			attr := bk shl 8 + fg;
		end
		else
		begin
			printc(x, y, attr, v);
			inc(x);
		end;
		inc(i);
	end;
end;

procedure       center(y: longint; text: string);
var	i, x, c	: longint;
begin
	c := 0;
	for i := 1 to length(text) do
		if text[i] = '~' then inc(c, 3);
	x := ((SCR_WIDTH-(length(text)-c)) shr 1)+1;
	print(x, y, text);
end;

function        lpad(text: string; width: longint): string;
begin
	while length(text) < width do text := ' '+text;
	lpad := text;
end;

function        ltoa(v: longint): string;
var	text	: string;
begin
	if v = 0 then text := '-' else str(v, text);
	ltoa := text;
end;

function        make_center(text: string; width: longint): string;
var	bound,
	len, i	: longint;
	pad	: string;
begin
	len := length(text);
	if width < len then width := len;
	bound := (width-len) shr 1;
	pad   := '';
	for i := 1 to bound do pad := pad+' ';
	text := pad+text+pad;
	while length(text) < width do text := ' '+text;
	make_center := text;
end;

function	wait(k: KEYS): char;
var	ch	: char;
begin
	repeat
		ch := upcase(readkey);
	until ch in k;
	wait := ch;
end;

function	query(mesg: string): boolean;
var	value	: boolean;
begin
	center(24, mesg);
	value := wait(['Y','N',#13,#27]) in ['Y',#13];
	center(24, lpad('',length(mesg)));
	query := value;
end;

procedure	info(mesg: string);
begin
	center(24, mesg);
	wait([#13,#27]);
	center(24, lpad('',length(mesg)));
end;

{$endif}
