#include "CAutoRegisterFactory.h"
//һ���򵥵��źŲۻ��Ƶ�ʵ�ַ�ʽ���ο�SOUI��Sigslot.hʵ��

/////////////����ͳһ�˸��֡�����ֵ�����������б���ȫ��ͬ�ĺ���ָ���ע����Ϊ/////////////
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#pragma warning(disable:4996)
/**
* @class ISlotFunctionr
* @brief ����һ���ۺ���������
* @note
* Ŀǰ����ʵ��һ���������͵�֧��
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
* @brief ������һ��ר�Ź���ȫ�ֺ�����̬(��Ա)����������
* @note
* ��������������ָ��ͨ������Ч�ģ��������������
* ֱ��ʹ�ó�������Ϣ�����������ɡ�
*/
template<typename ReturnType, typename ParamType1st>
class NormalFunctionr :public ISlotFunctionr<ReturnType, ParamType1st>
{
	typedef ReturnType(pSlotFunction)(ParamType1st);
public:
	NormalFunctionr(pSlotFunction* pNormalFunc)
		:m_pNormalFunc(pNormalFunc)
	{
		assert((pNormalFunc, "pNormalFuncָ��ΪNULL������"));
	}
	virtual ~NormalFunctionr() {}
	ReturnType InvokeFunction(ParamType1st Param1st)
	{
		assert((m_pNormalFunc, "m_pNormalFuncָ��ΪNULL������"));
		return m_pNormalFunc(Param1st);
	}
	ISlotFunctionr<ReturnType, ParamType1st>* Clone() const
	{
		return new NormalFunctionr(m_pNormalFunc);
	}

private:
	pSlotFunction *m_pNormalFunc;	/**< ��ͨ����ָ�� */
};

/**
* @brief: ����ע����ͨ����ָ���
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
* @brief ������һ��ר������ͳһ����ͬ��ĳ�Ա����������
* @note
* ������ķǾ�̬��Ա������=���ԣ���ˣ�����ͨ�������
* ʵ����ָ�������ô˺����������ҪΪ��ά������������
* ��1���洢��ʵ����ָ�롾2���Ǿ�̬��Ա������ָ��
* ע�⣬���뱣֤��������Աָ���������Ч��
*/
template<typename TClass, typename ReturnType, typename ParamType1st>
class MemberFunctionr :public ISlotFunctionr<ReturnType, ParamType1st>
{	
	typedef ReturnType(TClass::*pSlotFunction)(ParamType1st);//C++�߼�����
public:
	MemberFunctionr(TClass *pClassInst, pSlotFunction pMemberFunc)
		:m_pMemberFunc(pMemberFunc), m_pClassInst(pClassInst)
	{
		assert((pClassInst, "pClassInstָ��ΪNULL������"));
		assert((pMemberFunc, "pMemberFuncָ��ΪNULL������"));
	}
	virtual ~MemberFunctionr() {}
	ReturnType InvokeFunction(ParamType1st Param1st)
	{
		assert((m_pClassInst, "m_pClassInstָ��ΪNULL������"));
		assert((m_pMemberFunc, "m_pMemberFuncָ��ΪNULL������"));
		return (m_pClassInst->*m_pMemberFunc)(Param1st);//����Ǹ߼��﷨
	}

	ISlotFunctionr<ReturnType, ParamType1st>* Clone() const
	{
		return new MemberFunctionr(m_pClassInst, m_pMemberFunc);
	}

private:
	pSlotFunction m_pMemberFunc;	/**< ��ĳ�Ա����ָ�� */
	TClass *m_pClassInst;			/**< ��ʵ����ָ�� */
};

/**
* @brief: ����ע����ĳ�Ա����ָ���
* @note
*
*/
template<typename TClass, typename ReturnType, typename ParamType1st>
MemberFunctionr<TClass, ReturnType, ParamType1st> Subscriber(TClass *pClassInst, ReturnType(TClass::*pNormalFunc)(ParamType1st))
{
	return MemberFunctionr<TClass, ReturnType, ParamType1st>(pClassInst, pNormalFunc);
}

/////////////����ͳһ�˸��֡�����ֵ�����������б���ȫ��ͬ�ĺ���ָ���ע����Ϊ/////////////

