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

#ifndef SCANIMAGE_H
#define SCANIMAGE_H

#include <QThread>
#include <QImage>
#include <QBuffer>

#include "sane/sane.h"

class ScanImage : public QThread
{
	SANE_Int version_code;
	SANE_Status sane_status;
	SANE_Parameters params;
	QImage image;
	SANE_Byte *buffer;
	QBuffer ibuffer;

	bool ScanError(const QString& errorString);

public:
	ScanImage();
	ScanImage(const QImage& img);
	~ScanImage();

	QImage GetImage(){return image;}
	void run();
};

#endif // SCANIMAGE_H
