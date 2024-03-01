#include "mainwindow.h"
#include <QApplication>
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
                       const QRegularExpression& urlToWaitForRegex,
                       const QString certificateHashToTrust,
                       QWidget *parent) :
    QMainWindow(parent),
    webEnginePage(new QWebEnginePage()),
    webEngineProfile(new QWebEngineProfile("vpn", parent)),
    webEngine(new QWebEngineView(webEngineProfile, parent)),
    urlToWaitForRegex(urlToWaitForRegex),
    certificateHashToTrust(certificateHashToTrust),
    keepOpen(keepOpen)
{
    setCentralWidget(webEngine);
    webEngine->setPage(webEnginePage);

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
    delete webEngine;
    delete webEngineProfile;
    delete webEnginePage;
}

void MainWindow::loadUrl(const QString &url)
{
    webEngine->setUrl(url);
}

void MainWindow::onCookieAdded(const QNetworkCookie &cookie)
{
    if (cookie.name() == "SVPNCOOKIE") {
        svpncookie = QString(cookie.name()) + "=" + QString(cookie.value());

        qCDebug(category) << "SVPNCOOKIE has been received";

        // This should maybe also check that the cookie is not empty.
        if (didSeeUrlToWaitFor) {
            std::cout << svpncookie.toStdString() << std::endl;
            if (!keepOpen) {
                QApplication::exit(0);
            }
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
        bool hasCookieSet = !svpncookie.isEmpty();
        if (hasCookieSet) {
            std::cout << svpncookie.toStdString() << std::endl;
        }
        if (!keepOpen) {
            QApplication::exit(hasCookieSet ? 0 : 1);
        }
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
