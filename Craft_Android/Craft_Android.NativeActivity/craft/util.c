#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "lodepng.h"
#include "matrix.h"
#include "util.h"
#include <android\asset_manager.h>

#include <sys/time.h>
//#include "shader_source.h"

long long time_base = 0;
double time_resolution = 0.000001;

int rand_int(int n) {
	int result;
	while (n <= (result = rand() / (RAND_MAX / n)));
	return result;
}

double rand_double() {
	return (double)rand() / (double)RAND_MAX;
}

double craft_get_time()
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return (double)(tval.tv_sec / time_resolution + tval.tv_usec - time_base)*time_resolution;

}

void craft_set_time(double time)
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	time_base = tval.tv_sec / time_resolution + tval.tv_usec - (long)(time / time_resolution);

}

void update_fps(FPS* fps) {
	fps->frames++;
	double now = craft_get_time();
	double elapsed = now - fps->since;
	if (elapsed >= 1) {
		fps->fps = round(fps->frames / elapsed);
		fps->frames = 0;
		fps->since = now;
	}
}

const char* load_file(AAssetManager* asset_manager, const char* path) {
	AAsset* asset = AAssetManager_open(asset_manager, path, AASSET_MODE_BUFFER);
	int fileSize = AAsset_getLength(asset);
	void* data = malloc(fileSize+1);
	memset(data, 0, fileSize + 1);
	memcpy(data, AAsset_getBuffer(asset), fileSize);
	//uint8_t* data = (uint8_t*)AAsset_getBuffer(asset);
	AAsset_close(asset);
	return data;
}

GLuint gen_buffer(GLsizei size, GLfloat* data) {
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return buffer;
}

void del_buffer(GLuint buffer) {
	glDeleteBuffers(1, &buffer);
}

GLfloat* malloc_faces(int components, int faces) {
	return malloc(sizeof(GLfloat) * 6 * components * faces);
}

GLuint gen_faces(int components, int faces, GLfloat* data) {
	GLuint buffer = gen_buffer(
		sizeof(GLfloat) * 6 * components * faces, data);
	free(data);
	return buffer;
}

GLuint make_shader(GLenum type, const GLchar* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		GLchar* info = calloc(length, sizeof(GLchar));
		glGetShaderInfoLog(shader, length, NULL, info);
		fprintf(stderr, "glCompileShader failed:\n%s\n", info);
		LOGE("glCompileShader failed:\n%s\n", info);
		free(info);
	}
	return shader;
}

GLuint load_shader(AAssetManager* asset_manager, GLenum type, const char* path) {
	const char* data = load_file(asset_manager, path);
	GLuint result = make_shader(type, (GLchar*)data);
	free(data);
	return result;
}

GLuint make_program(GLuint shader1, GLuint shader2) {
	GLuint program = glCreateProgram();
	glAttachShader(program, shader1);
	glAttachShader(program, shader2);
	glLinkProgram(program);
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		GLchar* info = calloc(length, sizeof(GLchar));
		glGetProgramInfoLog(program, length, NULL, info);
		fprintf(stderr, "glLinkProgram failed: %s\n", info);
		free(info);
	}
	glDetachShader(program, shader1);
	glDetachShader(program, shader2);
	glDeleteShader(shader1);
	glDeleteShader(shader2);
	return program;
}

GLuint load_program(AAssetManager* asset_manager, const char* path1, const char* path2) {
	GLuint shader1 = load_shader(asset_manager, GL_VERTEX_SHADER, path1);
	GLuint shader2 = load_shader(asset_manager, GL_FRAGMENT_SHADER, path2);
	GLuint program = make_program(shader1, shader2);
	return program;
}

void flip_image_vertical(
	unsigned char* data, unsigned int width, unsigned int height)
{
	unsigned int size = width * height * 4;
	unsigned int stride = sizeof(char) * width * 4;
	unsigned char* new_data = malloc(sizeof(unsigned char) * size);
	for (unsigned int i = 0; i < height; i++) {
		unsigned int j = height - i - 1;
		memcpy(new_data + j * stride, data + i * stride, stride);
	}
	memcpy(data, new_data, size);
	free(new_data);
}

void load_png_texture(AAssetManager* asset_manager, const char* file_name) {
	AAsset* asset = AAssetManager_open(asset_manager, file_name, AASSET_MODE_BUFFER);
	const unsigned char* asset_buffer = AAsset_getBuffer(asset);
	off_t asset_lenght = AAsset_getLength(asset);
	uint width, height;
	unsigned int error;
	unsigned char* image_buffer;
	error = lodepng_decode_memory(&image_buffer, &width, &height, asset_buffer, asset_lenght, LCT_RGBA, 8);
	if (error) {
		fprintf(stderr, "load_png_texture %s failed, error %u: %s\n", file_name, error, lodepng_error_text(error));
		exit(1);
	}
	flip_image_vertical(image_buffer, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_buffer);
	free(image_buffer);
	AAsset_close(asset);
}

char* tokenize(char* str, const char* delim, char** key) {
	char* result;
	if (str == NULL) {
		str = *key;
	}
	str += strspn(str, delim);
	if (*str == '\0') {
		return NULL;
	}
	result = str;
	str += strcspn(str, delim);
	if (*str) {
		*str++ = '\0';
	}
	*key = str;
	return result;
}

int char_width(char input) {
	static const int lookup[128] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		4, 2, 4, 7, 6, 9, 7, 2, 3, 3, 4, 6, 3, 5, 2, 7,
		6, 3, 6, 6, 6, 6, 6, 6, 6, 6, 2, 3, 5, 6, 5, 7,
		8, 6, 6, 6, 6, 6, 6, 6, 6, 4, 6, 6, 5, 8, 8, 6,
		6, 7, 6, 6, 6, 6, 8,10, 8, 6, 6, 3, 6, 3, 6, 6,
		4, 7, 6, 6, 6, 6, 5, 6, 6, 2, 5, 5, 2, 9, 6, 6,
		6, 6, 6, 6, 5, 6, 6, 6, 6, 6, 6, 4, 2, 5, 7, 0
	};
	return lookup[(int)input];
}

int string_width(const char* input) {
	int result = 0;
	int length = strlen(input);
	for (int i = 0; i < length; i++) {
		result += char_width(input[i]);
	}
	return result;
}

int wrap(const char* input, int max_width, char* output, int max_length) {
	*output = '\0';
	char* text = malloc(sizeof(char) * (strlen(input) + 1));
	strcpy(text, input);
	int space_width = char_width(' ');
	int line_number = 0;
	char* key1, * key2;
	char* line = tokenize(text, "\r\n", &key1);
	while (line) {
		int line_width = 0;
		char* token = tokenize(line, " ", &key2);
		while (token) {
			int token_width = string_width(token);
			if (line_width) {
				if (line_width + token_width > max_width) {
					line_width = 0;
					line_number++;
					strncat(output, "\n", max_length - strlen(output) - 1);
				}
				else {
					strncat(output, " ", max_length - strlen(output) - 1);
				}
			}
			strncat(output, token, max_length - strlen(output) - 1);
			line_width += token_width + space_width;
			token = tokenize(NULL, " ", &key2);
		}
		line_number++;
		strncat(output, "\n", max_length - strlen(output) - 1);
		line = tokenize(NULL, "\r\n", &key1);
	}
	free(text);
	return line_number;
}
