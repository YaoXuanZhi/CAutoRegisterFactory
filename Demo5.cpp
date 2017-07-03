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
	__NormalFunctionr##n(pSlotFunction* pNormalFunc)														  \
		:m_pNormalFunc(pNormalFunc)																			  \
	{																										  \
		assert((pNormalFunc, "pNormalFuncָ��ΪNULL������"));												  \
	}																										  \
																											  \
	virtual ~__NormalFunctionr##n() {}																		  \
																											  \
	void InvokeFunction(PARAM_DEFEX(n) )																	  \
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
																											  \
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
	__MemberFunctionr##n(TClass *pClassInst, pSlotFunction pMemberFunc)										  \
		:m_pClassInst(pClassInst),m_pMemberFunc(pMemberFunc)												  \
	{																										  \
		assert((pClassInst, "pClassInstָ��ΪNULL������"));											    	  \
		assert((pMemberFunc, "pMemberFuncָ��ΪNULL������"));												  \
	}																										  \
																											  \
	virtual ~__MemberFunctionr##n() {}																		  \
																											  \
	void InvokeFunction(PARAM_DEFEX(n) )																	  \
	{																										  \
		assert((m_pClassInst, "m_pClassInstָ��ΪNULL������"));										    	  \
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
class classname##n																					    	  \
{																											  \
	typedef __ISlotFunctionr##n<REAL_DEFEX(n)> TSlotFunc;													  \
	typedef IForwarder<TSlotFunc> ImpForwarder;																  \
																											  \
public:																										  \
	~classname##n()																							  \
	{																										  \
		std::list<ImpForwarder*>::iterator it = m_listSenders.begin();										  \
		for(;it!=m_listSenders.end();it++)																	  \
		{																									  \
			ImpForwarder* pItem = *it;																		  \
			delete pItem;																					  \
			pItem = NULL;																					  \
		}																									  \
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
	void emit(PARAM_DEFEX(n) )																				  \
	{																										  \
		std::list<ImpForwarder*>::iterator it = m_listSenders.begin();										  \
		for(;it!=m_listSenders.end();it++)																	  \
		{																									  \
			ImpForwarder* pItem = *it;																		  \
			pItem->InvokeClass()->InvokeFunction(REAL_DEF(n));												  \
		}																									  \
	}																										  \
																											  \
private:																									  \
	/* ���Ӷ�Ӧ���źŲ� */																					  \
	void connect(const TSlotFunc &subscriber)																  \
	{																										  \
		ImpForwarder* pObj= new TplForwarderForSigslot<TSlotFunc>(subscriber);								  \
		std::list<ImpForwarder*>::iterator itResult = GetIterator(pObj); 									  \
		if(m_listSenders.end() == itResult)																	  \
		{																									  \
			ImpForwarder* pObj= new TplForwarderForSigslot<TSlotFunc>(subscriber);							  \
			m_listSenders.push_back(pObj);																	  \
		}																									  \
																											  \
		if(NULL!=pObj)																						  \
		{																									  \
			delete pObj;																					  \
			pObj = NULL;																					  \
		}																									  \
	}																										  \
																											  \
	/* �Ͽ��źŲ�֮������� */																				  \
	void disconnect(const TSlotFunc &subscriber)															  \
	{																										  \
		ImpForwarder* pObj= new TplForwarderForSigslot<TSlotFunc>(subscriber);								  \
		std::list<ImpForwarder*>::iterator itResult = GetIterator(pObj); 									  \
		if(m_listSenders.end()!= itResult)																	  \
		{																									  \
			ImpForwarder *p = *itResult;																	  \
			delete p;																						  \
			p = NULL;																						  \
			/* ɾ��std::list()�����Ԫ�� */																	  \
			itResult = m_listSenders.erase(itResult);														  \
		}																									  \
																											  \
		if(NULL!=pObj)																						  \
		{																									  \
			delete pObj;																					  \
			pObj = NULL;																					  \
		}																									  \
	}																										  \
																											  \
	/*�жϵ�ǰ�ۺ����Ƿ��ѱ�ע��*/													    					  \
	typename std::list<ImpForwarder*>::iterator GetIterator(ImpForwarder* pObj)								  \
	{																										  \
		std::list<ImpForwarder*>::iterator itResult = m_listSenders.end();									  \
																											  \
		if(NULL == pObj)																					  \
			return itResult;																				  \
																											  \
		std::list<ImpForwarder*>::iterator it = m_listSenders.begin();										  \
		for(;it!=m_listSenders.end();it++)																	  \
		{																									  \
			ImpForwarder* pItem = *it;																		  \
																											  \
			/* �ж������ߵ�ָ���Ӧ��ʵ����Ϣ�Ƿ���ͬ */													  \
			if( pItem->InvokeClass()->Equal(*pObj->InvokeClass()))											  \
			{																								  \
				itResult = it;																				  \
				break;																						  \
			}																								  \
		}																									  \
		return itResult;																					  \
	}																										  \
																											  \
private:																									  \
	std::list<ImpForwarder*> m_listSenders;   /**< ���źŶ�Ӧ�ķ������б� */								  \
};																											  \

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
2.Ȼ����ָ����ʱ��ִ��connect()������֪ͨ�����Ӧ�¼�
3.��������Ҫ����֪ͨ��Ĵ��봦ִ��emit()
4.����ʵ����Ҫ��ͨ��disconnect()���� ע��

�ĳ�QT�źŲ۵����ʱ���������淽ʽ���滻���
connect --> Connect
disconnect --> Disconnect
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

		m_evtTest.emit(47987,"��OnEvent_Click2��������\n");
	}

public:
	CTest2()
	{
		m_EvtAction.connectex(&CTest2::OnEvent_Click);
		m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
		m_EvtUI.disconnectex(this,&CTest2::OnEvent_Click2);
		m_EvtUI.connectex(this,&CTest2::OnEvent_Click2);
		//m_EvtAction.disconnectex(&CTest2::OnEvent_Click);
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

class CTest3
{
public:
	void OnEvt_OK(char *szMsg)
	{
		printf("�˺�����CTest3���ڣ�λ��OnEvt_OK���������ϢΪ����%s��\n",szMsg);
	}
};

void main()
{
	EnableMemLeakCheck();

	CTest2 obj2;
	CTest3 obj3;
	obj2.GetEventSetUI().connectex(&obj3,&CTest3::OnEvt_OK);

	obj2.PrintText();
	getchar();
}

