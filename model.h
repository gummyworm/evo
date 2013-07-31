#ifndef MODEL_H
#define MODEL_H
#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "tv_alloc.h"

#define MODEL_ATTRIBUTE_VERTEX_SIZE (sizeof(float)*3)
#define MODEL_ATTRIBUTE_COLOR_SIZE  (sizeof(float)*4)
#define MODEL_ATTRIBUTE_NORMAL_SIZE (sizeof(float)*3)
#define MODEL_ATTRIBUTE_TEXCO_SIZE  (sizeof(float)*2)
#define MODEL_ATTRIBUTE_INDEX_SIZE  (sizeof(GLshort)*1)

enum {
    MODEL_ATTRIBUTE_NONE,
    MODEL_ATTRIBUTE_VERTEX,
    MODEL_ATTRIBUTE_COLOR,
    MODEL_ATTRIBUTE_NORMAL,
    MODEL_ATTRIBUTE_TEXCO,
	MODEL_ATTRIBUTE_INDEX
};

typedef struct tagTvModelAttribute {
	tvuint type;
	tvuint size;
	TvArray* data;
} TvModelAttribute;

typedef struct TvModelAttributeList {
	TvModelAttribute attr;
	struct TvModelAttributeList* next;
}TvModelAttributeList;

typedef struct tagTvModel {
	TvModelAttributeList* attributes;
	GLuint* vbo_ids;
	GLshort* indices;
	GLuint vao;
	GLuint primitive;
	GLuint num_vertices;
	GLuint num_indices;
	tvchar* name;
	TvHashHandle hh;
}TvModel;

int tv_model_init();
TvModel* tv_model_new();
TvModel* tv_model_load_ply(tvchar* file);

void tv_model_add_attribute(TvModel* model, tvuint attribute);
void tv_model_buffer_attribute(TvModel* model, tvuint attribute, TvArray* buffer);
TvModelAttribute* tv_model_get_attribute(TvModel* model, tvuint attribute);

TvAABB tv_model_get_aabb(TvModel* model);
void tv_model_optimize(TvModel* model);
void tv_model_free(TvModel* model);

#ifdef __cplusplus
}
#endif
#endif
