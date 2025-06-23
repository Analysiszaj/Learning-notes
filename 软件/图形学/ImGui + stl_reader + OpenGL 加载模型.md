## 着色器程序
```c++
// 顶点着色器
const char* vs_src = R"(
#version 330 core
layout(location = 0) in vec3 aPos;                       
layout(location = 1) in vec3 aNormal;
out vec3 vNormal;
uniform mat4 mvp;
uniform mat4 model;
void main() {
vNormal = mat3(transpose(inverse(model))) * aNormal;
gl_Position = mvp * vec4(aPos, 1.0);
}
)";
```
- `layout(location = 0) in vec3 aPos;`
	- 输入变量（来自CPU传入的顶点属性）
- `out vec3 vNormal;`
	- 声明一个输出变量，会传递给片段着色器（Fragment Shader）
- `uniform mat4 mvp;`
	- 声明一个全局一致变量
- `vNormal = mat3(transpose(inverse(model))) * aNormal;`
	- 这是**将法线从模型空间变换到世界空间**的标准做法。
	- 其中 `transpose(inverse(model))` 计算模型矩阵的逆转置矩阵，`mat3(...)` 取前3x3用于法线变换。
- `gl_Position = mvp * vec4(aPos, 1.0);`
	- 计算顶点在裁剪空间的位置，赋值给内建变量 `gl_Position`。
	- vec4讲一个3维顶点坐标转换成四维齐次坐标
```c++
// 片段着色器
const char* fs_src = R"(
#version 330 core
in vec3 vNormal;
out vec4 FragColor;

void main() {
vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));
float light = dot(normalize(vNormal), lightDir);
light = clamp(light * 0.5 + 0.5, 0.0, 1.0);
FragColor = vec4(vec3(0.4, 0.6, 0.9) * light, 1.0);
}
)";
```
- `in vec3 vNormal;`
	- 顶点着色器传入进来的插值法线
- `out vec4 FragColor;`
	- 输出这个像素的最终颜色
- `vec3 lightDir = normalize(vec3(0.3, 0.5, 1.0));`
	- 构造一个三维向量，表示光照的方向向量
- `float light = dot(normalize(vNormal), lightDir);`
	- 计算法线与光照的点积(计算得到光照强度)
- `light = clamp(light * 0.5 + 0.5, 0.0, 1.0);`
	- 将[-1, 1] 映射到[0, 1] 区间
- `FragColor = vec4(vec3(0.4, 0.6, 0.9) * light, 1.0);`
	- 每个面应该输出的颜色
```c++
// 将着色器代码编译成着色器对象
GLuint compileShader(const char* src, GLenum type)
{
	// 创建一个着色器
	GLuint s = glCreateShader(type);
	// 给着色器对象上传源码(着色器对象句柄，源码字符串数量， 源码字符串地址， 源码长度nullptr 表示自动判断)
	glShaderSource(s, 1, &src, nullptr);
	// 编译着色器对象
	glCompileShader(s);
	return s;
}

// 创建着色器程序
GLuint createShaderProgram()
{
	// 创建顶点着色器
	GLuint vs = compileShader(vs_src, GL_VERTEX_SHADER);
	// 创建片段着色器
	GLuint fs = compileShader(fs_src, GL_FRAGMENT_SHADER);
	// 创建着色器程序
	GLuint prog = glCreateProgram();
	// 添加顶点着色器
	glAttachShader(prog, vs);
	// 添加片段着色器
	glAttachShader(prog, fs);
	// 连接着色器程序
	glLinkProgram(prog);
	// 着色器对象链接之后可以删除了（程序了已经有拷贝）
	glDeleteShader(vs);
	glDeleteShader(fs);
	// 返回这个程序对象
	return prog;
}
```

