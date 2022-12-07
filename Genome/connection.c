#include "connection.h"
#include "utils.h"

void mutate_connection(struct connection* connection, float mutate_weight_chance, float mutate_enabled_chance) {
	float random = rand_flt();
	if (random < mutate_weight_chance) {
		random = rand_flt();
		connection->weight = random;
	}
	random = rand_flt();
	if (random < mutate_enabled_chance) {
		connection->enabled = !connection->enabled;
	}
}