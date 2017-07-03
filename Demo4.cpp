#include "CAutoRegisterFactory.h"
//һ���򵥵��źŲۻ��Ƶ�ʵ�ַ�ʽ���ο�SOUI��Sigslot.hʵ��

/////////////����ͳһ�˸��֡�����ֵ�����������б���ȫ��ͬ�ĺ���ָ���ע����Ϊ/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)

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

/** @brief �źŲ����� */
enum {
	EM_FUNC_UNKNOWN,   /**< δ֪�������� */
	EM_FUNC_NORMAL,	/**< ��ͨ�������� */
	EM_FUNC_MEMBER,	/**< ��Ա�������� */
};

//////////////////////////////////////��������//////////////////////////////////////

/////////////////////////////////////ʵ�ֽ���һ���������¼��������/////////////////////////////////////
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
	virtual int GetFunctionrType() const { return EM_FUNC_UNKNOWN; }									\
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
		assert((pNormalFunc, "pNormalFuncָ��ΪNULL������"));											\
	}																									\
																										\
	virtual ~NormalFunctionr##n() {}																	\
																										\
	void InvokeFunction(PARAM_DEFEX(n))																	\
	{																									\
		assert((m_pNormalFunc, "m_pNormalFuncָ��ΪNULL������"));										\
		m_pNormalFunc(REAL_DEF(n));																		\
	}																									\
																										\
	TSlotFunc* Clone() const																			\
	{																									\
		return new NormalFunctionr##n(m_pNormalFunc);													\
	}																									\
																										\
	int GetFunctionrType() const { return EM_FUNC_NORMAL; }												\
																										\
	virtual bool Equal(const TSlotFunc & sour) const													\
	{																									\
		if (sour.GetFunctionrType() != EM_FUNC_NORMAL) return false;									\
		const NormalFunctionr##n *psour = static_cast<const NormalFunctionr##n*>(&sour);				\
		assert(psour);																					\
		return psour->m_pNormalFunc == m_pNormalFunc;													\
	}																									\
																										\
private:																								\
	pSlotFunction *m_pNormalFunc;	/**< ��ͨ����ָ�� */												\
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
		assert((pClassInst, "pClassInstָ��ΪNULL������"));											    \
		assert((pMemberFunc, "pMemberFuncָ��ΪNULL������"));											\
	}																									\
																										\
	virtual ~MemberFunctionr##n() {}																	\
																										\
	void InvokeFunction(PARAM_DEFEX(n))																	\
	{																									\
		assert((m_pClassInst, "m_pClassInstָ��ΪNULL������"));										    \
		assert((m_pMemberFunc, "m_pMemberFuncָ��ΪNULL������"));										\
		(m_pClassInst->*m_pMemberFunc)(REAL_DEF(n));													\
	}																									\
																										\
	TSlotFunc* Clone() const																			\
	{																									\
		return new MemberFunctionr##n(m_pClassInst, m_pMemberFunc);										\
	}																									\
																										\
	int GetFunctionrType() const { return EM_FUNC_MEMBER; }												\
																										\
	virtual bool Equal(const TSlotFunc & sour) const													\
	{																									\
		if (sour.GetFunctionrType() != EM_FUNC_MEMBER) return false;									\
		const MemberFunctionr##n *psour = static_cast<const MemberFunctionr##n*>(&sour);				\
		assert(psour);																					\
		return psour->m_pMemberFunc == m_pMemberFunc;													\
	}																									\
																										\
private:																								\
	pSlotFunction m_pMemberFunc;	/**< ��ĳ�Ա����ָ�� */											\
	TClass *m_pClassInst;			/**< ��ʵ����ָ�� */												\
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
	/*����VC 6.0�޷�����>>���ķ��ţ����Ϊ�ۺ����������������*/									    \
	typedef ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													\
																										\
