#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QWebView>
#include <QtGui>

class myWebView : public QWebView
{
    Q_OBJECT
public:
    myWebView():QWebView()
    {
        connect(this, SIGNAL(linkClicked(QUrl)), this, SLOT(linkClicked(QUrl url)));
    }

    ~myWebView() {}

private slots:
    void linkClicked(QUrl url);
};

#endif // MYWEBVIEW_H
