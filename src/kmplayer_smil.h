/* This file is part of the KDE project
 *
 * Copyright (C) 2005-2006 Koos Vriezen <koos.vriezen@xs4all.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KMPLAYER_SMILL_H_
#define _KMPLAYER_SMILL_H_

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>

#include "kmplayerplaylist.h"

class QTextStream;
class QPixmap;
class QPainter;

namespace KIO {
    class Job;
}

namespace KMPlayer {

class ElementRuntimePrivate;
class RemoteObjectData;
class TextDataPrivate;

/*
 * Event signaled before the actual starting takes place. Use by SMIL::Excl
 * to stop possible other children
 */
class ToBeStartedEvent : public Event {
public:
    ToBeStartedEvent (NodePtr n);
    NodePtrW node;
};

/*
 * Interpretation of sizes
 */
class KMPLAYER_NO_EXPORT SizeType {
public:
    SizeType ();
    void reset ();
    SizeType & operator = (const QString & s);
    int size (int relative_to = 100);
    bool isSet () const { return isset; }
private:
    int m_size;
    bool percentage;
    bool isset;
};

/**
 * Base class representing live time of elements
 */
class ElementRuntime : public Item <ElementRuntime> {
public:
    virtual ~ElementRuntime ();
    /**
     * calls reset() and pulls in the attached_element's attributes
     */
    virtual void init ();
    /**
     * Called when element is pulled in scope, from Node::activate()
     */
    virtual void begin () {}
    /**
     * Called when element gets out of scope, from Node::reset()
     */
    virtual void end () {}
    /**
     * Reset all data, called from end() and init()
     */
    virtual void reset ();
    /**
     * Change behaviour of this runtime, returns old value. If id is not NULL,
     * the value is treated as a modification on an existing value
     */
    QString setParam (const QString & name, const QString & value, int * id=0L);
    /**
     * get the current value of param name that's set by setParam(name,value)
     */
    QString param (const QString & name);
    /**
     * reset param with id as modification
     */
    void resetParam (const QString & name, int id);
    /**
     * called from setParam for specialized interpretation of params
     */
    virtual void parseParam (const QString &, const QString &) {}

    /**
     * If this element is attached to a region, region_node points to it
     */
    NodePtrW region_node;
protected:
    ElementRuntime (NodePtr e);
    NodePtrW element;
private:
    ElementRuntimePrivate * d;
};

ITEM_AS_POINTER(KMPlayer::ElementRuntime)

/**
 * For RegPoint, RegionRuntime and MediaRuntime, having sizes
 */
class KMPLAYER_NO_EXPORT CalculatedSizer {
public:
    KDE_NO_CDTOR_EXPORT CalculatedSizer () {}
    KDE_NO_CDTOR_EXPORT ~CalculatedSizer () {}

    void resetSizes ();
    void calcSizes (Node *, int w, int h, int & xoff, int & yoff, int & w1, int & h1);
    SizeType left, top, width, height, right, bottom;
    QString reg_point, reg_align;
    bool setSizeParam (const QString & name, const QString & value);
};

/**
 * Live representation of a SMIL element having timings
 */
class KMPLAYER_NO_EXPORT TimedRuntime : public ElementRuntime {
public:
    enum TimingState {
        timings_reset = 0, timings_began, timings_started, timings_stopped
    };
    enum DurationTime { begin_time = 0, duration_time, end_time, durtime_last };
    enum Fill { fill_unknown, fill_freeze };

    TimedRuntime (NodePtr e);
    virtual ~TimedRuntime ();
    virtual void begin ();
    virtual void end ();
    virtual void reset ();
    virtual void parseParam (const QString & name, const QString & value);
    TimingState state () const { return timingstate; }
    virtual void paint (QPainter &) {}
    void propagateStop (bool forced);
    void propagateStart ();
    void processEvent (unsigned int event);
    /**
     * Duration items, begin/dur/end, length information or connected element
     */
    struct DurationItem {
        DurationItem () : durval (0) {}
        unsigned int durval;
        ConnectionPtr connection;
    } durations [(const int) durtime_last];
    virtual void started ();
    virtual void stopped ();
private:
    void setDurationItem (DurationTime item, const QString & val);
protected:
    TimingState timingstate;
    Fill fill;
    TimerInfoPtrW start_timer;
    TimerInfoPtrW dur_timer;
    int repeat_count;
};

/**
 * Runtime data for a region
 */
