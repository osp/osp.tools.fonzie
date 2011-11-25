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

#include "fontupdate.h"
#include "glyphtrace.h"
#include <QFile>
#include <QTextStream>
#include <QDomDocument>
#include <QProcess>
#include <QStringList>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#define WITH_FONTFORGE
#ifdef WITH_FONTFORGE
#include "ff_wrapper.h"
#endif

FontUpdate::FontUpdate( const QString& wc)
	: workingcopy(wc)
{

}

void FontUpdate::Do(GlyphTrace* gt)
{
	if(!gt)
		return;
	glyphtrace = gt;
	QString filename(workingcopy + "/glyphs/" + glyphtrace->GetName() +".glif");
	QString header("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n <glyph name=\""
		       + glyphtrace->GetName()
		       + "\" format=\"1\">\n <advance width=\""
		       + QString::number(glyphtrace->GetAdvance())
		       + "\"/> \n<unicode hex=\""
		       + (QString("%1").arg(glyphtrace->GetHexcode(),4,'0'))
		       + "\"/>\n");

	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file);
		out << header;
		out << glyphtrace->GetOutline();
		out << QString("</glyph>");
		file.close();
	}

	QFile domfile(workingcopy + "/glyphs/" + "contents.plist");
	QDomDocument dom("plist");
	if (domfile.open(QIODevice::ReadWrite | QIODevice::Text))
	{
		dom.setContent(&domfile);
		domfile.resize(0);
//		domfile.close();
		QDomNode dict(dom.elementsByTagName("dict").at(0));
		QDomText keytext(dom.createTextNode(glyphtrace->GetName()));
		QDomElement key(dom.createElement("key"));
		QDomText gliffiletext(dom.createTextNode(glyphtrace->GetName()+".glif"));
		QDomElement gliffile(dom.createElement("string"));
		key.appendChild(keytext);
		gliffile.appendChild(gliffiletext);
		dict.appendChild(key);
		dict.appendChild(gliffile);

		QTextStream xml(&domfile);
		xml << dom.toString();
		domfile.close();
	}
}

void FontUpdate::ClearWC()
{

	QFileInfo fd(workingcopy);
	if(fd.exists())
	{
		QStringList ll(fd.absoluteFilePath().split("/"));
		ll.removeLast();
		QDir p(QString("/%1").arg(ll.join("/")));
		QDir d(workingcopy);
		QDir g(d.absoluteFilePath("glyphs"));
		foreach(QFileInfo gn, g.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
		{
			if(gn.isFile())
				g.remove(gn.fileName());
			if(gn.isDir())
				g.rmdir(gn.fileName());
		}

		foreach(QFileInfo dn, d.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
		{
			if(dn.isFile())
				d.remove(dn.fileName());
			if(dn.isDir())
				d.rmdir(dn.fileName());
		}
		p.rmdir(fd.fileName());
	}
}

QString FontUpdate::Commit()
{
#ifdef WITH_FONTFORGE

	FF_Struct * ff(newFF());
	QFile domfile(workingcopy + "/fontinfo.plist");
	QDomDocument dom;
	QString fname;
	if (domfile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		dom.setContent(&domfile);
		QDomNode dict(dom.elementsByTagName("dict").at(0));
		QString key("key");
		QString fontname("familyName");
		QDomNodeList nl(dict.childNodes());
		for(int i(0); i < nl.count(); ++i)
		{
			QDomNode n(nl.item(i));
			if(n.toElement().tagName() == key)
			{
				if(n.toElement().text() == fontname)
				{
					QDomNode v(n.nextSibling());
					fname = v.toElement().text();
					break;
				}
			}
		}
	}
	QString OTFName("%1.otf");
	ff->generate(workingcopy.toUtf8().data(), OTFName.arg(fname).toUtf8().data());
	QFileInfo ret( OTFName.arg(fname));
	return ret.absoluteFilePath();

#endif
//	QString glifname("uni" + hexcode);
//	QString filename(glifname +".glif");
//	QString filepath("glyphs/"+ filename);
//	QProcess * p = new QProcess;
//	p->setWorkingDirectory(workingcopy);
//	QString svn = "svn";
//	QStringList options;
//	qDebug()<< "SVN ADD"<<filepath;
//	options << "add" << filepath;
//	p->start(svn,options);
//	p->waitForFinished();
//
//	options.clear();
//	options << "--non-interactive" << "--username" << "pierremarc" << "--password"<< "plokplok";
//	options << "ci" << "-m" << QString("Glyph %1 Added at code point %2").arg(glifname).arg(hexcode);
//	qDebug()<< "SVN COMMIT";
////	p->setWorkingDirectory(workingcopy);
//	p->start(svn, options);
//	p->waitForFinished();
//	delete p;
}

