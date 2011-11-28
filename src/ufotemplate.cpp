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

#include "ufotemplate.h"

#include <QDate>

UFOTemplate::UFOTemplate(const QString &name, const QString &dirPath)
{
	fName = name;
	QString ufoDirName(name.split(" ").first() + ".ufo");
	fPath = dirPath + QDir::separator() + ufoDirName;

	if(fDir.mkpath(fPath) && fDir.cd(fPath))
	{
		makeUFO();
	}
}


void UFOTemplate::makeUFO()
{
	QString features("table GDEF {} GDEF;");
	QString fontinfo("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n\
<plist version=\"1.0\">\n<dict>\n\
	<key>familyName</key> <string>%1</string>\n \
	<key>styleName</key> <string>Medium</string>\n \
	<key>copyright</key> <string>Created by Fonzie</string>\n \
	<key>unitsPerEm</key> <integer>1000</integer>\n \
	<key>ascender</key> <integer>800</integer>\n \
	<key>descender</key> <integer>-200</integer>\n \
	<key>italicAngle</key> <real>0</real>\n \
	<key>note</key> <string>%2: Created.</string>\n \
	<key>openTypeHeadCreated</key> <string>%3</string>\n \
	<key>openTypeNameVersion</key> <string>Version 001.000</string>\n \
	<key>postscriptFontName</key> <string>%4</string>\n \
	<key>postscriptFullName</key> <string>%5</string>\n \
	<key>postscriptWeightName</key> <string>Medium</string>\n \
	<key>postscriptUnderlineThickness</key> <integer>50</integer>\n \
	<key>postscriptUnderlinePosition</key> <integer>-100</integer>\n \
	</dict>\n \
</plist>\n");
	QString groups("<!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"> <plist version=\"1.0\"> <dict> </dict> </plist>");
	QString kerning(groups);
	QString metainfo("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"> <plist version=\"1.0\"> <dict> <key>creator</key> <string>org.constantvzw.OSP.Fonzie</string> <key>formatVersion</key> <integer>2</integer> </dict> </plist>");

	QString content("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <!DOCTYPE plist PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"> <plist version=\"1.0\"> <dict> <key>space</key> <string>space.glif</string> </dict> </plist>");
	QString space("<?xml version=\"1.0\" encoding=\"UTF-8\"?> <glyph name=\"space\" format=\"1\"> <advance width=\"240\"/> <unicode hex=\"0020\"/> </glyph>");


	QFile fFeature(fDir.absoluteFilePath("features.fea"));
	if(fFeature.open(QIODevice::WriteOnly))
	{
		fFeature.write(features.toUtf8());
		fFeature.close();
	}
	QFile ffontinfo(fDir.absoluteFilePath("fontinfo.plist"));
	if(ffontinfo.open(QIODevice::WriteOnly))
	{
		ffontinfo.write(fontinfo
				.arg(fName) // familyName
				.arg(QDate::currentDate().toString()) // note
				.arg(QDate::currentDate().toString(Qt::ISODate)) // openTypeHeadCreated
				.arg(fName.replace(" ", "")) // postscriptFontName
				.arg(fName) // postscriptFullName
				.toUtf8());
		ffontinfo.close();
	}
	QFile fgroups(fDir.absoluteFilePath("groups.plist"));
	if(fgroups.open(QIODevice::WriteOnly))
	{
		fgroups.write(groups.toUtf8());
		fgroups.close();
	}
	QFile fkerning(fDir.absoluteFilePath("kerning.plist"));
	if(fkerning.open(QIODevice::WriteOnly))
	{
		fkerning.write(kerning.toUtf8());
		fkerning.close();
	}
	QFile fmetainfo(fDir.absoluteFilePath("metainfo.plist"));
	if(fmetainfo.open(QIODevice::WriteOnly))
	{
		fmetainfo.write(metainfo.toUtf8());
		fmetainfo.close();
	}


	if(fDir.mkdir("glyphs") && fDir.cd("glyphs"))
	{
		QFile fcontent(fDir.absoluteFilePath("contents.plist"));
		if(fcontent.open(QIODevice::WriteOnly))
		{
			fcontent.write(content.toUtf8());
			fcontent.close();
		}
		QFile fspace(fDir.absoluteFilePath("space.glif"));
		if(fspace.open(QIODevice::WriteOnly))
		{
			fspace.write(space.toUtf8());
			fspace.close();
		}

	}
}


QString UFOTemplate::path() const
{
	return fPath;
}
