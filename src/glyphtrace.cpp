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

#include "glyphtrace.h"
#include "potrace_util.h"

#include <iostream>
#include <QTransform>

#include <cstdio>
#include <cmath>

#define DEFAULTXADVANCE 1000.0
#define MINIMUMIMAGEAREA 10000.0

GlyphTrace::GlyphTrace(QImage image, const Options& opt)
{
	double iHeight(image.height());
	double iWidth(image.width());
	if(iHeight * iWidth > MINIMUMIMAGEAREA)
		iScale = 1.0;
	else
	{
		iScale = qMax(sqrt(MINIMUMIMAGEAREA / (iHeight * iWidth)), 0.000001);
	}
//	qDebug()<<iWidth<<iHeight<<iScale<< (iWidth * iScale) << (iHeight * iScale);
	bitmap = toBitmap(image.scaled(iWidth * iScale, iHeight * iScale, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	advance = DEFAULTXADVANCE;
	tx = ty = 0;
	potrace_param_t * par = potrace_param_default();
	par->progress.callback = captureTraceProgress;
	par->progress.min = 0;
	par->progress.max = 100;
	par->progress.epsilon = 0;
	par->progress.data = 0;
	par->alphamax = opt.alphamax;
	par->turdsize = opt.turdsize;
	par->opticurve = opt.opticurve;
	par->opttolerance = opt.opttolerance;

	potrace_bitmap_t pbm;
	pbm.h = bitmap.height();
	pbm.w = bitmap.width();
	pbm.dy = (pbm.w / potrace_N);
	if(pbm.w % potrace_N)
		pbm.dy += 1;
	pbm.map = new potrace_word[pbm.h * pbm.dy * potrace_N];
	memset(pbm.map, 0, sizeof(potrace_word) * pbm.h * pbm.dy * potrace_N);
	for(unsigned short i=0; i < pbm.h; ++i)
	{
		for(unsigned short j=0; j < pbm.w; ++j)
		{
			if(bitmap.pixelIndex(j,i) > 0)
				potrace_clear_pixel(&pbm, j, (pbm.h - 1) - i);
			else
				potrace_set_pixel(&pbm, j, (pbm.h - 1) - i);
		}
	}
//	image.save(QString("%1_image.png").arg(int(this)));
//	bitmap.save(QString("%1_bitmap.png").arg(int(this)));
	potrace_state_t *pState = potrace_trace(par, &pbm);
	potrace_path_t * p = pState->plist;
	while(p != 0)
	{
		storeCurve(&p->curve);
		p = p->next;
	}
	potrace_state_free(pState);
	potrace_param_free(par);
	delete[] pbm.map;
	bitmap = QImage();
}

GlyphTrace * GlyphTrace::Clone()
{
	GlyphTrace * ret = new GlyphTrace;
//	ret->bitmap = bitmap;
	ret->gstore = gstore;
	ret->outline = outline;
	ret->glyphName = glyphName;
	ret->hexcode = hexcode;
	ret->scale = scale;
	ret->advance = advance;
	ret->tx = tx;
	ret->ty = ty;
	return ret;
}


bool GlyphTrace::Glyph()
{
	outline.clear();
	if(gstore.isEmpty())
		return false;

	QTransform t;
	t.translate(tx, ty);
	t.scale(scale, scale);
	QPainterPath path(t.map(gstore));
//	qDebug()<<glyphName<<t;

	outline +=  "\n<outline><contour>\n";
	QPointF curPos;
	QString move("<point x=\"%1\" y=\"%2\" type=\"move\"/>\n");
	QString line("<point x=\"%1\" y=\"%2\" type=\"line\" smooth=\"no\"/>\n");
	QString curve("<point x=\"%1\" y=\"%2\"/>\n<point x=\"%3\" y=\"%4\"/>\n<point x=\"%5\" y=\"%6\" type=\"curve\" smooth=\"yes\"/>\n");
	for (int i = 0; i < path.elementCount(); ++i)
	{
		QPainterPath::Element cur = path.elementAt(i);
		QPointF curPoint(cur);
		if(cur.isMoveTo())
		{
			curPos = curPoint;
			if(0 != i)
			{
				outline +=  "</contour>\n";
				outline +=  "<contour>\n";
			}
//			outline += move.arg(curPos.x()).arg(curPos.y());
		}
		else if(cur.isLineTo())
		{
			curPos = curPoint;
			outline += line.arg(qRound(curPos.x())).arg(qRound(curPos.y()));
		}
		else if(cur.isCurveTo())
		{
			QPointF c1 = path.elementAt(i + 1);
			QPointF c2 = path.elementAt(i + 2);
			outline += curve.arg(qRound(curPoint.x())).arg(qRound(curPoint.y()))
					.arg(qRound(c1.x())).arg(qRound(c1.y()))
					.arg(qRound(c2.x())).arg(qRound(c2.y()));
			i += 2;
			curPos = c2;
		}
		else
			qDebug("Unknown point type");
	}
	outline +=  "\n</contour>\n</outline>";

	return true;
}

QPointF GlyphTrace::GetPoint(potrace_dpoint_t c)
{
	QPointF ret(c.x / iScale, c.y / iScale);
	return ret;
}

void GlyphTrace::storeCurve(potrace_curve_t *c)
{
	gstore.moveTo(GetPoint(c->c[c->n-1][2]));
	for(int i=0; i < c->n; ++i)
	{
		if(c->tag[i] == POTRACE_CURVETO)
		{
			gstore.cubicTo(GetPoint(c->c[i][0]),  GetPoint(c->c[i][1]), GetPoint(c->c[i][2]));
		}
		else // CORNER
		{
			gstore.lineTo(GetPoint(c->c[i][1]));
			gstore.lineTo(GetPoint(c->c[i][2]));
		}
	}
}

QImage GlyphTrace::toBitmap(const QImage &i, int limit)
{
	QImage r(i.size(),QImage::Format_Mono);
	int h(r.height());
	int w(r.width());
	for(unsigned int y=0; y < h; ++y)
	{
		for(unsigned int x=0; x < w; ++x)
		{
			if(qGray(i.pixel(x,y)) < limit)
				r.setPixel(x,y, 0);
			else
				r.setPixel(x,y, 1);

		}
	}
	return r;
}

void GlyphTrace::captureTraceProgress(double d, void *v)
{
	//todo
}
