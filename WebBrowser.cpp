/* ======================================================================
**  Calculator.h - 
** ======================================================================
**
** ======================================================================
**  Copyright (c) 2009 by Max Schlee
** ======================================================================
*/
#include <QtGui>
#include <QtWebKit>
#include <QNetworkCookie>
#include "WebBrowser.h"

// ----------------------------------------------------------------------
WebBrowser::WebBrowser(QWidget* wgt/*=0*/) :    QWidget(wgt)
                                                ,testNetWork(false)
                                                ,sendOnes(false)
                                                ,betStepMin(100)
                                                ,betStepMax(101)
{
    qDebug() << "AucParser starts";
    networkManager		= new MyNetworkAccessManager(this);
	nwManagerSend		= new MyNetworkAccessManager(this);
	nwManagerSend->setObjectName("sender");
	nwManagerSend->setCookieJar(networkManager->cookieJar());
//	timerEsc				= new QTimer(this);
//	connect(timerEsc, SIGNAL(timeout()), this, SLOT(slotTimerEsc()));

	lotsNames			= new QList<QLabel*>;
	lotsCaps				= new QList<QLineEdit*>;
	removeLots			= new QList<QPushButton*>;
	postRequestsText	= new QList<QLabel*>;

	m_pcmdRequest		= new QPushButton("Единичный\nзапрос");
	m_pcmdRequest->setObjectName("request button");
	m_pcmdStart			= new QPushButton("Запустить\nмониторинг");
	m_pcmdTest			= new QPushButton("Тест соединения");
	buttonQuit			= new QPushButton(QString::fromLocal8Bit("Выход"));

    m_pwv					= new QWebView;
	wp						= new QWebPage;
    wp->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);

	addLotLabel			= new QLabel("Добавить лот:");
    lotName				= new QLineEdit("27000");
	addLot				= new QPushButton("Добавить");

	ch.setCell(160);												// символ пробела в хтмл(&nbsp)
	ch.setRow(0);

	//m_ptxt			= new QLineEdit("http://www.syromonoed.org/forum/");
	//m_ptxt			= new QLineEdit("http://syroedenie.com/forum/");

	//m_ptxt			= new QLineEdit("http://livelymeal.ru/forum/ucp.php?mode=login");
	//m_ptxt			= new QLineEdit("http://www.prog.org.ru/index.php?action=login");


	//m_ptxt			= new QLineEdit("http://auction.uniconf.ru/Zones/MainZone/Tenders/ViewTender.aspx?TenderId=22073");
	m_ptxt			= new QLineEdit("http://auction.uniconf.ru/Zones/MainZone/Default.aspx");

	logString = QString("*********** %1 **********<br>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
	logView = new QTextEdit();
	logView->setHtml(logString);
	logView->setReadOnly(true);
	m_pwv->page()->setNetworkAccessManager(networkManager);
	m_pcmdStart->setCheckable(true);

	connect(networkManager, SIGNAL(sendingPostRequest(QString,int,QNetworkRequest)),
			  this, SLOT(postRequestRecived(QString,int,QNetworkRequest)));
	connect(m_pwv->page()->networkAccessManager(), SIGNAL(finished(QNetworkReply*)),
			  this, SLOT(replyFinished(QNetworkReply*)));
//	connect(nwManagerSend, SIGNAL(finished(QNetworkReply*)),
//			  this, SLOT(replyFinished(QNetworkReply*)));

    // нижняя правая часть, прогресс бар и кнопка рефреш
	QProgressBar* pprb        = new QProgressBar;
	QPushButton*  pcmdRefresh = new QPushButton("&Refresh");

    // задание времени начала мониторинга
    QLabel*       startMonitoringLabel          = new QLabel("Начало: ");
    startMonitoringHours          = new QSpinBox();
    startMonitoringHours->setRange(0,23);
    startMonitoringHours->setValue(QTime::currentTime().hour());
    startMonitoringMinutes        = new QSpinBox();
    startMonitoringMinutes->setRange(0,59);
    startMonitoringMinutes->setValue(QTime::currentTime().minute());
    startMonitoringSeconds        = new QSpinBox();
    startMonitoringSeconds->setRange(0,59);
    startMonitoringSeconds->setValue(QTime::currentTime().second());
    startMonitoringMiliSeconds    = new QSpinBox();
    startMonitoringMiliSeconds->setRange(0,999);
    startMonitoringMiliSeconds->setSingleStep(100);
    startMonitoringMiliSeconds->setValue(300);


    //QLineEdit*    startMonitoringEdit           = new QLineEdit(QTime::currentTime().toString("hh:mm:ss.zzz"));
    //QRegExp       startMonitoringRegExp("^[0-2][0-9]:[0-5][0-9]:[0-5][0-9]\\.[0-9]{3,3}&");
    //QRegExpValidator*   startMonitotinValidator = new QRegExpValidator(startMonitoringRegExp);
    //startMonitoringEdit->setValidator(startMonitotinValidator);
    startMonitoringButton         = new QPushButton("задать");
    startMonitoringButton->setCheckable(true);

    // задание минимальной ставки
    QLabel*       betMinLbl      = new QLabel("Минимальный шаг ставки:");
    QLineEdit*    betMinEdit     = new QLineEdit;
    QValidator*   validator   = new QIntValidator(100, 1000, this);
    betMinEdit->setValidator(validator);
    betMinEdit->setText(QString().setNum(betStepMin));
    betMinEdit->setObjectName("stepMin");

    // задание максимальной ставки
    QLabel*       betMaxLbl      = new QLabel("Максимальный шаг ставки:");
    QLineEdit*    betMaxEdit     = new QLineEdit;
    betMaxEdit->setValidator(validator);
    betMaxEdit->setText(QString().setNum(betStepMax));

    connect(startMonitoringButton,SIGNAL(toggled(bool)),this,SLOT(startMonitoringTimerSlot(bool)));
    connect(betMaxEdit,SIGNAL(textChanged(QString)),this,SLOT(setBetStep(QString)));
    connect(betMinEdit,SIGNAL(textChanged(QString)),this,SLOT(setBetStep(QString)));
	connect(m_pcmdStart,SIGNAL(clicked()),SLOT(sendRequest()));
	connect(buttonQuit,SIGNAL(clicked()),qApp,SLOT(quit()));
	connect(addLot,SIGNAL(clicked()),this,SLOT(addingLot()));
	connect(lotName,SIGNAL(returnPressed()),this,SLOT(addingLot()));
	connect(m_pcmdRequest,SIGNAL(clicked()),this, SLOT(sendRequest()));
	connect(m_ptxt, SIGNAL(returnPressed()), SLOT(slotGo()));
	connect(m_pcmdTest, SIGNAL(clicked()), this, SLOT(netWorkConnectionTestStart()));
	connect(pcmdRefresh, SIGNAL(clicked()), m_pwv, SLOT(reload()));
	connect(m_pwv, SIGNAL(loadProgress(int)), pprb, SLOT(setValue(int)));
	connect(m_pwv, SIGNAL(loadProgress(int)), this, SLOT(webLoadProgress(int)));
	connect(m_pwv, SIGNAL(loadFinished(bool)), SLOT(slotFinished(bool)));

	//Layout setup


	mainButtonsLayoutTop = new QHBoxLayout;
    mainButtonsLayoutTop->addWidget(m_pcmdRequest);
    mainButtonsLayoutTop->addWidget(m_pcmdStart);

	mainButtonsLayoutBot = new QHBoxLayout;
    mainButtonsLayoutBot->addWidget(m_pcmdTest);
    mainButtonsLayoutBot->addWidget(buttonQuit);

    startMonitoringLayout = new QHBoxLayout;
    startMonitoringLayout->addWidget(startMonitoringLabel);
    startMonitoringLayout->addWidget(startMonitoringHours);
    startMonitoringLayout->addWidget(new QLabel(":"));
    startMonitoringLayout->addWidget(startMonitoringMinutes);
    startMonitoringLayout->addWidget(new QLabel(":"));
    startMonitoringLayout->addWidget(startMonitoringSeconds);
    startMonitoringLayout->addWidget(new QLabel("."));
    startMonitoringLayout->addWidget(startMonitoringMiliSeconds);
    startMonitoringLayout->addWidget(startMonitoringButton);

    addLotLayout = new QHBoxLayout;
    addLotLayout->addWidget(addLotLabel);
    addLotLayout->addWidget(lotName);
    addLotLayout->addWidget(addLot);

    betStepLayoutMin = new QHBoxLayout;
    betStepLayoutMin->addWidget(betMinLbl);
    betStepLayoutMin->addWidget(betMinEdit);

    betStepLayoutMax = new QHBoxLayout;
    betStepLayoutMax->addWidget(betMaxLbl);
    betStepLayoutMax->addWidget(betMaxEdit);

	headLotRow = new QHBoxLayout;
    headLotRow->addWidget(new QLabel("Имя лота"));
    headLotRow->addWidget(new QLabel("Минимальная ставка"));
    headLotRow->addWidget(new QLabel("Удаление"));
	allLotsLayout = new QVBoxLayout;
    allLotsLayout->addLayout(headLotRow);

	supportLayout = new QHBoxLayout;
	//supportLayout->addWidget(pcmdRefresh);
	supportLayout->addWidget(m_ptxt);
	supportLayout->addWidget(pprb);

	confLayout = new QVBoxLayout;
	confLayout->addLayout(mainButtonsLayoutTop);
	confLayout->addLayout(mainButtonsLayoutBot);
    confLayout->addLayout(startMonitoringLayout);
    confLayout->addLayout(addLotLayout);
    confLayout->addLayout(betStepLayoutMin);
    confLayout->addLayout(betStepLayoutMax);
	confLayout->addLayout(allLotsLayout);
	confLayout->addWidget(logView);
	confLayout->addLayout(supportLayout);

    rightMainPart = new QWidget();
    rightMainPart->setLayout(confLayout);

    mainSplitter = new QSplitter(Qt::Horizontal,this);
    mainSplitter->addWidget(m_pwv);
    mainSplitter->addWidget(rightMainPart);
    mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainSplitter);
