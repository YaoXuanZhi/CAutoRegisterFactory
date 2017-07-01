/*****************************************************************************
*  @file     CAutoRegisterFactory.h
*  @brief    可注册的工厂类模板
*  @note
*  抽象被工厂类管理着的类指针的行为，从而来定制这么一个类模板
*****************************************************************************/
#ifndef __CAUTOREGISTERFACTORY_H
#define __CAUTOREGISTERFACTORY_H

#include <map>
#include <list>
#include <windows.h>

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
	/**
	* @brief: 通过此接口来拷贝一份类指针的“马甲”
	* @return TBase* 返回基类指针的副本
	* @note
	*
	*/
	virtual TBase *CloneClass() = 0;

	/**
	* @brief: 通过此接口来直接调用类指针
	* @return TBase* 返回基类指针
	* @note
	*
	*/
	virtual TBase* InvokeClass() = 0;
};

/**
* @class CSigleRegisterFactory
* @brief 一个支持自动注册的工厂类模板
* @note
* 注意，如果TKey是自定义数据类型，那么请重载其关系运算符哦，
* 具体看std::map的条件，通常仅需要重载运算符“<”，详情请看
* https://msdn.microsoft.com/ZH-CN/library/s44w4h2s(v=VS.100,d=hv.2).aspx
* 在gcc下编译可能遇到的问题：http://blog.csdn.net/pb1995/article/details/49532285
* 仅仅支持一对一的模式
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
	* @brief: 获得已注册的基类指针的副本
	* @param[in] TKey  Key  索引基类指针的条件
	* @return TBase* 返回基类指针，如果找不到，那么直接返回NULL
	* @note
	* 注意，调用此函数之后，还需要验证一下所得指针是否有效
	*/
	TBase* CloneClass(TKey Key)
	{
		if (IsAddedInFactory(Key))
		{
			return m_mapClasses[Key]->CloneClass();
		}
		OutputDebugStringA("返回空指针，此类并没有被注册！！！\n");
		return NULL;
	}

	/**
	* @brief: 获得已注册的基类指针
	* @param[in] TKey  Key  索引基类指针的条件
	* @return TBase* 返回基类指针，如果找不到，那么直接返回NULL
	* @note
	* 注意，调用此函数之后，还需要验证一下所得指针是否有效
	*/
	TBase* InvokeClass(TKey Key)
	{
		if (IsAddedInFactory(Key))
		{
			return m_mapClasses[Key]->InvokeClass();
		}
		OutputDebugStringA("返回空指针，此类并没有被注册！！！\n");
		return NULL;
	}

	/**
	* @brief: 把工厂类实例之中的某类进行反注册
	* @param[in] TKey  Key  索引基类指针的条件
	* @return 无
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
	* @brief: 将某类注册到工厂类实例之中
	* @param[in] TKey  Key  某类在外部检索时的所用到的条件，通常采用字符串或枚举等类型
	* @param[in|out] IForwarder<TBase>*  pObj  待注册到工厂类实例的基类指针，自动跳过空指针
	* @param[in] bool  bIsOverWrite  如果为true，则重新注册此类，否则，直接释放待注册的基类指针
	* @return 无
	* @note
	* 本类会自动管理所有想要注册到本类工厂的基类指针，因此需要对重复注册的类指针自动进行释放
	*/
	void RegisterFactory(TKey Key, ImpForwarder *pObj, bool bIsOverWrite = false)
	{
		if (NULL == pObj)
		{
			OutputDebugStringA("是不是想浑水摸鱼，待注册的基类指针竟然为空！！！\n");
			return;
		}

		if (IsAddedInFactory(Key))
		{
			if (!bIsOverWrite)
			{
				OutputDebugStringA("已经在这里释放重复注册的类指针了，不要重复释放哦！\n");
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
* @brief 一个支持自动注册的工厂类模板
* @note
* 注意，如果TKey是自定义数据类型，那么请重载其关系运算符哦，
* 具体看std::map的条件，通常仅需要重载运算符“<”，详情请看
* https://msdn.microsoft.com/ZH-CN/library/s44w4h2s(v=VS.100,d=hv.2).aspx
* 在gcc下编译可能遇到的问题：http://blog.csdn.net/pb1995/article/details/49532285
* 支持一对多的方式，通常用于信号槽类型类库里面
* 注意，需要为被处理的基类定义一个纯虚函数“virtual bool Equal(const TBase & sour)const = 0;”
* 然后在子类里进行重载，用来比较重复注册的事件的；原本此功能是采用重载运算符“==”来实现的
* 考虑到此时源码可读性不够好，故作此改变
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
	* @brief: 把工厂类实例之中的某类进行反注册
	* @param[in] TKey  Key  索引基类指针的条件
	* @return 无
	* @note
	* 将当前ID的事件全部全局事件
	* 注意，bIsAll为true的时候，释放所有被注册的事件指针，
	* 如果为false，则释放指定的事件对应的指针
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
						//判断这两者的指针对应的实例信息是否相同
						if( pTemp2->InvokeClass()->Equal(*pObj->InvokeClass()))
						{
							delete pTemp2;
							pTemp2 = NULL;
							//删除std::list()里面的元素
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
	* @brief: 将某类注册到工厂类实例之中
	* @param[in] TKey  Key  某类在外部检索时的所用到的条件，通常采用字符串或枚举等类型
	* @param[in|out] IForwarder<TBase>*  pObj  待注册到工厂类实例的基类指针，自动跳过空指针
	* @param[in] bool  bIsOverWrite  如果为true，则重新注册此类，否则，直接释放待注册的基类指针
	* @return 无
	* @note
	* 本类会自动管理所有想要注册到本类工厂的基类指针，因此需要对重复注册的类指针自动进行释放
	*/
	void RegisterFactory(TKey Key, ImpForwarder *pObj)
	{
		if (NULL == pObj)
		{
			OutputDebugStringA("是不是想浑水摸鱼，待注册的基类指针竟然为空！！！\n");
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
			//判断这两者的指针对应的实例信息是否相同
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
