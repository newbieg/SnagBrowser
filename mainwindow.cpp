#include "mainwindow.h"
#include <string>
#include <sstream>
#include <iostream>

#include <SDL2/SDL.h>

#include <QRect>
#include <QWebView>
#include <QWebSettings>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QUrl>
#include <QTimer>
#include <QCheckBox>
#include <QKeySequence>
#include <QShortcut>
#include <QPushButton>
#include <QWebFrame>

void linkClickedAction();
void pageLoaded();
void newAdress();

bool replacechar(QString &input, char original, char replacement);


QString quint64ToQString(quint64 number);
QString quint64ToClosestUnit(quint64 number);
QString toFullUrl(QString adressGiven);
QUrl urlparse(const QString &url, size_t & failLevel);

QString originalInput;

// Semi-slots
void updateUseage();
void smallTimeUpdate();
void toggleImages();
void toggleHideData();
void toggleJScripts();
void toggleVideo();


quint64 dataUsed_total_hold = 0;
quint64 dataUsed_total = 0;
quint64 dataUsed_page = 0;
QRect saveLastGeometry;
size_t badLink = 0;

bool imagesOn;
bool hideData;
bool javaScriptsOn;
bool videoPlayOn;
bool fullScreenOn = false;


QWidget * myWindow = NULL;
QWidget * hideShowOptions = NULL;
QWebView * web = NULL;
QWebSettings * webSettings = NULL;
QHBoxLayout *hlay = NULL;
QHBoxLayout * optionshlay = NULL;
QHBoxLayout * commandshlay = NULL;
QVBoxLayout * vlay = NULL;
QLineEdit * adressBar = NULL;
QLabel * dataUseage = NULL;
QString duString;
QPushButton * hideDataBtn = NULL;
QPushButton * showDataBtn = NULL;


QTimer * timer = NULL;
QCheckBox * noImagesBox = NULL;
QCheckBox * noJScriptBox = NULL;
QCheckBox * noVideoBox = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    badLink = 0;
    QWidget *window = new QWidget;
    QWidget * options = new QWidget;
    options->setMaximumHeight(35);
    myWindow = window;
    hideShowOptions = options;
    //Keyboard Shortcuts go here.
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(toggleFullScreen()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_B), this, SLOT(goBack()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this, SLOT(goBack()));
    new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this, SLOT(goForward()));

    hlay = new QHBoxLayout(this);
    optionshlay = new QHBoxLayout(this);
    commandshlay = new QHBoxLayout(this);
    vlay = new QVBoxLayout(this);
    web = new QWebView(this);
    timer = new QTimer(this);
//    webSettings = new QWebSettings();
    adressBar = new QLineEdit(this);
    dataUseage = new QLabel(this);
    noImagesBox = new QCheckBox(this);
    noJScriptBox = new QCheckBox(this);
    noVideoBox = new QCheckBox(this);
    hideDataBtn = new QPushButton(this);
    showDataBtn = new QPushButton(this);




    hideDataBtn->setMaximumHeight(15);
    hideDataBtn->setMaximumWidth(15);
    showDataBtn->setMaximumHeight(15);
    showDataBtn->setMaximumWidth(15);
    hideData = false;
    toggleHideData();
    imagesOn = true;
    toggleImages();
    javaScriptsOn = true;
    toggleJScripts();
    videoPlayOn = true;
    toggleVideo();
    //Load Home page (Google or yahoo)
    web->load(QUrl("http://www.google.com"));




    showDataBtn->setText("^");
    hideDataBtn->setText("v");
    updateUseage();
    noImagesBox->setText("<-Turn on images");
    noJScriptBox->setText("Turn on JavaScripts");
    noVideoBox->setText("Turn on Video Streaming");

    optionshlay->setAlignment(Qt::AlignBottom);

    vlay->addWidget(adressBar);
    vlay->addWidget(web);
    vlay->addWidget(showDataBtn);
    optionshlay->addWidget(hideDataBtn);
    optionshlay->addWidget(dataUseage);

    optionshlay->addWidget(noImagesBox);
    optionshlay->addWidget(noJScriptBox);
    optionshlay->addWidget(noVideoBox);
    options->setLayout(optionshlay);
    vlay->addWidget(options);
    hlay->addLayout(vlay);

    web->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(web, SIGNAL(linkClicked(QUrl)), this, SLOT(myLinkClicked(QUrl)));
    connect(web, SIGNAL(loadFinished(bool)), this, SLOT(loadCheck(bool)));
    connect(adressBar, &QLineEdit::returnPressed, newAdress);
    connect(timer, &QTimer::timeout, smallTimeUpdate);
    connect(noImagesBox, &QCheckBox::toggled, toggleImages);
    connect(noJScriptBox, &QCheckBox::toggled, toggleJScripts);
    connect(hideDataBtn, &QPushButton::clicked, toggleHideData);
    connect(showDataBtn, &QPushButton::clicked, toggleHideData);
    connect(showDataBtn, &QPushButton::clicked, toggleVideo);

    timer->start(500);

    this->setCentralWidget(window);
    window->setLayout(hlay);
}


// A page is finished loading, update dataUseage_total
void pageLoaded()
{

    dataUsed_total += dataUsed_page;
    updateUseage();
    QUrl nextLink = web->url();
    adressBar->setText(nextLink.toString());
//    web->stop();
//    web->load(nextLink);
}

void MainWindow::loadCheck(bool success)
{
    if(!success && badLink < 4)
    {
        std::cout << "Got to badCheck\n";
        QUrl newUrl = urlparse(adressBar->text(), badLink);

        adressBar->setText(newUrl.toString());
        badLink ++;

        emit adressBar->returnPressed();
    }
    else
    {
        dataUsed_total += dataUsed_page;
        updateUseage();

        badLink = 0;

        QUrl nextLink = web->url();
        adressBar->setText(nextLink.toString());
    }

}

