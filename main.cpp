#include <QApplication>
#include "GifOverlay.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Keep app running even when window is hidden (lives in system tray)
    app.setQuitOnLastWindowClosed(false);

    app.setApplicationName("GifOverlay");
    app.setOrganizationName("GifOverlay");

    GifOverlay overlay;
    overlay.show();

    return app.exec();
}
