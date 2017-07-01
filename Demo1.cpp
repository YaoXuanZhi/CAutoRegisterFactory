#include "CAutoRegisterFactory.h"
#include <string>
#include <tchar.h>
#include <windows.h>
#pragma warning(disable:4996)

#ifndef tstring
#ifndef _UNICODE
#define tstring std::string
#else
#define tstring std::wstring
#endif
#endif

void DebugStringEx(TCHAR* str, ...)
{
	va_list args;
	TCHAR szText[1024] = { 0 };
	va_start(args, str);
	_vstprintf(szText, str, args);
	OutputDebugString(szText);
	va_end(args);
}

class IObject {
public:
	IObject() { m_szName = _T("IObject"); }
	virtual ~IObject() {}
	virtual const TCHAR * GetName() { return &m_szName[0]; }
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject��"), &m_szName[0]); }
protected:
	tstring m_szName;
};

class IObject1st :public IObject
{
public:
	IObject1st() { m_szName = _T("IObject1st"); }
	virtual ~IObject1st() {}
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject1st��"), &m_szName[0]); }
};

class IObject2nd :public IObject1st
{
public:
	IObject2nd() { m_szName = _T("IObject2nd"); }
	virtual ~IObject2nd() {}
	virtual void Print() { DebugStringEx(_T("%s:%s\n"), _T("��IObject2nd��"), &m_szName[0]); }
};

template<typename TSub, typename TBase>
class TplClassTemplate :public IForwarder<TBase>
{
private:
	TSub m_Obj;

public:
	virtual ~TplClassTemplate() {}

public:
	virtual TBase* CloneClass() { return new TSub; }
	virtual TBase* InvokeClass() { return &m_Obj; }
};

#pragma region ԭ��ģʽ
void main1st()
{
	CSingleRegisteredFactory<tstring, IObject> tempfactory;
	TplClassTemplate<IObject2nd, IObject>* pNew = new TplClassTemplate<IObject2nd, IObject>;
	tempfactory.RegisterFactory(pNew->InvokeClass()->GetName(), pNew);

	IObject *pTemp = tempfactory.InvokeClass(pNew->InvokeClass()->GetName());
	if (NULL != pTemp)
	{
		pTemp->Print();
	}
}
#pragma endregion

#pragma region ��װģʽ
class CIObjectFactory
{
public:
	void RegisterIObject(IForwarder<IObject> *pObj, bool bIsOverWrite = false)
	{
		if (NULL == pObj)
			return;
		m_IObjectFactory.RegisterFactory(pObj->InvokeClass()->GetName(), pObj, bIsOverWrite);
	}

	void UnregisterIObject(tstring szName) { m_IObjectFactory.UnregisterFactory(szName); }

	IObject *InvokeIObject(tstring szName) { return m_IObjectFactory.InvokeClass(szName); };

private:
	CSingleRegisteredFactory<tstring, IObject> m_IObjectFactory;
};

template<typename TBase>
class TplClassOfIObject
{
public:
	TplClassOfIObject() :m_pTemp(NULL)
	{
		m_pTemp = new TplClassTemplate<TBase, IObject>;
	}

	operator IForwarder<IObject>*()
	{
		return m_pTemp;
	}
private:
	IForwarder<IObject> *m_pTemp;
};

void main2nd()
{
	CIObjectFactory tempfactory;
	tempfactory.RegisterIObject(TplClassOfIObject<IObject1st>());
	tempfactory.RegisterIObject(TplClassOfIObject<IObject2nd>());
	IObject *pTemp = tempfactory.InvokeIObject(_T("IObject1st"));
	if (NULL != pTemp)
	{
		pTemp->Print();
	}
}
#pragma endregion


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
	main1st();
	main2nd();
}

/*
��ע�����Ƶ�Ӧ�ó��ϣ������������ӿؼ���ע����ơ������ļ������֧�ֵȵȡ�
ͨ�ù�ʽΪ�������붨��һ������������������ͬ���࣬����ÿ�����ʵ�������ᴴ��
һ�ݣ���ô���Demo����·��ʾ�ͱȽ��ʺ���
*/
