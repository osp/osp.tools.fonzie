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

#include "ocr.h"
#include "composer.h"
#include <QDebug>
#include <QRect>
#include <QPoint>
#include <QList>
#include <QSize>
#include <QColor>
#include <QDir>
#include <QFileInfo>
#include <QByteArray>
#include <QPainter>
#include <QPen>

#include <cmath>
#include <locale.h>

#include "tesseract/pageres.h"
#include "tesseract/werd.h"

OCR::OCR(const QString& datadir)
	:tesseract::TessBaseAPI()
{
	scaleGlyphs = true;
	//	SetVariable("stopper_debug_level", "10");
	//	SetVariable("classify_learning_debug_level", "10");
	//	SetVariable("dawg_debug_level", "10");
	//	SetVariable("global_ambigs_debug_level", "");
	//	SetVariable("global_tessdata_manager_debug_level", "10");
	//	SetVariable("hyphen_debug_level", "10");
	//	SetVariable("stopper_debug_level", "10");
	//	SetVariable("tosp_debug_level", "10");

	setlocale(LC_NUMERIC, "C");
	QFileInfo fi(datadir);
	if(fi.exists())
	{
		QString dir(fi.dir().absolutePath());
		QString lang(fi.fileName().left(3));
		qDebug()<<"Init Tesseract:"<<dir<<lang;

		QByteArray bad(dir.toUtf8());
		QByteArray bal(lang.toUtf8());

		int error = Init(bad.constData(), bal.constData());
		CS << "=>" <<error;
	}
	else
	{
		int error = Init(NULL, NULL);
		CS <<"Init Tesseract without datadir specified, it might work if TESSDATA_PREFIX is set and you want to recognize english. Returned"<<error;
	}
}


void OCR::loadImage(const QImage& img)
{
#if QT_VERSION < 0x040700
	SetImage(img.bits(), img.width(), img.height(), img.depth() /8, img.bytesPerLine());
#else
	SetImage(img.constBits(), img.width(), img.height(), img.depth() /8, img.bytesPerLine());
#endif
	m_img = img;
}


