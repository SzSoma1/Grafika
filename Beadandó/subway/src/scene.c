#include "scene.h"
#include "texture.h"
#include <GL/gl.h>
#include <obj/model.h>
#include <obj/load.h>
#include <obj/draw.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>
#include <time.h>

void init_scene(Scene* scene)
{
	scene->f1help= load_texture("assets/textures/help.png");
	scene->light=0.5f;
	scene->max_lines=3;
	scene->next_line=0;
	scene->max_blocks=15;
	scene->block_movespeed=160.0f;
	scene->block_spawnspeed=0.35f;
	scene->block_spawnspeed_current=scene->block_spawnspeed;
	scene->spawned_lines=0;
	scene->block_size=1.0f;
	scene->player_pos=(scene->max_lines/2)+1;
	
	//allocate the lines
	scene->moving_blocks=(Blocks*)malloc(scene->max_blocks*sizeof(Blocks));
	//go through all lines
	for(int i=0;i<scene->max_blocks;i++){
		//allocate every block in the current line
		scene->moving_blocks[i].pos=(double*)malloc(scene->max_lines*sizeof(double));
		scene->moving_blocks[i].line_number=(int*)malloc(scene->max_lines*sizeof(int));
		for(int k=0;k<scene->max_lines;k++){
			scene->moving_blocks[i].pos[k]=14.0f;
			scene->moving_blocks[i].line_number[k]=k+1;
		}
	}
	load_model(&(scene->blockmodel), "assets/models/cube.obj");
	scene->blocktexture = load_texture("assets/textures/test.png");
	load_model(&(scene->background_model), "assets/models/sphere.obj");
	scene->background_texture = load_texture("assets/textures/background.jpg");
	load_model(&(scene->ground_model), "assets/models/cube.obj");
	scene->ground_texture = load_texture("assets/textures/ground.png");

    scene->material.ambient.red = 1.0;
    scene->material.ambient.green = 1.0;
    scene->material.ambient.blue = 1.0;

    scene->material.diffuse.red = 1.0;
    scene->material.diffuse.green = 1.0;
    scene->material.diffuse.blue = 0.0;

    scene->material.specular.red = 0.0;
    scene->material.specular.green = 0.0;
    scene->material.specular.blue = 0.0;

    scene->material.shininess = 0.0;
}



void set_lighting(const Scene* scene)
{
    float ambient_light[] = { scene->light, scene->light, scene->light, 1.0f };
    float diffuse_light[] = { scene->light, scene->light, scene->light, 1.0f };
    float specular_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}


int update_scene(Scene* scene,double elapsed_time)
{
	if (elapsed_time<0.0) return 0;
	//calculate the next spawntime of a block
	scene->block_spawnspeed_current-=0.1f*elapsed_time;
	//if the next line should spawn
	if(scene->block_spawnspeed_current<=0){
		//reset spawntime
		scene->block_spawnspeed_current=scene->block_spawnspeed;
		//if not every line is spawned
		if(scene->spawned_lines<scene->max_blocks){
			//generate the random missing position
			scene->moving_blocks[scene->spawned_lines].skipped=rand()%scene->max_lines;
			scene->spawned_lines++;
		}else{//if every line already spawned (delete the last line)
			for(int k=0;k<scene->max_lines;k++){
				//reset the position of the blocks in the line
				scene->moving_blocks[scene->next_line].pos[k]=14.0f;
			}
			//generate the random missing position
			scene->moving_blocks[scene->next_line].skipped=rand()%scene->max_lines;
			scene->next_line++;
			//increment the next_line, always the next_line will despawn
			if(scene->next_line>=scene->max_blocks){
				scene->next_line=0;
			}
		}
	}
	//go through every spawned lines
	for(int i=0;i<scene->spawned_lines;i++){
		//go through every block in the line
		for(int k=0;k<scene->max_lines;k++){
			//calculate the position
			scene->moving_blocks[i].pos[k]-=scene->block_movespeed/100*elapsed_time;
			//check collision detection
			if(scene->moving_blocks[i].pos[k]<(scene->block_size/2) && scene->moving_blocks[i].pos[k]>-(scene->block_size/2)){
				if(scene->moving_blocks[i].skipped+1!=scene->player_pos){
					printf("game over");
					return 1;
				}
			}
		}
	}
	//increment move and spawn speed
	scene->block_movespeed+=20.0f*elapsed_time;
	scene->block_spawnspeed-=0.01f*elapsed_time;
	if(scene->block_spawnspeed<0.05f){
		scene->block_spawnspeed=0.05f;
	}
	return 0;
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting(scene);
	
	glBindTexture(GL_TEXTURE_2D, scene->background_texture);
	//go through every line
	for(int i=0;i<scene->spawned_lines;i++){
		//go through every block
		for(int k=0;k<scene->max_lines;k++){
			//spawn the blocks
			if(scene->moving_blocks[i].skipped==k) continue;
			glTranslatef(scene->moving_blocks[i].pos[k],scene->moving_blocks[i].line_number[k]-(scene->max_lines/2)-1,0);
			draw_model(&(scene->blockmodel));
			glTranslatef(-scene->moving_blocks[i].pos[k],-(scene->moving_blocks[i].line_number[k]-(scene->max_lines/2)-1),0);
		}
	}
	glPushMatrix();
	glScalef(50.0,50.0,50.0);
	glBindTexture(GL_TEXTURE_2D, scene->background_texture);
	draw_model(&(scene->background_model));
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0.0,0.0,-1.0);
	glScalef(30.0,scene->max_lines,1.0);
	glBindTexture(GL_TEXTURE_2D, scene->ground_texture);
	draw_model(&(scene->ground_model));
	glPopMatrix();
}