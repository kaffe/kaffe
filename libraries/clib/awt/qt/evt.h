/**
 * evt.h - encapsulation of Qt Event dispatcher
 *
 * Copyright (c) 2002, 2003, 2004
 *      Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef EVT_H
#define EVT_H

#include <qevent.h>
#include <qwidget.h>

class EventDispatcher : public QWidget
{
  Q_OBJECT
public:
  EventDispatcher::EventDispatcher(QWidget *parent=0, const char *name=0);
protected:
  bool eventFilter(QObject* o, QEvent* e);
};

#endif

