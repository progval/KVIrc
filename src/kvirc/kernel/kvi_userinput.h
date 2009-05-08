#ifndef _KVI_USERINPUT_H_
#define _KVI_USERINPUT_H_
//=============================================================================
//
//   File : kvi_userinput.h
//   Created on Sun 25 Sep 2005 05:27:57 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC Client distribution
//   Copyright (C) 2005 Szymon Stefanek <pragma at kvirc dot net>
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
//=============================================================================

#include "kvi_settings.h"
#include "kvi_qstring.h"
#include "kvi_window.h"

namespace  KviUserInput
{
	// WARNING: May destroy szData
	// returns false if szData is a command and it fails to run
	KVIRC_API bool parse(QString &szData,KviWindow * pWindow,const QString &szContext = KviQString::empty,bool bUserFriendlyCommandline = false);
	
	KVIRC_API bool parseCommand(const QString &szData,KviWindow * pWindow,const QString &szContext = KviQString::empty,bool bUserFriendlyCommandline = false);
	//bool parseCommandWithSingleArgument(const QString &szData,KviWindow * pWindow,const QString &szContext = KviQString::empty);
	KVIRC_API void parseNonCommand(QString &szData,KviWindow * pWindow);
};


#endif //!_KVI_USERINPUT_H_
