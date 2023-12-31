/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef CANVAS_SDF_GLSL_GEN_HGLES3_GLES3
#define CANVAS_SDF_GLSL_GEN_HGLES3_GLES3


#include "drivers/gles3/shader_gles3.h"


class CanvasSdfShaderGLES3 : public ShaderGLES3 {

public:

	enum Uniforms {
		SIZE,
		STRIDE,
		SHIFT,
		BASE_SIZE,
	};

	enum ShaderVariant {
		MODE_LOAD,
		MODE_LOAD_SHRINK,
		MODE_PROCESS,
		MODE_STORE,
		MODE_STORE_SHRINK,
	};

	_FORCE_INLINE_ bool version_bind_shader(RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { return _version_bind_shader(p_version,p_variant,p_specialization); }

	_FORCE_INLINE_ int version_get_uniform(Uniforms p_uniform,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { return _version_get_uniform(p_uniform,p_version,p_variant,p_specialization); }

	#define _FU if (version_get_uniform(p_uniform,p_version,p_variant,p_specialization)<0) return; 

 	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, float p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1f(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, double p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1f(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, uint8_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1ui(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, int8_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1i(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, uint16_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1ui(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, int16_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1i(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, uint32_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1ui(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, int32_t p_value,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform1i(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_value); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Color& p_color,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU GLfloat col[4]={p_color.r,p_color.g,p_color.b,p_color.a}; glUniform4fv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,col); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Vector2& p_vec2,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU GLfloat vec2[2]={float(p_vec2.x),float(p_vec2.y)}; glUniform2fv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,vec2); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Size2i& p_vec2,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU GLint vec2[2]={GLint(p_vec2.x),GLint(p_vec2.y)}; glUniform2iv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,vec2); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Vector3& p_vec3,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU GLfloat vec3[3]={float(p_vec3.x),float(p_vec3.y),float(p_vec3.z)}; glUniform3fv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,vec3); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, float p_a, float p_b,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform2f(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_a,p_b); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, float p_a, float p_b, float p_c,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform3f(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_a,p_b,p_c); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, float p_a, float p_b, float p_c, float p_d,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) { _FU glUniform4f(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),p_a,p_b,p_c,p_d); }

	_FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Transform3D& p_transform,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) {  _FU

            const Transform3D &tr = p_transform;

            GLfloat matrix[16]={ /* build a 16x16 matrix */
                (GLfloat)tr.basis.rows[0][0],
                (GLfloat)tr.basis.rows[1][0],
                (GLfloat)tr.basis.rows[2][0],
                (GLfloat)0,
                (GLfloat)tr.basis.rows[0][1],
                (GLfloat)tr.basis.rows[1][1],
                (GLfloat)tr.basis.rows[2][1],
                (GLfloat)0,
                (GLfloat)tr.basis.rows[0][2],
                (GLfloat)tr.basis.rows[1][2],
                (GLfloat)tr.basis.rows[2][2],
                (GLfloat)0,
                (GLfloat)tr.origin.x,
                (GLfloat)tr.origin.y,
                (GLfloat)tr.origin.z,
                (GLfloat)1
            };

                    glUniformMatrix4fv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,false,matrix);

        }

        _FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Transform2D& p_transform,RID p_version,ShaderVariant p_variant,uint64_t p_specialization=0) {  _FU

            const Transform2D &tr = p_transform;

        GLfloat matrix[16]={ /* build a 16x16 matrix */
            (GLfloat)tr.columns[0][0],
            (GLfloat)tr.columns[0][1],
            (GLfloat)0,
            (GLfloat)0,
            (GLfloat)tr.columns[1][0],
            (GLfloat)tr.columns[1][1],
            (GLfloat)0,
            (GLfloat)0,
            (GLfloat)0,
            (GLfloat)0,
            (GLfloat)1,
            (GLfloat)0,
            (GLfloat)tr.columns[2][0],
            (GLfloat)tr.columns[2][1],
            (GLfloat)0,
            (GLfloat)1
        };

            glUniformMatrix4fv(version_get_uniform(p_uniform,p_version,p_variant,p_specialization),1,false,matrix);

        }

