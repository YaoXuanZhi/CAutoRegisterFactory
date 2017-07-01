#include "CAutoRegisterFactory.h"
//一个简单的信号槽机制的实现方式，参考SOUI和Sigslot.h实现

/////////////以下统一了各种“返回值”、“参数列表”完全相同的函数指针的注册行为/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)

//参考了《代码自动生成-宏递归思想》来实现此递归宏
//链接：http://www.cppblog.com/kevinlynx/archive/2008/08/20/59451.html
///////////////////////////////////////递归宏的实现///////////////////////////////////////
#define DECVALUE_1 0
#define DECVALUE_2 1
#define DECVALUE_3 2
#define DECVALUE_4 3
#define DECVALUE_5 4
#define DECVALUE_6 4
#define DECVALUE_7 4
#define DECVALUE_8 4
#define DECVALUE_9 4

#ifndef DECVALUE
#define DECVALUE(n) DECVALUE_##n
#else
#error "DECVALUE已被定义，请对其重命名"
#endif

//目的是为了延迟它的展开时间
#ifndef MERGECHAR
#define MERGECHAR(a,b) _MERGECHAR(a,b)
#define _MERGECHAR(a,b) a##b
#else
#error "MERGECHAR已被定义，请对其重命名"
#endif

#define REPEAT_1( n, f, e ) e(n)
#define REPEAT_2( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) )
#define REPEAT_3( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_4( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_5( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_6( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_7( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_8( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 
#define REPEAT_9( n, f, e ) MERGECHAR( MERGECHAR( REPEAT_, DECVALUE( n ) )( DECVALUE( n ), f, e ), f( n ) ) 

#define PARAM_ARG(n) typename T##n
#define PARAM_ARRARY(n) ,PARAM_ARG(n)
#define PARAM_DEF( n ) REPEAT_##n( n, PARAM_ARRARY, PARAM_ARG )

#define PARAM_ARGEX(n) T##n t##n
#define PARAM_ARRARYEX(n) ,PARAM_ARGEX(n)
#define PARAM_DEFEX( n ) REPEAT_##n( n, PARAM_ARRARYEX, PARAM_ARGEX )

#define REAL_ARG(n) t##n
#define REAL_ARRARY(n) ,REAL_ARG(n)
#define REAL_DEF( n ) REPEAT_##n( n, REAL_ARRARY, REAL_ARG )

#define REAL_ARGEX(n) T##n
#define REAL_ARRARYEX(n) ,REAL_ARGEX(n)
#define REAL_DEFEX( n ) REPEAT_##n( n, REAL_ARRARYEX, REAL_ARGEX )
///////////////////////////////////////递归宏的实现///////////////////////////////////////

//////////////////////////////////////公共代码//////////////////////////////////////
template<typename TBase>
class TplForwarderForEvent :public IForwarder<TBase>
{
private:
	TBase* m_Obj;

public:
	TplForwarderForEvent(const TBase &Obj)
	{
		m_Obj = Obj.Clone();
	}

