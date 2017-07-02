#### 概述：
本项目其实仅仅是一个支持自动注册的工厂类模板，实际上类似的资料已经汗牛充栋了，不过有鉴于本人在着手实现类似机制的时候，思路上有了点小突破，因此，特意将其上传到Github上。

#### 
在目前大天朝的网络上，已经流传了许多与之相关的博客以及讨论了：

 - [CSDN上的上古时期的讨论](http://bbs.csdn.net/topics/190123682)
 - [C++中实现自动注册的工厂](http://blog.csdn.net/zhx6044/article/details/50569482)
 - [C++如何不使用C++11的新特性来实现一个自动注册的工厂？](https://segmentfault.com/q/1010000004914471)
 - [三步实现自动注册工厂替代switch语句](http://blog.csdn.net/to_be_better/article/details/53968737)
 - [C++11实现一个自动注册的工厂](http://www.cnblogs.com/qicosmos/p/5090159.html)
 - etc

上面的文章和讨论已经将类似机制的实现原理讲解得很通透了，或用模板、宏、和C++11的新特性。不过实际上呢，其实这种机制的实现方式还可以更加优雅滴，在我看来，开发人员利用自动注册工厂类无非是简化了下面这两种操作：

 - 拷贝被管理的子类指针（多在界面库的控件注册机制上用到，比如解析XML文本，然后根据标签名来创建控件指针，将其插入到控件树上）
 - 直接调用被管理的子类指针的接口（多在策略设计模式下用到，比如实现一个加减乘除的算法类）

因此本人把工厂类里面被管理的子类指针的行为抽象为一个转发类(o゜▽゜)o☆，如下所示：
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

以上就是本人觉得有点小得意的地方啦，因为之前在各种界面库源码的阅读过程中，发现它们的控件、皮肤资源等注册机制的实现上，或多或少都存在了一些代码冗余，后来想到了这个法子，简化了代码o(〃'▽'〃)o。不过这种做法显而易见会略微增加点内存占用啦(*￣ω￣)。

由于在工厂类模板的实际应用之中，键值对(**Key --> Value(s)**) 的关系，通常存在一对一、一对多的情况，因此，分别为其实现一个用于实际管理的工厂类，`CSingleRegisteredFactory`和`CMutipleRegisterFactory`(PS：其实可以省略`CSingleRegisteredFactory`的，将其整合到`CMutipleRegisterFactory`也是很容易的，不过为了方便区分两种键值对的关系，才区分实现)。

#### Demo说明
 - [Demo1.cpp](https://github.com/YaoXuanZhi/CAutoRegisterFactory/blob/master/Demo1.cpp)：此Demo演示了如何用`CSingleRegisteredFactory`类模板来快速实现一个类似控件注册机制的工厂类，类似的应用场景还有图片编/解码器、支持多种文件编码的文本解析器等等
 - [Demo2.cpp](https://github.com/YaoXuanZhi/CAutoRegisterFactory/blob/master/Demo2.cpp)：此Demo演示了如何用`CSingleRegisteredFactory`类模板实现一个一对一关系的信号槽类库，具体代码参考了SOUI的事件订阅机制的源码。
 
- [Demo3.cpp](https://github.com/YaoXuanZhi/CAutoRegisterFactory/blob/master/Demo3.cpp)：此Demo演示了如何用`CMutipleRegisterFactory`类模板实现一个一对多关系的信号槽类库，具体实现还参考了`sigslot.h`，其实，原本本人并不打算实现这么一个类库，但是`sigslot.h`类库在实际调用的时候，实现slot的类必须继承自has_slots<>，这样显得不够解耦，或者按照逼乎里面的说法，不够优雅O(∩_∩)O哈哈~， 后来本人在阅读SOUI的事件机制的源码时，惊为天人，毕竟其类信号槽的实现方式比`sigslot.h`优雅多啦，而且解耦彻底，因此借鉴了这个思路撸了一个信号槽类库。

#### 拓展知识
 - 关于`sigslot.h`的源码分析，请查阅[《Sigslot介绍》](http://www.cnblogs.com/kanego/articles/sigslot.html)
 - 本人在编写信号槽类库的时候，用到了递归宏技术，具体借鉴了[《代码自动生成-宏递归思想》](http://www.cppblog.com/kevinlynx/archive/2008/08/20/59451.html)，利用递归宏的技术节省了几百行代码，真是太爽啦(/▽╲)。

最后，在此也顺带回答一下`《Sigslot介绍》`里专门提到了一个使用`sigslot`时的注意事项----`slot的类用来关联信号的成员函数（其实就是槽函数）的返回值必须为void类型`，为什么呢？众所周知，既然可以让槽函数的参数列表都用模板来代替，那么返回值自然也是可以的，技术上的难点是并不存在的，但问题是，如果返回值不指定为`void`，那么同样用`template<typename ReturnType>`来代替返回值类型的时候，那么`sigslot`在调用类似`InvokeFunction()`函数的时候，如何处理其返回值呢，比如默认为int或bool，在`InvokeFunction()`函数体内return false或0？这些做法其实都不好，因此才直接指定返回值类型为`void`，这样就不需要揪心了。当然，其实这个类库还是不够完善的，还需要像`sigslot.h`那样为此类库的线程安全做更多的工作呢，比如增加互斥锁等等。