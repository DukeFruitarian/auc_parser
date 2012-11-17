#include "mywebview.h"

myWebView::myWebView(QWidget *parent) :
    QWebView(parent)
{
}

QWebView* myWebView::createWindow(QWebPage::WebWindowType type)
{
    qDebug() << "myWebView::createWindow type = " << type;
    return QWebView::createWindow(type);
}
