#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void loadUrl(const QString &url);

private slots:
    void newCookieHandler(const QNetworkCookie &cookie);
    void updateTitle(const QString &title);
    void handleUrlChange(const QUrl &url);

private:
    QWebEngineView *webEngine;
    QString svpncookie;

    void createMenuBar();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
