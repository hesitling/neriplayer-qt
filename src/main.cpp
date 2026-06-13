#include "app/NeriPlayerApplication.h"

int main(int argc, char *argv[])
{
    NeriPlayerQt::NeriPlayerApplication app(argc, argv);
    if (!app.initialize()) {
        return 1;
    }

    app.showMainWindow();
    return app.exec();
}
