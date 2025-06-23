#include "log.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

#define numVAOs 1
GLuint renderingProgram;
GLuint vao[numVAOs];

GLuint createShaderProgram()
{
  const char *vshaderSource =
      "#version 330 \n"
      "void main(void) \n"
      "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";

  // const char *fshaderSource =
  //     "#version 330 \n"
  //     "out vec4 color; \n"
  //     "void main(void) \n"
  //     "{ color = vec4(0.0, 0.0, 1.0, 1.0); }";

  const char *fshaderSource = R"(
    #version 330
    out vec4 color;
    void main(void) {
      color = vec4(0.0, 1.0, 0.0, 1.0); // 默认绿色
      if(gl_FragCoord.x > 600.0) {
        color = vec4(1.0, 0.0, 0.0, 1.0);
      }
    }
  )";

  GLint vertCompiled;

  GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(vShader, 1, &vshaderSource, NULL);
  glShaderSource(fShader, 1, &fshaderSource, NULL);

  glCompileShader(vShader);
  checkOpenGlError();
  glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
  if (vertCompiled != 1)
  {
    cout << "vertex compilation failed" << endl;
    printShaderLog(vShader);
  }

  glCompileShader(fShader);

  GLuint vfProgram = glCreateProgram();
  glAttachShader(vfProgram, vShader);
  glAttachShader(vfProgram, fShader);
  glLinkProgram(vfProgram);

  return vfProgram;
}

void init(GLFWwindow *window)
{
  renderingProgram = createShaderProgram();
  glGenVertexArrays(numVAOs, vao);
  glBindVertexArray(vao[0]);
}

void display(GLFWwindow *window, double currentTime)
{
  glUseProgram(renderingProgram);
  glPointSize(100.0f);
  glDrawArrays(GL_POINTS, 0, 1);
  // glClearColor(0.0, 0.0, 0.0, 1.0f);
  // glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
  if (!glfwInit())
  {
    exit(EXIT_FAILURE);
  }
  // 设置主版本号
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  // 设置次版本号
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow *window = glfwCreateWindow(600, 600, "Chapter2- program1", NULL, NULL);
  glfwMakeContextCurrent(window);

  // glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK)
  {
    exit(EXIT_FAILURE);
  }

  // 垂直同步
  glfwSwapInterval(1);

  init(window);

  while (!glfwWindowShouldClose(window))
  {
    display(window, glfwGetTime());
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  exit(EXIT_SUCCESS);
}