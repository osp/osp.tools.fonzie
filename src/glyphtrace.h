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

#ifndef GLYPHTRACE_H
#define GLYPHTRACE_H

#include <QThread>
#include <QImage>
#include <QString>
#include <QPointF>
#include <QPainterPath>
#include <QList>
#include <QDebug>

#include "potracelib.h"



class GlyphTrace : public QObject
{
	Q_OBJECT

	GlyphTrace(){}
	static void captureTraceProgress(double d, void * v);

	QPointF GetPoint(potrace_dpoint_t c);
	void storeCurve(potrace_curve_t* c);

	potrace_dpoint_t  curpoint;
	double iScale;

	QPainterPath gstore;
	QImage bitmap;
	QString outline;

	QString glyphName;
	QString hexcode;
	unsigned int charcode;
	double scale;
	double advance;
	double tx;
	double ty;

	QImage toBitmap(const QImage& i, int limit = 254 / 2);

public:
	struct Options{
		double alphamax;
		double turdsize;
		double opticurve;
		double opttolerance;
		Options():
			alphamax(1.2),
			turdsize(3),
			opticurve(1),
			opttolerance(0.6)
		{}
	};

	GlyphTrace(QImage image, const Options& opt);
	GlyphTrace * Clone();

	bool Glyph();

	QString GetOutline() const {return outline;}

	QString GetName(bool escaped=false) const {return escaped ? QString("\\") + glyphName : glyphName;}
	void SetName(const QString& n) {glyphName = n;}

	QString GetHexcode() const {return hexcode;}
	void SetHexcode(const QString& h) {hexcode = h;}

	unsigned int GetCharcode() const {return charcode;}
	void SetCharcode(const unsigned int& c) {charcode = c;}

	double GetScale() const{return scale;}
	void SetScale(double a){scale = a;}

	double GetAdvance() const{return advance;}
	void SetAdvance(double a){advance = a;}

	double GetTx() const {return tx;}
	void SetTx(double val){tx = val;}

	double GetTy() const {return ty;}
	void SetTy(double val){ty = val;}

	// const QImage& GetBitmap() const {return bitmap;}
	const QPainterPath& GetPath() const {return gstore;}
	void setPath( QPainterPath path) {gstore = path;}


	void DumpMetrics() const
	{
		qDebug()<<glyphName<<scale<<advance<<tx<<ty;
	}


signals:
	void TraceProgress();
};

#endif // GLYPHTRACE_H
