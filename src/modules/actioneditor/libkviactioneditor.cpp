//=============================================================================
//
//   File : libkviactioneditor.cpp
//   Creation date : Tue 29 Dec 2004 02:45:59 2002 GMT by Szymon Stefanek
//
//   This toolbar is part of the KVirc irc client distribution
//   Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)
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

#include "kvi_module.h"
#include "kvi_locale.h"
#include "kvi_frame.h"

#include "actioneditor.h"
KviActionEditorWindow * g_pActionEditorWindow = 0;


/*
	@doc: actioneditor.open
	@type:
		command
	@title:
		actioneditor.open
	@short:
		Shows the action editor
	@syntax:
		actioneditor.open
	@description:
		Opens the script action editor dialog.
*/

static bool actioneditor_kvs_cmd_open(KviKvsModuleCommandCall * c)
{ 
	if(!g_pActionEditorWindow)
	{
		g_pActionEditorWindow = new KviActionEditorWindow(c->window()->frame());
		c->window()->frame()->addWindow(g_pActionEditorWindow);
	}

	g_pActionEditorWindow->setFocus();
	return true;
}
static bool actioneditor_module_init(KviModule * m)
{
	KVSM_REGISTER_SIMPLE_COMMAND(m,"open",actioneditor_kvs_cmd_open);
	g_pActionEditorWindow = 0;
	return true;
}

static bool actioneditor_module_can_unload(KviModule * m)
{
	return (g_pActionEditorWindow == 0);
}

static bool actioneditor_module_cleanup(KviModule *m)
{
	if(g_pActionEditorWindow)delete g_pActionEditorWindow;
	g_pActionEditorWindow = 0;
	return true;
}

KVIRC_MODULE(
	"ActionEditor",                                                 // module name
	"1.0.0",                                                // module version
	"Copyright (C) 2004 Szymon Stefanek (pragma at kvirc dot net)", // author & (C)
	"Editor for the script actions",
	actioneditor_module_init,
	actioneditor_module_can_unload,
	0,
	actioneditor_module_cleanup
)