//	mainLayout->addLayout(confLayout);

    setLayout(mainLayout);
	slotGo();
}

// ----------------------------------------------------------------------
void WebBrowser::slotGo()
{
	if (!m_ptxt->text().startsWith("ftp://")
		&& !m_ptxt->text().startsWith("http://")
      && !m_ptxt->text().startsWith("gopher://")
      ) {
		m_ptxt->setText("http://" + m_ptxt->text());
   }

	QList<QNetworkCookie> list;
	QString name,value;
	//QNetworkCookie netCook;
	QFile cookies("cookies/"+QString("%1.cook").arg(QUrl(m_ptxt->text()).host()));
	cookies.open(QIODevice::ReadOnly);
	if (!cookies.error()) {
		QTextStream in(&cookies);
		while (!in.atEnd()){
			in >> name >> value;
			list << QNetworkCookie(name.toLatin1(),value.toLatin1());
		}
		cookies.close();
		qDebug() << "reading the file";
		qDebug() << m_pwv->page()->networkAccessManager()->cookieJar()->setCookiesFromUrl(list,QUrl(m_ptxt->text()));
	}

	m_pwv->load(QUrl(m_ptxt->text()));
}

//-------------------------------------------------------------------------------------

void WebBrowser::slotFinished(bool bOk)
{    
	if (!bOk) {
		qDebug() << "загрузка с ошибкой";
	//	m_pwv->reload();
	}else {
	//	networkManager->get(QNetworkRequest(m_pwv->url()));
		m_ptxt->setText(m_pwv->url().toString());
	}
}

