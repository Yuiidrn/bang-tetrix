#include "EventLoopFilter.h"
#include <QCoreApplication>

EventloopFilter::EventloopFilter(QObject *parent)
    : QEventLoop{parent}
{
}


EventLoopFilter::~EventLoopFilter()
{
}

int EventLoopFilter::exec()
{
    qApp->installEventFilter(this);
    return QEventLoop::exec(QEventLoop::AllEvents);
}

bool EventLoopFilter::eventFilter(QObject* watched, QEvent* event)
{
    //过滤用户输入事件
    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease ||
        event->type() == QEvent::MouseButtonDblClick ||
        event->type() == QEvent::KeyPress ||
        event->type() == QEvent::KeyRelease ||
        event->type() == QEvent::Wheel ||
        event->type() == QEvent::Enter ||
        event->type() == QEvent::Leave ||
        event->type() == QEvent::HoverEnter ||
        event->type() == QEvent::HoverLeave ||
        event->type() == QEvent::HoverMove)
    {
        return true;
    }
    return false;
}
