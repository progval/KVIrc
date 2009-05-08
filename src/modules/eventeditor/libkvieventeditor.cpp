//
//   File : libkvieventeditor.cpp
//   Creation date : Mon 23 Dec 2002 20:23:59 2002 GMT by Szymon Stefanek
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_frame.h"

#include "eventeditor.h"

KviEventEditorWindow * g_pEventEditorWindow = 0;


/*
	@doc: eventeditor.open
	@type:
		command
	@title:
		eventeditor.open
	@short:
		Shows the event editor
	@syntax:
		toolbareditor.open
	@description:
		Opens the script event editor dialog.
*/

static bool eventeditor_kvs_cmd_open(KviKvsModuleCommandCall * c)
{
	if(!g_pEventEditorWindow)
	{
		g_pEventEditorWindow = new KviEventEditorWindow(c->window()->frame());
		c->window()->frame()->addWindow(g_pEventEditorWindow);
	}

	g_pEventEditorWindow->setFocus();
	return true;
}

static bool eventeditor_module_init(KviModule * m)
{

	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",eventeditor_kvs_cmd_open);

	g_pEventEditorWindow = 0;
	return true;
}

static bool eventeditor_module_can_unload(KviModule * m)
{
	return (g_pEventEditorWindow == 0);
}

static bool eventeditor_module_cleanup(KviModule *m)
{
	if(g_pEventEditorWindow)delete g_pEventEditorWindow;
	g_pEventEditorWindow = 0;
	return true;
}

KVIRC_MODULE(
	"EventEditor",                                                 // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2002 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Editor for the script events",
	eventeditor_module_init,
	eventeditor_module_can_unload,
	0,
	eventeditor_module_cleanup
)
