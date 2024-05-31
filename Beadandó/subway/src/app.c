#include "app.h"

#include <SDL2/SDL_image.h>

void init_app(App* app, int width, int height)
{
    int error_code;
    int inited_loaders;
	app->is_moved=false;
    app->is_running = false;
	app->f1help=false;
    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    app->window = SDL_CreateWindow(
        "Subway Surfers",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL);
    if (app->window == NULL) {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG);
    if (inited_loaders == 0) {
        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }

    init_opengl();
    reshape(width, height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->is_running = true;
}

void init_opengl()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.5, 0.5, 0.5, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);

    glClearDepth(1.0);
	glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void reshape(GLsizei width, GLsizei height)
{
    int x, y, w, h;
    double ratio;

    ratio = (double)width / height;
    if (ratio > VIEWPORT_RATIO) {
        w = (int)((double)height * VIEWPORT_RATIO);
        h = height;
        x = (width - w) / 2;
        y = 0;
    }
    else {
        w = width;
        h = (int)((double)width / VIEWPORT_RATIO);
        x = 0;
        y = (height - h) / 2;
    }

    glViewport(x, y, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(
        -.08, .08,
        -.06, .06,
        .1, 100
    );
}

void handle_app_events(App* app)
{
    SDL_Event event;
    static bool is_mouse_down = false;
    static int mouse_x = 0;
    static int mouse_y = 0;
    int x;
    int y;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
			//move the camera
			case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
				if(app->is_moved==false){
					set_camera_side_speed(&(app->camera), 1,&(app->scene.block_size),&(app->scene.player_pos),&(app->scene.max_lines));
				}
				app->is_moved=true;
                break;
			//move the camera
			case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
				if(app->is_moved==false){
					set_camera_side_speed(&(app->camera), -1,&(app->scene.block_size),&(app->scene.player_pos),&(app->scene.max_lines));
				}
				app->is_moved=true;
                break;
			//set the lightning
			case SDL_SCANCODE_KP_PLUS:
				if(app->scene.light<1.0){
					app->scene.light+=0.02;
				}
				glClearColor(app->scene.light, app->scene.light, app->scene.light, 1.0);
                break;
			case SDL_SCANCODE_KP_MINUS:
				if(app->scene.light>0.0){
					app->scene.light-=0.02;
				}
				glClearColor(app->scene.light, app->scene.light, app->scene.light, 1.0);
                break;
			//show the help menu
			case SDL_SCANCODE_F1:
				if(app->f1help){
					app->f1help=false;
				}else{
					app->f1help=true;
				}
				break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
			//reset movement button
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_RIGHT:
				app->is_moved=false;
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            is_mouse_down = true;
            break;
        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&x, &y);
            if (is_mouse_down) {
                rotate_camera(&(app->camera), mouse_x - x, mouse_y - y);
            }
            mouse_x = x;
            mouse_y = y;
            break;
        case SDL_MOUSEBUTTONUP:
            is_mouse_down = false;
            break;
        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
            break;
        }
    }
}

void update_app(App* app)
{
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000;
    elapsed_time = current_time - app->uptime;
    app->uptime = current_time;

    update_camera(&(app->camera), elapsed_time);
	//update scene and check collision detection
    int result=update_scene(&(app->scene), elapsed_time);
	if(result==1){ 
		printf("Your time:   %lf\n",app->uptime);
		app->is_running=false;
	}
}

void render_app(App* app)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
    set_view(&(app->camera));
    render_scene(&(app->scene));
    glPopMatrix();
	//show the help menu
	if(app->f1help){
		glBindTexture(GL_TEXTURE_2D, app->scene.f1help);
		show_texture_preview();
	}
    SDL_GL_SwapWindow(app->window);
}

void destroy_app(App* app)
{
    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}
