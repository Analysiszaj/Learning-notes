
> [!一些学习资料]
> 学习视频(OPENGL API):https://www.bilibili.com/video/BV1Ni4y1o7Au?spm_id_from=333.788.recommend_more_video.1&vd_source=a497d84a6236fd9a3c0d65fd88ab7d1c
> 参考书籍：计算机图形学编程(使用OpenGL和C++)(第二版)
> 学习网站：https://learnopengl-cn.github.io/
> API网站：https://docs.gl/

## 1.开始与数学基础
### 1.1 矩阵运算

## 2. 入门
### 2.1 开发环境搭建
- 语言：C/C++, OpenGL/GLSL
- 窗口管理库：GLFW
- 扩展库：GLAD,GLLoader, GLEW, GL3W
- 数学库：GLM
- 纹理图像加载库：SOLL2
- 其他：SOLL2(OBJ加载)

#### 2.1.1 window环境搭建

#### 2.1.2 macOS 环境搭建

### 2.2 OpenGL图形管线
OpenGL的渲染管线分为以下几个步骤：
- 顶点着色器
- 曲面细分着色器
- 几何着色器
- 光栅化
- 片段着色器
- 像素操作
![[Pasted image 20250601145219.png|500]]
以上操作都是通过GLSL编写， 将GLSL程序载入这些着色器片段也是C++/OpenGL应用程序的职责之一其大致流程如下：
- 获取着色器代码
- 创建openGL着色器对象
- 加载GLSL着色器代码
- 编译并链接着色器对象

### 2.3 第一个GL程序
```c++
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;
void init(GLFWwindow *window) {}

void display(GLFWwindow *window, double currentTime)
{
  // 用于清除的颜色
  glClearColor(1.0, 0.0, 0.0, 1.0);
  // 清除颜色缓冲区
  glClear(GL_COLOR_BUFFER_BIT);
}

int main()
{
  if (!glfwInit())
  {
    exit(EXIT_FAILURE);
  }
  // 设置主版本号（*不要填写成glfwInitHit()）
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  // 设置次版本号
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  GLFWwindow *window = glfwCreateWindow(600, 600, "Chapter2- program1", NULL, NULL);
  glfwMakeContextCurrent(window);

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
```
 整体流程为：
![[Pasted image 20250602121323.png]]
这时就能创建一个简单内容为红色的窗口。

### 2.4 顶点着色器和片段着色器
在上一个示例中，我们并没绘制任何东西，如果需要绘制点什么，这时候我们就得加入**顶点着色器**和**片段着色器**

openGL只能绘制几类非常简单的东西，点，线，三角形，这些简单的东西叫做**图元**，多数3d模型都是由三角形图元构成，图元由顶点组成

创建一个简单的着色器程序(在平面中间创建一个蓝色正方形)
```c++
GLuint createShaderProgram()
{
  // 顶点着色器
  const char *vshaderSource =
      "#version 330 \n"
      "void main(void) \n"
      "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";
  // 片段着色器
  const char *fshaderSource =
      "#version 330 \n"
      "out vec4 color; \n"
      "void main(void) \n"
      "{ color = vec4(0.0, 0.0, 1.0, 1.0);}";
  // 创建着色器对象
  GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

  // 关联着色代码
  glShaderSource(vShader, 1, &vshaderSource, NULL);
  glShaderSource(fShader, 1, &fshaderSource, NULL);

  // 编译着色器
  glCompileShader(vShader);
  glCompileShader(fShader);

  // 创建GL程序并链接着色器
  GLuint vfProgram = glCreateProgram();
  glAttachShader(vfProgram, vShader);
  glAttachShader(vfProgram, fShader);
  glLinkProgram(vfProgram);

  return vfProgram;
}

void init(GLFWwindow *window)
{
  renderingProgram = createShaderProgram();
  
  // 创建一个顶点
  glGenVertexArrays(numVAOs, vao);
  glBindVertexArray(vao[0]);
}

void display(GLFWwindow *window, double currentTime)
{
  // 渲染
  glUseProgram(renderingProgram);
  glPointSize(100.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDrawArrays(GL_POINTS, 0, 1);
}```
- **在创建的时候我们需要重点检查当前电脑的OpenGL版本是否支持，根据系统版本填写对应的`#version 330` 和设置`glfwWindowHint()` **
	- 当`glfwWindowHint()` 填写错误就会出现程序闪退， 
	- 着色器内代码填写错误就会出现图形不出现的问题
- 如果出现在创建`glGenVertexArrays(numVAOs, vao);` 出现空指针异常可能是GFEW没有初始化成功需添加`glewExperimental = GL_TRUE;`
- 在mac端如果使用glad的话需要使用额外的代码[参考](https://github.com/Dav1dde/glad/issues/282),而window端不需要(测试得到)
- OpenGL 从 **3.2 版本**开始引入了 Core Profile（核心模式）和 Compatibility Profile（兼容模式）的区分。  在 Core Profile 下，**所有固定管线（如 glBegin/glEnd、glVertex 等）和默认顶点属性都被废弃，必须使用自定义着色器和 VAO/VBO**。

### 2.4 检查GLSL错误
GLSL的编译发生在运行时，且GLSL代码并没有运行在CPU上，这两点使得调试变得很困难，我们可以实现一些Debugger函数用于捕获GLSL错误。
```c++

```