class KMPLAYER_NO_EXPORT RegionRuntime : public ElementRuntime {
public:
    RegionRuntime (NodePtr e);
    KDE_NO_CDTOR_EXPORT ~RegionRuntime () {}
    virtual void begin ();
    virtual void end ();
    virtual void reset ();
    virtual void parseParam (const QString & name, const QString & value);
    CalculatedSizer sizes;
    unsigned int background_color;
    bool have_bg_color;
private:
    bool active;
};

/**
 * Runtime data for a regPoint
 */
class KMPLAYER_NO_EXPORT RegPointRuntime : public ElementRuntime {
public:
    RegPointRuntime (NodePtr e);
    KDE_NO_CDTOR_EXPORT ~RegPointRuntime () {}
    virtual void parseParam (const QString & name, const QString & value);
    CalculatedSizer sizes;
};

class KMPLAYER_NO_EXPORT RemoteObject : public QObject {
    Q_OBJECT
public:
    RemoteObject ();
    KDE_NO_CDTOR_EXPORT ~RemoteObject () {}
    bool wget (const KURL & url);
    void killWGet ();
    void clear ();
    QString mimetype ();
private slots:
    void slotResult (KIO::Job*);
    void slotData (KIO::Job*, const QByteArray& qb);
    void slotMimetype (KIO::Job * job, const QString & mimestr);
protected:
    KDE_NO_EXPORT virtual void remoteReady () {}
    KURL m_url;
    KIO::Job * m_job;
    QByteArray m_data;
    QString m_mime;
};

/**
 * Some common runtime data for all mediatype classes
 */
class KMPLAYER_NO_EXPORT MediaTypeRuntime : public RemoteObject, public TimedRuntime {
public:
    ~MediaTypeRuntime ();
    virtual void end ();
    virtual void started ();
    virtual void stopped ();
    virtual void parseParam (const QString & name, const QString & value);
    virtual void paint (QPainter &) {}
    virtual void postpone (bool b);
    CalculatedSizer sizes;
protected:
    MediaTypeRuntime (NodePtr e);
    void checkedPostpone ();
    void checkedProceed ();
    ConnectionPtr document_postponed;      // pauze audio/video accordantly
    ConnectionPtr region_sized;            // attached region is sized
    ConnectionPtr region_paint;            // attached region needs painting
    ConnectionPtr region_mouse_enter;      // attached region has mouse entered
    ConnectionPtr region_mouse_leave;      // attached region has mouse left
    ConnectionPtr region_mouse_click;      // attached region is clicked
    QString source_url;
    Fit fit;
    bool needs_proceed;
};

/**
 * Data needed for audio/video clips
 */
class KMPLAYER_NO_EXPORT AudioVideoData : public MediaTypeRuntime {
public:
    AudioVideoData (NodePtr e);
    virtual bool isAudioVideo ();
    virtual void parseParam (const QString & name, const QString & value);
    virtual void started ();
    virtual void stopped ();
    virtual void postpone (bool b);
    void avStopped ();
};

/**
 * Data needed for an image
 */
class KMPLAYER_NO_EXPORT ImageData {
public:
    ImageData ();
    ~ImageData ();
    QPixmap * image;
    QPixmap * cache_image; // scaled cache
    QMovie * img_movie;
    QString url;
    bool have_frame;
};

class KMPLAYER_NO_EXPORT ImageRuntime : public MediaTypeRuntime {
    Q_OBJECT
public:
    ImageRuntime (NodePtr e);
    ~ImageRuntime ();
    void paint (QPainter & p);
    virtual void parseParam (const QString & name, const QString & value);
    virtual void postpone (bool b);
    ImageData * d;
protected:
    virtual void started ();
    virtual void stopped ();
    virtual void remoteReady ();
private slots:
    void movieUpdated (const QRect &);
    void movieStatus (int);
    void movieResize (const QSize &);
};

/**
 * Data needed for text
 */
class KMPLAYER_NO_EXPORT TextData : public MediaTypeRuntime {
public:
    TextData (NodePtr e);
    ~TextData ();
    void paint (QPainter & p);
    void end ();
    virtual void parseParam (const QString & name, const QString & value);
    TextDataPrivate * d;
protected:
    virtual void started ();
    virtual void remoteReady ();
};

/**
 * Stores runtime data of elements from animate group set/animate/..
 */
class KMPLAYER_NO_EXPORT AnimateGroupData : public TimedRuntime {
public:
    KDE_NO_CDTOR_EXPORT ~AnimateGroupData () {}
    virtual void parseParam (const QString & name, const QString & value);
    virtual void reset ();
protected:
    void restoreModification ();
    AnimateGroupData (NodePtr e);
    NodePtrW target_element;
    NodePtrW target_region;
    QString changed_attribute;
    QString change_to;
    int modification_id;
protected:
    virtual void stopped ();
};

