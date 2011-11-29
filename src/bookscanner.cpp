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

#define BS_COMMAND_TURN_1 "turn 1"
#define BS_COMMAND_TURN_0 "turn 0"
#define BS_COMMAND_SERVO "servo "
#define BS_TURNPAGE_COMPLETED 1

#include "bookscanner.h"

#include <QMap>
#include <QDebug>
#include <QString>


BookScanner::BookScanner(const QString& sfile, int baud)
{
	QMap<int, LibSerial::SerialStreamBuf::BaudRateEnum> itbr;
	itbr.insert(50, LibSerial::SerialStreamBuf::BAUD_50      );
	itbr.insert(75, LibSerial::SerialStreamBuf::BAUD_75      );
	itbr.insert(110, LibSerial::SerialStreamBuf::BAUD_110    );
	itbr.insert(134, LibSerial::SerialStreamBuf::BAUD_134    );
	itbr.insert(150, LibSerial::SerialStreamBuf::BAUD_150    );
	itbr.insert(200, LibSerial::SerialStreamBuf::BAUD_200    );
	itbr.insert(300, LibSerial::SerialStreamBuf::BAUD_300    );
	itbr.insert(600, LibSerial::SerialStreamBuf::BAUD_600    );
	itbr.insert(1200, LibSerial::SerialStreamBuf::BAUD_1200  );
	itbr.insert(1800, LibSerial::SerialStreamBuf::BAUD_1800  );
	itbr.insert(2400, LibSerial::SerialStreamBuf::BAUD_2400  );
	itbr.insert(4800, LibSerial::SerialStreamBuf::BAUD_4800  );
	itbr.insert(9600, LibSerial::SerialStreamBuf::BAUD_9600  );
	itbr.insert(19200, LibSerial::SerialStreamBuf::BAUD_19200);
	itbr.insert(38400, LibSerial::SerialStreamBuf::BAUD_38400  );
	itbr.insert(57600, LibSerial::SerialStreamBuf::BAUD_57600  );
	itbr.insert(115200, LibSerial::SerialStreamBuf::BAUD_115200);
	itbr.insert(230400, LibSerial::SerialStreamBuf::BAUD_230400);
	itbr.insert(460800, LibSerial::SerialStreamBuf::BAUD_460800);

	LibSerial::SerialStreamBuf::BaudRateEnum b = (baud > 0) ? itbr.value(baud) : LibSerial::SerialStreamBuf::BAUD_DEFAULT;

	serial.Open(sfile.toStdString());
	serial.SetBaudRate(b);
	serial.SetCharSize(LibSerial::SerialStreamBuf::CHAR_SIZE_8);
}

void BookScanner::clearSerial()
{
	int length;
	char * buffer;
	serial.seekg (0, std::ios::end);
	length = serial.tellg();
	serial.seekg (0, std::ios::beg);
	if(length > 0)
	{
	// allocate memory:
	buffer = new char [length];

	// read data as a block:
	serial.read (buffer,length);
	delete[] buffer;
	}
}

void BookScanner::TurnPage()
{
	if(!serial.IsOpen())
		return;

	// serial << BS_COMMAND_TURN_1;
	for(int i(10); i < 360; ++i)
	{

//		clearSerial();
		QString c(BS_COMMAND_SERVO + QString::number(i) + QString("\n"));
//		serial << BS_COMMAND_SERVO << i << std::endl;
		serial.write(c.toAscii().data(), c.length());

	}

}
