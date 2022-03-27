#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCursor>
#include <QDesktopWidget>
#include <QRect>
#include <QScreen>

static QScreen *findScreenWithCursor()
{
    QPoint pos = QCursor::pos();

    for (QScreen *screen : QGuiApplication::screens()) {
        QRect screenRect = screen->geometry();
        if (screenRect.contains(pos)) {
            return screen;
        }
    }

    return QApplication::desktop()->screen();
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCommandLineParser parser;
    parser.addPositionalArgument("gateway", "VPN gateway, possibly including port and realm");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption("remote-debugging-port", "Remote debugging server port"));
    parser.parse(QCoreApplication::arguments());

    if (parser.positionalArguments().size() < 1) {
        parser.showHelp(1);
    }
    QString gateway = parser.positionalArguments()[0];
    QString samlUrl = "https://" + gateway + "/remote/saml/start";

    MainWindow w;

    w.loadUrl(samlUrl);
    w.resize(1024, 760);
    w.move(findScreenWithCursor()->geometry().center() - w.rect().center());
    w.show();

    return app.exec();
}
