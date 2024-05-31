#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include <obj/model.h>
#include <GL/gl.h>
#include "texture.h"
#include <stdlib.h>
#include <stdio.h>

//stucture of a whole line
typedef struct blocks
{
	double *pos;						//position of the blocks in the line on the x axis
	int *line_number;					//position of the blocks in the line on the y axis(used for placement and for collision detection)
	int skipped;						//the missing blocks position in the line
}Blocks;

typedef struct Scene
{
	Blocks * moving_blocks;				//array of lines
	double block_movespeed;				//movement speed of a block
	double block_spawnspeed;			//spawning speed of a line
	double block_spawnspeed_current;	//next lines spawning time, if it's 0 or less, the line will spawn
	double block_size;					//size of a block ,used for collision detection and moving
	Model blockmodel;					//model of the block
	GLuint blocktexture;				//texture of the block
	Model background_model;
	GLuint background_texture;
	Model ground_model;
	GLuint ground_texture;
	Material material;					//materials, lights of the program
	int player_pos;						//current player position
	int max_lines;						//maximum block per line
	int spawned_lines;					//count of spawned lines
	int next_line;						//index of the next line that will despawn
	int max_blocks;						//maximum lines per game
	float light;						//program light
	GLuint f1help;						//helpmenu texture
} Scene;

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene* scene);

/**
 * Set the lighting of the scene.
 */
void set_lighting(const Scene* scene);

/**
 * Set the current material.
 */
void set_material(const Material* material);
/**
 * Update the scene.
 */
int update_scene(Scene* scene,double elapsed_time);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene);
#endif /* SCENE_H */
