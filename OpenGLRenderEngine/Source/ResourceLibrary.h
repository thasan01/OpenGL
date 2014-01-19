#pragma once 
#include "TypeDef.h"


  template<class KeyType, class DataType> class ResourceLibrary
  {
	public:
	typename typedef std::weak_ptr<DataType> DataWeakPtrTypeDef;
	typename typedef std::shared_ptr<DataType> DataSharedPtrTypeDef;
	typename typedef std::map<KeyType, DataWeakPtrTypeDef> DataMapTypeDef;

	ResourceLibrary(){}

	DataSharedPtrTypeDef get(const KeyType& key)
	{
		DataMapTypeDef::iterator iter = m_dataMap.find(key);
		return (iter == m_dataMap.end() ? DataSharedPtrTypeDef() : iter->second.lock() );
	}

	void add(const KeyType& key, DataSharedPtrTypeDef& ptr)
	{
		DataMapTypeDef::iterator iter = m_dataMap.find(key);
		if(iter == m_dataMap.end())
			m_dataMap.insert(pair<KeyType,DataWeakPtrTypeDef>(key,ptr));
		else
			ptr = iter->second.lock();
	}

	bool remove(const KeyType& key)
	{
		DataMapTypeDef::iterator iter = m_dataMap.find(key);
		if(iter == m_dataMap.end())
			return false;

		if(iter->second.use_count()<=1)
		{
			m_dataMap.erase(iter);
			return true;
		}

		return false;
	}

	unsigned int size() const
	{
		return m_dataMap.size();
	}

	void traverse(void (*func)(const KeyType& key, const DataType& data, unsigned int refCount)) const
	{
		for(auto iter = m_dataMap.begin(); iter!=m_dataMap.end(); iter++)
		{
			func(iter->first, *(iter->second.lock()), iter->second.use_count());
		}
	}

	private:
	DataMapTypeDef m_dataMap;
  };
