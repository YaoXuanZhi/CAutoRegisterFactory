/*****************************************************************************
*  @file     CAutoRegisterFactory.h
*  @brief    ��ע��Ĺ�����ģ��
*  @note
*  ���󱻹���������ŵ���ָ�����Ϊ���Ӷ���������ôһ����ģ��
*****************************************************************************/
#ifndef __CAUTOREGISTERFACTORY_H
#define __CAUTOREGISTERFACTORY_H

#include <map>
#include <list>
#include <windows.h>

/**
* @class IForwarder
* @brief ת��������
* @note
* �ѹ���ģʽ�µ����ֳ��õĵ��÷�ʽ���г��󣬽������
* �����ɸ���[CloneClass()]/ֱ�ӵ���[InvokeClass()]
*/
template<typename TBase>
class IForwarder
{
public:
	virtual ~IForwarder() {}

public:
	/**
	* @brief: ͨ���˽ӿ�������һ����ָ��ġ���ס�
	* @return TBase* ���ػ���ָ��ĸ���
	* @note
	*
	*/
	virtual TBase *CloneClass() = 0;

	/**
	* @brief: ͨ���˽ӿ���ֱ�ӵ�����ָ��
	* @return TBase* ���ػ���ָ��
	* @note
	*
	*/
	virtual TBase* InvokeClass() = 0;
};

/**
* @class CSigleRegisterFactory
* @brief һ��֧���Զ�ע��Ĺ�����ģ��
* @note
* ע�⣬���TKey���Զ����������ͣ���ô���������ϵ�����Ŷ��
* ���忴std::map��������ͨ������Ҫ�����������<���������뿴
* https://msdn.microsoft.com/ZH-CN/library/s44w4h2s(v=VS.100,d=hv.2).aspx
* ��gcc�±���������������⣺http://blog.csdn.net/pb1995/article/details/49532285
* ����֧��һ��һ��ģʽ
*/
template<typename TKey, typename TBase>
class CSingleRegisteredFactory
{
	typedef IForwarder<TBase> ImpForwarder;
public:
	CSingleRegisteredFactory() {}
	virtual ~CSingleRegisteredFactory()
	{
		typename std::map<TKey, ImpForwarder*>::iterator it = m_mapClasses.begin();
		for (; it != m_mapClasses.end(); it++)
		{
			ImpForwarder* pTemp = it->second;
			if (NULL != pTemp)
			{
				delete pTemp;
				pTemp = NULL;
			}
		}
	}

	/**
	* @brief: �����ע��Ļ���ָ��ĸ���
	* @param[in] TKey  Key  ��������ָ�������
	* @return TBase* ���ػ���ָ�룬����Ҳ�������ôֱ�ӷ���NULL
	* @note
	* ע�⣬���ô˺���֮�󣬻���Ҫ��֤һ������ָ���Ƿ���Ч
	*/
	TBase* CloneClass(TKey Key)
	{
		if (IsAddedInFactory(Key))
		{
			return m_mapClasses[Key]->CloneClass();
		}
		OutputDebugStringA("���ؿ�ָ�룬���ಢû�б�ע�ᣡ����\n");
		return NULL;
	}

	/**
	* @brief: �����ע��Ļ���ָ��
	* @param[in] TKey  Key  ��������ָ�������
	* @return TBase* ���ػ���ָ�룬����Ҳ�������ôֱ�ӷ���NULL
	* @note
	* ע�⣬���ô˺���֮�󣬻���Ҫ��֤һ������ָ���Ƿ���Ч
	*/
	TBase* InvokeClass(TKey Key)
	{
		if (IsAddedInFactory(Key))
		{
			return m_mapClasses[Key]->InvokeClass();
		}
		OutputDebugStringA("���ؿ�ָ�룬���ಢû�б�ע�ᣡ����\n");
		return NULL;
	}

	/**
	* @brief: �ѹ�����ʵ��֮�е�ĳ����з�ע��
	* @param[in] TKey  Key  ��������ָ�������
	* @return ��
	* @note
	*
	*/
	void UnregisterFactory(TKey Key)
	{
		if (!IsAddedInFactory(Key))
			return;
		ImpForwarder *pTemp = m_mapClasses[Key];
		if (NULL != pTemp)
		{
			delete pTemp;
			pTemp = NULL;
			m_mapClasses.erase(m_mapClasses.find(Key));
		}
	}

	/**
	* @brief: ��ĳ��ע�ᵽ������ʵ��֮��
	* @param[in] TKey  Key  ĳ�����ⲿ����ʱ�����õ���������ͨ�������ַ�����ö�ٵ�����
	* @param[in|out] IForwarder<TBase>*  pObj  ��ע�ᵽ������ʵ���Ļ���ָ�룬�Զ�������ָ��
	* @param[in] bool  bIsOverWrite  ���Ϊtrue��������ע����࣬����ֱ���ͷŴ�ע��Ļ���ָ��
	* @return ��
	* @note
	* ������Զ�����������Ҫע�ᵽ���๤���Ļ���ָ�룬�����Ҫ���ظ�ע�����ָ���Զ������ͷ�
	*/
	void RegisterFactory(TKey Key, ImpForwarder *pObj, bool bIsOverWrite = false)
	{
		if (NULL == pObj)
		{
			OutputDebugStringA("�ǲ������ˮ���㣬��ע��Ļ���ָ�뾹ȻΪ�գ�����\n");
			return;
		}

		if (IsAddedInFactory(Key))
		{
			if (!bIsOverWrite)
			{
				OutputDebugStringA("�Ѿ��������ͷ��ظ�ע�����ָ���ˣ���Ҫ�ظ��ͷ�Ŷ��\n");
				delete pObj;
				pObj = NULL;
				return;
			}
			UnregisterFactory(Key);
		}
		m_mapClasses[Key] = pObj;
	}

