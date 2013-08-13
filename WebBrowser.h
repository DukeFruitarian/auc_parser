/* ======================================================================
**  WebBrowser.h - 
** ======================================================================
**
** ======================================================================
**  Copyright (c) 2009 by Max Schlee
** ======================================================================
*/

#ifndef _WebBrowser_h_
#define _WebBrowser_h_

#include <QWidget>
#include <QNetworkRequest>
#include <QTextEdit>
#include "mynetworkaccessmanager.h"
#include <QList>
#include <QLayout>
#include <windows.h>


class QLineEdit;
class QWebView;
class QPushButton;
class QNetworkReply;
class QTextEdit;
class QWebPage;
class QNetworkRequest;
class QLabel;
class QWebElement;
class QSpinBox;
class QSplitter;



// =============================================================
class WebBrowser : public QWidget{
	Q_OBJECT
private:
    QWebView					*m_pwv;
    QWebPage					*wp;
	QString						logString;
	QList<QLabel*>				*postRequestsText;
	QTimer						*timerEsc;
    QTimer                      *startMonitoringTimer;

	QPushButton					*m_pcmdRequest;
	QPushButton					*m_pcmdStart;
	QPushButton					*m_pcmdTest;
	QPushButton					*buttonQuit;
	QHBoxLayout					*mainButtonsLayoutTop;
	QHBoxLayout					*mainButtonsLayoutBot;

    QSpinBox                    *startMonitoringSeconds
                                ,*startMonitoringHours
                                ,*startMonitoringMinutes
                                ,*startMonitoringMiliSeconds;
    QPushButton                 *startMonitoringButton;


	QLabel						*addLotLabel;
	QLineEdit					*lotName;
	QPushButton					*addLot;
    QHBoxLayout                 *startMonitoringLayout;
	QHBoxLayout					*addLotLayout;
    QHBoxLayout                 *betStepLayoutMin;
    QHBoxLayout                 *betStepLayoutMax;

	QList<QLabel*>				*lotsNames;
//	QVBoxLayout					*lotsNamesLayout;
	QList<QLineEdit*>			*lotsCaps;
//	QVBoxLayout					*lotsCapsLayout;
    QList<QPushButton*>     	*removeLots;
	QHBoxLayout					*headLotRow;
	QVBoxLayout					*allLotsLayout;

	QTextEdit					*logView;

	QVBoxLayout					*confLayout;
    QHBoxLayout					*mainLayout;
    QWidget                     *rightMainPart;
    QSplitter                   *mainSplitter;

	QLineEdit					*m_ptxt;
	//QPushButton					*m_pcmdForward;

    MyNetworkAccessManager      *networkManager;
    MyNetworkAccessManager      *nwManagerSend;
    QNetworkRequest             postRequest;
    bool                        testNetWork;
    bool                        sendOnes;
    uint                        requestCount;
	QHBoxLayout					*supportLayout;
	QList<QString>				*existLots;
    QChar                   	ch;
    int                         loadProggress;
    uint                        betStepMin,betStepMax;
//	QNetworkReply				*replyFromServer;

    inline void parsingTable(const QWebElement& table);
	inline void saveLog();
	inline void	sendingValues(int tr, int bet);
    inline int  getBetStep();

public:
	WebBrowser(QWidget* wgt = 0);

private slots:
	void slotGo      (    );
	void slotFinished(bool);
	void replyFinished(QNetworkReply*);
	void sendRequest();
	void netWorkConnectionTestStart();
	void netWorkConnectionTestFinish();
	void addingLot();
	void removingLot();
	void webLoadProgress(int);
	void postRequestRecived(const QString& message, int numTr, QNetworkRequest request);
	void recivedReplyForParsing();
	void slotTimerEsc();
    void setBetStep(QString betStp);
    void startMonitoringTimerSlot(bool pushed);
    void compareStartTimeAndCurrentTime();
signals:
	void testSetuped(bool);

};
#endif //_WebBrowser_h_

