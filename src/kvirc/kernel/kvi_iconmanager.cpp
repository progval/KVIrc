//================================================================================
//
//   File : kvi_iconmanager.cpp
//   Creation date : Sat Jun 24 2000 14:50:11 by Szymon Stefanek
//
//   This file is part of the KVirc irc client distribution
//   Copyright (C) 1999-2003 Szymon Stefanek (pragma at kvirc dot net)
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
//================================================================================

#define __KVIRC__

#define _KVI_ICONMANAGER_CPP_

#include "kvi_iconmanager.h"

#include "kvi_imagelib.h"
#include "kvi_app.h"
#include "kvi_settings.h"
#include "kvi_defaults.h"
#include "kvi_locale.h"
#include "kvi_fileutils.h"
#include "kvi_options.h"

#include <qlayout.h>
#include <qlabel.h>

#ifndef COMPILE_USE_QT4
	// In Qt.4 we need to use QMimeData ?
	#include <qdragobject.h>
#endif
#include <qcursor.h>

#ifdef COMPILE_USE_QT4
	#include <qevent.h>
#endif

// kvi_app.cpp
extern QPixmap * g_pUserChanStatePixmap;
extern QPixmap * g_pActivityMeterPixmap;

KviIconManager * g_pIconManager = 0;

static const char * g_szIconNames[KVI_NUM_SMALL_ICONS]=
{
	"none", // 0
	"close", // 1
	"maximize", // 2
	"minimize", // 3
	"restore", // 4
	"default", // 5
	"dock", // 6
	"undock", // 7
	"quitapp", // 8
	"console", // 9
	"floppy", // 10
	"parsererror", // 11
	"parserwarning", // 12
	"server", // 13
	"world", // 14
	"proxy", // 15
	"kvirc", // 16
	"cut", // 17
	"copy", // 18
	"paste", // 19
	"options", // 20
	"socketmessage", // 21
	"socketwarning", // 22
	"socketerror", // 23
	"systemerror", // 24
	"raw", // 25
	"systemwarning", // 26
	"systemmessage", // 27
	"unhandled", // 28
	"serverinfo", // 29
	"motd", // 30
	"channel", // 31
	"hidedoubleview", // 32
	"showdoubleview", // 33
	"op", // 34
	"voice", // 35
	"serverping", // 36
	"showlistview", // 37
	"hidelistview", // 38
	"join", // 39
	"part", // 40
	"unrecognized", // 41
	"topic", // 42
	"accept", // 43
	"discard", // 44
	"ownprivmsg", // 45
	"chanprivmsg", // 46
	"query", // 48
	"queryprivmsg", // 49
	"help", // 49
	"mdihelp", // 50
	"ctcpreply", // 51
	"ctcprequestreplied", // 52
	"ctcprequestignored", // 53
	"ctcprequestflood", // 54
	"ctcprequestunknown", // 55
	"action", // 56
	"avatar", // 57
	"quit", // 58
	"split", // 59
	"quitsplit", // 60
	"nick", // 61
	"deop", // 62
	"devoice", // 63
	"mode", // 64
	"key", // 65
	"limit", // 66
	"ban", // 67
	"unban", // 68
	"banexcept", // 69
	"banunexcept", // 70
	"inviteexcept", // 71
	"inviteunexcept", // 72
	"chanmode", // 73
	"chanmodehide", // 74
	"who", // 75
	"editor", // 76
	"dccrequest", // 77
	"dccmsg", // 78
	"dccerror", // 79
	"iconmanager", // 80
	"scriptcenter", // 81
	"bomb", // 82
	"event", // 83
	"eventnohandlers", // 84
	"handler", // 85
	"handlerdisabled", // 86
	"nicknameproblem", // 87
	"whoisuser", // 88
	"whoischannels", // 89
	"whoisidle", // 90
	"whoisserver", // 91
	"whoisother", // 92
	"time", // 93
	"notifyonline", // 94
	"notifyoffline", // 95
	"locked", // 96
	"unlocked", // 97
	"lockedoff", // 98
	"unlockedoff", // 99
	"ownprivmsgcrypted", // 100
	"chanprivmsgcrypted", // 101
	"queryprivmsgcrypted", // 102
	"dccchatmsg", // 103
	"dccchatmsgcrypted", // 104
	"irc", // 105
	"folder", // 106
	"home", // 107
	"bookmarks", // 108
	"spy", // 109
	"kick", // 110
	"linux", // 111
	"links", // 112
	"regusers", // 113
	"dockwidget", // 114
	"cascadewindows", // 115
	"maxvertical", // 116
	"maxhorizontal", // 117
	"tilewindows", // 118
	"log", // 119
	"spam", // 120
	"file", // 121
	"icq", // 122
	"icqyellow", // 123
	"icqred", // 124
	"icqblue", // 125
	"icqlightgreen", // 126
	"icqlightyellow", // 127
	"message", // 128
	"messagesent", // 129
	"bluesquare", // 130
	"violetsquare", // 131
	"yellowsquare", // 132
	"greensquare", // 133
	"blacksquare", // 134
	"redsquare", // 135
	"cyansquare", // 136
	"dkgreensquare", // 137
	"terminal", // 138
	"wallops", // 139
	"invisible", // 140
	"servernotice", // 141
	"gnutella", // 142
	"search", // 143
	"files", // 144
	"transfer", // 145
	"package", // 146
	"retry", // 147
	"idea", // 148
	"colors", // 149
	"gui", // 150
	"ircview", // 151
	"alias", // 152
	"channelnotice",// 153
	"channelnoticecrypted", // 154
	"querynotice", // 155
	"querynoticecrypted", // 156
	"servnotice", // 157
	"popup", // 158
	"prologue", // 159
	"epilogue", // 160
	"sharedfiles", // 161
	"ctcpreplyunknown", // 162
	"canvas", // 163
	"nickserv", // 164
	"chanserv", // 165
	"dccvoice", // 166
	"play", // 167
	"record", // 168
	"autotilewindows", // 169
	"away", // 170
	"ident", // 171
	"homepage", // 172
	"list", // 173
	"halfop", // 174
	"halfdeop", // 175
	"invite", // 176
	"multimedia", // 177
	"look", // 178
	"input", // 179
	"messages", // 180
	"querytrace", // 181
	"nochannel", // 182
	"broadcastprivmsg", // 183
	"broadcastnotice", // 184
	"url", // 185
	"rawevent", // 186
	"raweventnohandlers", // 187
	"mekick", // 188
	"meop", // 189
	"mevoice", // 190
	"medeop", // 191
	"medevoice", // 192
	"mehalfop", // 193
	"medehalfop", // 194
	"meban", // 195
	"meunban", // 196
	"mebanexcept", // 197
	"mebanunexcept", // 198
	"meinviteexcept", // 199
	"meinviteunexcept", // 200
	"classictaskbar", // 201
	"treetaskbar", // 202
	"ignore", // 203
	"userlist",// 204
	"stats", // 205
	"popupmenu", // 206
	"xy", // 207
	"irc0", // 208
	"irc1", // 209
	"irc2", // 210
	"irc3", // 211
	"irc4", // 212
	"irc5", // 213
	"heart", // 214
	"heartbroken", // 215
	"rose", // 215
	"biggrin", // 217
	"biggringlasses", // 218
	"biggrineyes", // 219
	"textexclamative", // 220
	"textpoints", // 221
	"kiss", // 222
	"surprised1", // 223
	"ugly", // 224
	"angry", // 225
	"surprised2", // 226
	"smile", // 227
	"tongue", // 228
	"ssl", // 229
	"cry", // 230
	"eye", // 231
	"deadchannel", // 232
	"deadquery", // 233
	"sound", // 234
	"toolbar", // 235
	"textencoding",// 236
	"newitem", // 237
	"newitembywizard", // 238
	"deleteitem", // 239
	"edititem", // 240
	"filetransfer", // 241
	"chanadmin", // 242
	"chanunadmin", // 243
	"mechanadmin", // 244
	"mechanunadmin", // 245
	"userop", // 246
	"deuserop", // 247
	"meuserop", // 248
	"medeuserop", // 249
	"applet", // 250
	"caption", // 251
	"transparent", // 252
	"alert", // 253
	"userwindow", // 254
	"statusbar", // 255
	"notaway",  // 256
	"plus",    // 257
	"minus",    // 258
	"binarytext", // 259
	"chanowner", // 260
	"chanunowner", // 261
	"mechanowner", // 262
	"mechanunowner", // 263
	"afraid", // 264
	"tongue2", // 265
	"sayicon", // 266
	"saycolors", // 267
	"finger", // 268
	"scriptaction", // 269
	"tearsmile", // 270
	"shy", // 271
	"servererror", // 272
	"cafe", // 273
	"addons", // 274
	"chanowneraway", // 275
	"opaway", // 276
	"voiceaway", // 277
	"chanadminaway", // 278
	"useropaway", // 279
	"halfopaway", // 280
	"aliashighlighted", // 281
	"demoralized", // 282
	"slurp", // 283
	"namespace",  // 284
	"saysmile", // 285
	"saykvs", // 286
	"themeoptions", // 287
	"bug", // 288
	"refresh", //289
	"theme", // 290
	"screenshot" // 291
};