//-------------------------------------------------------------------------------------

void WebBrowser::replyFinished(QNetworkReply *reply)
{
	//обработка ответа на запрос к северу
	connect(reply,SIGNAL(readyRead()),reply,SLOT(deleteLater()));
	if (testNetWork) {															//тестируем канал связи
		requestCount++;
		networkManager->get(QNetworkRequest(m_pwv->url()));
		return;
	}
    if (reply->operation()==QNetworkAccessManager::PostOperation) {//показываем только ПОСТ операции
//        QString str = QString::fromUtf8(reply->readAll()).toAscii();
//        QFile temp("temp.ini");
//        if (temp.open(QIODevice::Append | QIODevice::Text)) {
//            QTextStream ts(&temp);
//            ts << str;
//            temp.close();
//        }
//        temp.deleteLater();
	}
}

//-------------------------------------------------------------------------------------

void WebBrowser::sendRequest()
{
//    wp->mainFrame()->evaluateJavaScript("alert(\"asdads\");");
//    m_pwv->page()->mainFrame()->evaluateJavaScript("alert(\"asdads\");");
//    m_pwv->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
//    m_pwv->settings()->setAttribute(QWebSettings::JavaEnabled, false);
//    m_pwv->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
//    m_pwv->settings()->setAttribute(QWebSettings::PluginsEnabled, false);
    if (sender() && (sender()->objectName() == "request button")) {
        qDebug() << "getBetStep() = " <<getBetStep();
        sendOnes = true;
	} else if (!m_pcmdStart->isChecked()){
//        m_pwv->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
//        timerEsc->stop();
		saveLog();
		return;
	} else {
//        m_pwv->settings()->setAttribute(QWebSettings::JavascriptEnabled, false);
//        m_pwv->settings()->setAttribute(QWebSettings::JavaEnabled, false);
//        m_pwv->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, false);
//        timerEsc->start(15);
	}


	//формирование одиночного запроса к серверу
	//QWebElement webEl = wp->mainFrame()->findFirstElement("input[name=user]");
	//webEl.setAttribute("value","RealDuke");
	//webEl = wp->mainFrame()->findFirstElement("input[name=passwrd]");
	//webEl.setAttribute("value","Ghcnh8908100");
	//webEl = wp->mainFrame()->findFirstElement("input[type=submit]");
	//qDebug() << webEl.attribute("value");
	//webEl.evaluateJavaScript("this.click();");
	//	QWebElement tTable = m_pwv->page()->mainFrame()->findFirstElement("table[id=myTable]");
	//	QWebElement tBody = tTable.findFirst("tbody");
	//	parsingTable(tBody); //ОСНОВА
	//	QWebElement testEl = m_pwv->page()->mainFrame()->findFirstElement("input[name=user]");
	//	testEl.setAttribute("value","hzhzhz");
	//	testEl = m_pwv->page()->mainFrame()->findFirstElement("input[value=Войти]");
	//	testEl.evaluateJavaScript("this.click();");
	//	qDebug() << testEl.attribute("type") << endl<< endl<< endl;


	//	QNetworkRequest nr(m_pwv->url());
	//	nr.setRawHeader("Content-Type","text/html; charset=utf-8");
	QNetworkReply *replyFromServer = networkManager->get(QNetworkRequest(m_pwv->url()));
	replyFromServer->setObjectName("void WebBrowser::sendRequest()");
	connect(replyFromServer,SIGNAL(finished()),this,SLOT(recivedReplyForParsing()));

	//replyFromServer->deleteLater();
	//      networkManager->post(nr,"");
	//	QWebElement lastElement = tBody.lastChild();
	//	qDebug() << lastElement.tagName();
	//	QWebElementCollection elements = tBody.findAll("tr:first-child");
	//	foreach(QWebElement element, elements)
	//		qDebug() << element.toPlainText() << endl<< endl<< endl;
	//
	//
	//	foreach(QWebElement element, elements) {
	//		QString str = element.toPlainText();
	//		QString str1 = QString("СМ02984");
	//		if (str == str1){
	//			qDebug() << element.attribute("href");
	//			element.evaluateJavaScript("document.location = this.href;");
	//			break;
	//		}
	//	}




	//	QWebElement webEl=wp->mainFrame()->findFirstElement("a[href='/Zones/MainZone/Tenders/ViewTender.aspx?TenderId=21793']"); //рабочий

};

