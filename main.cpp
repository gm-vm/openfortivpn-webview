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
    parser.addPositionalArgument("url", "The SAML URL for the single sign-on authentication.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(QCommandLineOption("remote-debugging-port", "Remote debugging server port"));
    parser.parse(QCoreApplication::arguments());

    if (parser.positionalArguments().size() < 1) {
        parser.showHelp(1);
    }
    QString url = parser.positionalArguments()[0];

    MainWindow w;

    w.loadUrl(url);
    w.resize(1024, 760);
    w.move(findScreenWithCursor()->geometry().center() - w.rect().center());
    w.show();

    return app.exec();
}
