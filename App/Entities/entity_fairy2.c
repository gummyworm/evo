#include "app_entities.h"
#include "app_resources.h"

ENTITY(fairy2, 3.0f,0.0f,-5.0f, 1,1,1, 0,0,0)
ADDCOMPONENT(tv_animation_renderer, renderer)
ADDCOMPONENT(tv_animation, animation)
ADDCOMPONENT(tv_material, material)
ADDCOMPONENT(app_player_motor, pm)
ADDCOMPONENT(app_ability_move, ability_move)
ADDCOMPONENT(app_unit, unit)
	tv_model *model = tv_model_new();

	tv_animation_bone bone = {{3,0,0},{0.0f,0.0f,45.0f,0},model,material, TV_ANIMATION_BONE_END, TV_ANIMATION_BONE_END};
	tv_animation_bone bone2 = {{3,0,0},{0,0,0,0},model,material, TV_ANIMATION_BONE_END, TV_ANIMATION_BONE_END};
	tv_animation_bone bone3 = {{6,0,0},{0,0,-90.0f,0},model,material, TV_ANIMATION_BONE_END, TV_ANIMATION_BONE_END};
	tv_animation_bone bone4 = {{3,0,0},{0,0,0,0},model,material, TV_ANIMATION_BONE_END, TV_ANIMATION_BONE_END};
	tvuint b1id, b2id;

	unit->current_stats.speed = 1.0f;

	app_ability_set_target((app_ability*)ability_move, 10.0f, 0.0f, -5.0f);

	tv_model_load_ply(model, RSC_FAIRY);
	tv_model_optimize(model, TRUE, TRUE);
	tv_material_load(material, RSC_TEST_MAT);
	tv_animation_set_root(animation, model, material);

	b1id = tv_animation_add_bone(animation, 0, bone);
	tv_animation_add_bone(animation, b1id, bone2);
	b2id = tv_animation_add_bone(animation, b1id, bone3);
	tv_animation_add_bone(animation, b2id, bone4);

	e->tags = APP_TAG_UNIT;
	app_unit_set_move_ability(unit, (app_ability*)ability_move);
	pm->speed = 0.5f;

	app_unit_set_float_height(unit, 3.0f);
ENTITY_END
