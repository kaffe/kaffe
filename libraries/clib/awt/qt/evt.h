#ifndef EVT_H
#define EVT_H

#include "qevent.h"
#include "qwidget.h"

class EventDispatcher : public QWidget
{
  Q_OBJECT
public:
  EventDispatcher::EventDispatcher(QWidget *parent=0, const char *name=0);
protected:
  bool eventFilter(QObject* o, QEvent* e);
};

#endif /* EVT_H */
