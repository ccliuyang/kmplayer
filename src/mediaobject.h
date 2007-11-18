/**
  This file belong to the KMPlayer project, a movie player plugin for Konqueror
  Copyright (C) 2007  Koos Vriezen <koos.vriezen@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**/

#ifndef _KMPLAYER__MEDIA_OBJECT_H_
#define _KMPLAYER__MEDIA_OBJECT_H_

#include <config.h>

#include <qobject.h>
#include <qvaluelist.h>
#include <qmap.h>
#include <qstring.h>

#include "kmplayer_def.h"
#include "kmplayerplaylist.h"

class QMovie;
class QTextCodec;
class QImage;
namespace KIO {
    class Job;
}

namespace KMPlayer {

extern const unsigned int event_data_arrived;
extern const unsigned int event_img_updated;
extern const unsigned int event_img_anim_finished;

class IViewer;
class PartBase;
class Process;
class ProcessInfo;
class Viewer;
class AudioVideoMedia;


class KMPLAYER_EXPORT IProcess {
public:
    enum State { NotRunning = 0, Ready, Buffering, Playing };

    virtual ~IProcess () {}

    virtual bool ready () = 0;
    virtual bool play () = 0;
    virtual void pause () = 0;
    virtual void stop () = 0;
    virtual void quit () = 0;
    /* seek (pos, abs) seek position in deci-seconds */
    virtual bool seek (int pos, bool absolute) = 0;;
    /* volume from 0 to 100 */
    virtual bool volume (int pos, bool absolute) = 0;
    virtual bool saturation (int pos, bool absolute) = 0;
    virtual bool hue (int pos, bool absolute) = 0;
    virtual bool contrast (int pos, bool absolute) = 0;
    virtual bool brightness (int pos, bool absolute) = 0;
    virtual void setAudioLang (int, const QString &) = 0;
    virtual void setSubtitle (int, const QString &) = 0;
    virtual bool running () const = 0;

    State state () const { return m_state; }
    void setMediaObject (AudioVideoMedia *media) { media_object = media; }

protected:
    IProcess ();

    AudioVideoMedia *media_object;
    State m_state;

private:
    IProcess (const IViewer &);
};

/*
 * Class that creates MediaObject and keeps track objects
 */
class KMPLAYER_EXPORT MediaManager {
public:
    enum MediaType { Audio, AudioVideo, Image, Text };
    typedef QMap <QString, ProcessInfo *> ProcessInfoMap;
    typedef QValueList <IProcess *> ProcessList;
    typedef QValueList <MediaObject *> MediaList;

    MediaManager (PartBase *player);
    ~MediaManager ();

    MediaObject *createMedia (MediaType type, Node *node);

    // backend process state changed
    void stateChange (AudioVideoMedia *m, IProcess::State o, IProcess::State n);
    void playAudioVideo (AudioVideoMedia *m);

    IProcess *process (AudioVideoMedia *media);
    void processDestroyed (IProcess *process);
    ProcessInfoMap &processInfos () { return m_process_infos; }
    ProcessList &processes () { return m_processes; }
    MediaList &medias () { return m_media_objects; }
    PartBase *player () const { return m_player; }

private:
    MediaList m_media_objects;
    ProcessInfoMap m_process_infos;
    ProcessList m_processes;
    PartBase *m_player;
};


//------------------------%<----------------------------------------------------

/*
 * Abstract base of MediaObject types, handles downloading
 */

class KMPLAYER_NO_EXPORT DataCache : public QObject {
    Q_OBJECT
    typedef QMap <QString, QByteArray> DataMap;
    typedef QMap <QString, bool> PreserveMap;
    DataMap cache_map;
    PreserveMap preserve_map;
public:
    DataCache () {}
    ~DataCache () {}
    void add (const QString &, const QByteArray &);
    bool get (const QString &, QByteArray &);
    bool preserve (const QString &);
    bool unpreserve (const QString &);
    bool isPreserved (const QString &);
signals:
    void preserveRemoved (const QString &); // ready or canceled
};

class KMPLAYER_EXPORT MediaObject : public QObject {
    Q_OBJECT
    friend class MediaManager;
public:
    virtual MediaManager::MediaType type () const = 0;

