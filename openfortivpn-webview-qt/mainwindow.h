#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkCookie>
#include <QRegularExpression>
#include <QWebEngineCertificateError>
#include <QWebEngineView>

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
    void finish(const QString &svpncookie);
    void paintEvent(QPaintEvent *);

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
    QPixmap *authSuccessPixmap;
    const QRegularExpression& urlToWaitForRegex;
    const QString certificateHashToTrust;
    const bool keepOpen;
    QString svpncookie;
    bool didSeeUrlToWaitFor = false;

    void createMenuBar();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
