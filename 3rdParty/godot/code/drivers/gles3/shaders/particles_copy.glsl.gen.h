/* WARNING, THIS FILE WAS GENERATED, DO NOT EDIT */
#ifndef PARTICLES_COPY_GLSL_GEN_HGLES3_GLES3
#define PARTICLES_COPY_GLSL_GEN_HGLES3_GLES3


#include "drivers/gles3/shader_gles3.h"


class ParticlesCopyShaderGLES3 : public ShaderGLES3 {

public:

	enum Uniforms {
		SORT_DIRECTION,
		FRAME_REMAINDER,
		ALIGN_UP,
		ALIGN_MODE,
		INV_EMISSION_TRANSFORM,
	};

	enum ShaderVariant {
		MODE_DEFAULT,
	};

	enum Specializations {
		MODE_3D=1,
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
			"sort_direction",
			"frame_remainder",
			"align_up",
			"align_mode",
			"inv_emission_transform",
		};

		static const char* _variant_defines[]={
			"",
		};

		static TexUnitPair *_texunit_pairs=nullptr;
		static UBOPair *_ubo_pairs=nullptr;
		static Specialization _spec_pairs[]={
			{"MODE_3D",false},
		};

		static const Feedback _feedbacks[]={
			{"out_xform_1",0},
			{"out_xform_2",0},
			{"out_xform_3",1},
			{"instance_color_custom_data",0},
		};

