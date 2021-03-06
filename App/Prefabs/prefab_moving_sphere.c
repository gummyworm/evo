#include "app_prefabs.h"
#include "app_resources.h"

PREFAB(prefab_moving_sphere)
ADDCOMPONENT(tv_model_renderer, renderer)
ADDCOMPONENT(tv_model, model)
ADDCOMPONENT(tv_material, material)
ADDCOMPONENT(TV_collider, collider)
ADDCOMPONENT(APP_sine_move, movement)
ADDCOMPONENT(tv_line_renderer, liner)
	tv_vector3 speed = {10.0f, 0.0f, 0.0f};
	tv_model_load_ply(model, RSC_FAIRY);
	tv_model_optimize(model, TRUE, TRUE);
	tv_material_load(material, RSC_TEST_MAT);
	tv_collider_sphere(collider, 1.0f);
	app_sine_move_set_speed(movement, speed);
ENTITY_END
