/* This file is part of the KMPlayer application
   Copyright (C) 2004 Koos Vriezen <koos.vriezen@xs4all.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <config.h>
#include <dcopclient.h>
#include <qcstring.h>
#include <qtimer.h>
#include <qfile.h>
#include <qurl.h>
#include <qthread.h>
#include <qmutex.h>
#include <qdom.h>
#include "kmplayer_backend.h"
#include "kmplayer_callback_stub.h"
#include "kmplayer_callback.h"
#include "xvplayer.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>

static char                 configfile[2048];

static Display             *display;
static KXVideoPlayer       *xvapp;
static KMPlayerCallback_stub * callback;
static Window               wid;
static GC                   gc;
static bool                 window_created = true;
static bool                 wants_config;
static bool                 verbose;
static int                  running;
static int                  xvport;
static int                  screen;
Atom xv_hue_atom;
Atom xv_saturation_atom;
Atom xv_brightness_atom;
Atom xv_contrast_atom;
static QString elmentry ("entry");
static QString elmitem ("item");
static QString attname ("NAME");
static QString atttype ("TYPE");
static QString attdefault ("DEFAULT");
static QString attvalue ("VALUE");
static QString attstart ("START");
static QString attend ("END");
static QString valrange ("range");
static QString valnum ("num");
static QString valbool ("bool");
static QString valenum ("enum");
static QString valstring ("string");

extern "C" {

} // extern "C"

static void putVideo () {
    XWindowAttributes attr;
    XGetWindowAttributes (display, wid, &attr);
    XvPutVideo (display, xvport, wid, gc, 0, 0, attr.width, attr.height, 0, 0, attr.width, attr.height);
}

KMPlayerBackend::KMPlayerBackend ()
    : DCOPObject (QCString ("KMPlayerBackend")) {
}

KMPlayerBackend::~KMPlayerBackend () {}

void KMPlayerBackend::setURL (QString) {
}

void KMPlayerBackend::setSubTitleURL (QString) {
}

void KMPlayerBackend::play () {
    xvapp->play ();
}

void KMPlayerBackend::stop () {
    QTimer::singleShot (0, xvapp, SLOT (stop ()));
}

void KMPlayerBackend::pause () {
}

void KMPlayerBackend::seek (int, bool /*absolute*/) {
}

void KMPlayerBackend::hue (int h, bool) {
    xvapp->hue (10 * h);
}

void KMPlayerBackend::saturation (int s, bool) {
    xvapp->saturation (10 * s);
}

void KMPlayerBackend::contrast (int c, bool) {
    xvapp->contrast (10 * c);
}

void KMPlayerBackend::brightness (int b, bool) {
    xvapp->brightness (10 * b);
}

void KMPlayerBackend::volume (int v, bool) {
    xvapp->volume (v);
}

void KMPlayerBackend::quit () {
    delete callback;
    callback = 0L;
    if (running)
        stop ();
    else
        QTimer::singleShot (0, qApp, SLOT (quit ()));
}

bool updateConfigEntry (const QString & name, const QString & value) {
    fprintf (stderr, "%s=%s\n", name.ascii (), (const char *) value.local8Bit ());
    return true;
}

void KMPlayerBackend::setConfig (QByteArray data) {
    QString err;
    int line, column;
    QDomDocument dom;
    if (dom.setContent (data, false, &err, &line, &column)) {
        if (dom.childNodes().length() == 1) {
            for (QDomNode node = dom.firstChild().firstChild();
                    !node.isNull ();
                    node = node.nextSibling ()) {
                QDomNamedNodeMap attr = node.attributes ();
                updateConfigEntry (attr.namedItem (attname).nodeValue (),
                                   attr.namedItem (attvalue).nodeValue ());
            }
        } else
            err = QString ("invalid data");
    }
    if (callback)
        callback->errorMessage (0, err);
}

KXVideoPlayer::KXVideoPlayer (int _argc, char ** _argv)
  : QApplication (_argc, _argv, false) {
}

