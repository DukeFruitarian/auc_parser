#ifndef MYNETWORKACCESSMANAGER_H
#define MYNETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QIODevice>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QBuffer>

class MyNetworkAccessManager : public QNetworkAccessManager
{
	Q_OBJECT
public:
	MyNetworkAccessManager(QObject*);
private:
	bool test;
protected:
	virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData = 0);
public slots:
	void tested(bool);
signals:
	void sendingPostRequest(const QString&, int numTr , QNetworkRequest newRequest);
};


#endif // MYNETWORKACCESSMANAGER_H