//-------------------------------------------------------------------------------------

void WebBrowser::netWorkConnectionTestStart()
{
	//начало теста скорости отклика от сервера
	QPushButton *pb = qobject_cast<QPushButton*>(sender());
	pb->setEnabled(false);
	requestCount=0;
	testNetWork=true;
	networkManager->get(QNetworkRequest(m_pwv->url()));
	QTimer::singleShot(5000,this,SLOT(netWorkConnectionTestFinish()));
}

//-------------------------------------------------------------------------------------

void WebBrowser::netWorkConnectionTestFinish()
{
	//конец теста скорости отклика от сервера
	m_pcmdTest->setEnabled(true);
	QMessageBox::about(0,"Количество запросов","За 5 секунд было отослано "+QString().setNum(requestCount)+" запроса(ов)");
	testNetWork=false;
}

//-------------------------------------------------------------------------------------

void WebBrowser::addingLot()
{

	//добавление лота в список для мониторинга
	foreach (QLabel *label,*lotsNames) {
		if (label->text() == lotName->text()) {
			QMessageBox::warning(0,"Внимание","Этот лот уже мониторится");
			return;
		}
	}
	QWebElement tTable = m_pwv->page()->mainFrame()->findFirstElement("table[id=myTable]");
	QWebElement tBody = tTable.findFirst("tbody");
	QWebElement trElement = tBody.firstChild(),temp;
	bool flag = true, exist=false;
	while( flag && trElement.tagName()=="TR") {
		//qDebug() << trElement.toPlainText() << endl;
		QWebElementCollection ahrefs = trElement.findAll("a");
		//qDebug() << ahrefs.count()<< endl;
		foreach(QWebElement ahref, ahrefs) {
			//qDebug() << ahref.toPlainText() << lotName->text() << endl;
			if (ahref.toPlainText().startsWith(lotName->text()+ch)){
				exist=true;
			}//if (ahref.toPlainText().startsWith(lotName->text()))
		} //foreach(QWebElement ahref, ahrefs)
		if (temp==tBody.lastChild()) {
			flag=false;
		}
		temp=trElement;
		trElement=trElement.nextSibling();
	}
	if (!exist) {
		QMessageBox::warning(0,"Внимание","Данного лота не существует");
		return;
	}
	QHBoxLayout *rowLot = new QHBoxLayout;
	QPushButton *pb = new QPushButton("Удалить");
	pb->setObjectName(QString().setNum(removeLots->count()));
	removeLots->push_back(pb);
	connect(pb,SIGNAL(clicked()),this,SLOT(removingLot()))	;

	QLabel *lbl = new QLabel(lotName->text());
	lotsNames->push_back(lbl);
    rowLot->addWidget(lbl);

    QLineEdit *te = new QLineEdit("15000");
	lotsCaps->push_back(te);
	rowLot->addWidget(te);
	rowLot->addWidget(pb);

	allLotsLayout->addLayout(rowLot);
//    qDebug() << "removeLots.count = " << removeLots->count(); <<  " removeLots.capacity = " << removeLots->;
}

