#include "CAutoRegisterFactory.h"
//一个简单的信号槽机制的实现方式，参考SOUI和Sigslot.h实现

/////////////以下统一了各种“返回值”、“参数列表”完全相同的函数指针的注册行为/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)

namespace simsigslot
{
	//参考了《代码自动生成-宏递归思想》来实现此递归宏
	//链接：http://www.cppblog.com/kevinlynx/archive/2008/08/20/59451.html
	///////////////////////////////////////递归宏的实现///////////////////////////////////////
	#define DECVALUE_1 0
	#define DECVALUE_2 1
	#define DECVALUE_3 2
	#define DECVALUE_4 3
	#define DECVALUE_5 4
	#define DECVALUE_6 5
	#define DECVALUE_7 6
	#define DECVALUE_8 7
	#define DECVALUE_9 8
	
	#ifndef DECVALUE
	#define DECVALUE(n) DECVALUE_/*##n*/
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
	
	#define PARAM_ARG(n) typename T/*##n*/
	#define PARAM_ARRARY(n) ,PARAM_ARG(n)
	#define PARAM_DEF( n ) REPEAT_/*##n*/( n, PARAM_ARRARY, PARAM_ARG )
	
	#define PARAM_ARGEX(n) T/*##n*/ t/*##n*/
	#define PARAM_ARRARYEX(n) ,PARAM_ARGEX(n)
	#define PARAM_DEFEX( n ) REPEAT_/*##n*/( n, PARAM_ARRARYEX, PARAM_ARGEX )
	
	#define REAL_ARG(n) t/*##n*/
	#define REAL_ARRARY(n) ,REAL_ARG(n)
	#define REAL_DEF( n ) REPEAT_/*##n*/( n, REAL_ARRARY, REAL_ARG )
	
	#define REAL_ARGEX(n) T/*##n*/
	#define REAL_ARRARYEX(n) ,REAL_ARGEX(n)
	#define REAL_DEFEX( n ) REPEAT_/*##n*/( n, REAL_ARRARYEX, REAL_ARGEX )
	///////////////////////////////////////递归宏的实现///////////////////////////////////////
	
	//////////////////////////////////////公共代码//////////////////////////////////////
	template<typename TBase>
	class TplForwarderForSigslot :public IForwarder<TBase>
	{
	private:
		TBase* m_Obj;
	
	public:
		TplForwarderForSigslot(const TBase &Obj)
		{
			m_Obj = Obj.Clone();
		}
	
		virtual ~TplForwarderForSigslot()
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
		EM_FUNC_UNKNOWN,   /**< 未知函数类型 */
		EM_FUNC_NORMAL,	   /**< 普通函数类型 */
		EM_FUNC_MEMBER,	   /**< 成员函数类型 */
	};
	
	//////////////////////////////////////公共代码//////////////////////////////////////
	
	/////////////////////////////////////实现建立一个独立的事件订阅类库/////////////////////////////////////
	//#define DEF_SLOTFUNCTOR(classname,n)																		 
																												 
	template</*PARAM_DEF(n)*/ typename Param1st>																						 
	class __ISlotFunctionr/*##n*/																					 
	{																											 
	public:																										 
		virtual ~__ISlotFunctionr/*##n*/() {}																		 
		virtual void InvokeFunction(/*REAL_DEFEX(n)*/Param1st) = 0;															 
		virtual __ISlotFunctionr/*##n*/* Clone() const = 0;															 
		virtual bool Equal(const __ISlotFunctionr/*##n*/ & sour)const = 0;											 
		virtual int GetFunctionrType() const { return EM_FUNC_UNKNOWN; }										 
	};																											 
																												 
	template</*PARAM_DEF(n)*/ typename Param1st>																						 
	class __NormalFunctionr/*##n*/ :public __ISlotFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st>										 
	{																											 
		typedef __ISlotFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st> TSlotFunc;													 
		typedef void(pSlotFunction)(/*REAL_DEFEX(n)*/Param1st);																 
																												 
	public:																										 
		__NormalFunctionr/*##n*/(pSlotFunction* pNormalFunc)														 
			:m_pNormalFunc(pNormalFunc)																			 
		{																										 
			assert((pNormalFunc, "pNormalFunc指针为NULL！！！"));												 
		}																										 
																												 
