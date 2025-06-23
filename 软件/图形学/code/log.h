#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
// 输出着色器对象编译信息
void printShaderLog(GLuint shader)
{
  int   len      = 0;
  int   chWrittn = 0;
  char *log;
  // 查询着色器对象的日志信息长度（包括错误、警告、编译信息等）。
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
  if (len > 0)
  {
    // 分配日志存放内存
    log = (char *)malloc(len);
    // 读取着色器的日志信息到 log 指针指向的内存中
    glGetShaderInfoLog(shader, len, &chWrittn, log);
    cout << "shader Info Log: " << log << endl;
    // 释放内存
    free(log);
  }
}

// 输出程序链接时信息
void printProgramLog(GLuint prog)
{
  int   len      = 0;
  int   chWrittn = 0;
  char *log;
  glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
  if (len > 0)
  {
    log = (char *)malloc(len);
    glGetProgramInfoLog(prog, len, &chWrittn, log);
    cout << "Program Info Log: " << log << endl;
    free(log);
  }
}

// 检查并输出当前OPenGL的所有错误信息
bool checkOpenGlError()
{
  bool foundError = false;
  int  glErr      = glGetError();
  while (glErr != GL_NO_ERROR)
  {
    cout << "glError: " << glErr << endl;
    foundError = true;
    glErr      = glGetError();
  }
  return foundError;
}
