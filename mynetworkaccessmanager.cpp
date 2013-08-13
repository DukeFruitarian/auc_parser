#include "mynetworkaccessmanager.h"
#include <qdebug.h>
#include <QRegExp>

MyNetworkAccessManager::MyNetworkAccessManager(QObject *parent):QNetworkAccessManager(parent),test(false)
{

}


QNetworkReply* MyNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
	if (test) {

	} else if (outgoingData){
		QByteArray temp = outgoingData->readAll();
		const QString postString = QString(temp);
        if (postString.contains("NewStake=%D0%A1%D0%B4%D0%B5%D0%BB%D0%B0%D1%82%D1%8C+%D1%81%D1%82%D0%B0%D0%B2%D0%BA%D1%83")) {
            qDebug() << temp;
			QNetworkRequest newRequest = request;
			QString numTrStr = postString;
//			numTrStr.replace(QRegExp("^.+repBidsForTenderInfo%24ctl([^%]+)%24MakePurchase1%24btnMakePurchase=%D0%A1%D0%B4%D0%B5%D0%BB%D0%B0%D1%82%D1%8C\\+%D0%A1%D1%82%D0%B0%D0%B2%D0%BA%D1%83.*$"),"\\1");
            numTrStr.replace(QRegExp("^.+24MainContent%24gvBidsForStake%24ctl([^%]+)%24bNewStake=%D0%A1%D0%B4%D0%B5%D0%BB%D0%B0%D1%82%D1%8C\\+%D1%81%D1%82%D0%B0%D0%B2%D0%BA%D1%83.*$"),"\\1");

            qDebug() << "Отправлен пост запрос с подтверждением ставки tr # " <<numTrStr.toInt();
			qDebug() <<"numTrStr = " << numTrStr;
            emit sendingPostRequest(postString,numTrStr.toInt(),newRequest);
//            QString data=temp;
//            if (this->objectName() != "sender" && data.replace(QRegExp("^.+txtPurchasePrice=([^&]+).*&"),"\\1").toInt()>300000) {
//                qDebug() << "bet from WebView and bet > 300000 ";
//                return QNetworkAccessManager::createRequest(op,QNetworkRequest(),outgoingData);
//            }
		} else if (postString.contains("timer")){
			qDebug() << "timer event to space";
			return QNetworkAccessManager::createRequest(op,QNetworkRequest(),outgoingData);
		}
		QBuffer *buffer = new QBuffer;
		buffer->setData(temp);
		buffer->open(QIODevice::ReadOnly);
		outgoingData = buffer;
		buffer->setParent(outgoingData);

	}

	//newRequest.setRawHeader("Accept","application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");
	//newRequest.setRawHeader("Accept","application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5");
	return QNetworkAccessManager::createRequest(op,request,outgoingData);
}

void MyNetworkAccessManager::tested(bool priznak)
{
	test=priznak;
}
