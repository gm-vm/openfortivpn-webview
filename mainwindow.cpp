#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QMenuBar>
#include <QStandardPaths>
#include <QTextStream>
#include <QWebEngineCookieStore>
#include <QWebEngineHistory>
#include <QWebEngineProfile>
#include <QWebEngineView>
#include <iostream>

MainWindow::MainWindow(const bool keepOpen,
                       const QRegularExpression *urlToWaitForRegex,
                       QWidget *parent) :
    QMainWindow(parent),
    webEngine(new QWebEngineView(parent)),
    urlToWaitForRegex(urlToWaitForRegex),
    keepOpen(keepOpen)
{
    setCentralWidget(webEngine);

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
}

MainWindow::~MainWindow()
{
    delete webEngine;
}

void MainWindow::loadUrl(const QString &url)
{
    webEngine->setUrl(url);
}

void MainWindow::onCookieAdded(const QNetworkCookie &cookie)
{
    if (cookie.name() == "SVPNCOOKIE") {
        svpncookie = QString(cookie.name()) + "=" + QString(cookie.value());

        // This should maybe also check that the cookie is not empty.
        if (urlToWaitForRegex == nullptr || didSeeUrlToWaitFor) {
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
        svpncookie = QString();
    }
}

void MainWindow::handleUrlChange(const QUrl &url)
{
    if (urlToWaitForRegex == nullptr || didSeeUrlToWaitFor) return;

    if (urlToWaitForRegex->match(url.toString()).hasMatch()) {
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
