# CAutoRegisterFactory
一个支持自动注册的工厂类模板，参考**SOUI**（控件注册机制、事件订阅机制）和**Sigslot.h**实现

目前在大天朝的网络上，已经许多类似的博客和讨论了，详情请看：

 - [CSDN上的上古时期的讨论](http://bbs.csdn.net/topics/190123682)
 - [C++中实现自动注册的工厂](http://blog.csdn.net/zhx6044/article/details/50569482)
 - [C++如何不使用C++11的新特性来实现一个自动注册的工厂？](https://segmentfault.com/q/1010000004914471)
 - [三步实现自动注册工厂替代switch语句](http://blog.csdn.net/to_be_better/article/details/53968737)
 - [C++11实现一个自动注册的工厂](http://www.cnblogs.com/qicosmos/p/5090159.html)
 - etc

在这上面的博客、讨论之中，已经将类似机制的实现原理说明得很通透了。不过实际上呢，其实还可以进一步抽象的，在我看来，开发人员利用自动注册工厂类无非是简化了下面这两种操作：

 -1. 拷贝被管理的子类指针（多在界面库的控件注册机制上用到，比如解析XML文本，然后根据标签名来创建控件指针，将其插入到控件树上）
 -2. 直接调用被管理的子类指针的接口（多在策略设计模式下用到，比如定义一组数据的加减乘除的算法类）

因此本人抽象工厂类里面被管理的子类指针行为，设计了一个转发抽象类，如下所示：
```c++
/**
* @class IForwarder
* @brief 转发抽象类
* @note
* 把工厂模式下的两种常用的调用方式进行抽象，结果如下
* ：生成副本[CloneClass()]/直接调用[InvokeClass()]
*/
template<typename TBase>
class IForwarder
{
public:
	virtual ~IForwarder() {}

public:
	virtual TBase *CloneClass() = 0;
	virtual TBase* InvokeClass() = 0;
};
```

由于在工厂类的实际应用之中，**Key --> Value(s)** 的关系，通常存在一对一、一对多的情况，因此，分别为其实现一个用于实际管理的工厂类，`CSingleRegisteredFactory`和`CMutipleRegisterFactory`(ps：其实可以省略`CSingleRegisteredFactory`的，将其整合到`CMutipleRegisterFactory`也是很容易的，不过为了方便区分两种关系，因此分开实现这两种键值对关系的工厂类)。
