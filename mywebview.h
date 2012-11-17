#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtWebKit>

class myWebView : public QWebView
{
    Q_OBJECT
public:
    explicit myWebView(QWidget *parent = 0);
    
protected:
    virtual QWebView* createWindow(QWebPage::WebWindowType type);
signals:
    
public slots:
    
};

#endif // MYWEBVIEW_H