KviIconWidget::KviIconWidget()
#ifdef COMPILE_USE_QT4
: QWidget(0,"global_icon_widget" /*,WType_TopLevel | WStyle_Customize | WStyle_Title | WStyle_StaysOnTop | WStyle_DialogBorder | WStyle_SysMenu | WStyle_Minimize*/)
#else
: QWidget(0,"global_icon_widget",WType_TopLevel | WStyle_Customize | WStyle_Title | WStyle_StaysOnTop | WStyle_DialogBorder | WStyle_SysMenu | WStyle_Minimize)
#endif
{
	init();
}

KviIconWidget::KviIconWidget(QWidget * par)
: QWidget(par,"global_icon_widget")
{
	init();
}

void KviIconWidget::init()
{
	setCaption(__tr2qs("Icon Table"));
	setIcon(*(g_pIconManager->getSmallIcon(KVI_SMALLICON_ICONMANAGER)));

	int rows = KVI_NUM_SMALL_ICONS / 10;
	if((rows * 10) < KVI_NUM_SMALL_ICONS)rows++;
	QGridLayout * g = new QGridLayout(this,rows + 1,11,0,2);
	int i;
	for(i = 0;i < 10;i++)
	{
		KviStr tmp(KviStr::Format,"%d",i);
		QLabel * l = new QLabel(tmp.ptr(),this);
		g->addWidget(l,0,i + 1);
	}
	for(i = 0;i < rows;i++)
	{
		KviStr tmp(KviStr::Format,"%d",i * 10);
		QLabel * l = new QLabel(tmp.ptr(),this);
		g->addWidget(l,i + 1,0);
	}
	for(i = 0;i < KVI_NUM_SMALL_ICONS;i++)
	{
		KviStr tmp(KviStr::Format,"%d",i);
		QLabel * l = new QLabel(this,tmp.ptr());
		l->setPixmap(*(g_pIconManager->getSmallIcon(i)));
		g->addWidget(l,(i / 10) + 1,(i % 10) + 1);
		l->installEventFilter(this);
		l->setAcceptDrops(true);
	}
}

