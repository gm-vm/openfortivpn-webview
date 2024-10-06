#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QtGui/QCursor>
#include <QRect>
#include <QtGui/QScreen>
#include <QtNetwork/QSslConfiguration>
#include <iostream>

static QScreen *findScreenWithCursor()
{
    QPoint pos = QCursor::pos();

    for (QScreen *screen : QGuiApplication::screens()) {
        QRect screenRect = screen->geometry();
        if (screenRect.contains(pos)) {
            return screen;
        }
    }

    return QApplication::primaryScreen();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto optionRealm = QCommandLineOption("realm", "The authentication realm.", "realm");
    auto optionUrl = QCommandLineOption("url", "The already built SAML URL.\nThis takes precedence over [host:port].", "url");
    auto optionKeepOpen = QCommandLineOption("keep-open", "Do not close the browser automatically.");
    auto defaultUrlRegex = "/sslvpn/portal(/|\\.html)";
    auto urlRegexDescription = QString("A regex to detect the URL that needs to be visited before printing SVPNCOOKIE.\nThe default is \"%1\".").arg(defaultUrlRegex);
    auto optionUrlRegex = QCommandLineOption("url-regex", urlRegexDescription, "url-regex", defaultUrlRegex);
    auto extraCaCertsDescription = QString("Path to a file with extra certificates. The file should consist of one or more trusted certificates in PEM format.");
    auto optionExtraCaCerts = QCommandLineOption("extra-ca-certs", extraCaCertsDescription, "extra-ca-certs");
    auto certificateToTrustDescription = QString("The fingerprint of a certificate to always trust, even if invalid. The details of invalid certificates, fingerprint included, will be dumped in the console.");
    auto optionCertificateToTrust = QCommandLineOption("trusted-cert", certificateToTrustDescription, "trusted-cert");

    QCommandLineParser parser;
    parser.addPositionalArgument("host", "The VPN gateway host with an optional port.", "[host:port]");
    parser.addOption(optionRealm);
    parser.addOption(optionUrlRegex);
    parser.addOption(optionUrl);
    parser.addOption(optionKeepOpen);
    parser.addOption(optionExtraCaCerts);
    parser.addOption(optionCertificateToTrust);
    parser.addOption(QCommandLineOption("remote-debugging-port", "Remote debugging server port.", "port"));
    parser.addHelpOption();
    parser.addVersionOption();

    if (!parser.parse(QCoreApplication::arguments())) {
        parser.showHelp(1);
    }

    if (parser.isSet("help")) {
        parser.showHelp(0);
    }

    QString url = parser.value(optionUrl);
    if (url.isEmpty()) {
        if (parser.positionalArguments().size() < 1) {
            parser.showHelp(1);
        }
        url = "https://" + parser.positionalArguments().constFirst() + "/remote/saml/start";
        QString realm = parser.value(optionRealm);
        if (!realm.isEmpty()) {
            url += "?realm=" + realm;
        }
    }

    bool keepOpen = parser.isSet(optionKeepOpen);

    auto urlRegex = QRegularExpression(parser.value(optionUrlRegex));
    if (!urlRegex.isValid()) {
        std::cerr << "The given <url-regex> is not valid." << std::endl;
        exit(1);
    }

    auto extraCaCertsPath = parser.value(optionExtraCaCerts);
    QList<QSslCertificate> extraCaCerts;
    if (!extraCaCertsPath.isEmpty()) {
        // Add the custom CA to QSslConfiguration so that later we can verify the chain with it.
        extraCaCerts = QSslCertificate::fromPath(extraCaCertsPath, QSsl::Pem, QSslCertificate::PatternSyntax::FixedString);
        QSslConfiguration configuration = QSslConfiguration::defaultConfiguration();
        auto certs = configuration.caCertificates();
        certs.append(extraCaCerts);
        configuration.setCaCertificates(extraCaCerts);
        QSslConfiguration::setDefaultConfiguration(configuration);
    }

    auto certificateToTrust = parser.value(optionCertificateToTrust);

    MainWindow w(keepOpen, urlRegex, certificateToTrust);
    w.loadUrl(url);
    w.resize(1024, 760);
    w.move(findScreenWithCursor()->geometry().center() - w.rect().center());

    return app.exec();
}
