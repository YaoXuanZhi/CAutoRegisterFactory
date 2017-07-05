#include "CAutoRegisterFactory.h"
//һ���򵥵��źŲۻ��Ƶ�ʵ�ַ�ʽ���ο�SOUI��Sigslot.hʵ��

/////////////����ͳһ�˸��֡�����ֵ�����������б�����ȫ��ͬ�ĺ���ָ���ע����Ϊ/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)

namespace simsigslot
{
	//�ο��ˡ������Զ�����-��ݹ�˼�롷��ʵ�ִ˵ݹ��
	//���ӣ�http://www.cppblog.com/kevinlynx/archive/2008/08/20/59451.html
	///////////////////////////////////////�ݹ���ʵ��///////////////////////////////////////
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
	#define DECVALUE(n) DECVALUE_##n
	#else
	#error "DECVALUE�ѱ����壬�����������"
	#endif
	
	//Ŀ����Ϊ���ӳ�����չ��ʱ��
	#ifndef MERGECHAR
	#define MERGECHAR(a,b) _MERGECHAR(a,b)
	#define _MERGECHAR(a,b) a##b
	#else
	#error "MERGECHAR�ѱ����壬�����������"
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
	///////////////////////////////////////�ݹ���ʵ��///////////////////////////////////////
	
	//////////////////////////////////////��������//////////////////////////////////////	
	/** @brief �źŲ����� */
	enum {
		EM_FUNC_UNKNOWN,   /**< δ֪�������� */
		EM_FUNC_NORMAL,	   /**< ��ͨ�������� */
		EM_FUNC_MEMBER,	   /**< ��Ա�������� */
	};
	
	//////////////////////////////////////��������//////////////////////////////////////
	