// A link has been clicked, check if there's a local copy, if there isn't then
// load the page from url, if there is then load from local html.
void linkClickedAction()
{

}

// User pressed enter while in adressBar, asume new adress is requested.
void newAdress()
{
    originalInput = adressBar->text();

    if(replacechar(originalInput, ' ', '+'))
    {
        // the input is likely a search request, there is also a backup for single words in loadCheck() slot if search fails...
        QString temp;
        temp = "https://www.google.com/search?&q=";
        temp += originalInput;
        QUrl link(temp);
        web->load(link);
    }
    else
    {
        QUrl link = urlparse(adressBar->text(), badLink);
        web->load(link);
    }
}

// Turn number into a QString
QString quint64ToQString(quint64 number)
{
    long tempNum = number;
    std::string tempString;
    std::stringstream ss;
    ss << tempNum;
    getline(ss, tempString);
    return QString::fromStdString(tempString);

}

// Turn number into a QString with a data unit tacked on at the end.
QString quint64ToClosestUnit(quint64 number)
{
    long tempNum = number;
    std::string unit = "bytes";
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Kb";
    }
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Mb";
    }
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Gb";
     }
    std::string tempString;
    std::stringstream ss;
    ss << tempNum;
    getline(ss, tempString);
    tempString += unit;

    return QString::fromStdString(tempString);

}


// update the page information (for data used), however, it's not perfected yet,
// dataUsed_total is updated when full page indicates it is done loading (exterior to this function)
void updateUseage()
{

    dataUsed_page = web->page()->totalBytes();
    duString = "Bytes for page: ";
    duString += quint64ToClosestUnit(dataUsed_page);
    duString += "    Bytes this session: ";
    duString += quint64ToClosestUnit(dataUsed_total + dataUsed_page);
    dataUseage->setText(duString);
}

// slot for timer to update the useage data
void smallTimeUpdate()
{
    updateUseage();
}

// slot for the noImagesBox checkbox action (turn on/off images)
void toggleImages()
{
    imagesOn = !imagesOn;
    webSettings = web->settings();
    webSettings->setAttribute(webSettings->AutoLoadImages, imagesOn);
    if(!imagesOn)
    {
        //QWebElement z = web->page()->mainFrame().findAllElements(QString('*'));

    }
}

// slot for hide/show datafield (the options widget)
void toggleHideData()
{
    hideData = !hideData;
    if(hideData == true)
    {
        hideShowOptions->hide();
        showDataBtn->show();
        hideDataBtn->hide();
    }
    else
    {
        hideShowOptions->show();
        hideDataBtn->show();
        showDataBtn->hide();
    }

}

// enable/disable Javascript
void toggleJScripts()
{

    javaScriptsOn = !javaScriptsOn;
    webSettings = web->settings();
    webSettings->setAttribute(webSettings->JavascriptEnabled, javaScriptsOn);
}

// disable video streaming
void toggleVideo()
{
    webSettings->setAttribute(webSettings->PluginsEnabled, videoPlayOn);

}


// Take user input and parse it into a URL if they forget http://www. or combo thereof.
QUrl urlparse(const QString &url, size_t & failLevel)
{
    QUrl tempUrl(url);
    if(tempUrl.isValid() && failLevel == 0)
    {
        return tempUrl;
    }
    std::string temp = url.toStdString();

    if(temp.substr(0, 7) != "http://" && temp.substr(0, 8) != "https://" && failLevel <= 1)
    {
        temp = "http://" + temp;
        QString newurl = QString::fromStdString(temp);
        QUrl test(newurl);
        if(test.isValid())
        {
            return test;
        }
    }

    if(temp.substr(0, 11) != "http://www." && temp.substr(0, 12) != "https://www." && failLevel > 1)
    {
        temp = "http://www." + temp.substr(7);
        QString newurl = QString::fromStdString(temp);
        QUrl test(newurl);
        if(test.isValid())
        {
            return test;
        }
    }

    return QUrl(url);
}

MainWindow::~MainWindow()
{

}


// My First SLOT ever! This is where I might try to divert to a local copy of the file if
// it is available. Right now it just sets the address into the adressBar and loads the new page.
// Could be absolutely removed and the browser will run fine without it (just remove the connect statement in constructor)
void MainWindow::myLinkClicked(const QUrl & clickedUrl)
{

    adressBar->setText(clickedUrl.toString());
    web->load(clickedUrl);

}

void MainWindow::toggleFullScreen()
{
    fullScreenOn = !fullScreenOn;
    if(fullScreenOn)
    {
        saveLastGeometry = this->geometry();
        this->setWindowState(Qt::WindowFullScreen);
        QRect curentGeo = this->geometry();
        curentGeo.moveTop(curentGeo.y() + 20);
        curentGeo.moveBottom(curentGeo.height() - 40);

        web->setGeometry(curentGeo);

    }
    else
    {
        this->setWindowState(Qt::WindowMaximized);
    }
}

void MainWindow::goBack()
{
    web->page()->triggerAction(QWebPage::Back);
}
void MainWindow::goForward()
{
    web->page()->triggerAction(QWebPage::Forward);
}


bool replacechar(QString &input, char original, char replacement)
{
    bool foundReplacementChar = false;
    std::string temp = input.toStdString();

    for(size_t i = 0; i < temp.length(); i ++)
    {
        if(temp[i] == original)
        {
            foundReplacementChar = true;
            temp[i] = replacement;
        }
    }
    input = QString::fromStdString(temp);
    return foundReplacementChar;
}



