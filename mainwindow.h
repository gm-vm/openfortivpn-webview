#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool keepOpen, QWidget *parent = nullptr);
    void loadUrl(const QString &url);

private slots:
    void onCookieAdded(const QNetworkCookie &cookie);
    void onCookieRemoved(const QNetworkCookie &cookie);
    void updateTitle(const QString &title);
    void handleUrlChange(const QUrl &url);

private:
    QWebEngineView *webEngine;
    bool keepOpen;
    QString svpncookie;

    void createMenuBar();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
