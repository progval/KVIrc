//=============================================================================
//
//   File : kvi_kvs_treenode_localvariable.cpp
//   Created on Thu 16 Oct 2003 22:41:20 by Szymon Stefanek
//
//   This file is part of the KVIrc IRC client distribution
//   Copyright (C) 2003 Szymon Stefanek <pragma at kvirc dot net>
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

#define __KVIRC__

#include "kvi_kvs_treenode_localvariable.h"
#include "kvi_kvs_runtimecontext.h"

KviKvsTreeNodeLocalVariable::KviKvsTreeNodeLocalVariable(const QChar * pLocation,const QString &szIdentifier)
: KviKvsTreeNodeVariable(pLocation,szIdentifier)
{
}

KviKvsTreeNodeLocalVariable::~KviKvsTreeNodeLocalVariable()
{
}

void KviKvsTreeNodeLocalVariable::contextDescription(QString &szBuffer)
{
#ifdef COMPILE_NEW_KVS
	szBuffer = "Local Variable \"";
	szBuffer += m_szIdentifier;
	szBuffer += "\"";
#endif
}

void KviKvsTreeNodeLocalVariable::dump(const char * prefix)
{
#ifdef COMPILE_NEW_KVS
	debug("%s LocalVariable(%s)",prefix,m_szIdentifier.utf8().data());
#endif
}

bool KviKvsTreeNodeLocalVariable::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
#ifdef COMPILE_NEW_KVS
	KviKvsVariant * v = c->localVariables()->find(m_szIdentifier);
	if(v)
	{
		pBuffer->copyFrom(v);
	} else {
		pBuffer->setNothing();
	}
#endif
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeLocalVariable::evaluateReadWrite(KviKvsRunTimeContext * c)
{
#ifdef COMPILE_NEW_KVS
	return new KviKvsHashElement(0,c->localVariables()->get(m_szIdentifier),c->localVariables(),m_szIdentifier);
#else
	return 0;
#endif
}
