#include "Core/Application.hpp"

#ifdef PK_EDITOR
int main(int argc, char **argv)
{
    Piksela::Application *application = Piksela::CreateEditorApplication(argc, argv);

    application->Run();

    delete application;
    return 0;
}
#endif

#ifdef PK_RUNTIME
int main(int argc, char **argv)
{
    Piksela::Application *application = Piksela::CreateRuntimeApplication(argc, argv);

    application->Run();

    delete application;
    return 0;
}
#endif