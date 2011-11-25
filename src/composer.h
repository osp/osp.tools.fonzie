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

#ifndef COMPOSER_H
#define COMPOSER_H

#include <QChar>
#include <QPointF>
#include <QList>

class Composer
{
	Composer();
	static Composer * instance;
	static Composer * that();
	struct Pos_
	{
		QChar c;
		QPointF p;
		double sh;
		int l;

		Pos_():
			c(QChar()),
			p(QPointF()),
			sh(0.0),
			l(0)
		{}
		Pos_(QChar ccode, QPointF pos, double sizeHint, unsigned int line):
			c(ccode),
			p(pos),
			sh(sizeHint),
			l(line)
		{}
	};

public:
	typedef Pos_ Pos;
	typedef QList<Pos> PosList;

	static void Add(QChar ccode, QPointF pos, double sizeHint, unsigned int line);
	static const PosList& GetList();

private:
	PosList pl;
};

#endif // COMPOSER_H
