#include "component.h"
#include "actualizer.h"
#include "gatherer.h"

struct component create_thruster_component(float r, float g, float b) {
	struct component thruster = {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = OUTPUT,
		.io_component = {
			.output = {
				.size = V2,
				.actualizer = {
					.actualize_v2 = &thruster
				}
			}
		}
	};
	return thruster;
}

struct component create_food_sensor_component(float r, float g, float b) {
	struct component sensor = {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = INPUT,
		.io_component = {
			.input = {
				.size = V2,
				.gatherer = {
					.gather_v2 = &food_sensor
				}
			}
		}
	};
	return sensor;
}

struct component create_rotator_component(float r, float g, float b) {
	struct component rotator = {
		.color = {r,g,b},
		.children_count = 0,
		.io_type = OUTPUT,
		.io_component = {
			.output = {
				.size = V1,
				.actualizer = {
					.actualize_v2 = &rotator
				}
			}
		}
	};
	return rotator;
}