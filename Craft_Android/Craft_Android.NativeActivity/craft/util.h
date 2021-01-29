#ifndef _util_h_
#define _util_h_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "config.h"
#include <android\asset_manager.h>
#include <android/log.h>



#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

#define PI 3.14159265359
#define DEGREES(radians) ((radians) * 180 / PI)
#define RADIANS(degrees) ((degrees) * PI / 180)
#define ABS(x) ((x) < 0 ? (-(x)) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SIGN(x) (((x) > 0) - ((x) < 0))

#if DEBUG
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

typedef struct {
	unsigned int fps;
	unsigned int frames;
	double since;
} FPS;

int rand_int(int n);
double rand_double();
void update_fps(FPS* fps);

GLuint gen_buffer(GLsizei size, GLfloat* data);
void del_buffer(GLuint buffer);
GLfloat* malloc_faces(int components, int faces);
GLuint gen_faces(int components, int faces, GLfloat* data);
GLuint make_shader(GLenum type, const GLchar* source);
GLuint load_shader(AAssetManager* asset_manager, GLenum type, const char* path);
GLuint make_program(GLuint shader1, GLuint shader2);
GLuint load_program(AAssetManager* asset_manager, const char* path1, const char* path2);
void load_png_texture(AAssetManager* asset_manager, const char* file_name);
char* tokenize(char* str, const char* delim, char** key);
int char_width(char input);
int string_width(const char* input);
int wrap(const char* input, int max_width, char* output, int max_length);

double	craft_get_time();
void	craft_set_time(double time);

#endif