/**
 * Stores runtime data of set element
 */
class KMPLAYER_NO_EXPORT SetData : public AnimateGroupData {
public:
    KDE_NO_CDTOR_EXPORT SetData (NodePtr e) : AnimateGroupData (e) {}
    KDE_NO_CDTOR_EXPORT ~SetData () {}
protected:
    virtual void started ();
};

/**
 * Stores runtime data of animate element
 */
class KMPLAYER_NO_EXPORT AnimateData : public AnimateGroupData {
public:
    AnimateData (NodePtr e);
    KDE_NO_CDTOR_EXPORT ~AnimateData () {}
    virtual void parseParam (const QString & name, const QString & value);
    virtual void reset ();
    virtual void started ();
    virtual void stopped ();
    void timerTick();
private:
    TimerInfoPtrW anim_timer;
    enum { acc_none, acc_sum } accumulate;
    enum { add_replace, add_sum } additive;
    int change_by;
    enum { calc_discrete, calc_linear, calc_paced } calcMode;
    QString change_from;
    QStringList change_values;
    int steps;
    float change_delta, change_to_val, change_from_val;
    QString change_from_unit;
};

/**
 * Translates string to deci-seconds or 'special' high number
 */
bool parseTime (const QString & val, unsigned int & dur /*, const QString & dateformat*/);

//-----------------------------------------------------------------------------

namespace SMIL {

const short id_node_smil = 100;
const short id_node_head = 101;
const short id_node_body = 102;
const short id_node_layout = 103;
const short id_node_root_layout = 104;
const short id_node_region = 105;
const short id_node_regpoint = 106;
const short id_node_par = 110;
const short id_node_seq = 111;
const short id_node_switch = 112;
const short id_node_excl = 113;
const short id_node_img = 120;
const short id_node_audio_video = 121;
const short id_node_text = 122;
const short id_node_set = 132;
const short id_node_animate = 133;
const short id_node_title = 140;
const short id_node_param = 141;
const short id_node_first = id_node_smil;
const short id_node_first_timed_mrl = id_node_par;
const short id_node_last_timed_mrl = id_node_animate;
const short id_node_last = 200; // reserve 100 ids

inline bool isTimedMrl (const NodePtr & n) {
    return n->id >= id_node_first_timed_mrl && n->id <= id_node_last_timed_mrl;
}

/**
 * '<smil>' tag
 */
class Smil : public Mrl {
public:
    KDE_NO_CDTOR_EXPORT Smil (NodePtr & d) : Mrl (d, id_node_smil) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "smil"; }
    bool isMrl ();
    void activate ();
    void deactivate ();
    void closed ();
    bool expose () const;
    /**
     * Hack to mark the currently playing MediaType as finished
     * FIXME: think of a descent callback way for this
     */
    NodePtr realMrl ();
    NodePtrW current_av_media_type;
    NodePtrW layout_node;
};

/**
 * Represents optional 'head' tag of SMIL document as in
 * &lt;smil&gt;&lt;head/&gt;&lt;body/&gt;&lt;/smil&gt;
 */
class KMPLAYER_NO_EXPORT Head : public Element {
public:
    KDE_NO_CDTOR_EXPORT Head (NodePtr & d) : Element (d, id_node_head) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "head"; }
    void closed ();
    bool expose () const;
};

/**
 * Base class for SMIL::Region, SMIL::RootLayout and SMIL::Layout
 */
class KMPLAYER_NO_EXPORT RegionBase : public Element {
public:
    virtual ElementRuntimePtr getRuntime ();
    bool expose () const { return false; }
    virtual bool handleEvent (EventPtr event);
    /**
     * repaints region, calls scheduleRepaint(x,y,w,h) on view
     */
    void repaint ();
    /**
     * calculate the relative x,y,w,h on the child region elements
     * given this element's w and h value
     * and child's left/top/right/width/height/bottom attributes
     */
    virtual void updateDimensions ();

    /**
     * Creates a new transform matrix
     */
    Matrix transform ();
    int x, y, w, h;     // unscaled values
    /**
     * z-order of this region
     */
    int z_order;
protected:
    RegionBase (NodePtr & d, short id);
    ElementRuntimePtr runtime;
    virtual NodeRefListPtr listeners (unsigned int event_id);
    Matrix m_region_transform;
    NodeRefListPtr m_SizeListeners;        // region resized
    NodeRefListPtr m_PaintListeners;       // region need repainting
};