KviIconWidget::~KviIconWidget()
{
}

void KviIconWidget::closeEvent(QCloseEvent * e)
{
	e->ignore();
	emit closed();
}

bool KviIconWidget::eventFilter(QObject * o,QEvent *e)
{
	if(e->type() == QEvent::MouseButtonPress)
	{
		if(o->inherits("QLabel"))
		{
			KviStr szName = ((QLabel *)o)->name();
			bool bOk;
			int iVal = szName.toInt(&bOk);
			if(bOk)emit selected(iVal);

			if(parentWidget() && isVisible() && parentWidget()->inherits("KviTalPopupMenu"))
			{
				parentWidget()->close();
				return true;
			} else {
				if(const QPixmap * pix = ((QLabel *)o)->pixmap())
				{
// FIXME: In Qt 4.x this stuff needs to be ported to a different api.. which one ?
#ifndef COMPILE_USE_QT4
					QDragObject * drobj = new QTextDrag(o->name(),this);
					drobj->setPixmap(*pix,((QLabel *)o)->mapFromGlobal(QCursor::pos()));
					drobj->dragCopy();
#endif
					return true;
				}
			}
		}
	} else if(e->type() == QEvent::DragEnter)
	{
		((QDragEnterEvent *)e)->accept(true);
		return true;
	}
	return QWidget::eventFilter(o,e);
}



