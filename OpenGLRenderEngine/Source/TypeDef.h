#pragma once 

#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

typedef void* DataType;

  class AttributeHandle
  {
    public:
	AttributeHandle(void* data) : m_data(data) {}
	template<class T> T* getData() { return (T*)(m_data); }

	protected:
	void* m_data;
  };


  class AttributeListHandle
  {
    public:
	AttributeListHandle(const vector<void*>& data, const vector<unsigned int>& dataArraySize) 
		: m_data(data), m_dataArraySize(dataArraySize) 
	{
	}

	template<class T> T* getData(unsigned int n) { return (T*)(m_data[n]); }

	unsigned int getArraySize(const int n) const { return m_dataArraySize[n]; }
	unsigned int getListCount() const { return m_data.size(); }

	protected:
	vector<unsigned int> m_dataArraySize; 
	vector<void*> m_data;
  };