/**
 * Defines region layout, should reside below 'head' element
 */
class KMPLAYER_NO_EXPORT Layout : public RegionBase {
public:
    Layout (NodePtr & d);
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "layout"; }
    void activate ();
    void closed ();
    virtual bool handleEvent (EventPtr event);
    /**
     * recursively calculates dimensions of this and child regions
     */
    virtual void updateDimensions ();

    NodePtrW rootLayout;
};

/**
 * Represents a rectangle on the viewing area
 */
class KMPLAYER_NO_EXPORT Region : public RegionBase {
public:
    Region (NodePtr & d);
    KDE_NO_EXPORT const char * nodeName () const { return "region"; }
    NodePtr childFromTag (const QString & tag);
    void calculateBounds (int w, int h, const Matrix & parent_transform);
    virtual bool handleEvent (EventPtr event);
    virtual NodeRefListPtr listeners (unsigned int event_id);
    void addRegionUser (NodePtr mt);
private:
    NodeRefListPtr m_ActionListeners;      // mouse clicked
    NodeRefListPtr m_OutOfBoundsListeners; // mouse left
    NodeRefListPtr m_InBoundsListeners;    // mouse entered
    /**
     * boolean for check if pointerEntered/pointerLeft should be called by View
     */
    bool has_mouse;
};

/**
 * Represents the root area for the other regions
 */
class KMPLAYER_NO_EXPORT RootLayout : public RegionBase {
public:
    KDE_NO_CDTOR_EXPORT RootLayout (NodePtr & d)
        : RegionBase (d, id_node_root_layout) {}
    KDE_NO_EXPORT const char * nodeName () const { return "root-layout"; }
};

/**
 * Represents a regPoint element for alignment inside regions
 */
class KMPLAYER_NO_EXPORT RegPoint : public Element {
public:
    KDE_NO_CDTOR_EXPORT RegPoint (NodePtr & d) : Element (d, id_node_regpoint){}
    KDE_NO_CDTOR_EXPORT ~RegPoint () {}
    KDE_NO_EXPORT const char * nodeName () const { return "regPoint"; }
    KDE_NO_EXPORT bool expose () const { return false; }
    ElementRuntimePtr getRuntime ();
    ElementRuntimePtr runtime;
};

/**
 * Base for all SMIL media elements having begin/dur/end/.. attributes
 */
class KMPLAYER_NO_EXPORT TimedMrl : public Mrl {
public:
    KDE_NO_CDTOR_EXPORT ~TimedMrl () {}
    ElementRuntimePtr getRuntime ();
    void activate ();
    void begin ();
    void finish ();
    void deactivate ();
    void reset ();
    void childBegan (NodePtr child);
    void childDone (NodePtr child);
    virtual bool handleEvent (EventPtr event);
    TimedRuntime * timedRuntime ();
protected:
    TimedMrl (NodePtr & d, short id);
    virtual NodeRefListPtr listeners (unsigned int event_id);
    virtual ElementRuntimePtr getNewRuntime ();

    NodeRefListPtr m_StartedListeners;      // Element about to be started
    NodeRefListPtr m_StoppedListeners;      // Element stopped
    ElementRuntimePtr runtime;
};

KDE_NO_EXPORT inline TimedRuntime * TimedMrl::timedRuntime () {
    if (!runtime)
        runtime = getNewRuntime ();
    return static_cast <TimedRuntime *> (runtime.ptr ());
}

/**
 * Abstract base for the group elements (par/seq/excl/..)
 */
class KMPLAYER_NO_EXPORT GroupBase : public TimedMrl {
public:
    KDE_NO_CDTOR_EXPORT ~GroupBase () {}
    bool isMrl ();
    bool expose () const;
    void finish ();
protected:
    KDE_NO_CDTOR_EXPORT GroupBase (NodePtr & d, short id) : TimedMrl (d, id) {}
};

/**
 * A Par represents parallel processing of all its children
 */
class KMPLAYER_NO_EXPORT Par : public GroupBase {
public:
    KDE_NO_CDTOR_EXPORT Par (NodePtr & d) : GroupBase (d, id_node_par) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "par"; }
    void begin ();
    void reset ();
    void childDone (NodePtr child);
};

/**
 * A Seq represents sequential processing of all its children
 */
class KMPLAYER_NO_EXPORT Seq : public GroupBase {
public:
    KDE_NO_CDTOR_EXPORT Seq (NodePtr & d) : GroupBase(d, id_node_seq) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "seq"; }
    void begin ();