KviCachedPixmap::KviCachedPixmap(QPixmap * ptr,const QString &szPath)
{
	m_szPath = szPath;
	m_tLastAccess = kvi_unixTime();
	m_pPixmap = ptr;
	m_uSize = m_pPixmap->width() * m_pPixmap->height() * (m_pPixmap->depth() / 8);
}

KviCachedPixmap::~KviCachedPixmap()
{
	delete m_pPixmap;
}

void KviCachedPixmap::updateLastAccessTime()
{
	m_tLastAccess = kvi_unixTime();
}




KviIconManager::KviIconManager()
{
	int i=0;
	for(i=0;i<KVI_NUM_SMALL_ICONS;i++)m_smallIcons[i] = 0;

	//loadSmallIcons();

	m_pCachedImages = new KviPointerHashTable<QString,KviCachedPixmap>(21,true);
	m_pCachedImages->setAutoDelete(true);

	m_uCacheTotalSize = 0;
	m_uCacheMaxSize = 1024 * 1024; // 1 MB

	m_pIconWidget = 0;

	KviStr buffer;

	// Load the userchanstate image
	g_pApp->findImage(buffer,KVI_USERCHANSTATE_IMAGE_NAME);
	g_pUserChanStatePixmap = new QPixmap(buffer.ptr());

	g_pApp->findImage(buffer,KVI_ACTIVITYMETER_IMAGE_NAME);
	g_pActivityMeterPixmap = new QPixmap(buffer.ptr());
	
	m_pIconNames = 0;
	
	// TEMP
	
	/*
	for(i=0;i<KVI_NUM_SMALL_ICONS;i++)
	{
		QPixmap * tmp = getSmallIcon(i);
		QString nnn = "/home/pragma/kvismallicons/kcs_";
		nnn += g_szIconNames[i];
		nnn += ".png";
		if(tmp->isNull())
		{
			debug("OPS, %s is NULL",nnn.latin1());
		}
		if(!tmp->save(nnn,"PNG",90))
		{
			debug("FAILED TO SAVE %s",nnn.latin1());
		}
	}
	*/
}

KviIconManager::~KviIconManager()
{
	delete g_pUserChanStatePixmap;
	delete g_pActivityMeterPixmap;

	if(m_pIconWidget)delete m_pIconWidget;
	int i;

	for(i=0;i<KVI_NUM_SMALL_ICONS;i++)
	{
		if(m_smallIcons[i])delete m_smallIcons[i];
	}
	delete m_pCachedImages;
	if(m_pIconNames)delete m_pIconNames;
}

const char * KviIconManager::getSmallIconName(int idx)
{
	return g_szIconNames[idx];
}

