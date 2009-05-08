//=============================================================================
//
//   File : kvi_kvs_treenode_specialcommandunset.cpp
//   Created on Thu 14 Apr 2005 02:44:00 by Szymon Stefanek
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

#include "kvi_kvs_treenode_specialcommandunset.h"
#include "kvi_kvs_runtimecontext.h"
#include "kvi_kvs_rwevaluationresult.h"
#include "kvi_locale.h"

KviKvsTreeNodeSpecialCommandUnset::KviKvsTreeNodeSpecialCommandUnset(const QChar * pLocation,KviPointerList<KviKvsTreeNodeVariable> * pVariableList)
: KviKvsTreeNodeSpecialCommand(pLocation,"unset")
{
	m_pVariableList = pVariableList;
	for(KviKvsTreeNodeVariable * pVar = m_pVariableList->first();pVar;pVar = m_pVariableList->next())
	{
		pVar->setParent(this);
	}
}

KviKvsTreeNodeSpecialCommandUnset::~KviKvsTreeNodeSpecialCommandUnset()
{
	delete m_pVariableList;
}

void KviKvsTreeNodeSpecialCommandUnset::contextDescription(QString &szBuffer)
{
	szBuffer = "Special Command \"unset\"";
}

void KviKvsTreeNodeSpecialCommandUnset::dump(const char * prefix)
{
	debug("%s SpecialCommandUnset",prefix);
	QString tmp = prefix;
	tmp.append("  ");
	for(KviKvsTreeNodeVariable * pVar = m_pVariableList->first();pVar;pVar = m_pVariableList->next())
	{
		pVar->dump(tmp.utf8().data());
	}
}

bool KviKvsTreeNodeSpecialCommandUnset::execute(KviKvsRunTimeContext * c)
{
	for(KviKvsTreeNodeVariable * pVar = m_pVariableList->first();pVar;pVar = m_pVariableList->next())
	{
		KviKvsRWEvaluationResult * r = pVar->evaluateReadWrite(c);
		if(!r)return false;
		r->result()->setNothing();
		delete r; // this will remove the data from its container
	}
	return true;
}