void KXVideoPlayer::init () {
    int xpos    = 0;
    int ypos    = 0;
    int width   = 320;
    int height  = 200;

    XLockDisplay(display);
    if (window_created)
        wid = XCreateSimpleWindow(display, XDefaultRootWindow(display),
                xpos, ypos, width, height, 1, 0, 0);
    if (!callback)
        QTimer::singleShot (10, this, SLOT (play ()));
    XSelectInput (display, wid,
                  (PointerMotionMask | ExposureMask | KeyPressMask | ButtonPressMask | StructureNotifyMask)); // | SubstructureNotifyMask));
    XvAdaptorInfo * ai;
    unsigned int adaptors;
    if (!xvport && XvQueryAdaptors (display, XDefaultRootWindow (display), &adaptors, &ai) == Success) {
        for (int i = 0; i < adaptors; i++) {
            if ((ai[i].type & XvInputMask) &&
                    (ai[i].type & XvVideoMask) &&
                    ai[i].base_id > 0) {
                fprintf (stderr, "using xvport %d\n", ai[i].base_id);
                xvport = ai[i].base_id;
                break;
            }
            if (adaptors > 0)
                XvFreeAdaptorInfo(ai);
        }
    }
    if (xvport <= 0) {
        fprintf (stderr, "no valid xvport found\n");
        return;
    }
    if (XvGrabPort (display, xvport, CurrentTime) == Success) {
        gc = XCreateGC (display, wid, 0, NULL);
        XvSelectPortNotify (display, xvport, 1);
        XvSelectVideoNotify (display, wid, 1);
        running = true;
    }
    if (window_created) {
        fprintf (stderr, "map %lu\n", wid);
        XMapRaised(display, wid);
        XSync(display, False);
    }
    XUnlockDisplay(display);
}

KXVideoPlayer::~KXVideoPlayer () {
    if (window_created) {
        XLockDisplay (display);
        fprintf (stderr, "unmap %lu\n", wid);
        XUnmapWindow (display,  wid);
        XDestroyWindow(display,  wid);
        XSync (display, False);
        XUnlockDisplay (display);
    }
    xvapp = 0L;
}

void getConfigEntries (QByteArray & buf) {
    QDomDocument doc;
    QDomElement root = doc.createElement (QString ("document"));
    doc.appendChild (root);
    QCString exp = doc.toCString ();
    buf = exp;
    buf.resize (exp.length ()); // strip terminating \0
}

void KXVideoPlayer::play () {
    fprintf (stderr, "play\n");
    if (running) {
        XLockDisplay (display);
        putVideo ();
        XUnlockDisplay (display);
    }
}

void KXVideoPlayer::stop () {
    if (running) {
        XLockDisplay (display);
        XvStopVideo (display, xvport, wid);
        XvUngrabPort (display, xvport, CurrentTime);
        XClearArea (display, wid, 0, 0, 0, 0, true);
        running = false;
        XUnlockDisplay (display);
    }
    QTimer::singleShot (0, qApp, SLOT (quit ()));
}

void KXVideoPlayer::finished () {
    QTimer::singleShot (10, this, SLOT (stop ()));
}

void KXVideoPlayer::saturation (int val) {
    XLockDisplay(display);
    XvSetPortAttribute (display, xvport, xv_saturation_atom, val);
    XUnlockDisplay(display);
}

void KXVideoPlayer::hue (int val) {
    XLockDisplay(display);
    XvSetPortAttribute (display, xvport, xv_hue_atom, val);
    XUnlockDisplay(display);
}

void KXVideoPlayer::contrast (int val) {
    XLockDisplay(display);
    XvSetPortAttribute (display, xvport, xv_contrast_atom, val);
    XUnlockDisplay(display);
}

void KXVideoPlayer::brightness (int val) {
    XLockDisplay(display);
    XvSetPortAttribute (display, xvport, xv_brightness_atom, val);
    XUnlockDisplay(display);
}

void KXVideoPlayer::volume (int) {
}