		virtual ~__NormalFunctionr/*##n*/() {}																		 
																												 
		void InvokeFunction(/*PARAM_DEFEX(n)*/Param1st value1st)																	 
		{																										 
			assert((m_pNormalFunc, "m_pNormalFunc指针为NULL！！！"));											 
			m_pNormalFunc(/*REAL_DEF(n)*/value1st);																			 
		}																										 
																												 
		TSlotFunc* Clone() const																				 
		{																										 
			return new __NormalFunctionr/*##n*/(m_pNormalFunc);														 
		}																										 
																												 
		int GetFunctionrType() const { return EM_FUNC_NORMAL; }													 
																												 
		virtual bool Equal(const TSlotFunc & sour) const														 
		{																										 
			if (sour.GetFunctionrType() != EM_FUNC_NORMAL) return false;										 
			const __NormalFunctionr/*##n*/ *psour = static_cast<const __NormalFunctionr/*##n*/*>(&sour);				 
			assert(psour);																						 
			return psour->m_pNormalFunc == m_pNormalFunc;														 
		}																										 
																												 
	private:																									 
		pSlotFunction *m_pNormalFunc;	/**< 普通函数指针 */													 
	};																											 
																												 
	template<typename TClass, /*PARAM_DEF(n)*/ typename Param1st>																		 
	class __MemberFunctionr/*##n*/ :public __ISlotFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st>										 
	{																											 
		typedef __ISlotFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st> TSlotFunc;													 
		typedef void(TClass::*pSlotFunction)(/*REAL_DEFEX(n)*/Param1st);													 
																												 
	public:																										 
		__MemberFunctionr/*##n*/(TClass *pClassInst, pSlotFunction pMemberFunc)										 
			:m_pClassInst(pClassInst),m_pMemberFunc(pMemberFunc)												 
		{																										 
			assert((pClassInst, "pClassInst指针为NULL！！！"));											    	 
			assert((pMemberFunc, "pMemberFunc指针为NULL！！！"));												 
		}																										 
																												 
		virtual ~__MemberFunctionr/*##n*/() {}																		 
																												 
		void InvokeFunction(/*PARAM_DEFEX(n)*/Param1st value1st )																	 
		{																										 
			assert((m_pClassInst, "m_pClassInst指针为NULL！！！"));										    	 
			assert((m_pMemberFunc, "m_pMemberFunc指针为NULL！！！"));											 
			(m_pClassInst->*m_pMemberFunc)(/*REAL_DEF(n)*/value1st);														 
		}																										 
																												 
		TSlotFunc* Clone() const																				 
		{																										 
			return new __MemberFunctionr/*##n*/(m_pClassInst, m_pMemberFunc);										 
		}																										 
																												 
		int GetFunctionrType() const { return EM_FUNC_MEMBER; }													 
																												 
		virtual bool Equal(const TSlotFunc & sour) const														 
		{																										 
			if (sour.GetFunctionrType() != EM_FUNC_MEMBER) return false;										 
			const __MemberFunctionr/*##n*/ *psour = static_cast<const __MemberFunctionr/*##n*/*>(&sour);				 
			assert(psour);																						 
			return psour->m_pMemberFunc == m_pMemberFunc;														 
		}																										 
																												 
	private:																									 
		pSlotFunction m_pMemberFunc;	/**< 类的成员函数指针 */												 
		TClass *m_pClassInst;			/**< 类实例的指针 */													 
	};																											 
																												 