## 加载模型文件
```c++
bool ModelImage::LoadFromSTL(const char* filePath, int texWidth, int texHeight)
{
	// 检查OpenGL上下文是否有效
	const GLubyte* version = glGetString(GL_VERSION);
	if (!version)
	{
		ImVdas::log(ImVdas::LogLevel::Debug, "OpenGL 上下文无效！");
		return false;
	}

	// 清理之前的资源
	// 纹理对象 ID
	if (ID) glDeleteTextures(1, &ID);
	ID = 0;
	// 帧缓冲对象(OpenGL 里的“离屏画布”，可以把内容渲染到纹理或缓冲，而不是直接到屏幕)
	if (FBO) glDeleteFramebuffers(1, &FBO);
	FBO = 0;
	// Vertex Array Object（顶点数组对象）
	if (VAO) glDeleteVertexArrays(1, &VAO);
	VAO = 0
	// Vertex Buffer Object（顶点缓冲对象）
	if (VBO) glDeleteBuffers(1, &VBO);
	VBO = 0;
	//  Normal Buffer Object（法线缓冲对象）
	if (NBO) glDeleteBuffers(1, &NBO);
	NBO = 0;
	// Element Buffer Object（索引缓冲对象）
	if (EBO) glDeleteBuffers(1, &EBO);
	EBO = 0;
	// Shader Program（着色器程序）
	if (shader) glDeleteProgram(shader);
	shader = 0;
	//  三角形索引的数量
	indexCount = 0;

	Width = texWidth;
	Height = texHeight;

  

	// 1. 读取STL
	// 得到顶点坐标`coords`、顶点法线`normals`、三角面索引`tris`
	std::vector<float> coords, normals;
	std::vector<unsigned int> tris;
	std::vector<size_t> solidRanges;
	if (!stl_reader::ReadStlFile(filePath, coords, normals, tris, solidRanges)) 
		return false;
		
	indexCount = static_cast<GLuint>(tris.size());

  

	// 2. 上传到OpenGL
	// 生成一个VAO（顶点数组对象），并把它的ID保存在`VAO`变量里。
	glGenVertexArrays(1, &VAO);
	// 顶点对象
	glGenBuffers(1, &VBO);
	// 生成一个NBO（法线缓冲对象），用来存放所有顶点的法线数据
	glGenBuffers(1, &NBO);
	// 生成一个EBO（索引缓冲对象），用来存放三角面顶点索引
	glGenBuffers(1, &EBO);
	// 绑定刚刚创建的VAO，让后续所有的顶点属性配置（VBO、NBO、EBO的绑定和属性指针设置）都记录到这个VAO里
	glBindVertexArray(VAO);

  

	// 顶点
	// 把你刚才生成的VBO（顶点缓冲对象）绑定为当前“顶点缓冲”。
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 把你的顶点坐标数组（一般是 std::vector<float> coords）里的数据，上传到上面绑定的VBO里
	glBufferData(GL_ARRAY_BUFFER, coords.size() * sizeof(float), coords.data(), GL_STATIC_DRAW);
	// 告诉着色器 aPos 应该怎么去读
	// 每次读取3个float（x, y, z）
	// 数据类型是float
	// 不需要归一化
	// 每个顶点占用几个字节（步长stride，一般正好一组三个float）
	// 数据的起始偏移（从头开始）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// 法线（下面的同上）
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

  

	// 索引
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tris.size() * sizeof(unsigned int), tris.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);

  

	// 3. 创建FBO+纹理
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0);

  

	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, Width, Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteTextures(1, &ID);
		glDeleteFramebuffers(1, &FBO);
		return false;
	}

	// 4. 编译着色器
	shader = createShaderProgram();

  

	// 5. 渲染到FBO纹理
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, Width, Height);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  

	// 计算包围盒，自动居中缩放
	glm::vec3 minP(FLT_MAX), maxP(-FLT_MAX);
	for (size_t i = 0; i + 2 < coords.size(); i += 3)
	{
		glm::vec3 p(coords[i], coords[i + 1], coords[i + 2]);
		minP = glm::min(minP, p);
		maxP = glm::max(maxP, p);
	}
	glm::vec3 center = (minP + maxP) * 0.5f;
	glm::vec3 size = maxP - minP;
	float scale = 2.0f / std::max(std::max(size.x, size.y), size.z);

	center_ = center;
	scale_ = scale;

	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(scale));
	model = glm::translate(model, -center);

  

	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), Width / float(Height), 0.1f, 100.0f);
	glm::mat4 mvp = proj * view * model;

  

	glUseProgram(shader);
	GLint mvpLoc = glGetUniformLocation(shader, "mvp");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	GLint modelLoc = glGetUniformLocation(shader, "model")
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	// glDrawArrays(GL_TRIANGLES, 0, coords.size()/3);
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 删除RBO
	glDeleteRenderbuffers(1, &rbo);

	return true;

}
```