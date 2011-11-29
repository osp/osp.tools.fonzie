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

//#define WITH_TESSERACT
#include <QCoreApplication>

#include "scanimage.h"
#include "glyphtrace.h"
#include "fontupdate.h"
#include "options.h"
#include "gridspec.h"
#include "names.h"
#include "magicmetrics.h"
#include "ufotemplate.h"
#include "document.h"
#include "composer.h"


//#define WITH_TESSERACT
#ifdef WITH_TESSERACT
#include "ocr.h"
#endif

#include <QStringList>
#include <QDebug>
#include <QMap>
#include <QFile>
#include <QDir>

//#define WITH_CAPTURE
#ifdef WITH_CAPTURE
#include "orm.h"
#endif

//#define WITH_BOOK_SCANNER
#ifdef WITH_BOOK_SCANNER
#include "bookscanner.h"
#endif


int main(int ac, char ** av)
{
	QCoreApplication app(ac, av);
#ifdef WITH_TESSERACT
	if(app.argc() > 1)
	{
		Options opt(app.arguments());
		opt.Dump();

		GlyphTrace::Options ptOpt;
		if(opt.contains(OPTPT_ALPHAMAX))
			ptOpt.alphamax = opt.toDouble(OPTPT_ALPHAMAX);
		if(opt.contains(OPTPT_TURDSIZE))
			ptOpt.turdsize = opt.toDouble(OPTPT_TURDSIZE);
		if(opt.contains(OPTPT_OPTICURVE))
			ptOpt.opticurve = opt.toDouble(OPTPT_OPTICURVE);
		if(opt.contains(OPTPT_OPTTOLERANCE))
			ptOpt.opttolerance = opt.toDouble(OPTPT_OPTTOLERANCE);

		double scale(opt.toDouble(OPTSCALE));
		if(scale == 0.0)
			scale = 1.0;
		//	double offset(opt.toDouble(OPTOFFSET));
		//	int baseline(opt.toInt(OPTBASELINE));
		QList<GlyphTrace*> allGlyphs;

		int iCount(opt.Count(OPTIMAGE));
#ifdef WITH_CAPTURE
		ORM * orm;
		bool hasORM(false);
		if(opt.contains(OPTCAPTURE))
		{
			iCount = opt.toInt(OPTCAPTURE);
			orm = new ORM(opt.value(OPTCAMLEFT), opt.value(OPTCAMRIGHT));
			hasORM = true;
		}
		ORM::CameraSelect camsel(ORM::Left);
#endif

		qDebug()<<"Images Count: "<<iCount;
		QList<QChar> processed;
#ifdef WITH_BOOK_SCANNER
		BookScanner * bs = 0;
		if(opt.contains(OPTARDUINOSERIAL))
		{
			int asbr = opt.contains(OPTARDUINOSERIALBAUDRATE) ? opt.toInt(OPTARDUINOSERIALBAUDRATE) : 0;
			bs = new BookScanner(opt.value(OPTARDUINOSERIAL), asbr);
		}
#endif

		for(int ic(0); ic < iCount; ++ic)
		{
			Composer::NewPage();
#ifdef WITH_BOOK_SCANNER
			if(bs)
				bs->TurnPage();
#endif
#ifdef WITH_CAPTURE
			QImage image;
			if(hasORM)
			{
				qDebug()<<"CAPTURE FROM" << (camsel == ORM::Left ? "LEFT" : "RIGHT");
				image = orm->TakePicture(camsel);
				if(camsel == ORM::Left)
					camsel = ORM::Right;
				else
					camsel = ORM::Left;
			}
			else
				image = QImage(opt.toString(QString(OPTIMAGE), ic));

			qDebug()<<image.size();
			image.save(QString("capture_%1.png").arg(ic));
#else
			QImage image(opt.toString(QString(OPTIMAGE), ic));
#endif
			OCR ocr(opt.value(OPTTESSDATA));
			bool hasXHeight(false);
			double optXHeight;
			if(opt.contains(OPTXHEIGHT))
			{
				optXHeight = opt.toDouble(OPTXHEIGHT);
				hasXHeight = true;
			}
			ocr.loadImage(image);
			qDebug()<<"Image loaded";
			ocr.saveImages();
			foreach(const QChar& c, ocr.iDict.keys())
			{
				if(processed.contains(c)
					||!c.isPrint()
					|| c.category() == QChar::Separator_Line
					|| c.category() == QChar::Separator_Paragraph
					|| ocr.iDict.value(c).isNull())
					continue;
				processed << c;
				unsigned int charcode(c.unicode());
				bool isSpace(QChar::category(charcode) == QChar::Separator_Space);
				QString hexcode(QString::number(charcode ,16).toUpper());
				QString glifname(QString("uni%1").arg(hexcode,4,'0'));

				//ocr.iDict.value(c).save(QString("%1.png").arg(glifname));

				GlyphTrace* gt = new GlyphTrace(ocr.iDict.value(c), ptOpt);
				if(hasXHeight)
				{
					scale = optXHeight / ocr.xheightDict.value(c);
				}
				gt->SetScale(scale);
				gt->SetTy(ocr.offsetDict.value(c) * scale);
				gt->SetName(glifname);
				gt->SetCharcode(charcode);
				gt->SetHexcode(hexcode);
				qDebug()<<"Tracing"<<c<<glifname<<scale;

				bool hasPath(!gt->GetPath().isEmpty());
				if((hasPath) || (isSpace))
				{
					allGlyphs.append(gt);
				}
				else
				{
					ocr.iDict.value(c).save(QString("oops_%1.png").arg(glifname));
				}
			}
		}

		QString ref;
		if(opt.contains(OPTFONTREF))
			ref = opt.value(OPTFONTREF);
		MagicMetrics * mm;
		if(opt.contains(OPTFONTREFSTRICT))
			mm = new MagicMetrics(allGlyphs, ref, opt.toInt(OPTFONTREFSTRICT) > 0 ? true : false);
		else
			mm = new MagicMetrics(allGlyphs, ref);

		FontUpdate * fu;
		if(opt.contains("wc"))
		{
			fu = new FontUpdate(opt.value("wc"));
			fu->ClearWC();

		}
		else
		{
			QString fontDir(".");
			QString fontName("NN_");
			if(opt.contains(OPTDIRECTORY))
				fontDir = opt.value(OPTDIRECTORY);
			if(opt.contains(OPTFONTNAME))
				fontName = opt.value(OPTFONTNAME);
			UFOTemplate ut(fontName, fontDir);
			fu = new FontUpdate(ut.path());
		}
		foreach( GlyphTrace * gt, allGlyphs)
		{
			gt->Glyph();
			fu->Do(gt);

		}
		if(opt.contains(OPTGENERATEOTF) && opt.toBool(OPTGENERATEOTF))
			fu->Commit();
		else if(opt.contains(OPTPDF) )
		{
			QImage image(opt.toString(QString(OPTIMAGE)));
			QString theFont = fu->Commit();
			Document doc(theFont);
			doc.write(opt.value(OPTPDF), image.width(),image.height());

		}
		if(!opt.contains(OPTGENERATEUFO) || !opt.toBool(OPTGENERATEUFO))
			fu->ClearWC();
		delete fu;
		delete mm;
#ifdef WITH_CAPTURE
		if(hasORM)
			delete orm;
#endif
#ifdef WITH_BOOK_SCANNER
		if(bs)
			delete bs;
#endif
	}
#else
	if(app.argc() > 1)
	{
		Options opt(app.arguments());
		QString suffix;
		QString cspace(" ");
		QString cescape("\\");
		QList<QStringList> scans;
		QStringList basescans(opt.toString(QString(OPTIMAGE), 0).split(","));
		scans.append(basescans);
		QString supstr(QString(OPTIMAGE) + QString("%1"));
		for(int s(1); s < 10000; ++s)
		{
			if(opt.contains(supstr.arg(s)))
			{
				scans.append(opt.value(supstr.arg(s)).split(","));
			}
		}

		GlyphTrace::Options ptOpt;
		if(opt.contains(OPTPT_ALPHAMAX))
			ptOpt.alphamax = opt.toDouble(OPTPT_ALPHAMAX);
		if(opt.contains(OPTPT_TURDSIZE))
			ptOpt.turdsize = opt.toDouble(OPTPT_TURDSIZE);
		if(opt.contains(OPTPT_OPTICURVE))
			ptOpt.opticurve = opt.toDouble(OPTPT_OPTICURVE);
		if(opt.contains(OPTPT_OPTTOLERANCE))
			ptOpt.opttolerance = opt.toDouble(OPTPT_OPTTOLERANCE);

		QMap<int ,GlyphTrace*> baseglyphs;
		QList<GlyphTrace*> allGlyphs;
		bool base(true);

		int PUA = 0xe000;

		QString saltfeature("feature calt{\n");
		int virtualW(0);
		int virtualH(0);
		for(int scancount(0); scancount < scans[0].count(); ++scancount)
		{
			if(scancount > 0)
				base = false;
			QString featureset(QString("@%1_set = [").arg(base ? "base" : QString("var%1").arg(scancount)));
			for(int range(0); range < scans.count(); ++range)
			{
				if(scans[range].count() > scancount)
				{
					double scale(opt.toDouble(OPTSCALE, range));
					double offset(opt.toDouble(OPTOFFSET, range));
					double compressFactor(opt.toDouble(OPTCOMPRESS, range));
					double crop(opt.toDouble(OPTCROP, range));

					qDebug()<<"Processing"<<scans[range][scancount]<<"at scale"<<scale<< range << scancount;

					if(scale == 0.0)
						scale = 1.0;
					if(compressFactor == 0.0)
						compressFactor = 3.33;

					GridSpec spec = opt.toSpec(OPTSPEC, range);
					int d(spec.discard);
					int cw(spec.cellWidth);
					int ch(spec.cellHeight - d);


					if(!base)
						suffix = QString(".var%1").arg(scancount);
					else
						suffix = QString();

					QImage di(scans[range][scancount]);
					if(base)
					{
						virtualW = di.width();
						virtualH = di.height();
					}
					unsigned int charcode(opt.toInt(OPTCHARCODE , range));
					int BaseCharcode = charcode;
					if(!base)
						charcode = PUA;

					QString debugStr;
					int maxY(di.height() - (spec.marginBottom + ch) );
					int maxX(di.width() - (spec.marginRight + cw) );
					for( int x(spec.marginLeft);
					x < maxX;
					x += cw)
					{
						for(int y(spec.marginTop);
						y < maxY ;
						y += ch + d )
						{
							bool isSpace(QChar::category(charcode) == QChar::Separator_Space);
							debugStr += QString(QChar(charcode)) + QString(" ");
							QString hexcode(QString::number(charcode ,16).toUpper());
							QString glifname(QString("uni%1").arg(hexcode,4,'0') + suffix);
							QImage tmpImg(di.copy(x + crop, y + crop, cw - (2*crop), ch - (2*crop)));
							//qDebug()<<"Cropping"<<x + crop<<y + crop<<cw - (2*crop)<<ch - (2*crop);

							GlyphTrace* gt = new GlyphTrace(tmpImg, ptOpt);
							gt->SetScale(scale);
							gt->SetTy(offset);
							gt->SetName(glifname);
							gt->SetCharcode(charcode);
							gt->SetHexcode(hexcode);
							bool hasPath(!gt->GetPath().isEmpty());
							if(base)
							{
//								qDebug()<<hexcode<<glifname<<hasPath<<x <<y;
								if((hasPath) || (isSpace))
								{
									if(isSpace) qDebug()<<"Inserting space"<<hexcode<<glifname<<hasPath<<QChar::category(charcode);
									baseglyphs.insert(charcode,gt);
									featureset.append(cescape + glifname + cspace);
									allGlyphs.append(gt);
								}
								else
								{
									tmpImg.save(QString("oops_%1.png").arg(glifname));
//									qDebug()<<"OOOPS (A)";
								}
							}
							else
							{
								if((hasPath) || (isSpace))
								{
									featureset.append(cescape + glifname + cspace);
									allGlyphs.append(gt);
								}
								else if(baseglyphs.contains(BaseCharcode))
								{
									featureset.append(cescape + glifname + cspace);
									GlyphTrace* bc = baseglyphs[BaseCharcode]->Clone();
									bc->SetScale(scale);
									bc->SetTy(offset);
									bc->SetName(glifname);
									bc->SetHexcode(hexcode);
									bc->SetCharcode(BaseCharcode);
									allGlyphs.append(bc);
								}
								else
								{
									qDebug()<<"OOOPS (B)";
								}
							}
							++charcode;
							++BaseCharcode;
							if(!base)
								++PUA;
						}

					}
					qDebug()<<range << debugStr;
				}
				else if(range > 0)
				{
					qDebug()<<"???";
					int BaseCharcode(opt.toInt(OPTCHARCODE , range));
					int charcode(PUA);
					GridSpec spec = opt.toSpec(OPTSPEC, range);
					int d(spec.discard);
					int cw(spec.cellWidth);
					int ch(spec.cellHeight - d);
					int ph = qRound(cw / 20.0);
					int pv = qRound(ch / 20.0);
					for( int x(spec.marginLeft);
					x < (virtualW - (spec.marginRight) );
					x += cw)
					{

						for(int y(spec.marginTop);
						y < (virtualH - (spec.marginBottom) ) ;
						y += ch + d )
						{
							QString hexcode(QString::number(charcode, 16).toUpper());
							QString glifname(QString("uni%1").arg(hexcode,4,'0') + suffix);
							if(baseglyphs.contains(BaseCharcode))
							{
								featureset.append(cescape + glifname + cspace);
								GlyphTrace* bc = baseglyphs[BaseCharcode]->Clone();
								bc->SetName(glifname);
								bc->SetHexcode(hexcode);
								bc->SetCharcode(BaseCharcode);
								allGlyphs.append(bc);
							}
							++charcode;
							++BaseCharcode;
							++PUA;
						}

					}

				}
			}
			featureset += QString("];\n\n");
			saltfeature += featureset;
			//			qDebug()<< featureset;
		}

//		QFile featurefile(opt.value("wc")+QString(QDir::separator())+QString("feature.fea"));
//		bool ffIsOpen(featurefile.open(QIODevice::WriteOnly|QIODevice::Truncate));
//		if(scans[0].count() > 1)
//		{
//			for(int scancount(0); scancount < (scans[0].count()-1); ++scancount)
//			{
//				if(scancount == 0)
//					saltfeature += QString("sub @base_set @base_set' by @var1_set;\n");
//				else
//					saltfeature += QString("sub @var%1_set @base_set' by @var%2_set;\n")
//						       .arg(scancount)
//						       .arg(scancount + 1);
//			}
//			saltfeature += QString("} calt;\n");

//			if(ffIsOpen)
//			{
//				featurefile.write(saltfeature.toUtf8());
//			}
//			else
//			{
//				qDebug()<<"ERROR: Could not write salt feature in "<< (opt.value("wc")+QString(QDir::separator())+QString("feature.fea"));
//			}
//		}

		// Here we go metrics
		QString ref;
		if(opt.contains(OPTFONTREF))
			ref = opt.value(OPTFONTREF);
		MagicMetrics * mm;
		if(opt.contains(OPTFONTREFSTRICT))
			mm = new MagicMetrics(allGlyphs, ref, opt.toInt(OPTFONTREFSTRICT) > 0 ? true : false);
		else
			mm = new MagicMetrics(allGlyphs, ref);
//		if(ffIsOpen)
//		{
//			featurefile.write(mm->gposfeature.toUtf8());
//		}
//		else
//		{
//			qDebug()<<"ERROR: Could not write gpos feature in "<< (opt.value("wc")+QString(QDir::separator())+QString("feature.fea"));
//		}

		FontUpdate * fu;
		if(opt.contains("wc"))
		{
			fu = new FontUpdate(opt.value("wc"));

		}
		else
		{
			QString fontDir(".");
			QString fontName("NN_");
			if(opt.contains(OPTDIRECTORY))
				fontDir = opt.value(OPTDIRECTORY);
			if(opt.contains(OPTFONTNAME))
				fontName = opt.value(OPTFONTNAME);
			UFOTemplate ut(fontName, fontDir);
			fu = new FontUpdate(ut.path());
		}
		foreach( GlyphTrace * gt, allGlyphs)
		{
			gt->Glyph();
			fu->Do(gt);

		}
		if(opt.contains(OPTGENERATEOTF) && opt.toBool(OPTGENERATEOTF))
			fu->Commit();
		if(!opt.contains(OPTGENERATEUFO) || !opt.toBool(OPTGENERATEUFO))
			fu->ClearWC();
		delete fu;
		delete mm;
//		qDebug()<<saltfeature;

	}
#endif

//		app.exec();
}