	template</*PARAM_DEF(n)*/ typename Param1st>		
	class CSignalslot1
	//class classname/*##n*/																					    	 
	{																											 
		typedef __ISlotFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st> TSlotFunc;													 
		typedef IForwarder<TSlotFunc> ImpForwarder;																 
																												 
	public:																										 
		//~classname/*##n*/()		
		~CSignalslot1()
		{		
			std::list<TSlotFunc*>::iterator it2 = m_listSlots.begin();										 
			for(;it2!=m_listSlots.end();it2++)																	 
			{																									 
				delete (*it2);																					 																				 
			}	

			m_listSlots.clear();
		}																										 
																												 
		template</*PARAM_DEF(n)*/ typename Param1st>																					 
		void connectex(void(*pNormalFunc)(/*REAL_DEFEX(n)*/Param1st ))														 
		{																										 
			connect(__NormalFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st>(pNormalFunc));											 
		}																										 
																												 
		template<typename TClass, /*PARAM_DEF(n)*/ typename Param1st>																	 
		void connectex(TClass *pClassInst,																		 
			void(TClass::*pNormalFunc)(/*REAL_DEFEX(n)*/Param1st))															 
		{																										 
			connect(__MemberFunctionr/*##n*/<TClass, /*REAL_DEFEX(n)*/Param1st>(pClassInst, pNormalFunc));						 
		}																										 
																												 
		template</*PARAM_DEF(n)*/ typename Param1st>																					 
		void disconnectex(void(*pNormalFunc)(/*REAL_DEFEX(n)*/Param1st ))													 
		{																										 
			disconnect(__NormalFunctionr/*##n*/</*REAL_DEFEX(n)*/Param1st>(pNormalFunc));										 
		}																										 
																												 
		template<typename TClass, /*PARAM_DEF(n)*/ typename Param1st>																	 
		void disconnectex(TClass *pClassInst,																	 
			void(TClass::*pNormalFunc)(/*REAL_DEFEX(n)*/Param1st))															 
		{																										 
			disconnect(__MemberFunctionr/*##n*/<TClass, /*REAL_DEFEX(n)*/Param1st>(pClassInst, pNormalFunc));					 
		}																										 
																												 
		void operator()(/*PARAM_DEFEX(n)*/Param1st value1st)																		     
		{																										 
			emit(/*REAL_DEF(n)*/value1st);																					 
		}    																									 
																												 
		void emit(/*PARAM_DEFEX(n)*/Param1st value1st)																				 
		{																										 
			typename std::list<TSlotFunc*>::iterator it2 = m_listSlots.begin();										 
			for(;it2!=m_listSlots.end();it2++)																	 
			{																									 
				(*it2)->InvokeFunction(/*REAL_DEF(n)*/value1st);
			}	
		}																										 
																												 
	private:																									 
		/* 链接对应的信号槽 */																					 
		void connect(const TSlotFunc &subscriber)																 
		{																										 																								 
			if(findSlotFunctionr(subscriber)==m_listSlots.end())
			{
				m_listSlots.push_back(subscriber.Clone());
			}
		}																										 
																												 
		/* 断开信号槽之间的链接 */																				 
		void disconnect(const TSlotFunc &subscriber)															 
		{																										 
			typename std::list<TSlotFunc*>::iterator itResult2 = findSlotFunctionr(subscriber);  
			if( m_listSlots.end()!=itResult2)
			{
				delete (*itResult2);
				itResult2 = m_listSlots.erase(itResult2);
			}
		}																										 
																												 
		/*判断当前槽函数是否已被注册*/		
		typename std::list<TSlotFunc*>::iterator findSlotFunctionr(const TSlotFunc &subscriber)
		{																										 
			typename std::list<TSlotFunc*>::iterator it = m_listSlots.begin();										 
			for(;it!=m_listSlots.end();it++)																	 
			{																									 
				if((*it)->Equal(subscriber))
				{
					return it;					
				}
			}																									 
			return m_listSlots.end();																					 
		}	

	private:																									 
		std::list<TSlotFunc*> m_listSlots;   /**< 此信号对应的发送者列表 */								 
	};																											 
	
	//DEF_SLOTFUNCTOR(CSignalslot,1)
	//DEF_SLOTFUNCTOR(CSignalslot,2)
	//DEF_SLOTFUNCTOR(CSignalslot,3)
	//DEF_SLOTFUNCTOR(CSignalslot,5)
	//DEF_SLOTFUNCTOR(CSignalslot,6)
	//DEF_SLOTFUNCTOR(CSignalslot,7)
	//DEF_SLOTFUNCTOR(CSignalslot,8)
};


namespace SampleCode1
{
	using namespace simsigslot;

	void OnEvent_Create(char* szMsg)
	{
		printf("在%s函数内，消息：%s\n", /*__FUNCTION__*/"OnEvent_Create", szMsg);
	}

