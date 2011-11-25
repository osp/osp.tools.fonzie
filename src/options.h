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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDebug>

#include "gridspec.h"

class Options : public QMap<QString,QString>
{
	Options(){}
public:
	Options(const QStringList& opt);
	bool toBool(const QString& s, int n = 0);
	int toInt(const QString& s, int n = 0);
	double toDouble(const QString& s, int n = 0);
	GridSpec toSpec(const QString& s, int n = 0);
	QString toString(const QString& s, int n = 0);
	void Dump();
	int Count(const QString& countableOption) const;
};

#endif // OPTIONS_H
