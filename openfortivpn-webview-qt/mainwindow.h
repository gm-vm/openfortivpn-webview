#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkCookie>
#include <QRegularExpression>
#include <QWebEngineCertificateError>
#include <QWebEngineView>
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include "webauthdialog.h"
#endif

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
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    void handleWebAuthUxRequested(QWebEngineWebAuthUxRequest *request);
#endif

private:
    QWebEngineProfile *webEngineProfile;
    QWebEnginePage *webEnginePage;
    QWebEngineView *webEngine;
    const QRegularExpression& urlToWaitForRegex;
    const QString certificateHashToTrust;
    const bool keepOpen;
    QString svpncookie;
    bool didSeeUrlToWaitFor = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    WebAuthDialog *m_authDialog = nullptr;
#endif

    void createMenuBar();
    void closeEvent(QCloseEvent *);

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    void onStateChanged(QWebEngineWebAuthUxRequest::WebAuthUxState state);
#endif
};

#endif // MAINWINDOW_H
