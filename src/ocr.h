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
 *************************************************************************/

#ifndef OCR_H
#define OCR_H


#undef TESSDATA_DIR
#include <tesseract/baseapi.h>

#include <QString>
#include <QImage>
#include <QMap>
#include <QChar>
#include <QDebug>


class OCR : public tesseract::TessBaseAPI
{
	QImage m_img;
	bool scaleGlyphs;

	struct GInfo
	{
		double xheight;
		double ascender;
		double descender;
		double baseline;
		int line;

		void dump()
		{
			qDebug() << "XH"<<xheight<<"AS"<<ascender<<"DE"<<descender<<"BL"<<baseline<<"LI"<<line;
		}
	};

	class CommentStream : public QDebug
	{
	public:
		CommentStream() : QDebug(QtDebugMsg){}
	};

	CommentStream CS;

public:
    OCR(const QString& datadir);

    void loadImage(const QString& fn);
    void saveImages(int baseline = 0);

    QMap<QChar, QImage> iDict;
    QMap<QChar, double> xheightDict;
    QMap<QChar, int> offsetDict;
    int fontSizePix;
};

#endif // OCR_H
