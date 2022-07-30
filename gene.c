#include "gene.h"

struct gene create_gene(int id, int distance, struct component* component) {
	struct gene new_gene = {
		.id = id,
		.distance = distance,
		.component = component,
		.value = 0.0f,
		.active_incoming_connections = 0,
		.incoming_connections_executed = 0
	};
	return new_gene;
}