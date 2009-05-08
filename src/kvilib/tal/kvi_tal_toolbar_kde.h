#ifndef _KVI_TAL_TOOLBAR_KDE_H_
#define _KVI_TAL_TOOLBAR_KDE_H_
//
//   File : kvi_tal_toolbar_kde.h
//   Creation date : Mon Aug 13 05:05:52 2001 GMT by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 2001 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc. ,51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//

#include <ktoolbar.h>

#include "kvi_tal_toolbardocktype.h"

class KVILIB_API KviTalToolBar : public KToolBar
{
	Q_OBJECT
public:
	KviTalToolBar(const QString &label,QMainWindow *w,QT_TOOLBARDOCK_TYPE dock = QT_DOCK_TOP,bool bNewLine = false,const char * nam = 0);
	~KviTalToolBar();
};

#endif //_KVI_TAL_TOOLBAR_KDE_H_