	/////////////////////////////////////ʵ�ֽ���һ���������¼��������/////////////////////////////////////
	#define DEF_SLOTFUNCTOR(classname,n)																		  \
																												  \
	template<PARAM_DEF(n)>																						  \
	class __ISlotFunctionr##n																					  \
	{																											  \
	public:																										  \
		virtual ~__ISlotFunctionr##n() {}																		  \
		virtual void InvokeFunction(REAL_DEFEX(n)) = 0;															  \
		virtual __ISlotFunctionr##n* Clone() const = 0;															  \
		virtual bool Equal(const __ISlotFunctionr##n & sour)const = 0;											  \
		virtual int GetFunctionrType() const { return EM_FUNC_UNKNOWN; }										  \
	};																											  \
																												  \
	template<PARAM_DEF(n)>																						  \
	class __NormalFunctionr##n :public __ISlotFunctionr##n<REAL_DEFEX(n)>										  \
	{																											  \
		typedef __ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													  \
		typedef void(pSlotFunction)(REAL_DEFEX(n));																  \
																												  \
	public:																										  \
																												  \
		__NormalFunctionr##n(pSlotFunction* pNormalFunc)														  \
			:m_pNormalFunc(pNormalFunc)																			  \
		{																										  \
			assert((pNormalFunc, "pNormalFuncָ��ΪNULL������"));												  \
		}																										  \
																												  \
		virtual ~__NormalFunctionr##n() {}																		  \
																												  \
		void InvokeFunction(PARAM_DEFEX(n))																		  \
		{																										  \
			assert((m_pNormalFunc, "m_pNormalFuncָ��ΪNULL������"));											  \
			m_pNormalFunc(REAL_DEF(n));																			  \
		}																										  \
																												  \
		TSlotFunc* Clone() const																				  \
		{																										  \
			return new __NormalFunctionr##n(m_pNormalFunc);														  \
		}																										  \
																												  \
		int GetFunctionrType() const { return EM_FUNC_NORMAL; }													  \
																												  \
		virtual bool Equal(const TSlotFunc & sour) const														  \
		{																										  \
			if (sour.GetFunctionrType() != EM_FUNC_NORMAL) return false;										  \
			const __NormalFunctionr##n *psour = static_cast<const __NormalFunctionr##n*>(&sour);				  \
			assert(psour);																						  \
			return psour->m_pNormalFunc == m_pNormalFunc;														  \
		}																										  \
	private:																									  \
		pSlotFunction *m_pNormalFunc;	/**< ��ͨ����ָ�� */													  \
	};																											  \
																												  \
	template<typename TClass, PARAM_DEF(n)>																		  \
	class __MemberFunctionr##n :public __ISlotFunctionr##n<REAL_DEFEX(n)>										  \
	{																											  \
		typedef __ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													  \
		typedef void(TClass::*pSlotFunction)(REAL_DEFEX(n));													  \
																												  \
	public:																										  \
																												  \
		__MemberFunctionr##n(TClass *pClassInst, pSlotFunction pMemberFunc)										  \
			:m_pClassInst(pClassInst),m_pMemberFunc(pMemberFunc)												  \
		{																										  \
			assert((pClassInst, "pClassInstָ��ΪNULL������"));													  \
			assert((pMemberFunc, "pMemberFuncָ��ΪNULL������"));												  \
		}																										  \
																												  \
		virtual ~__MemberFunctionr##n() {}																		  \
																												  \
		void InvokeFunction(PARAM_DEFEX(n) )																	  \
		{																										  \
			assert((m_pClassInst, "m_pClassInstָ��ΪNULL������"));												  \
			assert((m_pMemberFunc, "m_pMemberFuncָ��ΪNULL������"));											  \
			(m_pClassInst->*m_pMemberFunc)(REAL_DEF(n));														  \
		}																										  \
																												  \
		TSlotFunc* Clone() const																				  \
		{																										  \
			return new __MemberFunctionr##n(m_pClassInst, m_pMemberFunc);										  \
		}																										  \
																												  \
		int GetFunctionrType() const { return EM_FUNC_MEMBER; }													  \
																												  \
		virtual bool Equal(const TSlotFunc & sour) const														  \
		{																										  \
			if (sour.GetFunctionrType() != EM_FUNC_MEMBER) return false;										  \
			const __MemberFunctionr##n *psour = static_cast<const __MemberFunctionr##n*>(&sour);				  \
			assert(psour);																						  \
			return psour->m_pMemberFunc == m_pMemberFunc;														  \
		}																										  \
																												  \
	private:																									  \
		pSlotFunction m_pMemberFunc;	/**< ��ĳ�Ա����ָ�� */												  \
		TClass *m_pClassInst;			/**< ��ʵ����ָ�� */													  \
	};																											  \
																												  \
	template<PARAM_DEF(n)>																						  \
	class classname##n																							  \
	{																											  \
		typedef __ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													  \
																												  \
	public:																										  \
																												  \
		~classname##n()																							  \
		{																										  \
			std::list<TSlotFunc*>::iterator it2 = m_listSlots.begin();											  \
			for(;it2!=m_listSlots.end();it2++)																	  \
			{																									  \
				delete (*it2);																					  \
			}																									  \
			m_listSlots.clear();																				  \
		}																										  \
																												  \
		template<PARAM_DEF(n)>																					  \
		void connectex(void(*pNormalFunc)(REAL_DEFEX(n) ))														  \
		{																										  \
			connect(__NormalFunctionr##n<REAL_DEFEX(n)>(pNormalFunc));											  \
		}																										  \
																												  \
		template<typename TClass, PARAM_DEF(n)>																	  \
		void connectex(TClass *pClassInst,																		  \
			void(TClass::*pNormalFunc)(REAL_DEFEX(n)))															  \
		{																										  \
			connect(__MemberFunctionr##n<TClass, REAL_DEFEX(n)>(pClassInst, pNormalFunc));						  \
		}																										  \
																												  \
		template<PARAM_DEF(n)>																					  \
		void disconnectex(void(*pNormalFunc)(REAL_DEFEX(n) ))													  \
		{																										  \
			disconnect(__NormalFunctionr##n<REAL_DEFEX(n)>(pNormalFunc));										  \
		}																										  \
																												  \
		template<typename TClass, PARAM_DEF(n)>																	  \
		void disconnectex(TClass *pClassInst,																	  \
			void(TClass::*pNormalFunc)(REAL_DEFEX(n)))															  \
		{																										  \
			disconnect(__MemberFunctionr##n<TClass, REAL_DEFEX(n)>(pClassInst, pNormalFunc));					  \
		}																										  \
																												  \
		void operator()(PARAM_DEFEX(n))																			  \
		{																										  \
			emit(REAL_DEF(n));																					  \
		}																										  \
																												  \
		void emit(PARAM_DEFEX(n))																				  \
		{																										  \
			typename std::list<TSlotFunc*>::iterator it2 = m_listSlots.begin();									  \
			for(;it2!=m_listSlots.end();it2++)																	  \
			{																									  \
				(*it2)->InvokeFunction(REAL_DEF(n));															  \
			}																									  \
		}																										  \
																												  \
	private:																									  \
																												  \
		/* ���Ӷ�Ӧ���źŲ� */																					  \
		void connect(const TSlotFunc &subscriber)																  \
		{																										  \
			if(findSlotFunctionr(subscriber)==m_listSlots.end())												  \
			{																									  \
				m_listSlots.push_back(subscriber.Clone());														  \
			}																									  \
		}																										  \
																												  \
		/* �Ͽ��źŲ�֮������� */																				  \
		void disconnect(const TSlotFunc &subscriber)															  \
		{																										  \
			typename std::list<TSlotFunc*>::iterator itResult2 = findSlotFunctionr(subscriber);					  \
			if( m_listSlots.end()!=itResult2)																	  \
			{																									  \
				delete (*itResult2);																			  \
				itResult2 = m_listSlots.erase(itResult2);														  \
			}																									  \
		}																										  \
																												  \
		/*�жϵ�ǰ�ۺ����Ƿ��ѱ�ע��*/																			  \
		typename std::list<TSlotFunc*>::iterator findSlotFunctionr(const TSlotFunc &subscriber)					  \
		{																										  \
			typename std::list<TSlotFunc*>::iterator it = m_listSlots.begin();									  \
			for(;it!=m_listSlots.end();it++)																	  \
			{																									  \
				if((*it)->Equal(subscriber))																	  \
				{																								  \
					return it;																					  \
				}																								  \
			}																									  \
			return m_listSlots.end();																			  \
		}																										  \
																												  \
	private:																									  \
		std::list<TSlotFunc*> m_listSlots;   /**< ���źŶ�Ӧ�ķ������б� */									      \
	};																										 	  \
	
	DEF_SLOTFUNCTOR(CSignalslot,1)
	DEF_SLOTFUNCTOR(CSignalslot,2)
	DEF_SLOTFUNCTOR(CSignalslot,3)
	DEF_SLOTFUNCTOR(CSignalslot,5)
	DEF_SLOTFUNCTOR(CSignalslot,6)
	DEF_SLOTFUNCTOR(CSignalslot,7)
	DEF_SLOTFUNCTOR(CSignalslot,8)
};

namespace SampleCode0
{
	using namespace simsigslot;

	void OnEvent_Create(char* szMsg)
	{
		printf("��%s�����ڣ���Ϣ��%s\n", /*__FUNCTION__*/"OnEvent_Create", szMsg);
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
		printf("nEventID:%d	��Ϣ��%s\n",nEventID,szEventMsg);
	}

	class CTest2
	{
	private:
		CSignalslot1<int> m_EvtAction;
		CSignalslot1<char*> m_EvtUI;
		CSignalslot2<int,char*> m_evtTest;

	protected:
		static void OnEvent_Click(int nTimes)
		{
			printf("��%s�����ڣ�������%d��\n", /*__FUNCTION__*/"OnEvent_Click", nTimes);
		}

		void OnEvent_Click2(char* szMsg)
		{
			printf("��%s�����ڣ���ʾ��ϢΪ:%s\n", /*__FUNCTION__*/"OnEvent_Click2", szMsg);

			m_evtTest.emit(47987,"m_evtTest.emit����OnEvent_Click2��������\n");
			//���ߵȼ�
			m_evtTest(47987,"m_evtTest����OnEvent_Click2��������\n");//ͨ������caozuofu()��ʵ�֣��ں������ڵ���emit
		}

	public:
		CTest2()
		{
			m_EvtAction.connectex(&CTest2::OnEvent_Click);
			m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
			m_EvtUI.disconnectex(this,&CTest2::OnEvent_Click2);
			m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
			m_EvtAction.disconnectex(&CTest2::OnEvent_Click);
			m_evtTest.connectex(OnEvent_Test);
		}

		void PrintText()
		{
			m_EvtAction.emit(5);
			m_EvtUI.emit("�٣�������CTest2֮�е���Ŷ");
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
			printf("�˺�����CTest3���ڣ�λ��OnEvt_OK���������ϢΪ����%s��\n",szMsg);
		}
	};

	void main0()
	{
		printf("SampleCode0::main0()\n");

		CTest2 obj2;
		CTest3 obj3;
		obj2.GetEventSetUI().connectex(&obj3,&CTest3::OnEvt_OK);

		obj2.PrintText();
		getchar();
	}
};

#include <iostream>
namespace SampleCode1
{
	using namespace std;
	using namespace simsigslot;
	class CSwitch
	{
	public:
		//signal0<> Clicked;//�ݲ�֧���޲������ݹ�겻����ʵ�ִ������󣬻���Ҫ��һ���Ľ�
		CSignalslot1<char*> Clicked;
		CSignalslot1<int>Clicked1;
	};

	class CLight
	{
	public:
		CLight(bool state){b_state = state;Displaystate();}
		void ToggleState(char*){b_state = !b_state;Displaystate();} //��Ϊ��Ϣ����Ӧ
		void TurnOn(char*){b_state = TRUE;Displaystate();}
		void TurnOff(char*){b_state = FALSE;Displaystate();}
		void Displaystate(){cout<<"The state is "<<b_state<<endl;}
	private:
		bool b_state;
	};

	void main1()
	{
		printf("SampleCode1::main1()\n");		

		CSwitch sw1, sw2,all_on,all_off;
		CLight lp1(TRUE), lp2(FALSE);
		sw1.Clicked.connectex(&lp1,&CLight::ToggleState); //��
		sw2.Clicked.connectex(&lp2,&CLight::ToggleState);
		all_on.Clicked.connectex(&lp1,&CLight::TurnOn);
		all_on.Clicked.connectex(&lp2,&CLight::TurnOn);
		all_off.Clicked.connectex(&lp1,&CLight::TurnOff);
		all_off.Clicked.connectex(&lp2,&CLight::TurnOff);

		sw1.Clicked("sw1.Clicked");
		sw2.Clicked("sw2.Clicked");
		all_on.Clicked("all_on.Clicked");
		all_off.Clicked("all_off.Clicked");
		sw1.Clicked1(5987);

		sw1.Clicked.disconnectex(&lp1,&CLight::ToggleState);
		sw2.Clicked.disconnectex(&lp2,&CLight::ToggleState);
		all_on.Clicked.disconnectex(&lp1,&CLight::TurnOn);
		all_on.Clicked.disconnectex(&lp2,&CLight::TurnOn);
		all_off.Clicked.disconnectex(&lp1,&CLight::TurnOff);
		all_off.Clicked.disconnectex(&lp2,&CLight::TurnOff);
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
		signal0<> Clicked;//����һ���ź�
		signal1<int>Clicked1;
	};

	class CLight :public has_slots<>
	{
	public:
		CLight(bool state){b_state = state;Displaystate();}
		void ToggleState(){b_state = !b_state;Displaystate();} //��Ϊ��Ϣ����Ӧ
		void TurnOn(){b_state = TRUE;Displaystate();}
		void TurnOff(){b_state = FALSE;Displaystate();}
		void Displaystate(){cout<<"The state is "<<b_state<<endl;}
	private:
		bool b_state;
	};

	void main2()
	{
		printf("SampleCode2::main2()\n");

		CSwitch sw1, sw2,all_on,all_off;
		CLight lp1(TRUE), lp2(FALSE);
		sw1.Clicked.connect(&lp1,&CLight::ToggleState); //��
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
* @brief: ��ʼ����ڴ�й©
* @return ��
* @note   
* ͨ��������������������ô˺�����ע�⣬����Debug����Ч
*/
void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

void main()
{
	EnableMemLeakCheck();
	SampleCode0::main0();
	SampleCode1::main1();
	SampleCode2::main2();
}