class XEventThread : public QThread {
protected:
    void run () {
        while (true) {
            XEvent   xevent;
            XNextEvent(display, &xevent);
            switch(xevent.type) {
                case ClientMessage:
                    if (xevent.xclient.format == 8 &&
                            !strncmp(xevent.xclient.data.b, "quit_now", 8)) {
                        fprintf(stderr, "request quit\n");
                        return;
                    }
                    break;
                case KeyPress: {
                    XKeyEvent  kevent;
                    KeySym     ksym;
                    char       kbuf[256];
                    int        len;
                    kevent = xevent.xkey;
                    XLockDisplay(display);
                    len = XLookupString(&kevent, kbuf, sizeof(kbuf), &ksym, NULL);
                    XUnlockDisplay(display);
                    fprintf(stderr, "keypressed 0x%x 0x%x\n", kevent.keycode, ksym);
                    switch (ksym) {
                        case XK_q:
                        case XK_Q:
                            xvapp->lock ();
                            xvapp->stop ();
                            xvapp->unlock ();
                            break;
                    }
                    break;
                }
                case Expose:
                    if(xevent.xexpose.count != 0 || xevent.xexpose.window != wid)
                        break;
                    break;

                case ConfigureNotify:
                    putVideo ();
                    break;
                default:
                    if (xevent.type < LASTEvent)
                        ;//fprintf (stderr, "event %d\n", xevent.type);
            }
        }
    }
};

int main(int argc, char **argv) {
    if (!XInitThreads ()) {
        fprintf (stderr, "XInitThreads () failed\n");
        return 1;
    }
    display = XOpenDisplay(NULL);
    screen  = XDefaultScreen(display);

    unsigned int ver, rel, req, evb, err;
    if (XvQueryExtension (display, &ver, &rel, &req, &evb, &err) != Success) {
        fprintf (stderr, "XVideo not supported on display\n");
        XCloseDisplay (display);
        return 1;
    }
    xv_hue_atom = XInternAtom (display, "XV_HUE", false);
    xv_saturation_atom = XInternAtom (display, "XV_SATURATION", false);
    xv_brightness_atom = XInternAtom (display, "XV_BRIGHTNESS", false);
    xv_contrast_atom = XInternAtom (display, "XV_CONTRAST", false);

    xvapp = new KXVideoPlayer (argc, argv);

    for(int i = 1; i < argc; i++) {
        if (!strcmp (argv [i], "-port")) {
            xvport = strtol (argv [++i], 0L, 10);
        } else if (!strcmp (argv [i], "-wid") || !strcmp (argv [i], "-window-id")) {
            wid = atol (argv [++i]);
            window_created = false;
        } else if (!strcmp (argv [i], "-root")) {
            wid =  XDefaultRootWindow (display);
            window_created = false;
        } else if (!strcmp (argv [i], "-window")) {
            ;
        } else if (!strcmp (argv [i], "-v")) {
            verbose = true;
        } else if (!strcmp (argv [i], "-c")) {
            wants_config = true;
        } else if (!strcmp (argv [i], "-f") && i < argc - 1) {
            strncpy (configfile, argv [++i], sizeof (configfile));
            configfile[sizeof (configfile) - 1] = 0;
        } else if (!strcmp (argv [i], "-cb")) {
            QString str = argv [++i];
            int pos = str.find ('/');
            if (pos > -1) {
                fprintf (stderr, "callback is %s %s\n", str.left (pos).ascii (), str.mid (pos + 1).ascii ());
                callback = new KMPlayerCallback_stub 
                    (str.left (pos).ascii (), str.mid (pos + 1).ascii ());
            }
        } else  {
            fprintf (stderr, "usage: %s [-port <xv port>] [-f <config file>] [-v] [(-wid|-window-id) <window>] [(-root|-window)] [-cb <DCOP callback name> [-c]]\n", argv[0]);
            delete xvapp;
            return 1;
        }
    }

    DCOPClient dcopclient;
    dcopclient.registerAs ("kxvideoplayer");
    KMPlayerBackend player;

    XEventThread * eventThread = new XEventThread;
    eventThread->start ();

    xvapp->init ();

    if (callback) {
        QByteArray buf;
        if (wants_config)
            getConfigEntries (buf);
        callback->started (buf);
    }
    xvapp->exec ();

    XLockDisplay(display);
    XClientMessageEvent ev = {
        ClientMessage, 0, true, display, wid, 
        XInternAtom (display, "XVIDEO", false), 8, {b: "quit_now"}
    };
    XSendEvent (display, wid, FALSE, StructureNotifyMask, (XEvent *) & ev);
    XFlush (display);
    XUnlockDisplay(display);
    eventThread->wait (500);
    delete eventThread;

    xvapp->stop ();
    delete xvapp;

    fprintf (stderr, "closing display\n");
    XCloseDisplay (display);
    fprintf (stderr, "done\n");
    return 0;
}

#include "xvplayer.moc"