//-------------------------------------------------------------------------------------

void WebBrowser::removingLot()
{
	// Удаление лота из списка мониториваемых лотов
	int num = sender()->objectName().toInt();
	for (int i=removeLots->count()-1;i>=num;--i) {

		removeLots->at(i)->setObjectName
				(QString().setNum((removeLots->at(i)->objectName()).toInt()-1));
	}
	QPushButton *pb = removeLots->at(num);
	removeLots->removeAt(num);
	pb->deleteLater();

	QLabel *lbl = lotsNames->at(num);
	lotsNames->removeAt(num);
	lbl->deleteLater();

	QLineEdit *le = lotsCaps->at(num);
	lotsCaps->removeAt(num);
	le->deleteLater();

	qDebug() << removeLots->count();
}

//-------------------------------------------------------------------------------------

void WebBrowser::parsingTable(const QWebElement &tBody, bool finished)
{
	QTime qt;
	qt.start();
	bool aucFinished=false;
	int numTr = 1;
	QWebElement trElement = tBody.firstChild();
	while(trElement.tagName()=="TR") {								// Пока тег <tr>

		//qDebug() << trElement.toPlainText() << endl;
		QWebElementCollection ahrefs = trElement.findAll("a");
		//qDebug() << ahrefs.count()<< endl;
		foreach(QWebElement ahref, ahrefs) {						// Для каждого тега <a>
			//qDebug() << ahref.toPlainText() << lotName->text() << endl;
			for (int numLot=0;numLot<lotsNames->count();++numLot){ // Для каждого лота в контейнере лотов для мониторинга
				if (ahref.toPlainText().startsWith(lotsNames->at(numLot)->text()+ch)){ // Если имя лота совпадает с именем лота для мониторинга
					QWebElement divElement = trElement.findFirst("div");
					if (!finished) {					// Если не закончены торги
						//qDebug() << "У кнопки атрибут он_клик = " << betSubmit.attribute("onclick");
						if (!divElement.parent().attribute("style").contains("background-color: #66FF66")) { // Если цвет не зелёный(ставка не наша)
                            int newBet = ((divElement.toPlainText().toInt() - getBetStep())/100)*100;
							if ((lotsCaps->at(numLot))->text().toInt()<=newBet) { //
								//QWebElement betField = trElement.findFirst("input");
								//betField.setAttribute("value",QString().setNum(newBet));
								//betSubmit.evaluateJavaScript("this.click();");
								logString+=QString("%1 сделана ставка <i>%2</i> на лот <b>%3</b><br>")
											  .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
											  .arg(newBet)
											  .arg(lotsNames->at(numLot)->text());
								logView->setHtml(logString);
								sendingValues(numTr,newBet);
                                return; // выход из функции. Ожидание приёма нового ответа с сервера
                                qDebug() << "Вышли из функции но пошли дальше ????";
							} else {
								if (!logString.contains(QString("ставка <i>%1</i> на лот <b>%2</b> ниже предельной цены <br>")
									.arg(newBet)
									.arg(lotsNames->at(numLot)->text()))){
									logString+=QString("%1 ставка <i>%2</i> на лот <b>%3</b> ниже предельной цены <br>")
												  .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
												  .arg(newBet)
												  .arg(lotsNames->at(numLot)->text());
									logView->setHtml(logString);
								}
							}
						} else {
							qDebug() << QString("На лот %1 наша ставка равна %2").arg(lotsNames->at(numLot)->text()).arg(divElement.toPlainText());
						}
					} else { // Если торги закончены
//						timerEsc->stop();
						foreach (QLabel* lbl, *postRequestsText) {
							lbl->deleteLater();
						}
						postRequestsText->clear();
						if (!aucFinished) {//Если первое упоминание о завершенных торгов(для добавления строки в лог)
							aucFinished=true;
							logString+=QString("<br>%1 <b>аукцион закончен</b><br>")
										  .arg(QTime::currentTime().toString("hh:mm:ss.zzz"));
						}
						if (!divElement.parent().attribute("style").contains("background-color: #66FF66")) {//Если цвет не зелёный(ставка не наша)
							logString+=QString("%1 лот <b>%2</b> был прекуплен<br>")
										  .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
										  .arg(lotsNames->at(numLot)->text());
							logView->setHtml(logString);
						} else {
							logString+=QString("%1 лот <b>%2</b> был куплен за <i>%3</i> <br>")
										  .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
										  .arg(lotsNames->at(numLot)->text())
										  .arg(divElement.toPlainText().toInt());
							logView->setHtml(logString);
						}
					}
				}//if (ahref.toPlainText().startsWith(lotName->text()))

			}//for (int numLot=0;numLot<lotsNames->count();++numLot){
		} //foreach(QWebElement ahref, ahrefs)
		if (trElement==tBody.lastChild() && aucFinished) {
			logString+="<hr>";
			logView->setHtml(logString);
			saveLog();
			m_pcmdStart->setChecked(false);
            m_pwv->settings()->setAttribute(QWebSettings::JavascriptEnabled, true);
			return;
		}
		trElement=trElement.nextSibling();
		numTr++;
	}
	//	networkManager->post(QNetworkRequest(m_pwv->url()),"");
	if (m_pcmdStart->isChecked()) {
		QNetworkReply *replyFromServer = nwManagerSend->get(QNetworkRequest(m_pwv->url()));
		replyFromServer->setObjectName("WebBrowser::parsingTable");
		connect(replyFromServer,SIGNAL(finished()),this,SLOT(recivedReplyForParsing()));
//	} else {
//		timerEsc->stop();
	}
	qDebug() << "elapsed in parsing table: " <<qt.elapsed();
}