        _FORCE_INLINE_ void version_set_uniform(Uniforms p_uniform, const Projection& p_matrix, RID p_version, ShaderVariant p_variant,uint64_t p_specialization=0) {  _FU

            GLfloat matrix[16];

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    matrix[i * 4 + j] = p_matrix.columns[i][j];
                }
            }

            glUniformMatrix4fv(version_get_uniform(p_uniform, p_version, p_variant, p_specialization), 1, false, matrix);
    }

#undef _FU


protected:

	virtual void _init() override {

		static const char* _uniform_strings[]={
			"size",
			"stride",
			"shift",
			"base_size",
		};

		static const char* _variant_defines[]={
			"#define MODE_LOAD",
			"#define MODE_LOAD_SHRINK",
			"#define MODE_PROCESS",
			"#define MODE_STORE",
			"#define MODE_STORE_SHRINK",
		};

		static TexUnitPair _texunit_pairs[]={
			{"src_pixels",0},
			{"src_process",0},
		};

		static UBOPair *_ubo_pairs=nullptr;
		static Specialization *_spec_pairs=nullptr;
		static const Feedback* _feedbacks=nullptr;
		static const char _vertex_code[]={
10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,48,41,32,105,110,32,118,101,99,50,32,118,101,114,116,101,120,95,97,116,116,114,105,98,59,10,10,47,42,32,99,108,97,110,103,45,102,111,114,109,97,116,32,111,110,32,42,47,10,10,117,110,105,102,111,114,109,32,105,118,101,99,50,32,115,105,122,101,59,10,117,110,105,102,111,114,109,32,105,110,116,32,115,116,114,105,100,101,59,10,117,110,105,102,111,114,109,32,105,110,116,32,115,104,105,102,116,59,10,117,110,105,102,111,114,109,32,105,118,101,99,50,32,98,97,115,101,95,115,105,122,101,59,10,10,118,111,105,100,32,109,97,105,110,40,41,32,123,10,9,103,108,95,80,111,115,105,116,105,111,110,32,61,32,118,101,99,52,40,118,101,114,116,101,120,95,97,116,116,114,105,98,44,32,49,46,48,44,32,49,46,48,41,59,10,125,10,10,47,42,32,99,108,97,110,103,45,102,111,114,109,97,116,32,111,102,102,32,42,47,10,		0};

		static const char _fragment_code[]={
10,35,100,101,102,105,110,101,32,83,68,70,95,77,65,88,95,76,69,78,71,84,72,32,49,54,51,56,52,46,48,10,10,35,105,102,32,100,101,102,105,110,101,100,40,77,79,68,69,95,76,79,65,68,41,32,124,124,32,100,101,102,105,110,101,100,40,77,79,68,69,95,76,79,65,68,95,83,72,82,73,78,75,41,10,117,110,105,102,111,114,109,32,108,111,119,112,32,115,97,109,112,108,101,114,50,68,32,115,114,99,95,112,105,120,101,108,115,59,47,47,116,101,120,117,110,105,116,58,48,10,35,101,108,115,101,10,117,110,105,102,111,114,109,32,104,105,103,104,112,32,105,115,97,109,112,108,101,114,50,68,32,115,114,99,95,112,114,111,99,101,115,115,59,47,47,116,101,120,117,110,105,116,58,48,10,35,101,110,100,105,102,10,10,117,110,105,102,111,114,109,9,105,118,101,99,50,32,115,105,122,101,59,10,117,110,105,102,111,114,109,9,105,110,116,32,115,116,114,105,100,101,59,10,117,110,105,102,111,114,109,9,105,110,116,32,115,104,105,102,116,59,10,117,110,105,102,111,114,109,9,105,118,101,99,50,32,98,97,115,101,95,115,105,122,101,59,10,10,35,105,102,32,100,101,102,105,110,101,100,40,77,79,68,69,95,76,79,65,68,41,32,124,124,32,100,101,102,105,110,101,100,40,77,79,68,69,95,76,79,65,68,95,83,72,82,73,78,75,41,32,124,124,32,100,101,102,105,110,101,100,40,77,79,68,69,95,80,82,79,67,69,83,83,41,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,48,41,32,111,117,116,32,105,118,101,99,52,32,100,105,115,116,97,110,99,101,95,102,105,101,108,100,59,10,35,101,108,115,101,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,48,41,32,111,117,116,32,118,101,99,52,32,100,105,115,116,97,110,99,101,95,102,105,101,108,100,59,10,35,101,110,100,105,102,10,10,118,101,99,52,32,102,108,111,97,116,95,116,111,95,118,101,99,52,40,102,108,111,97,116,32,112,95,102,108,111,97,116,41,32,123,10,32,32,32,32,104,105,103,104,112,32,118,101,99,52,32,99,111,109,112,32,61,32,102,114,97,99,116,40,112,95,102,108,111,97,116,32,42,32,118,101,99,52,40,50,53,53,46,48,32,42,32,50,53,53,46,48,32,42,32,50,53,53,46,48,44,32,50,53,53,46,48,32,42,32,50,53,53,46,48,44,32,50,53,53,46,48,44,32,49,46,48,41,41,59,10,9,99,111,109,112,32,45,61,32,99,111,109,112,46,120,120,121,122,32,42,32,118,101,99,52,40,48,46,48,44,32,49,46,48,32,47,32,50,53,53,46,48,44,32,49,46,48,32,47,32,50,53,53,46,48,44,32,49,46,48,32,47,32,50,53,53,46,48,41,59,10,9,114,101,116,117,114,110,32,99,111,109,112,59,10,125,10,10,118,111,105,100,32,109,97,105,110,40,41,32,123,10,9,105,118,101,99,50,32,112,111,115,32,61,32,105,118,101,99,50,40,103,108,95,70,114,97,103,67,111,111,114,100,46,120,121,41,59,10,10,35,105,102,100,101,102,32,77,79,68,69,95,76,79,65,68,10,10,9,98,111,111,108,32,115,111,108,105,100,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,105,120,101,108,115,44,32,112,111,115,44,32,48,41,46,114,32,62,32,48,46,53,59,10,9,100,105,115,116,97,110,99,101,95,102,105,101,108,100,32,61,32,115,111,108,105,100,32,63,32,105,118,101,99,52,40,105,118,101,99,50,40,45,51,50,55,54,55,41,44,32,48,44,32,48,41,32,58,32,105,118,101,99,52,40,105,118,101,99,50,40,51,50,55,54,55,41,44,32,48,44,32,48,41,59,10,35,101,110,100,105,102,10,10,35,105,102,100,101,102,32,77,79,68,69,95,76,79,65,68,95,83,72,82,73,78,75,10,10,9,105,110,116,32,115,32,61,32,49,32,60,60,32,115,104,105,102,116,59,10,9,105,118,101,99,50,32,98,97,115,101,32,61,32,112,111,115,32,60,60,32,115,104,105,102,116,59,10,9,105,118,101,99,50,32,99,101,110,116,101,114,32,61,32,98,97,115,101,32,43,32,105,118,101,99,50,40,115,104,105,102,116,41,59,10,10,9,105,118,101,99,50,32,114,101,108,32,61,32,105,118,101,99,50,40,51,50,55,54,55,41,59,10,9,102,108,111,97,116,32,100,32,61,32,49,101,50,48,59,10,9,105,110,116,32,102,111,117,110,100,32,61,32,48,59,10,9,105,110,116,32,115,111,108,105,100,95,102,111,117,110,100,32,61,32,48,59,10,9,102,111,114,32,40,105,110,116,32,105,32,61,32,48,59,32,105,32,60,32,115,59,32,105,43,43,41,32,123,10,9,9,102,111,114,32,40,105,110,116,32,106,32,61,32,48,59,32,106,32,60,32,115,59,32,106,43,43,41,32,123,10,9,9,9,105,118,101,99,50,32,115,114,99,95,112,111,115,32,61,32,98,97,115,101,32,43,32,105,118,101,99,50,40,105,44,32,106,41,59,10,9,9,9,105,102,32,40,97,110,121,40,103,114,101,97,116,101,114,84,104,97,110,69,113,117,97,108,40,115,114,99,95,112,111,115,44,32,98,97,115,101,95,115,105,122,101,41,41,41,32,123,10,9,9,9,9,99,111,110,116,105,110,117,101,59,10,9,9,9,125,10,9,9,9,98,111,111,108,32,115,111,108,105,100,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,105,120,101,108,115,44,32,115,114,99,95,112,111,115,44,32,48,41,46,114,32,62,32,48,46,53,59,10,9,9,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,9,9,102,108,111,97,116,32,100,105,115,116,32,61,32,108,101,110,103,116,104,40,118,101,99,50,40,115,114,99,95,112,111,115,32,45,32,99,101,110,116,101,114,41,41,59,10,9,9,9,9,105,102,32,40,100,105,115,116,32,60,32,100,41,32,123,10,9,9,9,9,9,100,32,61,32,100,105,115,116,59,10,9,9,9,9,9,114,101,108,32,61,32,115,114,99,95,112,111,115,59,10,9,9,9,9,125,10,9,9,9,9,115,111,108,105,100,95,102,111,117,110,100,43,43,59,10,9,9,9,125,10,9,9,9,102,111,117,110,100,43,43,59,10,9,9,125,10,9,125,10,10,9,105,102,32,40,115,111,108,105,100,95,102,111,117,110,100,32,61,61,32,102,111,117,110,100,41,32,123,10,9,9,47,47,109,97,114,107,32,115,111,108,105,100,32,111,110,108,121,32,105,102,32,97,108,108,32,97,114,101,32,115,111,108,105,100,10,9,9,114,101,108,32,61,32,105,118,101,99,50,40,45,51,50,55,54,55,41,59,10,9,125,10,10,9,100,105,115,116,97,110,99,101,95,102,105,101,108,100,32,61,32,105,118,101,99,52,40,114,101,108,44,32,48,44,32,48,41,59,10,35,101,110,100,105,102,10,10,35,105,102,100,101,102,32,77,79,68,69,95,80,82,79,67,69,83,83,10,10,9,105,118,101,99,50,32,98,97,115,101,32,61,32,112,111,115,32,60,60,32,115,104,105,102,116,59,10,9,105,118,101,99,50,32,99,101,110,116,101,114,32,61,32,98,97,115,101,32,43,32,105,118,101,99,50,40,115,104,105,102,116,41,59,10,10,9,105,118,101,99,50,32,114,101,108,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,114,111,99,101,115,115,44,32,112,111,115,44,32,48,41,46,120,121,59,10,10,9,98,111,111,108,32,115,111,108,105,100,32,61,32,114,101,108,46,120,32,60,32,48,59,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,114,101,108,32,61,32,45,114,101,108,32,45,32,105,118,101,99,50,40,49,41,59,10,9,125,10,10,9,105,102,32,40,99,101,110,116,101,114,32,33,61,32,114,101,108,41,32,123,10,9,9,47,47,111,110,108,121,32,112,114,111,99,101,115,115,32,105,102,32,105,116,32,100,111,101,115,32,110,111,116,32,112,111,105,110,116,32,116,111,32,105,116,115,101,108,102,10,9,9,99,111,110,115,116,32,105,110,116,32,111,102,115,95,116,97,98,108,101,95,115,105,122,101,32,61,32,56,59,10,9,9,99,111,110,115,116,32,105,118,101,99,50,32,111,102,115,95,116,97,98,108,101,91,111,102,115,95,116,97,98,108,101,95,115,105,122,101,93,32,61,32,105,118,101,99,50,91,93,40,10,9,9,9,9,105,118,101,99,50,40,45,49,44,32,45,49,41,44,10,9,9,9,9,105,118,101,99,50,40,48,44,32,45,49,41,44,10,9,9,9,9,105,118,101,99,50,40,43,49,44,32,45,49,41,44,10,10,9,9,9,9,105,118,101,99,50,40,45,49,44,32,48,41,44,10,9,9,9,9,105,118,101,99,50,40,43,49,44,32,48,41,44,10,10,9,9,9,9,105,118,101,99,50,40,45,49,44,32,43,49,41,44,10,9,9,9,9,105,118,101,99,50,40,48,44,32,43,49,41,44,10,9,9,9,9,105,118,101,99,50,40,43,49,44,32,43,49,41,41,59,10,10,9,9,102,108,111,97,116,32,100,105,115,116,32,61,32,108,101,110,103,116,104,40,118,101,99,50,40,114,101,108,32,45,32,99,101,110,116,101,114,41,41,59,10,9,9,102,111,114,32,40,105,110,116,32,105,32,61,32,48,59,32,105,32,60,32,111,102,115,95,116,97,98,108,101,95,115,105,122,101,59,32,105,43,43,41,32,123,10,9,9,9,105,118,101,99,50,32,115,114,99,95,112,111,115,32,61,32,112,111,115,32,43,32,111,102,115,95,116,97,98,108,101,91,105,93,32,42,32,115,116,114,105,100,101,59,10,9,9,9,105,102,32,40,97,110,121,40,108,101,115,115,84,104,97,110,40,115,114,99,95,112,111,115,44,32,105,118,101,99,50,40,48,41,41,41,32,124,124,32,97,110,121,40,103,114,101,97,116,101,114,84,104,97,110,69,113,117,97,108,40,115,114,99,95,112,111,115,44,32,115,105,122,101,41,41,41,32,123,10,9,9,9,9,99,111,110,116,105,110,117,101,59,10,9,9,9,125,10,9,9,9,105,118,101,99,50,32,115,114,99,95,114,101,108,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,114,111,99,101,115,115,44,32,115,114,99,95,112,111,115,44,32,48,41,46,120,121,59,10,9,9,9,98,111,111,108,32,115,114,99,95,115,111,108,105,100,32,61,32,115,114,99,95,114,101,108,46,120,32,60,32,48,59,10,9,9,9,105,102,32,40,115,114,99,95,115,111,108,105,100,41,32,123,10,9,9,9,9,115,114,99,95,114,101,108,32,61,32,45,115,114,99,95,114,101,108,32,45,32,105,118,101,99,50,40,49,41,59,10,9,9,9,125,10,10,9,9,9,105,102,32,40,115,114,99,95,115,111,108,105,100,32,33,61,32,115,111,108,105,100,41,32,123,10,9,9,9,9,115,114,99,95,114,101,108,32,61,32,105,118,101,99,50,40,115,114,99,95,112,111,115,32,60,60,32,115,104,105,102,116,41,59,32,47,47,112,111,105,110,116,32,116,111,32,105,116,115,101,108,102,32,105,102,32,111,102,32,100,105,102,102,101,114,101,110,116,32,116,121,112,101,10,9,9,9,125,10,10,9,9,9,102,108,111,97,116,32,115,114,99,95,100,105,115,116,32,61,32,108,101,110,103,116,104,40,118,101,99,50,40,115,114,99,95,114,101,108,32,45,32,99,101,110,116,101,114,41,41,59,10,9,9,9,105,102,32,40,115,114,99,95,100,105,115,116,32,60,32,100,105,115,116,41,32,123,10,9,9,9,9,100,105,115,116,32,61,32,115,114,99,95,100,105,115,116,59,10,9,9,9,9,114,101,108,32,61,32,115,114,99,95,114,101,108,59,10,9,9,9,125,10,9,9,125,10,9,125,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,114,101,108,32,61,32,45,114,101,108,32,45,32,105,118,101,99,50,40,49,41,59,10,9,125,10,10,9,100,105,115,116,97,110,99,101,95,102,105,101,108,100,32,61,32,105,118,101,99,52,40,114,101,108,44,32,48,44,32,48,41,59,10,35,101,110,100,105,102,10,10,35,105,102,100,101,102,32,77,79,68,69,95,83,84,79,82,69,10,10,9,105,118,101,99,50,32,114,101,108,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,114,111,99,101,115,115,44,32,112,111,115,44,32,48,41,46,120,121,59,10,10,9,98,111,111,108,32,115,111,108,105,100,32,61,32,114,101,108,46,120,32,60,32,48,59,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,114,101,108,32,61,32,45,114,101,108,32,45,32,105,118,101,99,50,40,49,41,59,10,9,125,10,10,9,102,108,111,97,116,32,100,32,61,32,108,101,110,103,116,104,40,118,101,99,50,40,114,101,108,32,45,32,112,111,115,41,41,59,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,100,32,61,32,45,100,59,10,9,125,10,10,9,100,32,47,61,32,83,68,70,95,77,65,88,95,76,69,78,71,84,72,59,10,9,100,32,61,32,99,108,97,109,112,40,100,44,32,45,49,46,48,44,32,49,46,48,41,59,10,9,100,105,115,116,97,110,99,101,95,102,105,101,108,100,32,61,32,102,108,111,97,116,95,116,111,95,118,101,99,52,40,100,42,48,46,53,43,48,46,53,41,59,10,10,35,101,110,100,105,102,10,10,35,105,102,100,101,102,32,77,79,68,69,95,83,84,79,82,69,95,83,72,82,73,78,75,10,10,9,105,118,101,99,50,32,98,97,115,101,32,61,32,112,111,115,32,60,60,32,115,104,105,102,116,59,10,9,105,118,101,99,50,32,99,101,110,116,101,114,32,61,32,98,97,115,101,32,43,32,105,118,101,99,50,40,115,104,105,102,116,41,59,10,10,9,105,118,101,99,50,32,114,101,108,32,61,32,116,101,120,101,108,70,101,116,99,104,40,115,114,99,95,112,114,111,99,101,115,115,44,32,112,111,115,44,32,48,41,46,120,121,59,10,10,9,98,111,111,108,32,115,111,108,105,100,32,61,32,114,101,108,46,120,32,60,32,48,59,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,114,101,108,32,61,32,45,114,101,108,32,45,32,105,118,101,99,50,40,49,41,59,10,9,125,10,10,9,102,108,111,97,116,32,100,32,61,32,108,101,110,103,116,104,40,118,101,99,50,40,114,101,108,32,45,32,99,101,110,116,101,114,41,41,59,10,10,9,105,102,32,40,115,111,108,105,100,41,32,123,10,9,9,100,32,61,32,45,100,59,10,9,125,10,9,100,32,47,61,32,83,68,70,95,77,65,88,95,76,69,78,71,84,72,59,10,9,100,32,61,32,99,108,97,109,112,40,100,44,32,45,49,46,48,44,32,49,46,48,41,59,10,9,100,105,115,116,97,110,99,101,95,102,105,101,108,100,32,61,32,102,108,111,97,116,95,116,111,95,118,101,99,52,40,100,42,48,46,53,43,48,46,53,41,59,10,10,35,101,110,100,105,102,10,125,10,		0};

		_setup(_vertex_code,_fragment_code,"CanvasSdfShaderGLES3",4,_uniform_strings,0,_ubo_pairs,0,_feedbacks,2,_texunit_pairs,0,_spec_pairs,5,_variant_defines);
	}

};

#endif