int KviIconManager::getSmallIconIdFromName(const QString &szName)
{
	if(!m_pIconNames)
	{
		m_pIconNames = new KviPointerHashTable<QString,int>(257,false);
		m_pIconNames->setAutoDelete(true);
		
		for(int i=0;i<KVI_NUM_SMALL_ICONS;i++)
		{
			int * pInt = new int;
			*pInt = i;
			m_pIconNames->replace(QString(g_szIconNames[i]),pInt);
		}
	}
	int * pInt = m_pIconNames->find(szName);
	if(!pInt)return 0;
	return *pInt;
}


void KviIconManager::showIconWidget()
{
	if(!m_pIconWidget)
	{
		m_pIconWidget = new KviIconWidget();
		connect(m_pIconWidget,SIGNAL(closed()),this,SLOT(iconWidgetClosed()));
	}
	m_pIconWidget->show();
	m_pIconWidget->raise();
	m_pIconWidget->setFocus();
}

void KviIconManager::iconWidgetClosed()
{
	delete m_pIconWidget;
	m_pIconWidget = 0;
}

/*
	@doc: image_id
	@title:
		The image identifier
	@keyterms:
		image identifier, using builtin images, how kvirc locates image files
	@type:
		language
	@short:
		Semantics of the <image_id> parameter
	@body:
		Some KVIrc commands and functions accept
		the <image_id> as a parameter.
		The <image_id> indicates an image to be displayed
		by some gui element. Since KVIrc has a set of
		builtin icons, we want the user to be able to use it :).[br]
		The <image_id> can be either a signed integer number
		or a filename.[br]
		If it is a signed integer it is interpreted as
		index of the internal KVIrc image to use:
		positive integers (starting at 0) indicate
		small (16x16) icons (the ones used in the view widget).[br]
		If <image_id> is anything else than a signed integer,
		it is interpreted as a filename.
		The filename can be an absolute file path or a relative one.
		In this last case KVIrc will look for the file in a predefined set
		of directories: First in the local "pics" directory (usually
		in ~/kvirc-version/pics/), then in the local "incoming" directory,
		then in the global "pics" directory , then in the user home (~)
		and in the current directory.[br]
*/

KviCachedPixmap * KviIconManager::getPixmapWithCache(const QString &szName)
{
	if(szName.isEmpty())return 0;

	KviCachedPixmap * p = m_pCachedImages->find(szName);

	if(p)
	{
		p->updateLastAccessTime();
		return p;
	}

	QPixmap * pix = 0;

	QString szRetPath;

	if(g_pApp->findImage(szRetPath,szName))
	{
		pix = new QPixmap(szRetPath);
		if(pix->isNull())
		{
			delete pix; // it is not an valid image!!! (really bad situation...)
			pix = 0;
			return 0;
		}
	} else {
		return 0;
	}

	p = new KviCachedPixmap(pix,QString(szRetPath));
	addToCache(szName,p);

	return p;
}

KviCachedPixmap * KviIconManager::getPixmapWithCacheScaleOnLoad(const QString &szName,int iMaxWidth,int iMaxHeight)
{
	if(szName.isEmpty())return 0;

	KviCachedPixmap * p = m_pCachedImages->find(szName);

	if(p)
	{
		p->updateLastAccessTime();
		return p;
	}

	QPixmap * pix = 0;

	QString szRetPath;

	if(g_pApp->findImage(szRetPath,szName))
	{
		pix = new QPixmap(szRetPath);
		if(pix->isNull())
		{
			delete pix; // it is not an valid image!!! (really bad situation...)
			pix = 0;
			return 0;
		}
		if((pix->width() > iMaxWidth) || (pix->height() > iMaxHeight))
		{
			// scale to fit
			int scaleW = iMaxWidth;
			int scaleH;
			scaleH = (pix->height() * iMaxWidth) / pix->width();
			if(scaleH > iMaxHeight)
			{
				scaleH = iMaxHeight;
				scaleW = (scaleH * pix->width()) / pix->height();
			}
	
			QImage img = pix->convertToImage();
			pix->convertFromImage(img.smoothScale(scaleW,scaleH));
		}
	} else {
		return 0;
	}

	p = new KviCachedPixmap(pix,QString(szRetPath));
	addToCache(szName,p);

	return p;
}