	bool IsAddedInFactory(TKey Key) { return m_mapClasses.find(Key) != m_mapClasses.end() ? true : false; }

private:
	std::map<TKey, ImpForwarder*> m_mapClasses;

};


/**
* @class CMutipleRegisterFactory
* @brief һ��֧���Զ�ע��Ĺ�����ģ��
* @note
* ע�⣬���TKey���Զ����������ͣ���ô���������ϵ�����Ŷ��
* ���忴std::map��������ͨ������Ҫ�����������<���������뿴
* https://msdn.microsoft.com/ZH-CN/library/s44w4h2s(v=VS.100,d=hv.2).aspx
* ��gcc�±���������������⣺http://blog.csdn.net/pb1995/article/details/49532285
* ֧��һ�Զ�ķ�ʽ��ͨ�������źŲ������������
* ע�⣬��ҪΪ������Ļ��ඨ��һ�����麯����virtual bool Equal(const TBase & sour)const = 0;��
* Ȼ����������������أ������Ƚ��ظ�ע����¼��ģ�ԭ���˹����ǲ��������������==����ʵ�ֵ�
* ���ǵ���ʱԴ��ɶ��Բ����ã������˸ı�
*/
template<typename TKey, typename TBase>
class CMutipleRegisteredFactory
{
	typedef IForwarder<TBase> ImpForwarder;
	typedef std::list<ImpForwarder*> ForwardContainter;
public:
	CMutipleRegisteredFactory() {}
	virtual ~CMutipleRegisteredFactory()
	{
		typename std::map<TKey, ForwardContainter>::iterator it = m_mapContainer.begin();
		for (; it != m_mapContainer.end(); it++)
		{
			UnregisterContainter(it->first);
		}
	}

	/**
	* @brief: �ѹ�����ʵ��֮�е�ĳ����з�ע��
	* @param[in] TKey  Key  ��������ָ�������
	* @return ��
	* @note
	* ����ǰID���¼�ȫ��ȫ���¼�
	* ע�⣬bIsAllΪtrue��ʱ���ͷ����б�ע����¼�ָ�룬
	* ���Ϊfalse�����ͷ�ָ�����¼���Ӧ��ָ��
	*/
	void UnregisterContainter(TKey Key,bool bIsAll = true, ImpForwarder *pObj = NULL)
	{
		if(IsAddedInFactory(Key))
		{
			ForwardContainter &temp = m_mapContainer[Key];
			typename ForwardContainter::iterator it = temp.begin();
			for(;it!=temp.end();it++)
			{
				ImpForwarder *pTemp2 = *it;
				if(bIsAll)
				{
					delete pTemp2;
					pTemp2 = NULL;
				}else{
					if(NULL != pObj)
					{
						//�ж������ߵ�ָ���Ӧ��ʵ����Ϣ�Ƿ���ͬ
						if( pTemp2->InvokeClass()->Equal(*pObj->InvokeClass()))
						{
							delete pTemp2;
							pTemp2 = NULL;
							//ɾ��std::list()�����Ԫ��
							it = temp.erase(it);
							break;
						}
					}else
						break;
				}
			}
		}
		if(NULL != pObj)
		{
			delete pObj;
			pObj = NULL;
		}
	}

	/**
	* @brief: ��ĳ��ע�ᵽ������ʵ��֮��
	* @param[in] TKey  Key  ĳ�����ⲿ����ʱ�����õ���������ͨ�������ַ�����ö�ٵ�����
	* @param[in|out] IForwarder<TBase>*  pObj  ��ע�ᵽ������ʵ���Ļ���ָ�룬�Զ�������ָ��
	* @param[in] bool  bIsOverWrite  ���Ϊtrue��������ע����࣬����ֱ���ͷŴ�ע��Ļ���ָ��
	* @return ��
	* @note
	* ������Զ�����������Ҫע�ᵽ���๤���Ļ���ָ�룬�����Ҫ���ظ�ע�����ָ���Զ������ͷ�
	*/
	void RegisterFactory(TKey Key, ImpForwarder *pObj)
	{
		if (NULL == pObj)
		{
			OutputDebugStringA("�ǲ������ˮ���㣬��ע��Ļ���ָ�뾹ȻΪ�գ�����\n");
			return;
		}

		if(!IsAddedInFactory(Key))
		{
			ForwardContainter obj;
			m_mapContainer[Key]= obj;
		}

		ForwardContainter &temmp = m_mapContainer[Key];
		typename ForwardContainter::iterator it = temmp.begin();
		for(;it!=temmp.end();it++)
		{
			ImpForwarder *pTemp2 = *it;
			//�ж������ߵ�ָ���Ӧ��ʵ����Ϣ�Ƿ���ͬ
			if(pTemp2->InvokeClass()->Equal(*pObj->InvokeClass()))
			{
				delete pObj;
				pObj = NULL;
				return;
			}
		}
		m_mapContainer[Key].push_back(pObj);
	}

	bool IsAddedInFactory(TKey Key) 
	{ 
		return m_mapContainer.find(Key) != m_mapContainer.end() ? true : false; 
	}

	ForwardContainter* InvokeContanter(TKey Key)
	{
		if (IsAddedInFactory(Key))
		{
			return &m_mapContainer[Key];
		}
		return NULL;
	}

private:
	std::map<TKey, ForwardContainter> m_mapContainer;
};

#endif
