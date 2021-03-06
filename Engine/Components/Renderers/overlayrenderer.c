#include "overlayrenderer.h"

typedef struct tv_my_vertex {
	tvfloat x;
	tvfloat y;
	tvfloat z;
}tv_my_vertex;

typedef struct tv_textrenderer_vertex {
	tv_my_vertex pos;
}tv_textrenderer_vertex;


void tv_overlay_renderer_set_model(tv_overlay_renderer *self, tv_model *model)
{
	self->model = model;
}

HANDLER_NEW(tv_overlay_renderer, tv_renderer, render, TV_UPDATE_STAGE_GUI_RENDER)
	self->base.render_func = render;
	self->base.material = NULL;
END_HANDLER_NEW(tv_overlay_renderer)

HANDLER_START(tv_overlay_renderer)
	if(self->base.material == NULL) {
		self->base.material = (tv_material*)tv_component_get((tv_component*)self, tv_material_id());
	}
END_HANDLER_START

HANDLER_UPDATE(tv_overlay_renderer)
END_HANDLER_UPDATE

static void render(tv_component *self)
{
	tv_overlay_renderer *renderer = (tv_overlay_renderer*)self;
	tv_material* mat;

	tv_vector3 pos; 
	tv_vector3 scale; 
	tvuint i;

	if(renderer->model == NULL) {
		return;
	}

	pos = self->entity->transform.pos;
	scale  = self->entity->transform.scale;

	glDisable(GL_DEPTH_TEST);
	tv_mat4x4_push(&tv_camera_gui->modelview_mat);
	tv_mat4x4_load_identity(&tv_camera_gui->modelview_mat);
	tv_mat4x4_scale(&tv_camera_gui->modelview_mat, scale.x, scale.y, scale.z);
	tv_mat4x4_translate(&tv_camera_gui->modelview_mat, pos.x, pos.y, pos.z);

	/* render all passes of the bone */
	mat = renderer->base.material;

	/* bind the models' vertex attribute object. */
	glBindVertexArray(renderer->model->vao);
	for(i = 0; i < mat->num_passes; ++i) {
		tv_material_do_pass_gui(mat, i, renderer->model);
	}
	glBindVertexArray(0);

	tv_mat4x4_pop(&tv_camera_gui->modelview_mat);
	glEnable(GL_DEPTH_TEST);
}