//-------------------------------------------------------------------------------------

void WebBrowser::saveLog()
{
	QDir logDir(QDir::current());
	if (!logDir.exists("logs")){
		logDir.mkdir("logs");
	}
	//		cookDir.cd("");
	//		qDebug() << cookDir.dirName();
	logString = logString.replace("<br>","\n");
	logString = logString.replace("<hr>","\n-----------------------------------------------------\n");
	logString = logString.replace("<b>","");
	logString = logString.replace("</b>","");
	logString = logString.replace("<i>","");
	logString = logString.replace("</i>","");

	QFile logs("logs/"+QString("%1.log").arg(QDate::currentDate().toString("dd.MM.yyyy")));
	logs.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
	if (!logs.error()) {
		QTextStream out(&logs);
		out << logString;
		logs.close();
	}
	logString = QString("*********** %1 **********<br>").arg(QDate::currentDate().toString("dd.MM.yyyy"));
}

//-------------------------------------------------------------------------------------

void WebBrowser::sendingValues(const int numTr, const int bet)
{
	int newBet=bet;
//    newBet = (newBet/100)*100;
//    newBet=22900;

	QTime qt;
	qt.start();

	foreach (QLabel *lbl,*postRequestsText){
		if (lbl->objectName().toInt() == numTr) {
			QString data = lbl->text();
			data.replace(QRegExp("txtPurchasePrice=[^&]+"),QString("txtPurchasePrice=%1").arg(newBet));
			//if (m_pcmdStart->isChecked()){
                            QNetworkReply *replyFromServer = nwManagerSend->post(postRequest,data.toUtf8());
                            replyFromServer->setObjectName("WebBrowser::sendingValues");
                            connect(replyFromServer,SIGNAL(finished()),this,SLOT(recivedReplyForParsing()));
			//}
			break;
		}
	}
//	if (m_pcmdStart->isChecked()){
//		QNetworkReply *replyFromServer = networkManager->post(QNetworkRequest(m_pwv->url()),"");
//		replyFromServer->setObjectName("WebBrowser::sendingValues");
//		connect(replyFromServer,SIGNAL(finished()),this,SLOT(recivedReplyForParsing()));
//	}



	/*QWebElement tTable = m_pwv->page()->mainFrame()->findFirstElement("table[id=myTable]");
	QWebElement tBody = tTable.findFirst("tbody");
	QWebElement trElement = tBody.firstChild();
	qDebug() << "numTr = " <<numTr;
	for (int i=1;i<numTr;++i){
		trElement = trElement.nextSibling();
	}



	QWebElement firstA = trElement.findFirst("a");
	qDebug() << firstA.toPlainText();
	QWebElement betField = trElement.findFirst("input");
	betField.setAttribute("value",QString().setNum(newBet));
	QWebElement betSubmit = trElement.findFirst("input[value='Сделать Ставку']");
	betSubmit.evaluateJavaScript("this.onclick()");


	//betSubmit.evaluateJavaScript("this.click();");
//	m_pwv->reload();
//	QEventLoop *loop = new QEventLoop;
//	connect(m_pwv,SIGNAL(loadFinished(bool)),loop,SLOT(quit()));
//	loop->exec();
//	delete loop;
	//QMessageBox::about(0,"поставлено",QString("ставка прошла %1").arg(betField.attribute("value")));
	qDebug() << QString("ставка прошла %1").arg(betField.attribute("value"));
	*/
    qDebug() << QString("ставка прошла %1").arg(newBet);
//	networkManager->get(QNetworkRequest(m_pwv->url()));
	qDebug() << "elapsed in sending Values:" << qt.elapsed();
}

