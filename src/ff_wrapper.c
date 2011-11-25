/***************************************************************************
 *   Copyright (C) 2011 by Pierre Marchand   *
 *   pierre@oep-h.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


/*
In splinefont.h, "class" is used as a variable, hence the need to wrap it in C code.
Although I cant see how it will work out as I finally need to include this wrapper. let's see

*/

#define _NO_LIBSPIRO
#include "fontforge/fontforge.h"

#include "ff_wrapper.h"

void generateFontOTF(char* src, char* trgt)
{
//	struct fontdict * fd = ReadPSFont(src);
//	SplineFont * sf = SplineFontFromPSFont(fd);
	SplineFont * sf = SFReadUFO(src, 0);
	EncMap * a_encmap = EncMapFromEncoding(sf, FindOrMakeEncoding("UnicodeBmp"));
	WriteTTFFont(trgt, sf, ff_otf, NULL, bf_none, 0, a_encmap, ly_fore);

	EncMapFree(a_encmap);
	SplineFontFree(sf);
//	PSFontFree(fd);
}

FF_Struct* newFF()
{
	FF_Struct*  FF = (FF_Struct*) malloc(sizeof(FF_Struct)) ;
	FF->generate = &generateFontOTF;
}

void freeFF(FF_Struct* FF)
{
	free(FF);
}

