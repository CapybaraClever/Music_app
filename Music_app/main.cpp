#include <QApplication>
#include "Music_app.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("MusicArchive");
    app.setApplicationDisplayName("Музыкальный архив");
    app.setApplicationVersion("1.0");

    Music_app window;
    window.show();

    return app.exec();
}