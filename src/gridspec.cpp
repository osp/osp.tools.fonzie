/***************************************************************************
 *   Copyright (C) 2010 by Pierre Marchand   *
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

#include "gridspec.h"

#include <QStringList>

GridSpec::GridSpec(const QString& spec)
{
	// separator is '+'
	// for margins, we follow css order (top right bottom left)
	// then cell size (width height)
	// then discard if any

	QStringList l(spec.split("+"));

	marginTop = l[0].toInt();
	marginRight = l[1].toInt();
	marginBottom = l[2].toInt();
	marginLeft = l[3].toInt();

	cellWidth = l[4].toInt();
	cellHeight = l[5].toInt();

	if(l.count() > 6)
		discard = l[6].toInt();
	else
		discard = 0;
}
