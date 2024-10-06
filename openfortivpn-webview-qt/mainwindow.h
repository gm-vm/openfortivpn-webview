#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork/QNetworkCookie>
#include <QRegularExpression>
#include <QtWebEngineCore/QWebEngineCertificateError>
#include <QtWebEngineWidgets/QWebEngineView>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const bool keepOpen,
                        const QRegularExpression& urlToWaitForRegex,
                        const QString certificateHashToTrust,
                        QWidget *parent = nullptr);
    ~MainWindow();
    void loadUrl(const QString &url);

private slots:
    void onCookieAdded(const QNetworkCookie &cookie);
    void onCookieRemoved(const QNetworkCookie &cookie);
    void onCertificateError(QWebEngineCertificateError certificateError);
    void updateTitle(const QString &title);
    void handleUrlChange(const QUrl &url);

private:
    QWebEngineProfile *webEngineProfile;
    QWebEnginePage *webEnginePage;
    QWebEngineView *webEngine;
    const QRegularExpression& urlToWaitForRegex;
    const QString certificateHashToTrust;
    const bool keepOpen;
    QString svpncookie;
    bool didSeeUrlToWaitFor = false;

    void createMenuBar();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
