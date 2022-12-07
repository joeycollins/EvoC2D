#ifndef CREATURE_CONTEXT_INCLUDED
#define CREATURE_CONTEXT_INCLUDED

#include <cglm/mat4.h>

struct creature;

//singleton context for our entites, handles shape hashing, etc 
struct creature_context {
	unsigned int shader;
	int creatures_capacity;
	int creatures_count;
	int alive_creatures;
	struct creature** creatures; 
	void (*update)(struct creature_context*);
};

struct creature_context create_creature_context(int count, void (*create_creature)(struct creature*), unsigned int shader);

struct creature* add_to_context(struct creature_context* context);
#endif