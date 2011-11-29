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

#include "options.h"
#include "names.h"

#include <QDir>
#include <QFile>
#include  <QDebug>

Options::Options(const QStringList& opt)
{
	QList<QString> images;
	foreach(QString s, opt)
	{
		QStringList arg(s.split("="));
		if(arg.size() > 1)
		{
			// special case for images
			if(arg.at(0) == QString(OPTIMAGE))
			{
				images << arg.at(1);
			}
			else
			{
				insert(arg.at(0),arg.at(1));
			}
		}
	}
	if(images.count() >= 1)
	{
		for(int i(0); i < images.count(); ++i)
		{
			insert(QString(OPTIMAGE) + QString::number(i), images.at(i));
		}
	}
//	else
//	{
//		QDir dir(".");
//		QStringList globbed(dir.entryList(images));
//		for(int i(0); i < globbed.count(); ++i)
//		{
//			insert(QString(OPTIMAGE) + QString::number(i), globbed.at(i));
//			qDebug()<<QString(OPTIMAGE) + QString::number(i)<<" => "<<globbed.at(i);
//		}

//	}
}

bool Options::toBool(const QString &s, int n)
{
	QStringList trueString;
	QStringList falseString;
	QStringList allString;
	trueString << "true" << "True" << "TRUE";
	falseString << "false" << "False" << "FALSE";
	allString << falseString << trueString;

	QString key( s );
	if((n > 0)
		|| contains(key + QString::number(0)))
		key.append( QString::number(n) );
	QString v(value(key));
	if(allString.contains(v))
	{
		if(trueString.contains(v))
			return true;
		else
			return false;
	}
	int ival(value(key,"0").toInt());
	return (ival == 0 ? false : true);
}

int Options::toInt(const QString &s , int n)
{
	QString key( s );
	if((n > 0)
		|| contains(key + QString::number(0)))
		key.append( QString::number(n) );
	return value(key,"0").toInt();
}


double Options::toDouble(const QString &s , int n)
{
	QString key( s );
	if((n > 0)
		|| contains(key + QString::number(0)))
		key.append( QString::number(n) );
	return value(key,"0").toDouble();
}

GridSpec Options::toSpec(const QString &s , int n)
{
	QString key( s );
	if((n > 0)
		|| contains(key + QString::number(0)))
		key.append( QString::number(n) );
	GridSpec ret(value(key,"0+0+0+0+0+0"));
	return ret;
}

QString Options::toString(const QString &s, int n)
{
	QString key( s );
	if((n > 0)
		|| contains(key + QString::number(0)))
		key.append( QString::number(n) );
	return value(key, QString());
}

void Options::Dump()
{
	foreach(QString k, keys())
	{
		qDebug()<<k << value(k) << toInt(k);
	}
}

int Options::Count(const QString &countableOption) const
{
	if(!contains(countableOption)
		&& !contains(countableOption + QString::number(0)))
		return 0;
	QString supstr(countableOption + QString("%1"));
	for(int ret(1); ret < 10000; ++ret)
	{
		if(!contains(supstr.arg(ret)))
		{
			return ret;
		}
	}
}