////////////////////Ϊÿ����ע��ĺ���ָ�뽨������������֮������һ��������//////////////////
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
* @brief �����������źŲ۹�����                                                                                         
* @note
* ע�⣬��Ҫ�����̰߳�ȫŶ
* ���������沢���ǹ�����ģ�����ר������������ָ��ģ�
* ��������滹��Ҫ�����޸ģ�Ҳ����ÿ����ע��ĺ�������ʵ����һ����ʵ��
* ��ָ�룬���Ǳ�ע��Ķ����Ա����������Դ��ͬһ����
*/
template<typename TKEY, typename ReturnType, typename ParamType1st>
class CEventFactory
{
	//����VC 6.0�޷�����>>���ķ��ţ����Ϊ�ۺ����������������
	typedef ISlotFunctionr<ReturnType, ParamType1st> TSlotFunc;

public:
	//ͳһ��ע��ӿڣ�Ҳ���Խ�����ΪConnect();
	void SubscribeEvent(TKEY Key, const TSlotFunc &subscriber)
	{
		m_IObjectFactory.RegisterFactory(Key, TplClassOfTest<TSlotFunc>(subscriber));
	}

	//��ע��ĳ�¼���Ҳ���Խ�����ΪDisconnect();
	void UnsubscribeEvent(TKEY Key) { m_IObjectFactory.UnregisterFactory(Key); }

	ReturnType FireEvent(TKEY Key, ParamType1st Parm1st)
	{
		TSlotFunc *pSlotFunc = m_IObjectFactory.InvokeClass(Key);
		assert(("pSlotFuncָ��ΪNULL!!!������һ���Ƿ��Ѿ���ȷ��ע���˴˺���", pSlotFunc));
		return pSlotFunc->InvokeFunction(Parm1st);
	}

	CEventFactory *GetEventSet()
	{
		return this;
	}

private:
	//ע�⣬���¼��������棬�¼�IDͨ����UINT���͵ģ�����ڴ�ֱ��ָ�����ɡ�
	CSingleRegisteredFactory<TKEY, TSlotFunc> m_IObjectFactory;
};

bool OnEvent_Create(char* szMsg)
{
	printf("��%s�����ڣ���Ϣ��%s\n", __FUNCTION__, szMsg);
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
		m_EvtUI.FireEvent(EVT_CREATE, "��������");
	}

	void OnPaint()
	{
		m_EvtUI.FireEvent(EF_PAINT, "�����ػ�");
	}

private:
	CEventFactory<UINT,bool, char*> m_EvtUI;

public:
	bool OnEvent_Paint(char* szMsg)
	{
		printf("��%s�����ڣ���Ϣ��%s\n", __FUNCTION__,szMsg);
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
		printf("��%s�����ڣ�������%d��\n", __FUNCTION__, nTimes);
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
		m_EvtUI.GetEventSet()->FireEvent(EVT_CREATE, "�٣�������CTest2֮�е���Ŷ");
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
	CTest1 obj;
	obj.OnCreate();
	obj.OnPaint();

	CTest2 obj2;
	obj2.GetEventSetUI().SubscribeEvent(EVT_CREATE, Subscriber(&obj, &CTest1::OnEvent_Paint));
	obj2.PrintText();
	getchar();
}

/*
��ע�����Ƶ�Ӧ�ó��ϣ��źŲۻ��Ƶ�ʵ�֣���Demo�ǲο���SOUI������¼����Ļ���Դ��(��Ҫ��EventSubscriber.h���ļ���)�Լ�
����sigslot.h���Դ������ʵ�֡���ʵsigslot.h����Ѿ��㹻ǿ���ˣ������м��ں�http://www.cnblogs.com/kanego/articles/sigslot.html
���߲����뷨���Ա���̳���has_slots<>�������䲻ˬ����ˣ���д�����Ƶ���⣬��ΪͬʱҲ��Ϊ������ģ�����ʾDemo�����⣬����
���˵�ĳЩ�ã�ϣ�������ܼ���VC 6.0�����ľ���õ�C++�°汾�����ԡ����������������ʵ������һ���۲���ģʽ��ʵ�ֵģ����Բο���
SOUI�������ˣ����⣬�ⲿ��Դ�벢����ֱ����ӵ���������Ŷ����Ϊ���̰߳�ȫ�ⲿ��û���������и��ֺ���ָ���ת����ʽ�����ͼ��Ҳ��û�����ģ�
��ˣ�����Ҫ���ϻ����������ͼ��֮��Ĺ���ඡ�
*/