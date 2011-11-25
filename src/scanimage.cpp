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

#include "scanimage.h"

#include <QString>
//#include <iostream>
#include <QDebug>
#include <QColor>

ScanImage::ScanImage()
{
	sane_status = sane_init(&version_code, NULL);
}

ScanImage::ScanImage(const QImage& img)
	:image(img)
{

}

ScanImage::~ScanImage()
{
	sane_exit();
}

void ScanImage::run()
{
	if(!image.isNull())
		return;
	// TODO
	if(ScanError("Sane can't init"))
		return;
	const SANE_Device ** dl;
	sane_status = sane_get_devices(&dl,true);
	if((ScanError("Sane can't get any device")) || (0 == dl[0]))
		return;
	SANE_Handle  h;
	sane_status = sane_open(dl[0]->name, &h);
	if(ScanError("Sane can't open device"))
		return;

	sane_status = sane_get_parameters(h, &params);
	if(ScanError("Sane can't get parameters"))
		return;


	QString bMode("mode");
//	unsigned int bModeOptionIdx(0);
	unsigned int bModeOptionIdx(1);
	QString binary;
	unsigned int bModeIndex(0);

	SANE_Int numOptions = 0;
	void * optionValue = malloc(sizeof(SANE_Int));

	sane_status = sane_control_option(h, 0, SANE_ACTION_GET_VALUE, optionValue, 0);
	if(ScanError("Sane can't get num options"))
		return;

	numOptions = *(static_cast<SANE_Int*>(optionValue));
	free(optionValue);

	for (unsigned int i = 1; i < numOptions; ++i)
	{
		const SANE_Option_Descriptor *opt = sane_get_option_descriptor(h, i);
		optionValue = malloc(opt->size + 1);
		if(opt->type != SANE_TYPE_GROUP)
		{
			QString name(opt->name);
			QString title(opt->title);
			qDebug()<<"#"<<name <<"\t\t"<<title;
			if(opt->constraint_type == SANE_CONSTRAINT_STRING_LIST)
			{
				for(int si=0; opt->constraint.string_list[si]!=0;  si++)
				{
					qDebug()<<"\t\t*"<<QString(opt->constraint.string_list[si]);
				}
				if(name == bMode)
				{
					bModeIndex = i;
					binary =  opt->constraint.string_list[bModeOptionIdx];
				}
			}
			else if(opt->constraint_type == SANE_CONSTRAINT_WORD_LIST)
			{
				for (int j=1; j <= opt->constraint.word_list[0]; j++)
				{
					qDebug()<<"\t\t*"<< (opt->constraint.word_list[j]);
				}

			}
			else if(opt->constraint_type == SANE_CONSTRAINT_RANGE)
			{
				qDebug()<<"\t\t*"<< opt->constraint.range->min
						<< opt->constraint.range->max
						<< opt->constraint.range->quant;

			}
		}
		free(optionValue);
	}

	//	return;

	qDebug()<<"Index"<<bModeIndex<<"BinaryOption"<< binary;
	const SANE_Option_Descriptor *opt_dbg = sane_get_option_descriptor(h, bModeIndex);
	qDebug()<<opt_dbg->name<<opt_dbg->title;
	sane_status =  sane_control_option(h,bModeIndex,SANE_ACTION_SET_VALUE, binary.toAscii().data() ,0);
	if(ScanError("Sane can't set option"))
		return;

	sane_status = sane_start(h);
	if(ScanError("Sane can't start"))
		return;

	sane_status = sane_get_parameters(h, &params);
	int W = params.pixels_per_line;
	int H = params.lines;
	qDebug()<<"Width:"<<W;
	qDebug()<<"Height:"<<H;
	qDebug()<<"BPL"<<params.bytes_per_line;

	SANE_Int maxlen = 6400;
	SANE_Int len = 0;
	buffer = new  SANE_Byte[maxlen];
	ibuffer.open(QIODevice::WriteOnly);

	while(SANE_STATUS_GOOD == sane_status)
	{
		sane_status = sane_read(h, buffer, maxlen, &len);
		ibuffer.write(reinterpret_cast<char*>(buffer), len);
		qDebug()<<"Write"<<len<<"pixels from scan";
		len = 0;
	}
	delete[] buffer;

	//	return;

	qDebug()<<"ImageDataSize:"<< (W*H) << "BufferDataLen:"<<ibuffer.buffer().length();

	uint blackColor = QColor(Qt::black).rgb();
	uint whiteColor = QColor(Qt::white).rgb();
	int bsize = ibuffer.buffer().length() ;
	if((W*H) <= bsize)
	{
		QImage tmp(W ,H , QImage::Format_RGB32);
		QString dbgS;
		for(int y=0; y<H ;++y)
		{
			dbgS.clear();
			for(int x=0;x < W;++x)
			{
				quint8 bv = ibuffer.buffer().at(x + (y*W));
				tmp.setPixel(x,y,(bv > 128) ? whiteColor : blackColor);
				dbgS.append((bv < 128) ? "#" : " ");
			}
			qDebug()<<dbgS;
		}
		image = tmp.copy();
	}

}

bool ScanImage::ScanError(const QString& errorString)
{
	if(SANE_STATUS_GOOD != sane_status)
	{
		qDebug()<<errorString<<"["<<  sane_strstatus(sane_status) <<"]";
		return true;
	}
	return false;
}


