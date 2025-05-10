#ifndef EVENTLOOPFILTER_H
#define EVENTLOOPFILTER_H

#include <QEventLoop>
#include <QEvent>
#include <QThread>

class EventloopFilter : public QEventLoop
{
    Q_OBJECT
public:
    explicit EventloopFilter(QObject *parent = nullptr);
    ~EventLoopFilter();
    int exec();
    void deleteLater();
    virtual bool eventFilter(QObject* watched, QEvent* event);

private:
    bool m_uiThread = false;
};

#endif // EVENTLOOPFILTER_H