void KviIconManager::addToCache(const QString &szName,KviCachedPixmap * p)
{
	if((m_uCacheTotalSize + p->size()) >= m_uCacheMaxSize)cacheCleanup();

	m_pCachedImages->insert(szName,p);
	m_uCacheTotalSize += p->size();
}

QPixmap * KviIconManager::getImage(const char * id,bool bCanBeNumber,QString* pRetPath)
{
	if(!id)return 0;
	if(bCanBeNumber)
	{
		bool bOk;
		QString tmp = id;
		int idx = tmp.toInt(&bOk);
		if(bOk)
		{
			// was a number : this is not a filename
			if(idx >= 0)return getSmallIcon(idx % KVI_NUM_SMALL_ICONS);
		}
	}

	KviCachedPixmap * p = getPixmapWithCache(id);
	if(!p)return 0;
	if(pRetPath)*pRetPath = p->path();
	return p->pixmap();
}



QPixmap * KviIconManager::getBigIcon(const QString &szName)
{
	QPixmap * p = getPixmap(szName);
	if(p)return p;

	bool bOk;
	int idx = szName.toInt(&bOk);
	if(bOk && (idx >= 0))
	{
		// was a number : this is not a filename
		// it was a small icon: scale it and cache it
		QString tmpName = szName;
		tmpName += ".scaled16to32";
		p = getPixmap(tmpName);
		if(p)return p;
		p = getSmallIcon(idx % KVI_NUM_SMALL_ICONS);
		if(p)
		{
			QImage tmpi = p->convertToImage();
			QImage tmp2 = tmpi.smoothScale(32,32);
			KviCachedPixmap * cp = new KviCachedPixmap(new QPixmap(tmp2),QString::null);
			addToCache(tmpName,cp);
			return cp->pixmap();
		}
	}

	p = getPixmap("kvi_bigicon_unknown.png");
	if(p)return p;
	KviCachedPixmap * cp = new KviCachedPixmap(new QPixmap(32,32),QString::null);
	addToCache(szName,cp);
	return cp->pixmap();
}

void KviIconManager::urlToCachedFileName(KviStr &szFName)
{
	szFName.replaceAll(':',"_");
	szFName.replaceAll(';',"_");
	szFName.replaceAll('/',"_");
	szFName.replaceAll('\\',"_");
	szFName.replaceAll("\"","_");
	szFName.replaceAll("\'","_");
	szFName.replaceAll("(","_");
	szFName.replaceAll(")","_");
	szFName.replaceAll("?","_");
	szFName.replaceAll("___",".");
}

void KviIconManager::urlToCachedFileName(QString &szFName)
{
	szFName.replace(":","_");
	szFName.replace(";","_");
	szFName.replace("/","_");
	szFName.replace("\\","_");
	szFName.replace("\"","_");
	szFName.replace("\'","_");
	szFName.replace("(","_");
	szFName.replace(")","_");
	szFName.replace("?","_");
	szFName.replace("___",".");
}

