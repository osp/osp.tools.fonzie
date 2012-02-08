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


#include <QDebug>
#include <QByteArray>
#include <QList>
#include <QFile>
#include "document.h"
#include "composer.h"

#include "ff_wrapper.h"

#include "podofo/podofo.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H

/// We got a little problem with file path in podofo metrics constructor
/// and because we do not "install" the font, we need to embbed the font, hence the need for a correct file path

class FonzieMetrics : public PoDoFo::PdfFontMetricsFreetype
{
public:
	FonzieMetrics(FT_Library pLibrary, FT_Face face, const QString& fn):
		PoDoFo::PdfFontMetricsFreetype(&pLibrary, face)
	{
		m_sFilename = std::string(fn.toUtf8());
	}
};


/// A more reasonnable CID font

class FonzieCID : public PoDoFo::PdfFontCID
{
	FT_Face m_face;
public:
	FonzieCID(PoDoFo::PdfFontMetrics* pMetrics,
		  const PoDoFo::PdfEncoding* const pEncoding,
		  PoDoFo::PdfVecObjects* pParent,
		  bool bEmbed,
		  FT_Face face):
		PoDoFo::PdfFontCID(pMetrics, pEncoding, pParent,bEmbed),
		m_face(face)
	{
		PoDoFo::PdfArray  descFonts = this->GetObject()->GetDictionary().GetKey(PoDoFo::PdfName("DescendantFonts"))->GetArray();
		PoDoFo::PdfObject * dfont( &(*(descFonts.begin())) );
		while(dfont->IsReference())
		{
			dfont = pParent->GetObject(dfont->GetReference());
		}

		QByteArray ba(128 *1024, 0);
		FT_UInt index (1);
		FT_UInt cc =  FT_Get_First_Char ( m_face, &index );
		while ( index )
		{
			quint16 gid(index);
			ba[cc * 2] = gid >> 8;
			ba[(cc*2) + 1] = gid & 0xFF;
			qDebug()<<cc<<gid;
			cc = FT_Get_Next_Char ( m_face, cc, &index );
		}

		PoDoFo::PdfDictionary dict;
		PoDoFo::PdfObject * stream(pParent->CreateObject(dict));
		qDebug()<<"stream->stream"<<stream->GetStream();
		stream->GetStream()->Set(ba.constData(),ba.length());
		std::string rsData;
		dfont->ToString(rsData);
		qDebug()<<rsData.c_str();
		// to be honest, I don't get it. i just suppose PDF readers don't read this and directly read the TTF cmap
		// Which means I'm not finished with fixing the CIDToGIDMap
		dfont->GetDictionary().AddKey("CIDToGIDMap", PoDoFo::PdfName("Identity"));
//		dfont->GetDictionary().AddKey("CIDToGIDMap", stream->Reference());

	}
};

Document::Document(const QString& ff):
	fontfile(ff)
{
}


void Document::write(const QString &output, double width, double height)
{

	const QList<Composer::PosList>& pl(Composer::GetList());

	unsigned int startC = 0xFFFF;
	unsigned int endC = 0;
	foreach(const Composer::PosList& l, pl)
	{
		foreach(const Composer::Pos& p, l)
		{
			QChar c(p.c);
			startC = qMin(uint(c.unicode()), startC);
			endC = qMax(uint(c.unicode()), endC);
		}
	}

	PoDoFo::PdfMemDocument *targetDoc = new PoDoFo::PdfMemDocument;

	FT_Face face;
	FT_Error      ft_error;
	FT_Library ftlib = targetDoc->GetFontLibrary();
	//const char * fn (fontfile.toUtf8().constData());
	qDebug()<<fontfile<< QFile::exists(fontfile);

	ft_error = FT_New_Face(ftlib, fontfile.toUtf8().constData() , 0, &face);
	if ( ft_error )
	{
		//		for(int i(0); i <100; i++)
		//		{
		//			ft_error = FT_New_Face(ftlib, fn, 0, &face);
		//			qDebug()<<"FTERROR"<<ft_error;
		//			if(ft_error == 0)
		//				break;
		//		}
		//		if(ft_error)
		qDebug() << "Error loading face [" << fontfile <<"]"<<ft_error;
	}
	PoDoFo::PdfEncoding * enc(new PoDoFo::PdfIdentityEncoding(startC,endC));
	//	Doesnt work for opentype fonts
	//	PoDoFo::PdfFont *  font(targetDoc->CreateFont("Fonzie", false, false, enc, PoDoFo::PdfFontCache::eFontCreationFlags_AutoSelectBase14 , true, fontfile.toUtf8().constData()));
	PoDoFo::PdfFontMetrics * metrics(new FonzieMetrics(targetDoc->GetFontLibrary(), face, fontfile));
	qDebug()<<"FILE NAME:"<< metrics->GetFilename();
	//	PoDoFo::PdfFont * font(PoDoFo::PdfFontFactory::CreateFontObject(metrics, PoDoFo::ePdfFont_Embedded, enc, &(targetDoc->GetObjects())));

	PoDoFo::PdfFont * font(new FonzieCID(metrics, enc, &(targetDoc->GetObjects()), true, face));
	PoDoFo::PdfPainter *painter(new PoDoFo::PdfPainter);


	//	painter->SetTransformationMatrix(1,0,0,-1,0,-height);
	foreach(const Composer::PosList& l, pl)
	{
		PoDoFo::PdfPage * newpage(targetDoc->CreatePage ( PoDoFo::PdfRect ( 0.0, 0.0, width, height ) ));
		painter->SetPage(newpage);
		painter->SetFont(font);
		unsigned int line(0);
		QPointF pos;
		QString str;
		foreach(const Composer::Pos& p, l)
		{
			//		qDebug( )<<p.c<<p.p;
			if(p.l != line)
			{
				if(!str.isEmpty())
				{
					PoDoFo::pdf_utf8* us = reinterpret_cast<PoDoFo::pdf_utf8*>( str.toUtf8().data() );
					PoDoFo::PdfString s(us);
					painter->DrawText(pos.x(),height - pos.y(),s);
					qDebug( )<<str<<pos.x() << height - pos.y();
					str.clear();
				}
				font->SetFontSize(p.sh);
				if(!p.c.isSpace())
				{
					pos = p.p;
					line = p.l;
				}
			}
			//		else
			{
				str.append(p.c);
			}
		}
		if(!str.isEmpty())
		{
			PoDoFo::pdf_utf8* us = reinterpret_cast<PoDoFo::pdf_utf8*>( str.toUtf8().data() );
			PoDoFo::PdfString s(us);
			painter->DrawText(pos.x(),height - pos.y(),s);
			str.clear();
		}

		painter->FinishPage();
	}
	delete painter;

	targetDoc->SetWriteMode(PoDoFo::ePdfWriteMode_Clean);
	targetDoc->Write ( output.toUtf8().constData() );

	delete targetDoc;
}
