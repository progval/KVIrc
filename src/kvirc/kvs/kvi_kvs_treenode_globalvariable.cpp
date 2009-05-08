//=============================================================================
//
//   File : kvi_kvs_treenode_globalvariable.cpp
//   Created on Thu 16 Oct 2003 22:45:01 by Szymon Stefanek
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

#include "kvi_kvs_treenode_globalvariable.h"
#include "kvi_kvs_runtimecontext.h"

KviKvsTreeNodeGlobalVariable::KviKvsTreeNodeGlobalVariable(const QChar * pLocation,const QString &szIdentifier)
: KviKvsTreeNodeVariable(pLocation,szIdentifier)
{
}

KviKvsTreeNodeGlobalVariable::~KviKvsTreeNodeGlobalVariable()
{
}

void KviKvsTreeNodeGlobalVariable::contextDescription(QString &szBuffer)
{
	szBuffer = "Global Variable \"";
	szBuffer += m_szIdentifier;
	szBuffer += "\"";
}

void KviKvsTreeNodeGlobalVariable::dump(const char * prefix)
{
	debug("%s GlobalVariable(%s)",prefix,m_szIdentifier.utf8().data());
}


bool KviKvsTreeNodeGlobalVariable::evaluateReadOnly(KviKvsRunTimeContext * c,KviKvsVariant * pBuffer)
{
	KviKvsVariant * v = c->globalVariables()->find(m_szIdentifier);
	if(v)pBuffer->copyFrom(v);
	else pBuffer->setNothing();
	return true;
}

KviKvsRWEvaluationResult * KviKvsTreeNodeGlobalVariable::evaluateReadWrite(KviKvsRunTimeContext * c)
{
	return new KviKvsHashElement(0,c->globalVariables()->get(m_szIdentifier),c->globalVariables(),m_szIdentifier);
}
