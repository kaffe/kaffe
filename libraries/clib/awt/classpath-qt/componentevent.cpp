/* componentevent.cpp --
   Copyright (C)  2005  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

#include "componentevent.h"
  
AWTInitEvent::AWTInitEvent(JNIEnv *env, jobject obj) : AWTEvent()
{
  env->GetJavaVM( &vm );
  target = env->NewGlobalRef( obj );
}

void AWTInitEvent::runEvent()
{
  JNIEnv *env;
  vm->GetEnv((void **)&env, JNI_VERSION_1_1);
  jclass targetCls = env->GetObjectClass( target );
  jmethodID initID = env->GetMethodID( targetCls,
				       "init",
				       "()V" );
  env->MonitorEnter( target );
  env->CallVoidMethod( target, initID );
  env->MonitorExit( target );
  env->DeleteGlobalRef( target );
}

AWTShowEvent::AWTShowEvent(QWidget *w, bool v) : AWTEvent()
{
  widget = w;
  visible = v;
}

void AWTShowEvent::runEvent()
{
  widget->setVisible( visible );
}

AWTEnableEvent::AWTEnableEvent(QWidget *w, bool v) : AWTEvent()
{
  widget = w;
  enabled = v;
}

void AWTEnableEvent::runEvent()
{
  widget->setEnabled( enabled );
}

AWTCursorEvent::AWTCursorEvent(QWidget *w, Qt::CursorShape s) : AWTEvent()
{
  widget = w;
  shape = s;
}

void AWTCursorEvent::runEvent()
{
  QCursor *s = new QCursor(shape);
  widget->setCursor( *s );
}

AWTResizeEvent::AWTResizeEvent(QWidget *wid, int x0, int y0, int w0, int h0)
{
  widget = wid;
  x = x0; y = y0;
  w = w0; h = h0;
  if(w == 0 && h == 0) w = h = 10;
}

void AWTResizeEvent::runEvent()
{
  widget->setGeometry( x, y, w, h );
}

AWTBackgroundEvent::AWTBackgroundEvent(QWidget *wid, bool fg, QColor *clr)
{
  widget = wid;
  foreground = fg;
  color = clr;
}

void AWTBackgroundEvent::runEvent()
{  
  QPalette p = widget->palette();
  if (foreground)
    {
      p.setColor(QPalette::Active, QPalette::Foreground, *color);
      p.setColor(QPalette::Active, QPalette::Text, *color);
    }
  else
    {
      p.setColor(QPalette::Active, QPalette::Background, *color);
      p.setColor(QPalette::Active, QPalette::Button, *color);
      p.setColor(QPalette::Active, QPalette::Base, *color);
      p.setColor(QPalette::Active, QPalette::AlternateBase, *color);
    }
  widget->setPalette(p);
  widget->repaint();
  delete color;
}

