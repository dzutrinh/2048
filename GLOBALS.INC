{$ifndef	__GLOBALS_2048__}
{$define	__GLOBALS_2048__}

const
	{ Full version format : hi(VERSION).lo(VERSION).BUILD }
	GAME_VERSION	: word = $0000;		{ Game version number }
	GAME_BUILD	: string[4] = '';	{ Build string }

	COPYRIGHTS	: string = #13#10+
				   '2048 (c) 2019 by Infinity Group'#13#10+
				   'Coded by Trinh D.D. Nguyen';

{$ifdef __TMT__}
	COMPILER	: string = 'TMT';
{$else}
{$ifdef FPC}
	COMPILER	: string = 'FPC';
{$else}
	COMPILER	: string = 'BPC';
{$endif}
{$endif}

{$endif}