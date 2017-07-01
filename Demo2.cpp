#include "CAutoRegisterFactory.h"
//一个简单的信号槽机制的实现方式，参考SOUI和Sigslot.h实现

/////////////以下统一了各种“返回值”、“参数列表”完全相同的函数指针的注册行为/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)
/**
* @class ISlotFunctionr
* @brief 定义一个槽函数抽象类
* @note
* 目前仅仅实现一个参数类型的支持
*/
template<typename ReturnType, typename ParamType1st>
class ISlotFunctionr
{
public:
	virtual ~ISlotFunctionr() {}
	virtual ReturnType InvokeFunction(ParamType1st Param1st) = 0;
	virtual ISlotFunctionr* Clone() const = 0;
};

/**
* @class NoMemberFunctionr
* @brief 派生出一个专门管理全局函数或静态(成员)函数的子类
* @note
* 由于其它函数的指针通常是无效的，因此这里面我们
* 直接使用常见的消息函数来处理即可。
*/
template<typename ReturnType, typename ParamType1st>
class NormalFunctionr :public ISlotFunctionr<ReturnType, ParamType1st>
{
	typedef ReturnType(pSlotFunction)(ParamType1st);
public:
	NormalFunctionr(pSlotFunction* pNormalFunc)
		:m_pNormalFunc(pNormalFunc)
	{
		assert((pNormalFunc, "pNormalFunc指针为NULL！！！"));
	}
	virtual ~NormalFunctionr() {}
	ReturnType InvokeFunction(ParamType1st Param1st)
	{
		assert((m_pNormalFunc, "m_pNormalFunc指针为NULL！！！"));
		return m_pNormalFunc(Param1st);
	}
	ISlotFunctionr<ReturnType, ParamType1st>* Clone() const
	{
		return new NormalFunctionr(m_pNormalFunc);
	}

private:
	pSlotFunction *m_pNormalFunc;	/**< 普通函数指针 */
};

/**
* @brief: 用来注册普通函数指针的
* @note
*
*/
template<typename ReturnType, typename ParamType1st>
NormalFunctionr<ReturnType, ParamType1st> Subscriber(ReturnType(*pNormalFunc)(ParamType1st))
{
	return NormalFunctionr<ReturnType, ParamType1st>(pNormalFunc);
}

/**
* @class MemberFunctionr
* @brief 派生出一个专门用来统一处理不同类的成员函数的子类
* @note
* 由于类的非静态成员函数的=特性，因此，必须通过此类的
* 实例的指针来调用此函数，因此需要为它维护两个变量：
* 【1】存储类实例的指针【2】非静态成员函数的指针
* 注意，必须保证这两个成员指针变量是有效的
*/
template<typename TClass, typename ReturnType, typename ParamType1st>
class MemberFunctionr :public ISlotFunctionr<ReturnType, ParamType1st>
{	
	typedef ReturnType(TClass::*pSlotFunction)(ParamType1st);//C++高级特性
public:
	MemberFunctionr(TClass *pClassInst, pSlotFunction pMemberFunc)
		:m_pMemberFunc(pMemberFunc), m_pClassInst(pClassInst)
	{
		assert((pClassInst, "pClassInst指针为NULL！！！"));
		assert((pMemberFunc, "pMemberFunc指针为NULL！！！"));
	}
	virtual ~MemberFunctionr() {}
	ReturnType InvokeFunction(ParamType1st Param1st)
	{
		assert((m_pClassInst, "m_pClassInst指针为NULL！！！"));
		assert((m_pMemberFunc, "m_pMemberFunc指针为NULL！！！"));
		return (m_pClassInst->*m_pMemberFunc)(Param1st);//这个是高级语法
	}

	ISlotFunctionr<ReturnType, ParamType1st>* Clone() const
	{
		return new MemberFunctionr(m_pClassInst, m_pMemberFunc);
	}

private:
	pSlotFunction m_pMemberFunc;	/**< 类的成员函数指针 */
	TClass *m_pClassInst;			/**< 类实例的指针 */
};

/**
* @brief: 用来注册类的成员函数指针的
* @note
*
*/
template<typename TClass, typename ReturnType, typename ParamType1st>
MemberFunctionr<TClass, ReturnType, ParamType1st> Subscriber(TClass *pClassInst, ReturnType(TClass::*pNormalFunc)(ParamType1st))
{
	return MemberFunctionr<TClass, ReturnType, ParamType1st>(pClassInst, pNormalFunc);
}

/////////////以上统一了各种“返回值”、“参数列表”完全相同的函数指针的注册行为/////////////

////////////////////为每个被注册的函数指针建立索引，换言之，生成一个工厂类//////////////////
template<typename TBase>
class TplClass :public IForwarder<TBase>
{
private:
	TBase* m_Obj;

public:
	TplClass(const TBase &Obj)
	{
		m_Obj = Obj.Clone();
	}

	virtual ~TplClass()
	{
		if (NULL != m_Obj)
		{
			delete m_Obj;
			m_Obj = NULL;
		}
	}

public:
	virtual TBase* CloneClass() { return m_Obj->Clone(); }
	virtual TBase* InvokeClass() { return m_Obj; }
};

template<typename TBase>
class TplClassOfTest
{
public:
	TplClassOfTest(const TBase &Obj) :m_pTemp(NULL)
	{
		m_pTemp = new TplClass<TBase>(Obj);
	}

