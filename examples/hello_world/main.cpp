#include "my_app.hpp"
#include "my_engine.hpp"

int main(int, char**)
{
    MyApp    app;
    MyEngine engine;

    return engine.Run(&app);
}