public:																									\
	/*ͳһ��ע��ӿڣ�Ҳ���Խ�����ΪConnect();*/														\
	void SubscribeEvent(TKEY Key, const TSlotFunc &subscriber)											\
	{																									\
		m_ContainterFactory.RegisterFactory(Key, 														\
		new TplForwarderForSigslot<TSlotFunc>(subscriber));												\
	}																									\
																										\
	/*��ע��ָ���¼���Ҳ���Խ�����ΪDisconnect();*/													    \
	void UnsubscribeEvent(TKEY Key,const TSlotFunc &subscriber)											\
	{																									\
		m_ContainterFactory.UnregisterContainter(Key,false,												\
		new TplForwarderForSigslot<TSlotFunc>(subscriber));												\
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

DEF_SLOTFUNCTOR(CSignalslot,1)
DEF_SLOTFUNCTOR(CSignalslot,2)
DEF_SLOTFUNCTOR(CSignalslot,3)
DEF_SLOTFUNCTOR(CSignalslot,5)
DEF_SLOTFUNCTOR(CSignalslot,6)
DEF_SLOTFUNCTOR(CSignalslot,7)
DEF_SLOTFUNCTOR(CSignalslot,8)

/*
�¼���������ʹ����·��
1.��ִ��AddEventID()��ע���¼�֪ͨ��
2.Ȼ����ָ����ʱ��ִ��SubscribeEvent()������֪ͨ�����Ӧ�¼�
3.��������Ҫ����֪ͨ��Ĵ��봦ִ��FireEvent()
4.����ʵ����Ҫ��ͨ��UnsubscribeEvent()���� ע��

�ĳ�QT�źŲ۵����ʱ���������淽ʽ���滻���
SubscribeEvent --> Connect
UnsubscribeEvent --> Disconnect
EventID --> SignalID
*/

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

class CTest1
{
public:
	CTest1()
	{
		m_EvtUI.SubscribeEvent(EF_PAINT, Subscriber(this, &CTest1::OnEvent_Paint));
		m_EvtUI.SubscribeEvent(EVT_CREATE, Subscriber(OnEvent_Create));

		m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(&CTest1::OnEventTest));//VC 6.0����֧��������ľ�̬��Ա����ָ���д��
		//m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//�״�ע��
		//m_EvtTest.SubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//�ظ�ע��
		//m_EvtTest.UnsubscribeEvent(EVT_TEST,Subscriber(OnEventTest));//��ע��
	}

	void OnCreate()
	{
		m_EvtUI.FireEvent(EVT_CREATE, "��������");
	}

	void OnPaint()
	{
		m_EvtUI.FireEvent(EF_PAINT, "�����ػ�");
	}

	void OnTest()
	{
		m_EvtTest.FireEvent(EVT_TEST,"ִ��OnTestŶ",12345,"ִ�����Ŷ");
	}

private:
	CSignalslot1<UINT,char*> m_EvtUI;
	CSignalslot3<UINT,char*,int ,char*> m_EvtTest;

public:
	void OnEvent_Paint(char* szMsg)
	{
		printf("��%s�����ڣ���Ϣ��%s\n", /*__FUNCTION__*/"OnEvent_Paint",szMsg);
	}


	static void OnEventTest(char *szMsg,int nID,char *szErr)
	{
		printf("%d,%s  %s\n",nID,szMsg,szErr);
	}
};

class CTest2
{
private:
	CSignalslot1<UINT,int> m_EvtAction;
	CSignalslot1<UINT,char*> m_EvtUI;

protected:
	static void OnEvent_Click(int nTimes)
	{
		printf("��%s�����ڣ�������%d��\n", /*__FUNCTION__*/"OnEvent_Click", nTimes);
	}

public:
	CTest2()
	{
		m_EvtAction.SubscribeEvent(EF_CLICK, Subscriber(&CTest2::OnEvent_Click));
	}

	void PrintText()
	{
		m_EvtAction.FireEvent(EF_CLICK, 5);
		m_EvtUI.FireEvent(EVT_CREATE, "�٣�������CTest2֮�е���Ŷ");
	}

	CSignalslot1<UINT, char*>& GetEventSetUI()
	{
		return m_EvtUI;
	}
	
};

void main1()
{
	CTest1 obj;
	obj.OnCreate();
	obj.OnPaint();
	obj.OnTest();

	CTest2 obj2;
	obj2.GetEventSetUI().SubscribeEvent(EVT_CREATE, Subscriber(&obj, &CTest1::OnEvent_Paint));
	obj2.PrintText();
	getchar();
}


/////////////////////////////////////////����Sigslot.h/////////////////////////////////////////

#include "sigslot.h"
#include <iostream>
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

/////////////////////////////////////////����Sigslot.h/////////////////////////////////////////

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
	main1();
	main2();
}

/*
��ע�����Ƶ�Ӧ�ó��ϣ��źŲۻ��Ƶ�ʵ�֣���Demo�ǲο���SOUI������¼����Ļ���Դ��(��Ҫ��EventSubscriber.h���ļ���)�Լ�
����sigslot.h���Դ������ʵ�֡���ʵsigslot.h����Ѿ��㹻ǿ���ˣ������м��ں�http://www.cnblogs.com/kanego/articles/sigslot.html
���߲����뷨���Ա���̳���has_slots<>�������䲻ˬ����ˣ���д�����Ƶ���⣬��ΪͬʱҲ��Ϊ������ģ�����ʾDemo�����⣬����
���˵�ĳЩ�ã�ϣ�������ܼ���VC 6.0�����ľ���õ�C++�°汾�����ԡ����������������ʵ������һ���۲���ģʽ��ʵ�ֵģ����Բο���
SOUI�������ˣ����⣬�ⲿ��Դ�벢����ֱ����ӵ���������Ŷ����Ϊ���̰߳�ȫ�ⲿ��û���������и��ֺ���ָ���ת����ʽ�����ͼ��Ҳ��û�����ģ�
��ˣ�����Ҫ���ϻ����������ͼ��֮��Ĺ���ඡ�
*/