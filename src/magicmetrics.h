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

#ifndef MAGICMETRICS_H
#define MAGICMETRICS_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

#include <ft2build.h>
#include FT_FREETYPE_H

class GlyphTrace;

class MagicMetrics
{
	MagicMetrics()
	{

	}
	struct M
	{
		QList<double> left;
		QList<double> right;
		double minR;
		double minL;
		M()
		{
			minL = 99999.0;
			minR = 99999.0;
		}
	};
	QMap<GlyphTrace*, M> metrics;

	struct CPair{
		QString left;
		QString right;
		int val;
	};

	double Mean(const QList<double>& l);
	double Min(const QList<double>& l);
	void Do(QList<GlyphTrace*> gtl);

	bool hasReference;
	bool strictReference;
	FT_Face face;
	FT_Library library;

public:
	MagicMetrics(QList<GlyphTrace*> gtl,QString ref = QString(), bool strictR = false);
	double advance;
	double tx;
	QString gposfeature;
};

#endif // MAGICMETRICS_H
