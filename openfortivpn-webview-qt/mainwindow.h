#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkCookie>
#include <QRegularExpression>
#include <QWebEngineView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const bool keepOpen,
                        const QRegularExpression& urlToWaitForRegex,
                        QWidget *parent = nullptr);
    ~MainWindow();
    void loadUrl(const QString &url);

private slots:
    void onCookieAdded(const QNetworkCookie &cookie);
    void onCookieRemoved(const QNetworkCookie &cookie);
    void updateTitle(const QString &title);
    void handleUrlChange(const QUrl &url);

private:
    QWebEngineView *webEngine;
    const QRegularExpression& urlToWaitForRegex;
    const bool keepOpen;
    QString svpncookie;
    bool didSeeUrlToWaitFor = false;

    void createMenuBar();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
