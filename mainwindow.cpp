#include "mainwindow.h"
#include <string>
#include <sstream>

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

void linkClickedAction();
void saveNewPage();
void newAdress();


QString quint64ToQString(quint64 number);
QString quint64ToClosestUnit(quint64 number);
QString toFullUrl(Qstring adressGiven);
void updateUseage();
void smallTimeUpdate();
void toggleImages();

quint64 dataUsed_total_hold = 0;
quint64 dataUsed_total = 0;
quint64 dataUsed_page = 0;

bool imagesOn;

QWebView * web = NULL;
QWebSettings * webSettings = NULL;
QHBoxLayout *hlay = NULL;
QHBoxLayout * optionshlay = NULL;
QVBoxLayout * vlay = NULL;
QLineEdit * adressBar = NULL;
QLabel * dataUseage = NULL;
QString duString;

QTimer * timer = NULL;
QCheckBox * noImagesBox = NULL;
QLabel * noImagesLabel = NULL;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *window = new QWidget;
    hlay = new QHBoxLayout(this);
    optionshlay = new QHBoxLayout(this);
    vlay = new QVBoxLayout(this);
    web = new QWebView(this);
    timer = new QTimer(this);
//    webSettings = new QWebSettings();
    adressBar = new QLineEdit(this);
    dataUseage = new QLabel(this);
    noImagesBox = new QCheckBox(this);
    noImagesLabel = new QLabel(this);


    imagesOn = true;
    toggleImages();
    //Load Home page (Google or yahoo)
    web->load(QUrl("http://www.google.com"));


    updateUseage();
    noImagesBox->setText("<-Turn on images");

    vlay->addWidget(adressBar);
    vlay->addWidget(web);
    optionshlay->addWidget(dataUseage);
    optionshlay->addWidget(noImagesLabel);
    optionshlay->addWidget(noImagesBox);
    vlay->addLayout(optionshlay);
//     hlay->addWidget(adressBar);
    hlay->addLayout(vlay);

//    web->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    connect(web, &QWebView::linkClicked, linkClickedAction);
    connect(web, &QWebView::loadFinished, saveNewPage);
    connect(adressBar, &QLineEdit::returnPressed, newAdress);
    connect(timer, &QTimer::timeout, smallTimeUpdate);
    connect(noImagesBox, &QCheckBox::toggled, toggleImages);

    timer->start(500);
    window->setLayout(hlay);
    setCentralWidget(window);

}


// A page is finnished loading, check if url matches a local copy
//      or load new page and save to manager.

void saveNewPage()
{

    dataUsed_total += dataUsed_page;
    updateUseage();
    QUrl nextLink = web->url();

//    web->stop();
//    web->load(nextLink);
}

// A link has been clicked, check if there's a local copy, if there isn't then
// load the page from url, if there is then load from local html.
void linkClickedAction()
{

    QUrl nextLink = web->url();


}

// User pressed enter while in adressBar, asume new adress is requested.
void newAdress()
{
    QUrl link = adressBar->text();

    web->load(link);
}

QString quint64ToQString(quint64 number)
{
    long tempNum = number;
    std::string tempString;
    std::stringstream ss;
    ss << tempNum;
    getline(ss, tempString);
    return QString::fromStdString(tempString);

}

QString quint64ToClosestUnit(quint64 number)
{
    long tempNum = number;
    std::string unit = "bytes";
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Kbs";
    }
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Mbs";
    }
    if(tempNum > 1024)
    {
        tempNum /= 1024;
        unit = "Gbs";
    }
    std::string tempString;
    std::stringstream ss;
    ss << tempNum;
    getline(ss, tempString);
    tempString += unit;

    return QString::fromStdString(tempString);

}

void updateUseage()
{

    dataUsed_page = web->page()->totalBytes();
    duString = "Bytes for page: ";
    duString += quint64ToClosestUnit(dataUsed_page);
    duString += "    Bytes this session: ";
    duString += quint64ToClosestUnit(dataUsed_total);
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
}

MainWindow::~MainWindow()
{

}