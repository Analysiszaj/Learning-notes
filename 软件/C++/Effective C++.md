## 1. 视C++为一个语言联邦
c++是一门**多范式编程语言**， 同时支持过程式， 面向对象，函数式， 泛型和元编程特性，这种能力和灵活使用C++成为无可比拟的工具，但也会引起一些混乱，即==所有的正确使用规则似乎都有例外.==

c++的主要子语言如图所示：
![[Pasted image 20250621183212.png|475]]
- c语言部分：C++是基于c的。块， 语句，预处理器，内置数据结构，数组，指针等，多数情况下，==c++提供等解决问题的方法优于c中对应的方法==
- 面向对象：就是关于类的部分(构造函数， 析构函数)， 封装，继承，多态， 虚函数
- 模版部分：C++的泛型编程部分，是一种全新的编程范式，模版元编程
- STL部分：STL是一个模版库，但他是一个非常特殊的模版库， 它通过一些约定，很好的将容器，迭代器，算法，和函数对象融合在一起，使用STL时，需要确保遵循它的约定。

## 2. 尽量以const,enum,inline 替换 #define
```c++
// 不推荐的做法
#define ASPECT_RATIO 1.653
// 替代方案
const double AspectRatio = 1.653;
// c++11
constexpr double AspectRatio = 1.653;
```
- 优先选择编译器而不是预处理器
-  符号名称永远不会被编译器看到 (==如果发生错误，编译器给出的错误信息可能是1.653， 而不是ASPECT_RATIO==)

**常量指针的场景**
```c++
// 错误的写法(还是可以修改)
const char* authorName = "Test";
// 正确的写法
const char* const authorName = "Test";
// 推荐的写法
const std::string authorName = "Test";
```

**作用于特定类的常量**
```c++
class GamePlayer {
	private:
		// 也可以使用枚举
		enum {NumTurns = 5 };

		// #define 宏将无法作用域现在类的内部声明
		static const int NumTurns = 5;   
		
		int source[NumTurns];
	public:
		int numTurns() {
			return NumTurns;
		}
}
// 类常量的初始值是在声明是指定的，所以在定义时不允许赋初始值
const int GamePlayer::NumTurns;
```
- 通常，c++要求为所使用的任何东西提供定义，但累专用的静态整数类型(如integer, char, bool)常量是个例外，只要不获取它们的地址，就可以在不提供定义的情况下声明并使用它们。
- 静态成员与类实例无关
- 创建或删除类实例不会触发静态成员的初始化
- 静态成员在程序开始执行前的静态初始化阶段初始化
- ==这里可以理解为用到了两个联邦：c的静态变量，面对对象， 所以我们在使用的时候如果需要获取NumTurns就需要再定义 ==

`#define` 的另外一个常见的做法(错误)用法是使用它来实现看起来像函数的宏， 但不会引起函数调用的开销
```c++
int f(int num) {return num; }
#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b))
```
这样使用宏必须将宏主体中的所有参数都用括号括起来，否则，当有人用表达式调用宏时，可能会发生很奇怪的事情， 下面的例子哪怕是阔起来了还是回产生问题：
```c++
int a = 5, b = 0;
 // 递增了两次， 本质上是替换所以就成了 ++a > b ? ++a : b;
std::cout << CLL_WITH_MAX(++a, b) << std::endl;   
// 递增一次
std::cout << CLL_WITH_MAX(++a, b + 10) << std::endl;   
```

**替代方案**: 使用内联函数模版，可获得宏的所有效率，以及普通函数的所有可预测行为和类型安全
```c++
template<typename T>
inline T callWithMax(const T& a, const T&b) {
	return f(a > b ? a : b);
}
```

==虽然有了const, 枚举， 内联函数， 对预处理(特别是#define)的需求减少了， 但并没有完全消除， `#include` 任是必不可少的，`#ifdef/#ifndef`在控制编译方面继续发挥重要作用。==
- 对于简单常量，首选const对象或枚举
- 对于类似函数的宏，优先选择内联函数


## 3. 尽可能使const
**const 关键字用于指针**
```c++
// const 出现在*左侧： 指向的是常量
// const 出现在*右侧： 指针本身是常量
char greeting[] = "Hello";
char* p = greeting;

const char* p = greeting;  // 不可以修改字符串，但是可以修改指针指向的空间
char* const p = greeting;  // 不可以修改指针，但是可以修改字符串
 
const char* const p = greeting; // 都不可以修改

// 在函数中 等效
void f1(const Widget* pw);
void f2(Widget* const pw);

// STL迭代器以指针为模型，因此迭代器的行为很像T* 指针
std::vecotr<int> vec;
const std::vector<int>::iterator iter = vec.begin();
*iter = 10;    // 可以修改
++iter;        // 不可修改

std::vector<int>::const_terator cIter = vec.begin();
*cIter = 10;   // 不可以修改
++cIter;   // 可以修改
```
**在函数声明中的应用**
```c++
class Rational {};  
// 返回值也得是const(语意上返回值不可修改)
const Rational operator*(const Rational& lhs, const Rational& rhs);

// 内置类型，以下代码是非法的, 优秀的自定义类型需要与内置类型保持一致
Rational a, b, c;
(a*b) = c;
if(a * b = c)
```
**在成员函数上使用const**
- 使得类接口意图更明确，知道那些函数可以修改一个对象，那些不能
- 使得使用const对象成为可能
```c++
class TextBlock {
private:
	std::string text;

 public:
	 TextBlock(std::string str){
		 text = str;
	 }

	// 标准写法
	const char& operator[](std::size_t position) const {
		return text[position];
	}

	char& operator[](std::size_t position){
		return text[postion];
	}
}

// 函数重载
TextBlock tb("Hello");          // 使用的是普通可修改版本
const TextBlock ctb("World");   // const 对象使用的是const 版本， 确保不可以修改
```