	enum
	{
		EVT_CREATE = 0,
		EF_PAINT,
		EF_CLICK,
		EVT_TEST,
	};

	void OnEvent_Test(int nEventID, char* szEventMsg)
	{
		printf("nEventID:%d	消息：%s\n",nEventID,szEventMsg);
	}

	class CTest2
	{
	private:
		CSignalslot1<int> m_EvtAction;
		CSignalslot1<char*> m_EvtUI;
		//CSignalslot2<int,char*> m_evtTest;

	protected:
		static void OnEvent_Click(int nTimes)
		{
			printf("在%s函数内，连击了%d次\n", /*__FUNCTION__*/"OnEvent_Click", nTimes);
		}

		void OnEvent_Click2(char* szMsg)
		{
			printf("在%s函数内，显示信息为:%s\n", /*__FUNCTION__*/"OnEvent_Click2", szMsg);

			//m_evtTest.emit(47987,"m_evtTest.emit：在OnEvent_Click2函数体呢\n");
			////两者等价
			//m_evtTest(47987,"m_evtTest：在OnEvent_Click2函数体呢\n");//通过重载caozuofu()来实现，在函数体内调用emit
		}

	public:
		CTest2()
		{
			m_EvtAction.connectex(&CTest2::OnEvent_Click);
			m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
			m_EvtUI.disconnectex(this,&CTest2::OnEvent_Click2);
			m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
			//m_EvtAction.disconnectex(&CTest2::OnEvent_Click);
			//m_evtTest.connectex(OnEvent_Test);
		}

		void PrintText()
		{
			m_EvtAction.emit(5);
			m_EvtUI.emit("嘿，现在在CTest2之中调用哦");
		}

		CSignalslot1<char*>& GetEventSetUI()
		{
			return m_EvtUI;
		}

	};


	class CTest3
	{
	public:
		void OnEvt_OK(char *szMsg)
		{
			printf("此函数在CTest3类内，位于OnEvt_OK，传入的信息为：“%s”\n",szMsg);
		}
	};

	void main1()
	{

		CTest2 obj2;
		CTest3 obj3;
		obj2.GetEventSetUI().connectex(&obj3,&CTest3::OnEvt_OK);

		obj2.PrintText();
		getchar();
	}
};

#include "sigslot.h"
#include <iostream>
namespace SampleCode2
{
	using namespace std;
	using namespace sigslot;
	class CSwitch
	{
	public:
		signal0<> Clicked;//这是一个信号
		signal1<int>Clicked1;
	};

	class CLight :public has_slots<>
	{
	public:
		CLight(bool state){b_state = state;Displaystate();}
		void ToggleState(){b_state = !b_state;Displaystate();} //作为消息的响应
		void TurnOn(){b_state = TRUE;Displaystate();}
		void TurnOff(){b_state = FALSE;Displaystate();}
		void Displaystate(){cout<<"The state is "<<b_state<<endl;}
	private:
		bool b_state;
	};

	void main2()
	{
		CSwitch sw1, sw2,all_on,all_off;
		CLight lp1(TRUE), lp2(FALSE);
		sw1.Clicked.connect(&lp1,&CLight::ToggleState); //绑定
		sw2.Clicked.connect(&lp2,&CLight::ToggleState);
		all_on.Clicked.connect(&lp1,&CLight::TurnOn);
		all_on.Clicked.connect(&lp2,&CLight::TurnOn);
		all_off.Clicked.connect(&lp1,&CLight::TurnOff);
		all_off.Clicked.connect(&lp2,&CLight::TurnOff);

		sw1.Clicked();
		sw2.Clicked();
		all_on.Clicked();
		all_off.Clicked();
		sw1.Clicked1(5987);

		sw1.Clicked.disconnect(&lp1);
		sw2.Clicked.disconnect(&lp2);
		all_on.Clicked.disconnect(&lp1);
		all_on.Clicked.disconnect(&lp2);
		all_off.Clicked.disconnect(&lp1);
		all_off.Clicked.disconnect(&lp2);
		getchar();
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
	SampleCode1::main1();
	SampleCode2::main2();
}