//-------------------------------------------------------------------------------------

void WebBrowser::webLoadProgress(int progress)
{
	loadProggress=progress;
}

//-------------------------------------------------------------------------------------

void WebBrowser::postRequestRecived(const QString &message, int numTr, QNetworkRequest request)
{
	qDebug() << "Получено сообщение с информацией о пост запросах";


	foreach (QLabel* lbl, *postRequestsText) {
		if (lbl->objectName() == QString().setNum(numTr)) {
			qDebug() << "Существует информация о пост запросах на данный лот";
			return;
		}
	}
	qDebug() << "Добавление пост запроса на данный лот в список";
	QLabel* temp = new QLabel(message);
	temp->setObjectName(QString().setNum(numTr));
	postRequest=request;
	postRequestsText->append(temp);

}

//-------------------------------------------------------------------------------------

void WebBrowser::recivedReplyForParsing()
{
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
	qDebug() << "in void WebBrowser::recivedReplyForParsing() sender name = " << reply->objectName();
//	if (loadProggress == 100){
		QTime qt;
		qt.start();
		//		qDebug() << "Request headers:  ";

		//		QList<QByteArray> reqHeaders = reply->request().rawHeaderList();
		//		//QList<QByteArray> reqHeaders = request.rawHeaderList();
		//		foreach (QByteArray reqName, reqHeaders)
		//		{
		//			 QByteArray reqValue = reply->request().rawHeader(reqName);
		//			 //QByteArray reqValue = request.rawHeader(reqName);
		//			 qDebug() << reqName << ": " << reqValue;
		//		}
		//		qDebug() << "Reply headers:  ";
		//		QList<QByteArray> reqHeadersReply = reply->rawHeaderList();
		//		foreach( QByteArray reqName, reqHeadersReply )
		//		{
		//			 QByteArray reqValue = reply->rawHeader( reqName );
		//			 qDebug() << reqName << ": " << reqValue;
		//		}

		//
//		QDir cookDir(QDir::current());
//		if (!cookDir.exists("cookies")){
//			cookDir.mkdir("cookies");
//		}
		//	cookDir.cd("");
		//
		//	QFile cookies("cookies/"+QString("%1.cook").arg(reply->url().host()));
		//	cookies.open(QIODevice::Truncate | QIODevice::WriteOnly);
		//	if (!cookies.error()) {
		//		QTextStream out(&cookies);
		//		foreach (QNetworkCookie netCook,m_pwv->page()->networkAccessManager()->cookieJar()->cookiesForUrl(reply->url()))
		//			out << netCook.name() << " " << netCook.value() << endl;
		//		cookies.close();
		//		qDebug() << "writing the file";
		//	}
		//QString str =QString::fromUtf8(reply->readAll()).toAscii();
		//qDebug() << str;


        wp->mainFrame()->setHtml(QString::fromUtf8(reply->readAll()).toAscii());
		//	QWebElement testEl = m_pwv->page()->mainFrame()->findFirstElement("input[name=user]");
		//	//testEl.setAttribute("value","hzhzhz");
		//	QWebElement testEl2 = testEl.parent().findFirst("input[type=submit]");
		//	testEl2.evaluateJavaScript("this.click();");
		//	qDebug() << testEl2.attribute("type") << endl<< endl<< endl;
		//return;
		if (m_pcmdStart->isChecked() || sendOnes){
			QTime qt;
			qt.start();
			sendOnes=false;
			bool finish=false;
			QWebElement tTable = wp->mainFrame()->findFirstElement("table[id=myTable]");
			QWebElementCollection ElColl= wp->mainFrame()->findAllElements("td");
			//qDebug() <<ElColl.count();
			foreach (QWebElement wbel,ElColl){
				if (wbel.toPlainText() == "Закрыты"){
                    finish=true;
					break;
				}
			}
			qDebug() << "ms in find CLOSE:" << qt.elapsed();
			QWebElement tBody = tTable.findFirst("tbody");
			parsingTable(tBody,finish);											//ОСНОВА
			qDebug() << "elapsed in foreach:" << qt.elapsed();
		}
		reply->deleteLater();
		qDebug() << "ms total:" << qt.elapsed();
//	}
}

