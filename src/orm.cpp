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

#include <iostream>

#include "orm.h"

#include <gphoto2-camera.h>
#include <gphoto2-port-info-list.h>
#include <fcntl.h>
#include <cstring>

#include <QDebug>

ORM::ORM(const QString& leftPort, const QString& rightPort)
{
	context = gp_context_new();
	CameraList	*list;
	if(gp_list_new(&list) == GP_OK)
	{
		int ccount = gp_camera_autodetect(list, context);
		const char	*model, *port;
		GPPortInfoList *portinfolist = 0;
		gp_port_info_list_new(&portinfolist);
		gp_port_info_list_load(portinfolist);

		for(int i(0); i < ccount; i++)
		{
			gp_list_get_name  (list, i, &model);
			gp_list_get_value (list, i, &port);
			std::cout<<"Found Camera ["<<model<<"] on port ["<<port<<"]"<<std::endl;
			QString cport(port);

			Camera *camera;
			gp_camera_new(&camera);


			GPPortInfo	pi;
			int p = gp_port_info_list_lookup_path (portinfolist, port);
			gp_port_info_list_get_info (portinfolist, p, &pi);
			gp_camera_set_port_info (camera, pi);


			int rinit = gp_camera_init(camera, context);
			if(rinit >= GP_OK)
				std::cout<<"Init Camera Success"<<std::endl;
			else
				std::cout<<"Failed Init Camera with code ["<<gp_port_result_as_string(rinit)<<"]"<<std::endl;
			gp_camera_exit(camera, context);

			if(cport == leftPort)
			{
				cameraLeft = camera;
				qDebug()<<"Got Left Camera!!";
			}
			else if(cport == rightPort)
			{
				cameraRight = camera;
				qDebug()<<"Got Right Camera!!";
			}
		}
	}

}

ORM::~ORM()
{

}

QImage ORM::TakePicture(ORM::CameraSelect camsel)
{
	Camera *camera;
	if(camsel == Left)
		camera = cameraLeft;
	else
		camera = cameraRight;

	qDebug()<<"TP"<<camera;
	int fd;
	CameraFile *file;
	CameraFilePath camera_file_path;
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");
	gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);
	fd = open("test.jpg", O_CREAT | O_WRONLY, 0644);
	gp_file_new_from_fd(&file, fd);
	gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, context);
	gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);
	gp_file_free(file);

	close(fd);

	return QImage(QString("test.jpg"));

}

