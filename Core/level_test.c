/*
 * A test for functionality of level files
 * Michelle Shu
 * 2/18/2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "level.h"

int main (int argc, char *argv[])
{
	// Test memory allocation and initialization of level.
    Level* l = level_new("level.lvl");

    // If we get this far with no error, level has been loaded.
    printf("Level loaded.\n");

    // Should have read in contents of "level.lvl", which contains properties
    // of two boxes and one star. Let's check.
    printf("Level Number: %d\n", l->level_number);
    printf("Level has %d boxes, %d polygons and %d stars.\n", l->box_number, l->poly_number, l->star_number);

    // The level contains dimensions of the world.
	printf("The world has dimensions (%5.2lf, %5.2lf)\n", l->world_width, l->world_height);

    for (int i = 0; i < l->box_number; i++) {
    	Box *b = (&l->boxes)[i];
    	printf("Box Number %d:\n", i);
    	printf("\tPosition: (%5.2lf, %5.2lf)\n", b->x, b->y);
    	printf("\tSize: (%5.2lf, %5.2lf)\n", b->width, b->height);
    	printf("\tMass: %5.2lf\n", b->mass);
    	printf("\tColor: (%5.2lf, %5.2lf, %5.2lf)\n", b->red, b->green, b->blue);
    }

    for (int i = 0; i < l->poly_number; i++) {
		Poly *p = (&l->polygons)[i];
    	printf("Poly Number %d:\n", i);
    	printf("\tCenter: (%5.2lf, %5.2lf)\n", p->c_x, p->c_y);
		printf("\tNumber of Vertices: %d\n", p->num_vertices);
		int j = 0;
		for (j = 0; j < 2 * p->num_vertices; j += 2)
		{
			printf("\t\tVertex: ");
			printf("(%5.2lf, %5.2lf)\n", p->vertices[j], p->vertices[j + 1]);
		} 
    	printf("\tMass: %5.2lf\n", p->mass);
    	printf("\tColor: (%5.2lf, %5.2lf, %5.2lf)\n", p->red, p->green, p->blue);
    }

    for (int i = 0; i < l->star_number; i++) {
    	Star *s = (&l->stars)[i];
    	printf("Star Number %d:\n", i);
    	printf("\tPosition: (%5.2lf, %5.2lf)\n", s->x, s->y);
    	printf("\tRadius: %5.2lf\n", s->radius);
    	printf("\tMass: %5.2lf\n", s->mass);
    	printf("\tColor: (%5.2lf, %5.2lf, %5.2lf)\n", s->red, s->green, s->blue);
    }

	Ball *b = &l->ball;
    printf("Ball:\n");
    printf("\tPosition: (%5.2lf, %5.2lf)\n", b->x, b->y);
    printf("\tRadius: %5.2lf\n", b->radius);
    printf("\tMass: %5.2lf\n", b->mass);
    printf("\tColor: (%5.2lf, %5.2lf, %5.2lf)\n", b->red, b->green, b->blue);
   
	// Free the memory associated with the level.
    level_destroy(l);

	return EXIT_SUCCESS;
}

