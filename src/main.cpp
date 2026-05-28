#include "app.h"

int main(int, char**)
{
    App app;

    if (!app.Initialize())
        return 1;

    return app.Run();
}