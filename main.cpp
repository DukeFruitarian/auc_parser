/* ======================================================================
**  main.cpp
** ======================================================================
**
** ======================================================================
**  Copyright (c) 2009 by Max Schlee
** ======================================================================
*/
#include <QtGui>
#include "WebBrowser.h"

// ----------------------------------------------------------------------
int main(int argc, char** argv)
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("Windows-1251"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));
//	QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	QApplication  app(argc, argv);
	WebBrowser    webBrowser;
	webBrowser.resize(QSize(1024,600));

	webBrowser.show();

	return app.exec();
}
