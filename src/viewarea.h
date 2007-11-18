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

#ifndef KMPLAYER_VIEW_AREA_H
#define KMPLAYER_VIEW_AREA_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qxembed.h>

#include "mediaobject.h"

class KActionCollection;

namespace KMPlayer {

class View;
class IViewer;
class ViewAreaPrivate;

/*
 * The area in which the video widget and controlpanel are laid out
 */
class KMPLAYER_EXPORT ViewArea : public QWidget {
    Q_OBJECT
public:
    ViewArea (QWidget * parent, View * view);
    ~ViewArea ();
    KDE_NO_EXPORT bool isFullScreen () const { return m_fullscreen; }
    KDE_NO_EXPORT bool isMinimalMode () const { return m_minimal; }
    KDE_NO_EXPORT KActionCollection * actionCollection () const { return m_collection; }
    KDE_NO_EXPORT QRect topWindowRect () const { return m_topwindow_rect; }
    SurfacePtr getSurface (NodePtr node);
    void mouseMoved ();
    void scheduleRepaint (const IRect &rect);
    void resizeEvent (QResizeEvent *);
    void minimalMode ();
    IViewer *createVideoWidget ();
    void destroyVideoWidget (IViewer *widget);
    void setVideoWidgetVisible (bool show);
public slots:
    void fullScreen ();
    void accelActivated ();
    void scale (int);
protected:
    void showEvent (QShowEvent *);
    void mouseMoveEvent (QMouseEvent *);
    void mousePressEvent (QMouseEvent *);
    void mouseDoubleClickEvent (QMouseEvent *);
    void dragEnterEvent (QDragEnterEvent *);
    void dropEvent (QDropEvent *);
    void contextMenuEvent (QContextMenuEvent * e);
    void paintEvent (QPaintEvent *);
    void timerEvent (QTimerEvent * e);
    void closeEvent (QCloseEvent * e);
    bool x11Event (XEvent *e);
private:
    void syncVisual (const IRect & rect);
    void updateSurfaceBounds ();
    ViewAreaPrivate * d;
    QWidget * m_parent;
    View * m_view;
    KActionCollection * m_collection;
    SurfacePtr surface;
    IRect m_repaint_rect;
    QRect m_topwindow_rect;
    typedef QValueList <IViewer *> VideoWidgetList;
    VideoWidgetList video_widgets;
    int m_mouse_invisible_timer;
    int m_repaint_timer;
    int m_fullscreen_scale;
    int scale_lbl_id;
    int scale_slider_id;
    bool m_fullscreen;
    bool m_minimal;
};

/*
 * The video widget
 */
class KMPLAYER_EXPORT VideoOutput : public QXEmbed, public IViewer {
    Q_OBJECT
public:
    VideoOutput (QWidget *parent, View * view);
    ~VideoOutput ();

    int heightForWidth (int w) const;

    virtual WindowId windowHandle ();
    using QWidget::setGeometry;
    virtual void setGeometry (const IRect &rect);
    virtual void setAspect (float a);
    virtual float aspect () { return m_aspect; }
    virtual void useIndirectWidget (bool);
    virtual void map ();
    virtual void unmap ();

    void sendKeyEvent (int key);
    void setBackgroundColor (const QColor & c);
    void resetBackgroundColor ();
    void setCurrentBackgroundColor (const QColor & c);
    KDE_NO_EXPORT View * view () const { return m_view; }
public slots:
    void sendConfigureEvent ();
protected:
    void dragEnterEvent (QDragEnterEvent *);
    void dropEvent (QDropEvent *);
    void mouseMoveEvent (QMouseEvent * e);
    void contextMenuEvent (QContextMenuEvent * e);
    virtual void windowChanged( WId w );
private:
    WId m_plain_window;
    unsigned int m_bgcolor;
    float m_aspect;
    View * m_view;
};

} // namespace KMPlayer

#endif
