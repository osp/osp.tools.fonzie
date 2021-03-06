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

#include "composer.h"

#include <QString>

Composer* Composer::instance = 0;
Composer::Composer()
{
	pl.clear();
}

Composer* Composer::that()
{
	if(instance == 0)
		instance = new Composer;
	return instance;
}

int Composer::NewPage()
{
	PosList p;
	that()->pl.append(p);
	return that()->pl.count();
}

void Composer::Add(QChar ccode, QPointF pos, double sizeHint, unsigned int line)
{
	Pos p(ccode, pos, sizeHint, line);
	that()->pl.last().append(p);
}

const QList<Composer::PosList>& Composer::GetList()
{
	return that()->pl;
}


QString Composer::GetText()
{
    QList<Composer::PosList>& pl = that()->pl;
    QString ret;
    foreach(const Composer::PosList& l, pl)
    {
        foreach(const Composer::Pos& p, l)
        {
            ret.append(p.c);
        }
    }
    return ret;
}
