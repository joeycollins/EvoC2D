#include "multilayerperceptron.h"
#include "component.h"
#include <stdbool.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>

struct input_call_link {
	enum size size;
	union gatherer* gatherer;
	struct input_call_link* next;
};

struct multilayer_perceptron create_multilayer_perceptron(struct genome* genome) {

	struct input_call_link input_head;
	struct input_call_link* input_current = NULL;
	int input_vector_size = 0;

	for (unsigned int i = 0; i < genome->input_genes.count; i++) {
		enum size input_size = genome->input_genes.buffer[i].component->io_component.input.size;
		switch (input_size) {
		case V2:
			input_vector_size += 2;
			break;
		case V3:
			input_vector_size += 3;
			break;
		}

		struct input_call_link new_input = {
			.size = input_size,
			.gatherer = &genome->input_genes.buffer[i].component->io_component.input.gatherer
		};

		if (i == 0) {
			input_head = new_input;
			input_current = &input_head;
		}
		else {
			input_current->next = &new_input;
			input_current = input_current->next;
		}
	}
	

}

float* get_input_vector(int size, struct input_call_link* head) {
	float* vector = malloc(size * sizeof(float));
	if (vector == NULL) { return; }
	int vector_current_size = 0;
	struct input_call_link* current = head;
	while (current != NULL) {
		switch (current->size) {
		case V2:
		{
			vec2 v2;
			(*current->gatherer->gather_v2)(&v2);
			vector[vector_current_size] = v2[0];
			vector[vector_current_size + 1] = v2[1];
			vector_current_size += 2;
			break;
		}
		case V3:
		{
			vec3 v3;
			(*current->gatherer->gather_v3)(&v3);
			vector[vector_current_size] = v3[0];
			vector[vector_current_size + 1] = v3[1];
			vector[vector_current_size + 2] = v3[2];
			vector_current_size += 3;
			break;
		}
		}
		current = current->next;
	}
	return vector;
}