		static const char _vertex_code[]={
10,10,35,105,102,100,101,102,32,85,83,69,95,71,76,69,83,95,79,86,69,82,95,71,76,10,47,47,32,70,108,111,97,116,105,110,103,32,112,111,105,110,116,32,112,97,99,107,47,117,110,112,97,99,107,32,102,117,110,99,116,105,111,110,115,32,97,114,101,32,112,97,114,116,32,111,102,32,116,104,101,32,71,76,83,76,32,69,83,32,51,48,48,32,115,112,101,99,105,102,105,99,97,116,105,111,110,32,117,115,101,100,32,98,121,32,119,101,98,32,97,110,100,32,109,111,98,105,108,101,46,10,117,105,110,116,32,102,108,111,97,116,50,104,97,108,102,40,117,105,110,116,32,102,41,32,123,10,9,117,105,110,116,32,101,32,61,32,102,32,38,32,117,105,110,116,40,48,120,55,102,56,48,48,48,48,48,41,59,10,9,105,102,32,40,101,32,60,61,32,117,105,110,116,40,48,120,51,56,48,48,48,48,48,48,41,41,32,123,10,9,9,114,101,116,117,114,110,32,117,105,110,116,40,48,41,59,10,9,125,32,101,108,115,101,32,123,10,9,9,114,101,116,117,114,110,32,40,40,102,32,62,62,32,117,105,110,116,40,49,54,41,41,32,38,32,117,105,110,116,40,48,120,56,48,48,48,41,41,32,124,10,9,9,9,9,40,40,40,101,32,45,32,117,105,110,116,40,48,120,51,56,48,48,48,48,48,48,41,41,32,62,62,32,117,105,110,116,40,49,51,41,41,32,38,32,117,105,110,116,40,48,120,55,99,48,48,41,41,32,124,10,9,9,9,9,40,40,102,32,62,62,32,117,105,110,116,40,49,51,41,41,32,38,32,117,105,110,116,40,48,120,48,51,102,102,41,41,59,10,9,125,10,125,10,10,117,105,110,116,32,104,97,108,102,50,102,108,111,97,116,40,117,105,110,116,32,104,41,32,123,10,9,117,105,110,116,32,104,95,101,32,61,32,104,32,38,32,117,105,110,116,40,48,120,55,99,48,48,41,59,10,9,114,101,116,117,114,110,32,40,40,104,32,38,32,117,105,110,116,40,48,120,56,48,48,48,41,41,32,60,60,32,117,105,110,116,40,49,54,41,41,32,124,32,117,105,110,116,40,40,104,95,101,32,62,62,32,117,105,110,116,40,49,48,41,41,32,33,61,32,117,105,110,116,40,48,41,41,32,42,32,40,40,40,104,95,101,32,43,32,117,105,110,116,40,48,120,49,99,48,48,48,41,41,32,60,60,32,117,105,110,116,40,49,51,41,41,32,124,32,40,40,104,32,38,32,117,105,110,116,40,48,120,48,51,102,102,41,41,32,60,60,32,117,105,110,116,40,49,51,41,41,41,59,10,125,10,10,117,105,110,116,32,112,97,99,107,72,97,108,102,50,120,49,54,40,118,101,99,50,32,118,41,32,123,10,9,114,101,116,117,114,110,32,102,108,111,97,116,50,104,97,108,102,40,102,108,111,97,116,66,105,116,115,84,111,85,105,110,116,40,118,46,120,41,41,32,124,32,102,108,111,97,116,50,104,97,108,102,40,102,108,111,97,116,66,105,116,115,84,111,85,105,110,116,40,118,46,121,41,41,32,60,60,32,117,105,110,116,40,49,54,41,59,10,125,10,10,118,101,99,50,32,117,110,112,97,99,107,72,97,108,102,50,120,49,54,40,117,105,110,116,32,118,41,32,123,10,9,114,101,116,117,114,110,32,118,101,99,50,40,117,105,110,116,66,105,116,115,84,111,70,108,111,97,116,40,104,97,108,102,50,102,108,111,97,116,40,118,32,38,32,117,105,110,116,40,48,120,102,102,102,102,41,41,41,44,10,9,9,9,117,105,110,116,66,105,116,115,84,111,70,108,111,97,116,40,104,97,108,102,50,102,108,111,97,116,40,118,32,62,62,32,117,105,110,116,40,49,54,41,41,41,41,59,10,125,10,10,117,105,110,116,32,112,97,99,107,85,110,111,114,109,50,120,49,54,40,118,101,99,50,32,118,41,32,123,10,9,117,118,101,99,50,32,117,118,32,61,32,117,118,101,99,50,40,114,111,117,110,100,40,99,108,97,109,112,40,118,44,32,118,101,99,50,40,48,46,48,41,44,32,118,101,99,50,40,49,46,48,41,41,32,42,32,54,53,53,51,53,46,48,41,41,59,10,9,114,101,116,117,114,110,32,117,118,46,120,32,124,32,117,118,46,121,32,60,60,32,117,105,110,116,40,49,54,41,59,10,125,10,10,118,101,99,50,32,117,110,112,97,99,107,85,110,111,114,109,50,120,49,54,40,117,105,110,116,32,112,41,32,123,10,9,114,101,116,117,114,110,32,118,101,99,50,40,102,108,111,97,116,40,112,32,38,32,117,105,110,116,40,48,120,102,102,102,102,41,41,44,32,102,108,111,97,116,40,112,32,62,62,32,117,105,110,116,40,49,54,41,41,41,32,42,32,48,46,48,48,48,48,49,53,50,53,57,48,50,49,59,32,47,47,32,49,46,48,32,47,32,54,53,53,51,53,46,48,32,111,112,116,105,109,105,122,97,116,105,111,110,10,125,10,10,117,105,110,116,32,112,97,99,107,83,110,111,114,109,50,120,49,54,40,118,101,99,50,32,118,41,32,123,10,9,117,118,101,99,50,32,117,118,32,61,32,117,118,101,99,50,40,114,111,117,110,100,40,99,108,97,109,112,40,118,44,32,118,101,99,50,40,45,49,46,48,41,44,32,118,101,99,50,40,49,46,48,41,41,32,42,32,51,50,55,54,55,46,48,41,32,43,32,51,50,55,54,55,46,48,41,59,10,9,114,101,116,117,114,110,32,117,118,46,120,32,124,32,117,118,46,121,32,60,60,32,117,105,110,116,40,49,54,41,59,10,125,10,10,118,101,99,50,32,117,110,112,97,99,107,83,110,111,114,109,50,120,49,54,40,117,105,110,116,32,112,41,32,123,10,9,118,101,99,50,32,118,32,61,32,118,101,99,50,40,102,108,111,97,116,40,112,32,38,32,117,105,110,116,40,48,120,102,102,102,102,41,41,44,32,102,108,111,97,116,40,112,32,62,62,32,117,105,110,116,40,49,54,41,41,41,59,10,9,114,101,116,117,114,110,32,99,108,97,109,112,40,40,118,32,45,32,51,50,55,54,55,46,48,41,32,42,32,118,101,99,50,40,48,46,48,48,48,48,51,48,53,49,56,53,49,41,44,32,118,101,99,50,40,45,49,46,48,41,44,32,118,101,99,50,40,49,46,48,41,41,59,10,125,10,10,35,101,110,100,105,102,10,10,47,47,32,67,111,109,112,97,116,105,98,105,108,105,116,121,32,114,101,110,97,109,101,115,46,32,84,104,101,115,101,32,97,114,101,32,101,120,112,111,115,101,100,32,119,105,116,104,32,116,104,101,32,34,103,111,100,111,116,95,34,32,112,114,101,102,105,120,10,47,47,32,116,111,32,119,111,114,107,32,97,114,111,117,110,100,32,97,110,32,65,100,114,101,110,111,32,98,117,103,32,119,104,105,99,104,32,119,97,115,32,101,120,112,111,115,105,110,103,32,116,104,101,115,101,32,69,83,51,49,48,32,102,117,110,99,116,105,111,110,115,10,47,47,32,105,110,32,69,83,51,48,48,32,115,104,97,100,101,114,115,46,32,73,110,116,101,114,110,97,108,108,121,44,32,119,101,32,109,117,115,116,32,117,115,101,32,116,104,101,32,34,103,111,100,111,116,95,34,32,112,114,101,102,105,120,44,32,98,117,116,32,117,115,101,114,32,115,104,97,100,101,114,115,10,47,47,32,119,105,108,108,32,98,101,32,109,97,112,112,101,100,32,97,117,116,111,109,97,116,105,99,97,108,108,121,46,10,117,105,110,116,32,103,111,100,111,116,95,112,97,99,107,85,110,111,114,109,52,120,56,40,118,101,99,52,32,118,41,32,123,10,9,117,118,101,99,52,32,117,118,32,61,32,117,118,101,99,52,40,114,111,117,110,100,40,99,108,97,109,112,40,118,44,32,118,101,99,52,40,48,46,48,41,44,32,118,101,99,52,40,49,46,48,41,41,32,42,32,50,53,53,46,48,41,41,59,10,9,114,101,116,117,114,110,32,117,118,46,120,32,124,32,40,117,118,46,121,32,60,60,32,117,105,110,116,40,56,41,41,32,124,32,40,117,118,46,122,32,60,60,32,117,105,110,116,40,49,54,41,41,32,124,32,40,117,118,46,119,32,60,60,32,117,105,110,116,40,50,52,41,41,59,10,125,10,10,118,101,99,52,32,103,111,100,111,116,95,117,110,112,97,99,107,85,110,111,114,109,52,120,56,40,117,105,110,116,32,112,41,32,123,10,9,114,101,116,117,114,110,32,118,101,99,52,40,102,108,111,97,116,40,112,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,40,112,32,62,62,32,117,105,110,116,40,56,41,41,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,40,112,32,62,62,32,117,105,110,116,40,49,54,41,41,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,112,32,62,62,32,117,105,110,116,40,50,52,41,41,41,32,42,32,48,46,48,48,51,57,50,49,53,54,56,54,50,59,32,47,47,32,49,46,48,32,47,32,50,53,53,46,48,10,125,10,10,117,105,110,116,32,103,111,100,111,116,95,112,97,99,107,83,110,111,114,109,52,120,56,40,118,101,99,52,32,118,41,32,123,10,9,117,118,101,99,52,32,117,118,32,61,32,117,118,101,99,52,40,114,111,117,110,100,40,99,108,97,109,112,40,118,44,32,118,101,99,52,40,45,49,46,48,41,44,32,118,101,99,52,40,49,46,48,41,41,32,42,32,49,50,55,46,48,41,32,43,32,49,50,55,46,48,41,59,10,9,114,101,116,117,114,110,32,117,118,46,120,32,124,32,117,118,46,121,32,60,60,32,117,105,110,116,40,56,41,32,124,32,117,118,46,122,32,60,60,32,117,105,110,116,40,49,54,41,32,124,32,117,118,46,119,32,60,60,32,117,105,110,116,40,50,52,41,59,10,125,10,10,118,101,99,52,32,103,111,100,111,116,95,117,110,112,97,99,107,83,110,111,114,109,52,120,56,40,117,105,110,116,32,112,41,32,123,10,9,118,101,99,52,32,118,32,61,32,118,101,99,52,40,102,108,111,97,116,40,112,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,40,112,32,62,62,32,117,105,110,116,40,56,41,41,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,40,112,32,62,62,32,117,105,110,116,40,49,54,41,41,32,38,32,117,105,110,116,40,48,120,102,102,41,41,44,32,102,108,111,97,116,40,112,32,62,62,32,117,105,110,116,40,50,52,41,41,41,59,10,9,114,101,116,117,114,110,32,99,108,97,109,112,40,40,118,32,45,32,118,101,99,52,40,49,50,55,46,48,41,41,32,42,32,118,101,99,52,40,48,46,48,48,55,56,55,52,48,49,53,55,52,41,44,32,118,101,99,52,40,45,49,46,48,41,44,32,118,101,99,52,40,49,46,48,41,41,59,10,125,10,10,35,100,101,102,105,110,101,32,112,97,99,107,85,110,111,114,109,52,120,56,32,103,111,100,111,116,95,112,97,99,107,85,110,111,114,109,52,120,56,10,35,100,101,102,105,110,101,32,117,110,112,97,99,107,85,110,111,114,109,52,120,56,32,103,111,100,111,116,95,117,110,112,97,99,107,85,110,111,114,109,52,120,56,10,35,100,101,102,105,110,101,32,112,97,99,107,83,110,111,114,109,52,120,56,32,103,111,100,111,116,95,112,97,99,107,83,110,111,114,109,52,120,56,10,35,100,101,102,105,110,101,32,117,110,112,97,99,107,83,110,111,114,109,52,120,56,32,103,111,100,111,116,95,117,110,112,97,99,107,83,110,111,114,109,52,120,56,10,10,47,47,32,80,97,114,116,105,99,108,101,68,97,116,97,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,48,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,99,111,108,111,114,59,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,49,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,118,101,108,111,99,105,116,121,95,102,108,97,103,115,59,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,50,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,99,117,115,116,111,109,59,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,51,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,120,102,111,114,109,95,49,59,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,52,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,120,102,111,114,109,95,50,59,10,35,105,102,100,101,102,32,77,79,68,69,95,51,68,10,108,97,121,111,117,116,40,108,111,99,97,116,105,111,110,32,61,32,53,41,32,105,110,32,104,105,103,104,112,32,118,101,99,52,32,120,102,111,114,109,95,51,59,10,35,101,110,100,105,102,10,10,47,42,32,99,108,97,110,103,45,102,111,114,109,97,116,32,111,110,32,42,47,10,111,117,116,32,104,105,103,104,112,32,118,101,99,52,32,111,117,116,95,120,102,111,114,109,95,49,59,32,47,47,116,102,98,58,10,111,117,116,32,104,105,103,104,112,32,118,101,99,52,32,111,117,116,95,120,102,111,114,109,95,50,59,32,47,47,116,102,98,58,10,35,105,102,100,101,102,32,77,79,68,69,95,51,68,10,111,117,116,32,104,105,103,104,112,32,118,101,99,52,32,111,117,116,95,120,102,111,114,109,95,51,59,32,47,47,116,102,98,58,77,79,68,69,95,51,68,10,35,101,110,100,105,102,10,102,108,97,116,32,111,117,116,32,104,105,103,104,112,32,117,118,101,99,52,32,105,110,115,116,97,110,99,101,95,99,111,108,111,114,95,99,117,115,116,111,109,95,100,97,116,97,59,32,47,47,116,102,98,58,10,10,117,110,105,102,111,114,109,32,108,111,119,112,32,118,101,99,51,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,59,10,117,110,105,102,111,114,109,32,104,105,103,104,112,32,102,108,111,97,116,32,102,114,97,109,101,95,114,101,109,97,105,110,100,101,114,59,10,10,117,110,105,102,111,114,109,32,104,105,103,104,112,32,118,101,99,51,32,97,108,105,103,110,95,117,112,59,10,117,110,105,102,111,114,109,32,104,105,103,104,112,32,117,105,110,116,32,97,108,105,103,110,95,109,111,100,101,59,10,10,117,110,105,102,111,114,109,32,104,105,103,104,112,32,109,97,116,52,32,105,110,118,95,101,109,105,115,115,105,111,110,95,116,114,97,110,115,102,111,114,109,59,10,10,35,100,101,102,105,110,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,68,73,83,65,66,76,69,68,32,117,105,110,116,40,48,41,10,35,100,101,102,105,110,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,90,95,66,73,76,76,66,79,65,82,68,32,117,105,110,116,40,49,41,10,35,100,101,102,105,110,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,89,95,84,79,95,86,69,76,79,67,73,84,89,32,117,105,110,116,40,50,41,10,35,100,101,102,105,110,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,90,95,66,73,76,76,66,79,65,82,68,95,89,95,84,79,95,86,69,76,79,67,73,84,89,32,117,105,110,116,40,51,41,10,10,35,100,101,102,105,110,101,32,80,65,82,84,73,67,76,69,95,70,76,65,71,95,65,67,84,73,86,69,32,117,105,110,116,40,49,41,10,10,35,100,101,102,105,110,101,32,70,76,84,95,77,65,88,32,102,108,111,97,116,40,51,46,52,48,50,56,50,51,52,54,54,101,43,51,56,41,10,10,118,111,105,100,32,109,97,105,110,40,41,32,123,10,9,47,47,32,83,101,116,32,115,99,97,108,101,32,116,111,32,122,101,114,111,32,97,110,100,32,116,114,97,110,115,108,97,116,101,32,116,111,32,45,73,78,70,32,115,111,32,112,97,114,116,105,99,108,101,32,119,105,108,108,32,98,101,32,105,110,118,105,115,105,98,108,101,10,9,47,47,32,101,118,101,110,32,102,111,114,32,109,97,116,101,114,105,97,108,115,32,116,104,97,116,32,105,103,110,111,114,101,32,114,111,116,97,116,105,111,110,47,115,99,97,108,101,32,40,105,46,101,46,32,98,105,108,108,98,111,97,114,100,115,41,46,10,9,109,97,116,52,32,116,120,102,111,114,109,32,61,32,109,97,116,52,40,118,101,99,52,40,48,46,48,41,44,32,118,101,99,52,40,48,46,48,41,44,32,118,101,99,52,40,48,46,48,41,44,32,118,101,99,52,40,45,70,76,84,95,77,65,88,44,32,45,70,76,84,95,77,65,88,44,32,45,70,76,84,95,77,65,88,44,32,48,46,48,41,41,59,10,9,105,102,32,40,98,111,111,108,40,102,108,111,97,116,66,105,116,115,84,111,85,105,110,116,40,118,101,108,111,99,105,116,121,95,102,108,97,103,115,46,119,41,32,38,32,80,65,82,84,73,67,76,69,95,70,76,65,71,95,65,67,84,73,86,69,41,41,32,123,10,35,105,102,100,101,102,32,77,79,68,69,95,51,68,10,9,9,116,120,102,111,114,109,32,61,32,116,114,97,110,115,112,111,115,101,40,109,97,116,52,40,120,102,111,114,109,95,49,44,32,120,102,111,114,109,95,50,44,32,120,102,111,114,109,95,51,44,32,118,101,99,52,40,48,46,48,44,32,48,46,48,44,32,48,46,48,44,32,49,46,48,41,41,41,59,10,35,101,108,115,101,10,9,9,116,120,102,111,114,109,32,61,32,116,114,97,110,115,112,111,115,101,40,109,97,116,52,40,120,102,111,114,109,95,49,44,32,120,102,111,114,109,95,50,44,32,118,101,99,52,40,48,46,48,44,32,48,46,48,44,32,49,46,48,44,32,48,46,48,41,44,32,118,101,99,52,40,48,46,48,44,32,48,46,48,44,32,48,46,48,44,32,49,46,48,41,41,41,59,10,35,101,110,100,105,102,10,10,9,9,115,119,105,116,99,104,32,40,97,108,105,103,110,95,109,111,100,101,41,32,123,10,9,9,9,99,97,115,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,68,73,83,65,66,76,69,68,58,32,123,10,9,9,9,125,32,98,114,101,97,107,59,32,47,47,110,111,116,104,105,110,103,10,9,9,9,99,97,115,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,90,95,66,73,76,76,66,79,65,82,68,58,32,123,10,9,9,9,9,109,97,116,51,32,108,111,99,97,108,32,61,32,109,97,116,51,40,110,111,114,109,97,108,105,122,101,40,99,114,111,115,115,40,97,108,105,103,110,95,117,112,44,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,41,41,44,32,97,108,105,103,110,95,117,112,44,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,41,59,10,9,9,9,9,108,111,99,97,108,32,61,32,108,111,99,97,108,32,42,32,109,97,116,51,40,116,120,102,111,114,109,41,59,10,9,9,9,9,116,120,102,111,114,109,91,48,93,46,120,121,122,32,61,32,108,111,99,97,108,91,48,93,59,10,9,9,9,9,116,120,102,111,114,109,91,49,93,46,120,121,122,32,61,32,108,111,99,97,108,91,49,93,59,10,9,9,9,9,116,120,102,111,114,109,91,50,93,46,120,121,122,32,61,32,108,111,99,97,108,91,50,93,59,10,10,9,9,9,125,32,98,114,101,97,107,59,10,9,9,9,99,97,115,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,89,95,84,79,95,86,69,76,79,67,73,84,89,58,32,123,10,9,9,9,9,118,101,99,51,32,118,32,61,32,118,101,108,111,99,105,116,121,95,102,108,97,103,115,46,120,121,122,59,10,9,9,9,9,102,108,111,97,116,32,115,32,61,32,40,108,101,110,103,116,104,40,116,120,102,111,114,109,91,48,93,41,32,43,32,108,101,110,103,116,104,40,116,120,102,111,114,109,91,49,93,41,32,43,32,108,101,110,103,116,104,40,116,120,102,111,114,109,91,50,93,41,41,32,47,32,51,46,48,59,10,9,9,9,9,105,102,32,40,108,101,110,103,116,104,40,118,41,32,62,32,48,46,48,41,32,123,10,9,9,9,9,9,116,120,102,111,114,109,91,49,93,46,120,121,122,32,61,32,110,111,114,109,97,108,105,122,101,40,118,41,59,10,9,9,9,9,125,32,101,108,115,101,32,123,10,9,9,9,9,9,116,120,102,111,114,109,91,49,93,46,120,121,122,32,61,32,110,111,114,109,97,108,105,122,101,40,116,120,102,111,114,109,91,49,93,46,120,121,122,41,59,10,9,9,9,9,125,10,10,9,9,9,9,116,120,102,111,114,109,91,48,93,46,120,121,122,32,61,32,110,111,114,109,97,108,105,122,101,40,99,114,111,115,115,40,116,120,102,111,114,109,91,49,93,46,120,121,122,44,32,116,120,102,111,114,109,91,50,93,46,120,121,122,41,41,59,10,9,9,9,9,116,120,102,111,114,109,91,50,93,46,120,121,122,32,61,32,118,101,99,51,40,48,46,48,44,32,48,46,48,44,32,49,46,48,41,32,42,32,115,59,10,9,9,9,9,116,120,102,111,114,109,91,48,93,46,120,121,122,32,42,61,32,115,59,10,9,9,9,9,116,120,102,111,114,109,91,49,93,46,120,121,122,32,42,61,32,115,59,10,9,9,9,125,32,98,114,101,97,107,59,10,9,9,9,99,97,115,101,32,84,82,65,78,83,70,79,82,77,95,65,76,73,71,78,95,90,95,66,73,76,76,66,79,65,82,68,95,89,95,84,79,95,86,69,76,79,67,73,84,89,58,32,123,10,9,9,9,9,118,101,99,51,32,115,118,32,61,32,118,101,108,111,99,105,116,121,95,102,108,97,103,115,46,120,121,122,32,45,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,32,42,32,100,111,116,40,115,111,114,116,95,100,105,114,101,99,116,105,111,110,44,32,118,101,108,111,99,105,116,121,95,102,108,97,103,115,46,120,121,122,41,59,32,47,47,115,99,114,101,101,110,32,118,101,108,111,99,105,116,121,10,9,9,9,9,102,108,111,97,116,32,115,32,61,32,40,108,101,110,103,116,104,40,116,120,102,111,114,109,91,48,93,41,32,43,32,108,101,110,103,116,104,40,116,120,102,111,114,109,91,49,93,41,32,43,32,108,101,110,103,116,104,40,116,120,102,111,114,109,91,50,93,41,41,32,47,32,51,46,48,59,10,10,9,9,9,9,105,102,32,40,108,101,110,103,116,104,40,115,118,41,32,61,61,32,48,46,48,41,32,123,10,9,9,9,9,9,115,118,32,61,32,97,108,105,103,110,95,117,112,59,10,9,9,9,9,125,10,10,9,9,9,9,115,118,32,61,32,110,111,114,109,97,108,105,122,101,40,115,118,41,59,10,10,9,9,9,9,116,120,102,111,114,109,91,48,93,46,120,121,122,32,61,32,110,111,114,109,97,108,105,122,101,40,99,114,111,115,115,40,115,118,44,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,41,41,32,42,32,115,59,10,9,9,9,9,116,120,102,111,114,109,91,49,93,46,120,121,122,32,61,32,115,118,32,42,32,115,59,10,9,9,9,9,116,120,102,111,114,109,91,50,93,46,120,121,122,32,61,32,115,111,114,116,95,100,105,114,101,99,116,105,111,110,32,42,32,115,59,10,10,9,9,9,125,32,98,114,101,97,107,59,10,9,9,125,10,10,9,9,116,120,102,111,114,109,91,51,93,46,120,121,122,32,43,61,32,118,101,108,111,99,105,116,121,95,102,108,97,103,115,46,120,121,122,32,42,32,102,114,97,109,101,95,114,101,109,97,105,110,100,101,114,59,10,10,35,105,102,110,100,101,102,32,77,79,68,69,95,51,68,10,9,9,47,47,32,73,110,32,103,108,111,98,97,108,32,109,111,100,101,44,32,98,114,105,110,103,32,50,68,32,112,97,114,116,105,99,108,101,115,32,116,111,32,108,111,99,97,108,32,99,111,111,114,100,105,110,97,116,101,115,10,9,9,47,47,32,97,115,32,116,104,101,121,32,119,105,108,108,32,98,101,32,100,114,97,119,110,32,119,105,116,104,32,116,104,101,32,110,111,100,101,32,112,111,115,105,116,105,111,110,32,97,115,32,111,114,105,103,105,110,46,10,9,9,116,120,102,111,114,109,32,61,32,105,110,118,95,101,109,105,115,115,105,111,110,95,116,114,97,110,115,102,111,114,109,32,42,32,116,120,102,111,114,109,59,10,35,101,110,100,105,102,10,9,125,10,9,116,120,102,111,114,109,32,61,32,116,114,97,110,115,112,111,115,101,40,116,120,102,111,114,109,41,59,10,10,9,105,110,115,116,97,110,99,101,95,99,111,108,111,114,95,99,117,115,116,111,109,95,100,97,116,97,32,61,32,117,118,101,99,52,40,112,97,99,107,72,97,108,102,50,120,49,54,40,99,111,108,111,114,46,120,121,41,44,32,112,97,99,107,72,97,108,102,50,120,49,54,40,99,111,108,111,114,46,122,119,41,44,32,112,97,99,107,72,97,108,102,50,120,49,54,40,99,117,115,116,111,109,46,120,121,41,44,32,112,97,99,107,72,97,108,102,50,120,49,54,40,99,117,115,116,111,109,46,122,119,41,41,59,10,9,111,117,116,95,120,102,111,114,109,95,49,32,61,32,116,120,102,111,114,109,91,48,93,59,10,9,111,117,116,95,120,102,111,114,109,95,50,32,61,32,116,120,102,111,114,109,91,49,93,59,10,35,105,102,100,101,102,32,77,79,68,69,95,51,68,10,9,111,117,116,95,120,102,111,114,109,95,51,32,61,32,116,120,102,111,114,109,91,50,93,59,10,35,101,110,100,105,102,10,125,10,10,47,42,32,99,108,97,110,103,45,102,111,114,109,97,116,32,111,102,102,32,42,47,10,		0};

		static const char _fragment_code[]={
10,118,111,105,100,32,109,97,105,110,40,41,32,123,10,125,10,47,42,32,99,108,97,110,103,45,102,111,114,109,97,116,32,111,110,32,42,47,10,		0};

		_setup(_vertex_code,_fragment_code,"ParticlesCopyShaderGLES3",5,_uniform_strings,0,_ubo_pairs,4,_feedbacks,0,_texunit_pairs,1,_spec_pairs,1,_variant_defines);
	}

};

#endif
