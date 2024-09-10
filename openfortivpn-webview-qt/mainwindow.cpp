#include "mainwindow.h"
#include <QApplication>
#include <QTimer>
#include <QPainter>
#include <QDebug>
#include <QLoggingCategory>
#include <QMenuBar>
#include <QSslError>
#include <QStandardPaths>
#include <QTextStream>
#include <QWebEngineCookieStore>
#include <QWebEngineHistory>
#include <QWebEnginePage>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <iostream>

Q_LOGGING_CATEGORY(category, "webview")

MainWindow::MainWindow(const bool keepOpen,
                       const bool quiet,
                       const QRegularExpression& urlToWaitForRegex,
                       const QString certificateHashToTrust,
                       QWidget *parent) :
    QMainWindow(parent),
    webEngineProfile(new QWebEngineProfile("vpn", parent)),
    webEnginePage(new QWebEnginePage(webEngineProfile)),
    webEngine(new QWebEngineView(webEngineProfile, parent)),
    authSuccessPixmap(new QPixmap()),
    urlToWaitForRegex(urlToWaitForRegex),
    certificateHashToTrust(certificateHashToTrust),
    keepOpen(keepOpen),
    quiet(quiet)
{
    setCentralWidget(webEngine);
    webEngine->setPage(webEnginePage);

    authSuccessPixmap->load(":/auth_success.png");

    createMenuBar();

    QString appDataLocation = QStandardPaths::locate(QStandardPaths::AppDataLocation,
                                                     QString(),
                                                     QStandardPaths::LocateDirectory);

    QWebEngineProfile *webEngineProfile = webEngine->page()->profile();
    webEngineProfile->setPersistentCookiesPolicy(QWebEngineProfile::AllowPersistentCookies);
    webEngineProfile->setCachePath(appDataLocation);
    webEngineProfile->setPersistentStoragePath(appDataLocation);

    connect(webEngine, &QWebEngineView::titleChanged, this, &MainWindow::updateTitle);
    connect(webEngine, &QWebEngineView::urlChanged, this, &MainWindow::handleUrlChange);

    connect(webEngineProfile->cookieStore(), &QWebEngineCookieStore::cookieAdded, this,
            &MainWindow::onCookieAdded);
    connect(webEngineProfile->cookieStore(), &QWebEngineCookieStore::cookieRemoved, this,
            &MainWindow::onCookieRemoved);

    connect(webEnginePage, &QWebEnginePage::certificateError, this, &MainWindow::onCertificateError);
}

MainWindow::~MainWindow()
{
    delete webEnginePage;
    delete webEngineProfile;
    delete webEngine;
    delete authSuccessPixmap;
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(QRect(0, 0, 1024, 768), *authSuccessPixmap);
    painter.drawText(QRect(0, 200, 1024, 768), Qt::AlignCenter, "Authentication Successful\nYou can close this window, or it will close automatically in 5 seconds.");
}

void MainWindow::loadUrl(const QString &url)
{
    webEngine->setUrl(url);
}

void MainWindow::finish(const QString &svpncookie) {
   bool hasCookieSet = !svpncookie.isEmpty();
    if (hasCookieSet) {
        std::cout << svpncookie.toStdString() << std::endl;
    }
    if (!keepOpen) {
        if (quiet) {
            QApplication::exit(hasCookieSet ? 0 : 1);
            return;
        }
        webEngine->close();
        QTimer::singleShot(5000, [hasCookieSet]() {
                QApplication::exit(hasCookieSet ? 0 : 1);
        });
    }
}

void MainWindow::onCookieAdded(const QNetworkCookie &cookie)
{
    if (cookie.name() == "SVPNCOOKIE") {
        svpncookie = QString(cookie.name()) + "=" + QString(cookie.value());

        qCDebug(category) << "SVPNCOOKIE has been received";

        if (didSeeUrlToWaitFor) {
            finish(svpncookie);
        }
    }
}

void MainWindow::onCookieRemoved(const QNetworkCookie &cookie)
{
    if (cookie.name() == "SVPNCOOKIE") {
        qCDebug(category) << "SVPNCOOKIE has been removed";
        svpncookie = QString();
    }
}

void MainWindow::onCertificateError(QWebEngineCertificateError certificateError) {
    auto sha256base64 = certificateError.certificateChain().constFirst().digest(QCryptographicHash::Sha256).toBase64();
    auto hashString = "sha256/" + sha256base64;
    if (certificateHashToTrust == hashString) {
        certificateError.acceptCertificate();
        return;
    }

    // Check the certificate chain using the possibly updated QSslConfiguration (--extra-ca-certs).
    // The documentation states that the CA should not be included in the chain, so here we remove it.
    auto chainWithoutCa = certificateError.certificateChain();
    chainWithoutCa.removeLast();
    auto errors = QSslCertificate::verify(chainWithoutCa, certificateError.url().host());
    if (errors.isEmpty()) {
        certificateError.acceptCertificate();
        return;
    }

    qCDebug(category) << "Found an invalid certificate:";
    for (auto& certificate : certificateError.certificateChain()) {
        qCDebug(category).noquote() << certificate.toText();
    }
    qCDebug(category).noquote() << "If you know that this certificate can be trusted, relaunch the application passing the following argument to ignore the error:";
    qCDebug(category).noquote() << "--trusted-cert='" + hashString + "'";
}

void MainWindow::handleUrlChange(const QUrl &url)
{
    qCDebug(category) << url.toString();

    if (didSeeUrlToWaitFor) return;

    if (urlToWaitForRegex.match(url.toString()).hasMatch()) {
        qCDebug(category) << "The current URL matches the given regex";
        didSeeUrlToWaitFor = true;
        finish(svpncookie);
    }
}

void MainWindow::updateTitle(const QString &title)
{
   setWindowTitle(title);
}

void MainWindow::createMenuBar()
{
    QAction *reload = new QAction(tr("&Reload"), this);
    connect(reload, &QAction::triggered, this,
            [this]() {
                webEngine->reload();
            });

    QAction *clearData = new QAction(tr("&Clear data"), this);
    connect(clearData, &QAction::triggered, this,
            [this]() {
                auto profile = webEngine->page()->profile();
                profile->clearHttpCache();
                profile->clearAllVisitedLinks();
                profile->cookieStore()->deleteAllCookies();
                webEngine->history()->clear();
            });

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(reload);
    fileMenu->addSeparator();
    fileMenu->addAction(clearData);
}

void MainWindow::closeEvent(QCloseEvent *)
{
    QApplication::exit(keepOpen ? 0 : 1);
}
