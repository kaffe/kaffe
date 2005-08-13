#ifndef CALLBACKEVENT_H
#define CALLBACKEVENT_H

#include <jni.h>
#include <QEvent>
#include <QColor>
#include <QCursor>
#include <QFont>
#include <QPoint>
#include <QWidget>
#include <QSize>

#include "mainthreadinterface.h"

class AWTInitEvent : public AWTEvent {
  
 private:
  JavaVM* vm;
  jobject target;

 public:
  AWTInitEvent(JNIEnv *env, jobject obj);
  void runEvent();
};

class AWTDestroyEvent : public AWTEvent {
  
 private:
  QWidget *widget;

 public:
  AWTDestroyEvent(QWidget *w)
    {
      widget = w;
    }

  void runEvent()
  {
    delete widget;
  }
};

class AWTFontEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  QFont *font;

 public:
  AWTFontEvent(QWidget *w, QFont *f)
    {
      widget = w;
      font = f;
    }

  void runEvent()
  {
    widget->setFont( *font );
  }
};

class AWTUpdateEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  int x,y,w,h;
  bool updateAll;

 public:
  AWTUpdateEvent(QWidget *src, bool all, int x0, int y0, int w0, int h0)
    {
      widget = src;
      updateAll = all;
      x = x0; y = y0; w = w0; h = h0;
    }

  void runEvent()
  {
    if(updateAll)
      widget->update();
    else
      widget->update(x,y,w,h);
  }
};

class AWTShowEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  bool visible;

 public:
  AWTShowEvent(QWidget *w, bool v);
  void runEvent();
};

class AWTEnableEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  bool enabled;

 public:
  AWTEnableEvent(QWidget *w, bool v);
  void runEvent();
};

class AWTCursorEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  Qt::CursorShape shape;
  
 public:
  AWTCursorEvent(QWidget *w,  Qt::CursorShape s);
  void runEvent();
};

class AWTResizeEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  int x, y, w, h;
  
 public:
  AWTResizeEvent(QWidget *wid, int x0, int y0, int w0, int h0);
  void runEvent();
};

class AWTBackgroundEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  bool foreground;
  QColor *color;
  
 public:
  AWTBackgroundEvent(QWidget *wid, bool fg, QColor *clr);
  void runEvent();
};

class AWTReqFocusEvent : public AWTEvent {
  
 private:
  QWidget *widget;

 public:
  AWTReqFocusEvent(QWidget *w) : AWTEvent()
    { 
      widget = w; 
    }
  void runEvent()
  {
    widget->setFocus();
  }
};

class AWTGetOriginEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  QPoint **origin;

 public:
  AWTGetOriginEvent(QWidget *w, QPoint **o) : AWTEvent()
    { 
      widget = w; 
      origin = o;
    }
  void runEvent()
  {
    *origin = new QPoint( widget->mapToGlobal( QPoint(0, 0) ) );
  }
};

class AWTGetSizeEvent : public AWTEvent {
  
 private:
  QWidget *widget;
  bool preferred;

 public:

  QSize **size;

  AWTGetSizeEvent(QWidget *w, QSize **s, bool pref) : AWTEvent()
    { 
      widget = w; 
      preferred = pref;
      size = s;
    }
  void runEvent()
  {
    QSize s;
    if( preferred )
      s = widget->sizeHint();
    else
      s = widget->minimumSizeHint();
    *size = new QSize( s.width(), s.height() );
  }
};

#endif