	virtual ~TplForwarderForEvent()
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

/** @brief 信号槽类型 */
enum {
	Slot_Unknown,   /**< 未知函数类型 */
	Slot_Normal,	/**< 普通函数类型 */
	Slot_Member,	/**< 成员函数类型 */
};

//////////////////////////////////////公共代码//////////////////////////////////////

/////////////////////////////////////实现建立一个独立的事件订阅类库/////////////////////////////////////
#define DEF_SLOTFUNCTOR(classname,n)																	\
																										\
template<PARAM_DEF(n)>																					\
class ISlotFunctionr##n																					\
{																										\
public:																									\
	virtual ~ISlotFunctionr##n() {}																		\
	virtual void InvokeFunction(REAL_DEFEX(n)) = 0;														\
	virtual ISlotFunctionr##n* Clone() const = 0;														\
	virtual bool Equal(const ISlotFunctionr##n & sour)const = 0;										\
	virtual int GetSlotType() const { return Slot_Unknown; }											\
};																										\
																										\
template<PARAM_DEF(n)>																					\
class NormalFunctionr##n :public ISlotFunctionr##n<REAL_DEFEX(n)>										\
{																										\
	typedef ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													\
	typedef void(pSlotFunction)(REAL_DEFEX(n));															\
																										\
public:																									\
	NormalFunctionr##n(pSlotFunction* pNormalFunc)														\
		:m_pNormalFunc(pNormalFunc)																		\
	{																									\
		assert((pNormalFunc, "pNormalFunc指针为NULL！！！"));											\
	}																									\
																										\
	virtual ~NormalFunctionr##n() {}																	\
																										\
	void InvokeFunction(PARAM_DEFEX(n))																	\
	{																									\
		assert((m_pNormalFunc, "m_pNormalFunc指针为NULL！！！"));										\
		m_pNormalFunc(REAL_DEF(n));																		\
	}																									\
																										\
	TSlotFunc* Clone() const																			\
	{																									\
		return new NormalFunctionr##n(m_pNormalFunc);													\
	}																									\
																										\
	int GetSlotType() const { return Slot_Normal; }														\
																										\
	virtual bool Equal(const TSlotFunc & sour) const													\
	{																									\
		if (sour.GetSlotType() != Slot_Normal) return false;											\
		const NormalFunctionr##n *psour = static_cast<const NormalFunctionr##n*>(&sour);				\
		assert(psour);																					\
		return psour->m_pNormalFunc == m_pNormalFunc;													\
	}																									\
																										\
private:																								\
	pSlotFunction *m_pNormalFunc;	/**< 普通函数指针 */												\
};																										\
																										\
template<PARAM_DEF(n)>																					\
NormalFunctionr##n<REAL_DEFEX(n)> Subscriber(void(*pNormalFunc)(REAL_DEFEX(n)))							\
{																										\
	return NormalFunctionr##n<REAL_DEFEX(n)>(pNormalFunc);												\
}																										\
																										\
template<typename TClass, PARAM_DEF(n)>																	\
class MemberFunctionr##n :public ISlotFunctionr##n<REAL_DEFEX(n)>										\
{																										\
	typedef ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													\
	typedef void(TClass::*pSlotFunction)(REAL_DEFEX(n));												\
																										\
public:																									\
	MemberFunctionr##n(TClass *pClassInst, pSlotFunction pMemberFunc)									\
		:m_pClassInst(pClassInst),m_pMemberFunc(pMemberFunc)											\
	{																									\
		assert((pClassInst, "pClassInst指针为NULL！！！"));											    \
		assert((pMemberFunc, "pMemberFunc指针为NULL！！！"));											\
	}																									\
																										\
	virtual ~MemberFunctionr##n() {}																	\
																										\
	void InvokeFunction(PARAM_DEFEX(n))																	\
	{																									\
		assert((m_pClassInst, "m_pClassInst指针为NULL！！！"));										    \
		assert((m_pMemberFunc, "m_pMemberFunc指针为NULL！！！"));										\
		(m_pClassInst->*m_pMemberFunc)(REAL_DEF(n));													\
	}																									\
																										\
	TSlotFunc* Clone() const																			\
	{																									\
		return new MemberFunctionr##n(m_pClassInst, m_pMemberFunc);										\
	}																									\
																										\
	int GetSlotType() const { return Slot_Member; }														\
																										\
	virtual bool Equal(const TSlotFunc & sour) const													\
	{																									\
		if (sour.GetSlotType() != Slot_Member) return false;											\
		const MemberFunctionr##n *psour = static_cast<const MemberFunctionr##n*>(&sour);				\
		assert(psour);																					\
		return psour->m_pMemberFunc == m_pMemberFunc;													\
	}																									\
																										\
private:																								\
	pSlotFunction m_pMemberFunc;	/**< 类的成员函数指针 */											\
	TClass *m_pClassInst;			/**< 类实例的指针 */												\
};																										\
																										\
template<typename TClass, PARAM_DEF(n)>																	\
MemberFunctionr##n<TClass, REAL_DEFEX(n)> Subscriber(TClass *pClassInst,								\
													void(TClass::*pNormalFunc)(REAL_DEFEX(n)))			\
{																										\
	return MemberFunctionr##n<TClass, REAL_DEFEX(n)>(pClassInst, pNormalFunc);							\
}																										\
																										\
template<typename TKEY, PARAM_DEF(n)>																	\
class classname##n																					    \
{																										\
	/*由于VC 6.0无法处理“>>”的符号，因此为槽函数抽象类起个别名*/									    \
	typedef ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													\
																										\
public:																									\
	/*统一的注册接口，也可以将它改为Connect();*/														\
	void SubscribeEvent(TKEY Key, const TSlotFunc &subscriber)											\
	{																									\
		m_ContainterFactory.RegisterFactory(Key, 														\
		new TplForwarderForEvent<TSlotFunc>(subscriber));												\
	}																									\
																										\
	/*反注册指定事件，也可以将它改为Disconnect();*/													    \
	void UnsubscribeEvent(TKEY Key,const TSlotFunc &subscriber)											\
	{																									\
		m_ContainterFactory.UnregisterContainter(Key,false,												\
		new TplForwarderForEvent<TSlotFunc>(subscriber));												\
	}																									\
																										\
	void FireEvent(TKEY Key, PARAM_DEFEX(n))															\
	{																									\
		typedef std::list<IForwarder<TSlotFunc>*> ForwardContainter;									\
		std::list<IForwarder<TSlotFunc>*> *pContainter = m_ContainterFactory.InvokeContanter(Key);		\
		if(NULL!=pContainter)																			\
		{																								\
			typename ForwardContainter::iterator it = pContainter->begin();								\
			for(;it!=pContainter->end();it++)															\
			{																							\
				IForwarder<TSlotFunc>* pItem = *it;														\
				pItem->InvokeClass()->InvokeFunction(REAL_DEF(n));										\
			}																							\
		}																								\
	}																									\
																										\
private:																								\
	CMutipleRegisteredFactory<TKEY, TSlotFunc> m_ContainterFactory;										\
};			

DEF_SLOTFUNCTOR(CEventFactory,1)
DEF_SLOTFUNCTOR(CEventFactory,2)
DEF_SLOTFUNCTOR(CEventFactory,3)
DEF_SLOTFUNCTOR(CEventFactory,5)
DEF_SLOTFUNCTOR(CEventFactory,6)
DEF_SLOTFUNCTOR(CEventFactory,7)
DEF_SLOTFUNCTOR(CEventFactory,8)

/*
事件订阅类库的使用套路：
1.先执行AddEventID()来注册事件通知码
2.然后在指定的时候，执行SubscribeEvent()来订阅通知码的响应事件
3.接着在需要发送通知码的代码处执行FireEvent()
4.根据实际需要，通过UnsubscribeEvent()来反 注册

改成QT信号槽的马甲时，按照下面方式来替换概念：
SubscribeEvent --> Connect
UnsubscribeEvent --> Disconnect
EventID --> SignalID
*/

void OnEvent_Create(char* szMsg)
{
	printf("在%s函数内，消息：%s\n", __FUNCTION__, szMsg);
}

enum
{
	EVT_CREATE = 0,
	EF_PAINT,
	EF_CLICK,
	EVT_TEST,
};

class CTest1
{
public:
	CTest1()
	{
		m_EvtUI.SubscribeEvent(EVT_CREATE, Subscriber(this, &CTest1::OnEvent_Paint));
		m_EvtUI.SubscribeEvent(EF_PAINT, Subscriber(OnEvent_Create));

#if (_MSC_VER == 1200)
		m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(&CTest1::OnEventTest));//VC 6.0仅仅支持这种类的静态成员函数指针的写法
#else
		m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//首次注册
		//m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//重复注册
		//m_EvtTest.UnsubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//反注册
#endif
	}

