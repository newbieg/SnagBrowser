#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void myLinkClicked(const QUrl & clickedUrl);
    void toggleFullScreen();
    void loadCheck(bool);
    void goBack();
    void goForward();

};

#endif // MAINWINDOW_H
