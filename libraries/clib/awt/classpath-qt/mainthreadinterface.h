#ifndef MAINTHREADINTERFACE_H
#define MAINTHREADINTERFACE_H

#include <jni.h>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QEvent>

class AWTEvent : public QEvent {
  
 public:
  AWTEvent() : QEvent( QEvent::User )
    {
    }

    virtual void runEvent()
    {
    }

};

#define AWT_INIT 0
#define AWT_SETVISIBLE 1

class MainThreadInterface : public QObject {
  
 private:
  QApplication *mainApp;

 public:
  MainThreadInterface(QApplication *parent);
  bool event ( QEvent * e );
  void postEventToMain(AWTEvent *event);
};

extern MainThreadInterface *mainThread;

#endif
