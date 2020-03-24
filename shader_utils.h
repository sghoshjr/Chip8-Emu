#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

GLuint LoadShaders(const char *vertexShader, const char *fragmentShader);

void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                     GLsizei length, const GLchar *message, GLvoid *userParam);

#endif // SHADER_UTILS_H