	void OnCreate()
	{
		m_EvtUI.FireEvent(EVT_CREATE, "创建窗口");
	}

	void OnPaint()
	{
		m_EvtUI.FireEvent(EF_PAINT, "正在重绘");
	}

	void OnTest()
	{
		m_EvtTest.FireEvent(EVT_TEST,"执行OnTest哦",12345,"执行完成哦");
	}

private:
	CEventFactory1<UINT,char*> m_EvtUI;
	CEventFactory3<UINT,char*,int ,char*> m_EvtTest;

public:
	void OnEvent_Paint(char* szMsg)
	{
		printf("在%s函数内，消息：%s\n", __FUNCTION__,szMsg);
	}


	static void OnEventTest(char *szMsg,int nID,char *szErr)
	{
		printf("%d,%s  %s\n",nID,szMsg,szErr);
	}
};

class CTest2
{
private:
	CEventFactory1<UINT,int> m_EvtAction;
	CEventFactory1<UINT,char*> m_EvtUI;

protected:
	static void OnEvent_Click(int nTimes)
	{
		printf("在%s函数内，连击了%d次\n", __FUNCTION__, nTimes);
	}

public:
	CTest2()
	{
		m_EvtAction.SubscribeEvent(EF_CLICK, Subscriber(OnEvent_Click));
	}

	void PrintText()
	{
		m_EvtAction.FireEvent(EF_CLICK, 5);
		m_EvtUI.FireEvent(EVT_CREATE, "嘿，现在在CTest2之中调用哦");
	}

	CEventFactory1<UINT, char*>& GetEventSetUI()
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
	obj.OnTest();

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