protected:
    KDE_NO_CDTOR_EXPORT Seq (NodePtr & d, short id) : GroupBase(d, id) {}
};

/**
 * Represents the 'body' tag of SMIL document as in
 * &lt;smil&gt;&lt;head/&gt;&lt;body/&gt;&lt;/smil&gt;
 */
class KMPLAYER_NO_EXPORT Body : public Seq {
public:
    KDE_NO_CDTOR_EXPORT Body (NodePtr & d) : Seq (d, id_node_body) {}
    KDE_NO_EXPORT const char * nodeName () const { return "body"; }
    bool expose () const { return false; }
};

/**
 * An Excl represents exclusive processing of one of its children
 */
class KMPLAYER_NO_EXPORT Excl : public GroupBase {
public:
    KDE_NO_CDTOR_EXPORT Excl (NodePtr & d) : GroupBase (d, id_node_excl) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "excl"; }
    void begin ();
    void deactivate ();
    void childDone (NodePtr child);
    virtual bool handleEvent (EventPtr event);
private:
    typedef ListNode <ConnectionPtr> ConnectionStoreItem;
    List <ConnectionStoreItem> started_event_list;
};

/*
 * An automatic selection between child elements based on a condition
 */
class KMPLAYER_NO_EXPORT Switch : public GroupBase {
public:
    KDE_NO_CDTOR_EXPORT Switch (NodePtr &d) : GroupBase (d, id_node_switch) {}
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return "switch"; }
    bool isMrl ();
    // Condition
    void activate ();
    void deactivate ();
    void reset ();
    void childDone (NodePtr child);
    NodePtr realMrl ();
    NodePtrW chosenOne;
};

/**
 * Abstract base for the MediaType classes (video/audio/text/img/..)
 */
class KMPLAYER_NO_EXPORT MediaType : public TimedMrl {
public:
    MediaType (NodePtr & d, const QString & t, short id);
    NodePtr childFromTag (const QString & tag);
    KDE_NO_EXPORT const char * nodeName () const { return m_type.latin1 (); }
    void opened ();
    void activate ();
    virtual bool handleEvent (EventPtr event);
    QString m_type;
    unsigned int bitrate;
protected:
    NodeRefListPtr listeners (unsigned int event_id);
    NodeRefListPtr m_ActionListeners;      // mouse clicked
    NodeRefListPtr m_OutOfBoundsListeners; // mouse left
    NodeRefListPtr m_InBoundsListeners;    // mouse entered
};

class KMPLAYER_NO_EXPORT AVMediaType : public MediaType {
public:
    AVMediaType (NodePtr & d, const QString & t);
    NodePtr childFromTag (const QString & tag);
    void positionVideoWidget ();
    virtual ElementRuntimePtr getNewRuntime ();
    virtual void undefer ();
    virtual void activate ();
    virtual void finish ();
    virtual bool handleEvent (EventPtr event);
};

class KMPLAYER_NO_EXPORT ImageMediaType : public MediaType {
public:
    ImageMediaType (NodePtr & d);
    ElementRuntimePtr getNewRuntime ();
    NodePtr childFromTag (const QString & tag);
};

class KMPLAYER_NO_EXPORT TextMediaType : public MediaType {
public:
    TextMediaType (NodePtr & d);
    ElementRuntimePtr getNewRuntime ();
};

class KMPLAYER_NO_EXPORT Set : public TimedMrl {
public:
    KDE_NO_CDTOR_EXPORT Set (NodePtr & d) : TimedMrl (d, id_node_set) {}
    KDE_NO_EXPORT const char * nodeName () const { return "set"; }
    virtual ElementRuntimePtr getNewRuntime ();
    bool isMrl () { return false; }
};

class KMPLAYER_NO_EXPORT Animate : public TimedMrl {
public:
    KDE_NO_CDTOR_EXPORT Animate (NodePtr & d) : TimedMrl (d, id_node_animate) {}
    KDE_NO_EXPORT const char * nodeName () const { return "animate"; }
    virtual ElementRuntimePtr getNewRuntime ();
    bool isMrl () { return false; }
    bool handleEvent (EventPtr event);
};

class KMPLAYER_NO_EXPORT Param : public Element {
public:
    KDE_NO_CDTOR_EXPORT Param (NodePtr & d) : Element (d, id_node_param) {}
    KDE_NO_EXPORT const char * nodeName () const { return "param"; }
    void activate ();
    bool expose () const { return false; }
};

} // SMIL namespace

}  // KMPlayer namespace

#endif //_KMPLAYER_SMIL_H_
