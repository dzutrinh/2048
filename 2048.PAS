{ ////////////////////////////////////////////////////////////////////////// }
{									     }
{   2048.PAS                                                                 }
{   --------                                                                 }
{   2048 Game for DOS                                                        }
{   Version 1.0                                                              }
{   Coded by Trinh D.D. Nguyen                                               }
{   Copyrights (c) 2019 by Infinity Group                                    }
{                                                                            }
{   Compilers                                                                }
{   ---------                                                                }
{   . Turbo/Borland Pascal 7.0 (16-bit)                                      }
{   . TMT Pascal 3.5+ (32-bit)                                               }
{   . Free Pascal 1.0.1+ (GO32)                                              }
{                                                                            }
{   Tools used                                                               }
{   ----------                                                               }
{   . TheDraw                                                                }
{                                                                            }
{   DDK16 | DDK32                                                            }
{									     }
{ ////////////////////////////////////////////////////////////////////////// }

{$i logo.inc}			{ main screen data            }
{$i globals.inc}		{ global constants, variables }
{$i helpers.inc}		{ utility routines            }
{$i engine.inc}			{ game engine                 }
{$i game.inc}			{ game handlers               }

begin
	gameInit;		{ init game }
	if gameDemo then	{ playing a so-called demo }
		gamePlay;	{ game loop }
	gameDone;		{ shutdown game }
end.