KviAvatar * KviIconManager::getAvatar(const QString &szLocalPath,const QString &szName)
{
	QString szP,szN;

	if(szLocalPath.isEmpty())
	{
		if(szName.isEmpty())return 0;
		if(KviQString::equalCIN("http://",szName,7))
		{
			szN = szName;
			szP = szName;
			urlToCachedFileName(szP);
		} else {
			szN = KviFileUtils::extractFileName(szName);
			szP = szName;
		}
	} else if(szName.isEmpty())
	{
		if(szLocalPath.isEmpty())return 0;
		szP = szLocalPath;
		szN = KviFileUtils::extractFileName(szLocalPath);
	} else {
		szP = szLocalPath;
		szN = szName;
	}

	// avatars bigger than 1024x768 just sux: they can't be seen on tooltips anyway
	KviCachedPixmap * p = KVI_OPTION_BOOL(KviOption_boolScaleAvatarsOnLoad) ? getPixmapWithCacheScaleOnLoad(szP,
		KVI_OPTION_UINT(KviOption_uintScaleAvatarsOnLoadWidth)
		,KVI_OPTION_UINT(KviOption_uintScaleAvatarsOnLoadHeight)) : getPixmapWithCache(szP);
	if(!p)return 0;

	return new KviAvatar(p->path(),szN,new QPixmap(*(p->pixmap())));
}

void KviIconManager::clearCache()
{
	m_pCachedImages->clear();
}

void KviIconManager::reloadImages()
{
	clearCache();
	for(int i=0;i<KVI_NUM_SMALL_ICONS;i++)
	{
		if(m_smallIcons[i])delete m_smallIcons[i];
		m_smallIcons[i] = 0;
	}
}


QPixmap * KviIconManager::loadSmallIcon(int idx)
{
	if(idx >= KVI_NUM_SMALL_ICONS)return 0;
	if(idx < 0)return 0;
	
	
	// COMPATIBILITY ENTRY: To be removed in some time.
	// This part loads the old smallicon library format kvi_smallicon_*.png
	// to preserve backward compatibility with old themes.
	// We lookup such files ONLY in the themes directories.

	QString szFileName = KVI_SMALLICONS_IMAGELIB_PREFIX;

	int fileidx = idx / 16;

	if(fileidx >= 10)KviQString::appendFormatted(szFileName,"%d.png",fileidx);
	else KviQString::appendFormatted(szFileName,"0%d.png",fileidx);

	QString buffer;
	if(g_pApp->findImageThemeOnlyCompat(buffer,szFileName))
	{
		KviImageLibrary l1(buffer,16,16);
	
		int offset = fileidx * 16;
	
		for(int i=0;i<16;i++)
		{
			int io = i + offset;
			if(io >= KVI_NUM_SMALL_ICONS)break;
			if(m_smallIcons[io])delete m_smallIcons[io];
			m_smallIcons[io] = new QPixmap(l1.getImage(i));
		}

		return m_smallIcons[idx];
	}

	// otherwise we use the NEW method: separate small icons in the "coresmall" subdirectory.
	QString szPath;
	KviQString::sprintf(szPath,KVI_SMALLICONS_PREFIX "%s.png",g_szIconNames[idx]);
	
	g_pApp->findSmallIcon(buffer,szPath);
	m_smallIcons[idx] = new QPixmap(buffer);

	//if(m_smallIcon[idx]->isNull())
	//{
	// // load an "unknown" image ?.. but should never happen
	//}
	return m_smallIcons[idx];
}

void KviIconManager::cacheCleanup()
{
	QStringList l;

	KviPointerHashTableIterator<QString,KviCachedPixmap> it(*m_pCachedImages);


//#warning "IMPLEMENT CLEANUP"
/*
	while(it.current())
	{
		kvi_time_t curTime = kvi_unixTime();
		
		if((((unsigned int)it.current()->timestamp) - ((unsigned int)curTime)) > 120)
		{
			// unused since 2 minutes
			// time to cleanup it...
			l.append(new KviStr(it.currentKey()));
		}
		++it;
	}

	for(QStringList::Iterator it = l.begin();it != l.end();++it)
	{
		m_pCachedImages->remove(*it);
	}
*/
/*
	for(KviStr * p =l.first();p;p = l.next())
	{
		m_pCachedImages->remove(p->ptr());
	}
	if(m_pCachedImages->isEmpty())m_pCleanupTimer->stop();
*/
}

#include "kvi_iconmanager.moc"