void OCR::saveImages(int baseline)
{
	int bl(baseline == 0 ? m_img.height() : baseline );
	char * resultText = GetUTF8Text();
//	QString exportString(QString::fromUtf8(resultText));
//	qDebug()<<"OCR:";
//	qDebug()<<exportString;
//	Recognize(0);
	char * text;
	int * lengths;
	float * costs;
	int *x0, *y0, *x1, *y1;
	const int result = TesseractExtractResult(&text, &lengths, &costs, &x0, &x1, &y0, &y1, page_res_);

	// This is not enough results, lets try to get xheight and baseline from the deepest of Tesseract :)
	PAGE_RES_IT page_res_it(page_res_);
	int word_count = 0;
	int letterCount(0);
	QList<GInfo> ginfo;
	ROW * block(0);
	unsigned int line(0);
	while (page_res_it.word() != NULL)
	{
		WERD_RES *word = page_res_it.word();
		ROW_RES *row = page_res_it.row();

		if (word_count)
		{
			GInfo gfn;
			gfn.line = line;
			ginfo << gfn;
			++letterCount;
		}

		if(row->row != block)
		{
			++line;
			block = row->row;
		}
		int wlen = strlen(word->best_choice->unichar_lengths().string());
		C_BLOB_LIST *blobs = word->word->cblob_list();
		C_BLOB_IT it(blobs);
		it.move_to_first();
//		for(int i(0); i < wordString.count(); ++i)
//		for(it.mark_cycle_pt();!it.cycled_list(); it.forward())
		for(int i = 0; i < wlen; i++)
		{
			C_BLOB *blob = it.data();
			TBOX current = blob->bounding_box();
//			TBOX current = word->bln_boxes->BlobBox(i);
			int cleft(current.left());
			int cright(current.width());
			GInfo gfn;
			gfn.xheight = row->row->x_height();
			gfn.ascender = row->row->ascenders();
			gfn.descender = row->row->descenders();
			gfn.baseline = m_img.height() - row->row->base_line(cleft + ( cright / 2));
			gfn.line = line;
			ginfo << gfn;
			//			gfn.dump();
			it.forward();
			++letterCount;
		}
		page_res_it.forward();
		++word_count;
	}
	///
//	PAGE_RES_IT page_res_it(page_res);
//	int word_count = 0;
//	while (page_res_it.word() != NULL)
//	{
//		WERD_RES *word = page_res_it.word();
//		const char *str = word->best_choice->unichar_string().string();
//		const char *len = word->best_choice->unichar_lengths().string();
//		TBOX real_rect = word->word->bounding_box();

//		if (word_count)
//			add_space(out);
//		int n = strlen(len);
//		for (int i = 0; i < n; i++)
//		{
//			TESS_CHAR *tc = new TESS_CHAR(rating_to_cost(word->best_choice->rating()),
//						      str, *len);
//			tc->box = real_rect.intersection(word->box_word->BlobBox(i));
//			out->add_after_then_move(tc);
//			str += *len;
//			len++;
//		}
//		page_res_it.forward();
//		word_count++;
//	}
	////

	qDebug()<<"RESULT:"<<result;
	int iHeight(m_img.height());
	int tLen(0);
	for(int i(0); i<result;++i)
		tLen += lengths[i];
	QMap<QChar, double> costDict;
	QMap<int, int> baselineDict;
	QMap<int, int> descDict;
	QString tc(QString::fromUtf8(text));
        int tcCount = tc.count();
       // CS<<tc.count()<<letterCount;

        Q_ASSERT(result == letterCount);

	bool firstDbg(false);
	for(int i(0); i < result; ++i)
	{
		QChar c(tc.at(i));

		// Pass the whole thing to the composer
		// space characters are not "boxed", we invent something here based on the previous char
//		CS << "\n"<< i << c << x0[i] << x1[i] << y0[i] << y1[i] << "\n";
		if(c.isSpace())
		{
			if(i > 0)
				Composer::Add(c, QPointF(y0[i - 1], iHeight - x1[i - 1]), ginfo.at(i - 1).xheight, ginfo.at(i).line);
		}
		else
			Composer::Add(c, QPointF(x0[i], iHeight - x1[i]), ginfo.at(i).xheight, ginfo.at(i).line);
		CS<<c<<ginfo.at(i).line;

		if(!iDict.contains(c))
		{
			if(!c.isPrint() || c.isSpace())
				continue;
			QPoint tl(x0[i], iHeight - y1[i]);
			QPoint br(y0[i],  /*descDict.value(i)*/ iHeight - x1[i]);
			QRect r(tl, br);
			// Search the larger rect enclosing the same letter
			int iCount(1);

//#define DBG_EXPORT_BOXES
#ifdef DBG_EXPORT_BOXES
			QImage dbgImage(m_img.copy());
			QPen pen(Qt::blue);
			QPen bpen(Qt::red);
			QPainter painter(&dbgImage);
			painter.setBrush(Qt::NoBrush);
			painter.setPen(pen);
#endif
			for(int j(i+1); j < result; ++j)
			{
				if(iCount >= 255)
					break;
				QChar c2(tc.at(j));
				if(c == c2)
				{
					QPoint tl2(x0[j], iHeight - y1[j]);
					QPoint br2(y0[j],  iHeight - x1[j]);
					QRect r2(tl2, br2);
#ifdef DBG_EXPORT_BOXES
					painter.drawRect(r2);
					painter.setPen(bpen);
					painter.drawLine(QPoint(r2.left(), ginfo.at(j).baseline),
							 QPoint(r2.right(), ginfo.at(j).baseline));
					painter.setPen(pen);
#endif
					if(r2.width() > r.width())
					{
						r.setWidth(qMax(r.width(), r2.width()));
						r.setLeft(r.left() - ((r2.width()- r.width())/2));
					}
					if(r2.height() > r.height())
					{
						r.setHeight(qMax(r.height(), r2.height()));
						r.setTop(r.top() - ((r2.top() - r2.top())/2));
					}
					++iCount;
				}
			}
#ifdef DBG_EXPORT_BOXES
			painter.end();
			dbgImage.save(QString("%1_BOXES.png").arg(c));
#endif
			qDebug()<<"Processing character:"<<c<<"; samples:"<<iCount;
			QImage ref(r.size(),QImage::Format_ARGB32);
			ref.fill(0xFFFFFFFF);
			int rwidth(r.width());
			int rheight(r.height());
			QList<double> xheights;
			QList<int> baselines;

				for(int j(i), nSamples(0); j < result && nSamples < 255; ++j)
				{
					QChar c2(tc.at(j));
					if(c == c2)
					{
						++nSamples;
						QPoint tl2(x0[j], iHeight - y1[j]);
						QPoint br2(y0[j],  iHeight - x1[j]);
						QRect r2(tl2, br2);
						double vScale(double(r2.height()) / double(r.height()));

						baselines.append(qRound(double(ginfo.at(j).baseline - r2.bottom()) * vScale));
						xheights.append(double(ginfo.at(j).xheight)* vScale);
						QImage cImg(m_img.copy(r2).scaled(r.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
						for(int y(0); y < r.height() ; ++y)
						{
							for(int x(0); x < r.width(); ++x)
							{
								int refX(x);
								int refY(y);
								int valR2(qGray(cImg.pixel(x,y)) < 128 ? 255 / iCount : 0);
								// int valR2((255 - qGray(cImg.pixel(x,y))) / iCount);
								int valRef(qGray(ref.pixel(refX, refY)));
								int newVal(qMax(0, valRef - valR2));
								int newColor(QColor(newVal, newVal, newVal, 255).rgba());
								ref.setPixel(refX, refY, newColor);
							}
						}

					}
				}
//				ref.save(QString("ref_%1.png").arg(c.unicode()));
			//			else
			//			{
			//				for(int j(i), nSamples(0); j < result && nSamples < 256; ++j)
			//				{
			//					QChar c2(tc.at(j));
			//					if(c == c2)
			//					{
			//						++nSamples;
			//						QPoint tl2(x0[j], iHeight - y1[j]);
			//						QPoint br2(y0[j],  iHeight - x1[j]);
			//						QRect r2(tl2, br2);
			//						// center extracted image on ref;
			//						int remainX((rwidth - r2.width()) / 2);
			//						int remainY((rheight - r2.height()) /*/ 2*/);
			//						//					r2.translate(remainX / 2, remainY / 2);
			//						for(int y(r2.y()); y < r2.y() + r2.height(); ++y)
			//						{
			//							for(int x(r2.x()); x < r2.x() + r2.width(); ++x)
			//							{
			//								int refX(x - r2.left() + remainX);
			//								int refY(y - r2.top() + remainY);
			//								int valR2((255 - qGray(m_img.pixel(x,y))) / iCount);
			//								int valRef(qGray(ref.pixel(refX, refY)));
			//								int newVal(qMax(0, valRef - valR2));
			//								int newColor(QColor(newVal, newVal, newVal, 255).rgba());
			//								if(firstDbg)
			//									qDebug()<<refX<<refY<<newColor;
			//								ref.setPixel(refX, refY, newColor);
			//							}
			//						}
			//						firstDbg = false;

			//					}
			//				}
			//			}

			double adjustedBaseline(0);
			foreach(int b, baselines)
				adjustedBaseline += b;
			adjustedBaseline /= double(baselines.count());

			double adjustedXheight(0);
			foreach(double d, xheights)
				adjustedXheight += d;
			adjustedXheight /= double(xheights.count());

			xheightDict.insert(c,adjustedXheight);
			iDict.insert(c, ref);
			offsetDict.insert(c, qRound(adjustedBaseline));
		}
		//		lists[QChar(text[i])].append(m_img.copy(r));
	}
	//	foreach(const QChar& c, lists.keys())
	//	{
	//		QString fname(QString("%1.png").arg(c.unicode()));
	////		bool is = lists.value(c).first().save(fname.toLocal8Bit());
	////		qDebug()<<"Save:"<<fname ;

	//		QSize maxSize;
	//		foreach(const QImage& im, lists.value(c))
	//		{
	//			maxSize = QSize(qMax(im.size().width(), maxSize.width()),
	//				qMax(im.size().height(), maxSize.height()));
	//		}

	//		QImage t(maxSize, QImage::Format_ARGB32);
	//		t.fill(0xFFFFFFFF);
	//		int icount(lists.value(c).count());
	//		foreach(const QImage& im, lists.value(c))
	//		{
	//			QImage ref = im.scaled(maxSize);
	//			for(int y(0); y < maxSize.height(); ++y)
	//			{
	//				for(int x(0); x <maxSize.width(); ++x)
	//				{
	//					int valRef((255 - qGray(ref.pixel(x,y))) / icount);
	//					int valT(qGray(t.pixel(x,y)));
	//					int newVal(valT - valRef);
	//					int newColor(QColor(newVal, newVal, newVal, 255).rgba());
	////					qDebug()<<valRef<<valT<<newVal<<newColor;
	//					t.setPixel(x,y, newColor);
	//				}
	//			}

	//		}
	//		iDict.insert(c, t);
	//		t.save(QString("%1.png").arg(c.unicode()));

	//	}



}
