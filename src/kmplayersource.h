/***************************************************************************
                          kmplayersource.h  -  description
                             -------------------
    begin                : Sat Mar  24 16:14:51 CET 2003
    copyright            : (C) 2003 by Koos Vriezen
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMPLAYERSOURCE_H
#define KMPLAYERSOURCE_H
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qptrlist.h>
#include <qmap.h>
#include <qpair.h>

#include <kurl.h>

#include "kmplayer_part.h"

class KMPlayerApp;
class QPopupMenu;
class QMenuItem;


class KMPlayerAppURLSource : public KMPlayerURLSource {
    Q_OBJECT
public:
    KMPlayerAppURLSource (KMPlayerApp * app);
    virtual ~KMPlayerAppURLSource ();
    virtual bool processOutput (const QString & line);
public slots:
    virtual void activate ();
    virtual void deactivate ();
    void finished ();
private:
    KURL m_url;
    KMPlayerApp * app;
};


class KMPlayerMenuSource : public KMPlayerSource {
    Q_OBJECT
public:
    KMPlayerMenuSource (KMPlayerApp * app, QPopupMenu * m);
    virtual ~KMPlayerMenuSource ();
protected:
    void menuItemClicked (QPopupMenu * menu, int id);
    QPopupMenu * m_menu;
    KMPlayerApp * app;
};


class KMPlayerDVDSource : public KMPlayerMenuSource {
    Q_OBJECT
public:
    KMPlayerDVDSource (KMPlayerApp * app, QPopupMenu * m);
    virtual ~KMPlayerDVDSource ();
    virtual bool processOutput (const QString & line);
    virtual QString filterOptions ();
public slots:
    virtual void activate ();
    virtual void deactivate ();
    virtual void play ();

    void finished ();
    void titleMenuClicked (int id);
    void subtitleMenuClicked (int id);
    void languageMenuClicked (int id);
    void chapterMenuClicked (int id);
private:
    QRegExp langRegExp;
    QRegExp subtitleRegExp;
    QRegExp titleRegExp;
    QRegExp chapterRegExp;
    QPopupMenu * m_dvdtitlemenu;
    QPopupMenu * m_dvdchaptermenu;
    QPopupMenu * m_dvdlanguagemenu;
    QPopupMenu * m_dvdsubtitlemenu;
};


class KMPlayerVCDSource : public KMPlayerMenuSource {
    Q_OBJECT
public:
    KMPlayerVCDSource (KMPlayerApp * app, QPopupMenu * m);
    virtual ~KMPlayerVCDSource ();
    virtual bool processOutput (const QString & line);
public slots:
    virtual void activate ();
    virtual void deactivate ();
    virtual void play ();

    void finished ();
    void trackMenuClicked (int id);
private:
    QRegExp trackRegExp;
    QPopupMenu * m_vcdtrackmenu;
};


class KMPlayerPipeSource : public KMPlayerSource {
    Q_OBJECT
public:
    KMPlayerPipeSource (KMPlayerApp * app);
    virtual ~KMPlayerPipeSource ();
    void setCommand (const QString & cmd) { m_pipe = cmd; }
    const QString & command () const { return m_pipe; }
public slots:
    virtual void activate ();
    virtual void deactivate ();
    virtual void play ();
private:
    KMPlayerApp * app;
    QString m_pipe;
};


class KMPlayerTVSource : public KMPlayerMenuSource {
    Q_OBJECT
public:
    KMPlayerTVSource (KMPlayerApp * app, QPopupMenu * m);
    virtual ~KMPlayerTVSource ();
    //virtual bool processOutput (const QString & line);
    virtual QString filterOptions ();
    void readConfig (KConfig * config);
public slots:
    virtual void activate ();
    virtual void deactivate ();
    virtual void play ();

    //void finished ();
    void menuClicked (int id);
private:
    struct Device {
        QSize size;
        int menuid;
    };
    typedef QMap <int, QPair <Device *, QString> > CommandMap;
    QString m_driver;
    Device * m_device;
    QString m_command;
    QPtrList <Device> devices;
    CommandMap commands;
    QPopupMenu * m_channelmenu;
};

#endif // KMPLAYERSOURCE_H