    virtual bool play () = 0;
    virtual void pause () {}
    virtual void unpause () {}
    virtual void stop () {}
    virtual void destroy ();

    bool wget (const QString & url);
    void killWGet ();
    void clearData ();
    QString mimetype ();
    bool downloading () const;

    Mrl *mrl ();
    QByteArray &rawData () { return data; }

private slots:
    void slotResult (KIO::Job *);
    void slotData (KIO::Job *, const QByteArray &qb);
    void slotMimetype (KIO::Job *job, const QString &mimestr);
    void cachePreserveRemoved (const QString &);

protected:
    MediaObject (MediaManager *manager, Node *node);
    virtual ~MediaObject ();

    virtual void remoteReady (const QString &url);

    MediaManager *m_manager;
    NodePtrW m_node;
    QString url;
    QByteArray data;
    QString mime;
    KIO::Job *job;
    bool preserve_wait;
};


//------------------------%<----------------------------------------------------

/*
 * MediaObject for audio/video, groups Mrl, Process and Viewer
 */

typedef unsigned long WindowId;

class AudioVideoMedia;

class KMPLAYER_NO_EXPORT IViewer {
public:
    IViewer () {}
    virtual ~IViewer () {}

    virtual WindowId windowHandle () = 0;
    virtual void setGeometry (const IRect &rect) = 0;
    virtual void setAspect (float a) = 0;
    virtual float aspect () = 0;
    virtual void useIndirectWidget (bool) = 0;
    virtual void map () = 0;
    virtual void unmap () = 0;
private:
    IViewer (const IViewer &);
};

class KMPLAYER_NO_EXPORT AudioVideoMedia : public MediaObject {
    friend class MediaManager;
public:
    enum Request { ask_nothing, ask_play, ask_stop, ask_delete };

    AudioVideoMedia (MediaManager *manager, Node *node);

    MediaManager::MediaType type () const { return MediaManager::AudioVideo; }

    virtual bool play ();
    virtual void stop ();
    virtual void pause ();
    virtual void unpause ();
    virtual void destroy ();

    IProcess *process;
    IViewer *viewer;
    Request request;

protected:
    ~AudioVideoMedia ();
};


//------------------------%<----------------------------------------------------

/*
 * MediaObject for (animated)images
 */

struct KMPLAYER_NO_EXPORT ImageData {
    ImageData( const QString & img);
    ~ImageData();
    QImage *image;
private:
    QString url;
};

typedef SharedPtr <ImageData> ImageDataPtr;
typedef WeakPtr <ImageData> ImageDataPtrW;

class KMPLAYER_NO_EXPORT ImageMedia : public MediaObject {
    Q_OBJECT
public:
    ImageMedia (MediaManager *manager, Node *node);

    MediaManager::MediaType type () const { return MediaManager::Image; }

    bool play ();
    void stop ();
    void pause ();
    void unpause ();

    void setUrl (const QString & url);
    bool isEmpty ();
    ImageDataPtr cached_img;

private slots:
    void movieUpdated (const QRect &);
    void movieStatus (int);
    void movieResize (const QSize &);

protected:
    ~ImageMedia ();

    void remoteReady (const QString &url);

private:
    QMovie *img_movie;
    int frame_nr;
};

//------------------------%<----------------------------------------------------

/*
 * MediaObject for text
 */
class KMPLAYER_NO_EXPORT TextMedia : public MediaObject {
public:
    TextMedia (MediaManager *manager, Node *node);

    MediaManager::MediaType type () const { return MediaManager::Text; }

    bool play ();

    QString text;
    QTextCodec *codec;
    int default_font_size;

protected:
    ~TextMedia ();

    void remoteReady (const QString &url);
};

} // namespace

#endif