	operator IForwarder<TBase>*()
	{
		return m_pTemp;
	}
private:
	IForwarder<TBase> *m_pTemp;
};

/**
* @class CEventFactory
* @brief 管理真正的信号槽管理类                                                                                         
* @note
* 注意，需要考虑线程安全哦
* 由于这里面并不是管理类的，而是专门用来管理函数指针的，
* 因此这里面还需要继续修改，也就是每个被注册的函数，其实都是一个类实例
* 的指针，但是被注册的多个成员函数可以来源于同一个类
*/
template<typename TKEY, typename ReturnType, typename ParamType1st>
class CEventFactory
{
	//由于VC 6.0无法处理“>>”的符号，因此为槽函数抽象类起个别名
	typedef ISlotFunctionr<ReturnType, ParamType1st> TSlotFunc;

public:
	//统一的注册接口，也可以将它改为Connect();
	void SubscribeEvent(TKEY Key, const TSlotFunc &subscriber)
	{
		m_IObjectFactory.RegisterFactory(Key, TplClassOfTest<TSlotFunc>(subscriber));
	}

	//反注册某事件，也可以将它改为Disconnect();
	void UnsubscribeEvent(TKEY Key) { m_IObjectFactory.UnregisterFactory(Key); }

	ReturnType FireEvent(TKEY Key, ParamType1st Parm1st)
	{
		TSlotFunc *pSlotFunc = m_IObjectFactory.InvokeClass(Key);
		assert(("pSlotFunc指针为NULL!!!，请检查一下是否已经正确地注册了此函数", pSlotFunc));
		return pSlotFunc->InvokeFunction(Parm1st);
	}

	CEventFactory *GetEventSet()
	{
		return this;
	}

private:
	//注意，在事件订阅里面，事件ID通常是UINT类型的，因此在此直接指定即可。
	CSingleRegisteredFactory<TKEY, TSlotFunc> m_IObjectFactory;
};

bool OnEvent_Create(char* szMsg)
{
	printf("在%s函数内，消息：%s\n", __FUNCTION__, szMsg);
	return false;
}

enum
{
	EVT_CREATE = 0,
	EF_PAINT,
	EF_CLICK,
};

class CTest1
{
public:
	CTest1()
	{
		m_EvtUI.GetEventSet()->SubscribeEvent(EVT_CREATE, Subscriber(this, &CTest1::OnEvent_Paint));
		m_EvtUI.GetEventSet()->SubscribeEvent(EF_PAINT, Subscriber(OnEvent_Create));
	}

	void OnCreate()
	{
		m_EvtUI.FireEvent(EVT_CREATE, "创建窗口");
	}

	void OnPaint()
	{
		m_EvtUI.FireEvent(EF_PAINT, "正在重绘");
	}

private:
	CEventFactory<UINT,bool, char*> m_EvtUI;

public:
	bool OnEvent_Paint(char* szMsg)
	{
		printf("在%s函数内，消息：%s\n", __FUNCTION__,szMsg);
		return false;
	}
};

class CTest2
{
private:
	CEventFactory<UINT, bool, int> m_EvtAction;
	CEventFactory<UINT, bool, char*> m_EvtUI;

protected:
	static bool OnEvent_Click(int nTimes)
	{
		printf("在%s函数内，连击了%d次\n", __FUNCTION__, nTimes);
		return false;
	}

public:
	CTest2()
	{
		m_EvtAction.GetEventSet()->SubscribeEvent(EF_CLICK, Subscriber(OnEvent_Click));
	}

	void PrintText()
	{
		m_EvtAction.GetEventSet()->FireEvent(EF_CLICK, 5);
		m_EvtUI.GetEventSet()->FireEvent(EVT_CREATE, "嘿，现在在CTest2之中调用哦");
	}

	CEventFactory<UINT, bool, char*>& GetEventSetUI()
	{
		return m_EvtUI;
	}
	
};

#define _CRTDBG_MAP_ALLOC 
#include <crtdbg.h>
#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

/** 
* @brief: 开始检测内存泄漏
* @return 无
* @note   
* 通常在主函数的首行里调用此函数，注意，仅在Debug下有效
*/
void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

void main()
{
	EnableMemLeakCheck();
	CTest1 obj;
	obj.OnCreate();
	obj.OnPaint();

	CTest2 obj2;
	obj2.GetEventSetUI().SubscribeEvent(EVT_CREATE, Subscriber(&obj, &CTest1::OnEvent_Paint));
	obj2.PrintText();
	getchar();
}

/*
脚注：类似的应用场合：信号槽机制的实现，此Demo是参考了SOUI里面的事件订阅机制源码(主要在EventSubscriber.h等文件上)以及
还有sigslot.h类库源码重新实现。其实sigslot.h类库已经足够强大了，不过有鉴于和http://www.cnblogs.com/kanego/articles/sigslot.html
作者差不多的想法，对必须继承自has_slots<>觉得尤其不爽，因此，重写了类似的类库，作为同时也作为工厂类模板的演示Demo，另外，由于
个人的某些癖好，希望尽可能兼容VC 6.0，因此木有用到C++新版本的特性。不过由于这个例子实质上是一个观察者模式下实现的，所以参考了
SOUI的命名了，另外，这部分源码并不能直接添加到工程里面哦，因为，线程安全这部分没有做，还有各种函数指针的转换方式的类型检测也是没有做的，
因此，还需要加上互斥锁，类型检测之类的功能喽。
*/