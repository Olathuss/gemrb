/* GemRB - Infinity Engine Emulator
 * Copyright (C) 2003 The GemRB Project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header: /data/gemrb/cvs2svn/gemrb/gemrb/gemrb/plugins/Core/Console.cpp,v 1.20 2005/04/10 16:40:00 avenger_teambg Exp $
 *
 */

#include "../../includes/win32def.h"
#include "Interface.h"
#include "Console.h"

Console::Console(void)
{
	ta = NULL;
	Cursor = NULL;
	Back = NULL;
	max = 128;
	Buffer = ( unsigned char * ) malloc( max );
	Buffer[0] = 0;
	CurPos = 0;
	palette = NULL;
}

Console::~Console(void)
{
	free( Buffer );
	Video *video = core->GetVideoDriver();

	video->FreePalette( palette );
	video->FreeSprite( Cursor );
}

/** Draws the Console on the Output Display */
void Console::Draw(unsigned short x, unsigned short y)
{
	if (ta) {
		ta->Draw( x, y - ta->Height );
	}
	if (Back) {
		core->GetVideoDriver()->BlitSprite( Back, 0, y, true );
	}
	Color black = {
		0x00, 0x00, 0x00, 0xff
	};
	Region r( x + XPos, y + YPos, Width, Height );
	core->GetVideoDriver()->DrawRect( r, black );
	font->Print( r, Buffer, palette,
			IE_FONT_ALIGN_LEFT | IE_FONT_ALIGN_MIDDLE, true, NULL,
			Cursor, CurPos );
}
/** Set Font */
void Console::SetFont(Font* f)
{
	if (f != NULL) {
		font = f;
	}
}
/** Set Cursor */
void Console::SetCursor(Sprite2D* cur)
{
	if (cur != NULL) {
		Cursor = cur;
	}
}
/** Set BackGround */
void Console::SetBackGround(Sprite2D* back)
{
	//if 'back' is NULL then no BackGround will be drawn
	Back = back;
}
/** Sets the Text of the current control */
int Console::SetText(const char* string, int /*pos*/)
{
	strncpy( ( char * ) Buffer, string, max );
	return 0;
}
/** Key Press Event */
void Console::OnKeyPress(unsigned char Key, unsigned short /*Mod*/)
{
	if (Key >= 0x20) {
		size_t len = strlen( ( char* ) Buffer );
		if (len + 1 < max) {
			for (size_t i = len; i > CurPos; i--) {
				Buffer[i] = Buffer[i - 1];
			}
			Buffer[CurPos++] = Key;
			Buffer[len + 1] = 0;
		}
	}
}
/** Special Key Press */
void Console::OnSpecialKeyPress(unsigned char Key)
{
	size_t len;

	switch (Key) {
		case GEM_BACKSP:
			if (CurPos != 0) {
				size_t len = strlen( ( char* ) Buffer );
				for (size_t i = CurPos; i < len; i++) {
					Buffer[i - 1] = Buffer[i];
				}
				Buffer[len - 1] = 0;
				CurPos--;
			}
			break;
		case GEM_HOME:
			CurPos = 0;
			break;
		case GEM_END:
                        CurPos = strlen( (char * ) Buffer);
                        break;
		case GEM_LEFT:
			if (CurPos > 0)
				CurPos--;
			break;
		case GEM_RIGHT:
			len = strlen( ( char * ) Buffer );
			if (CurPos < len) {
				CurPos++;
			}
			break;
		case GEM_DELETE:
			len = strlen( ( char * ) Buffer );
			if (CurPos < len) {
				for (size_t i = CurPos; i < len; i++) {
					Buffer[i] = Buffer[i + 1];
				}
			}
			break;			
		case GEM_RETURN:
			char* msg = core->GetGUIScriptEngine()->ExecString( ( char* )
														Buffer );
			if (ta)
				ta->SetText( msg );
			free( msg );
			Buffer[0] = 0;
			CurPos = 0;
			Changed = true;
			break;
	}
}