//-------------------------------------------------------------------------------------

void WebBrowser::setBetStep(QString betStp)
{
    sender()->objectName()=="stepMin"?betStepMin=betStp.toInt():betStepMax=betStp.toInt();
    qDebug() <<"betStepMin = " <<  betStepMin << "betStepMax = " << betStepMax;
}

//-------------------------------------------------------------------------------------

void WebBrowser::slotTimerEsc()
{
	  keybd_event(VK_ESCAPE,0,0,0);
	  keybd_event(VK_ESCAPE,0,KEYEVENTF_KEYUP,0);
}

//-------------------------------------------------------------------------------------

int WebBrowser::getBetStep()
{
    int roundValue=100;
    return (betStepMin<betStepMax)?(((betStepMin + rand()%(betStepMax-betStepMin+1))/roundValue)*roundValue):(betStepMin/roundValue)*roundValue;
}

//-------------------------------------------------------------------------------------

void WebBrowser::startMonitoringTimerSlot(bool pushed)
{
    if (pushed) {
        //qDebug("В задать мониторинг");
        startMonitoringTimer = new QTimer();
        startMonitoringTimer->start(20);
        connect(startMonitoringTimer,SIGNAL(timeout()),SLOT(compareStartTimeAndCurrentTime()));
    }   else {
        disconnect(startMonitoringTimer,SIGNAL(timeout()),this,SLOT(compareStartTimeAndCurrentTime()));
        delete startMonitoringTimer;
    }
}

//-------------------------------------------------------------------------------------

void WebBrowser::compareStartTimeAndCurrentTime()
{
    //qDebug() << "WebBrowser::compareStartTimeAndCurrentTime()";
    if (QTime::currentTime()>=QTime(startMonitoringHours->value()
                                    ,startMonitoringMinutes->value()
                                    ,startMonitoringSeconds->value()
                                    ,startMonitoringMiliSeconds->value())) {
        startMonitoringButton->setChecked(false);
        m_pcmdStart->setChecked(true);
        sendRequest();
//        QNetworkReply *replyFromServer = networkManager->get(QNetworkRequest(m_pwv->url()));
//        replyFromServer->setObjectName("void WebBrowser::compareStartTimeAndCurrentTime()");
//        connect(replyFromServer,SIGNAL(finished()),this,SLOT(recivedReplyForParsing()));
    }
}
