#include "app.h"

#include <stdio.h>
#include <time.h>

/**
 * Main function
 */
int main(int argc, char* argv[])
{
    App app;
	argc=argc;
	argv=argv;
	srand(time(0));
    init_app(&app, 1920, 1080);
    while (app.is_running) {
        handle_app_events(&app);
        update_app(&app);
        render_app(&app);
    }
    destroy_app(&app);

    return 0;
}
