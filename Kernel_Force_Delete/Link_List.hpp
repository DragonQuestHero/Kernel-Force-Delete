#include "CRTCPP.hpp"

template <typename T>
class Link_List
{
public:
	Link_List()
	{
		KeInitializeSpinLock(&_Lock);
		InitializeListHead(&_List);
	}
	~Link_List()
	{
		
	}
public:
	typedef void(*_Iterate)(T);
	typedef bool(*_Iterate_Remove)(T);
	typedef bool(*_Iterate_Remove_Pointer)(T*);
public:
	LIST_ENTRY* Get_List()
	{
		return &_List;
	}

	void Set_List(LIST_ENTRY* list)
	{
		_List = list;
	}
private:
	struct LINK_LIST
	{
		LIST_ENTRY _List_Entry;
		T _Node;
	};
public:
	void Push(T t)
	{
		LINK_LIST *temp_t = new LINK_LIST();
		temp_t->_Node = t;
		ExInterlockedInsertHeadList(&_List, &temp_t->_List_Entry, &_Lock);
	}
public:
	//return a point.need delete
	/*T* Pop_New()
	{
	LINK_LIST *temp_t = ExInterlockedRemoveHeadList(&_List, &_Lock);
	T *p = new char[temp_t->_Size];
	RtlCopyMemory(p, temp_t->_Node, temp_t->_Size);
	delete temp_t;
	return p;
	}*/

	T Pop()
	{
		LIST_ENTRY *list_point = ExInterlockedRemoveHeadList(&_List, &_Lock);
		LINK_LIST *temp_t = CONTAINING_RECORD(list_point, LINK_LIST, _List_Entry);
		T t = temp_t->_Node;
		delete temp_t;
		return t;
	}
public:
	size_t Get_Size()
	{
		int index = 0;
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			index++;
		}
		return index;
	}
public:
	void Iterate(_Iterate func)
	{
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			LINK_LIST *pmd = CONTAINING_RECORD(p, LINK_LIST, _List_Entry);
			T t = pmd->_Node;
			func(t);
		}
	}

	bool Search(T t)
	{
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			LINK_LIST *pmd = CONTAINING_RECORD(p, LINK_LIST, _List_Entry);
			if (RtlCompareMemory(&pmd->_Node, &t, sizeof(T)) == sizeof(T))
			{
				return true;
			}
		}
		return false;
	}

	template <typename T2>
	T Get(T2 t2, size_t offset, size_t size = 0)
	{
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			LINK_LIST *pmd = CONTAINING_RECORD(p, LINK_LIST, _List_Entry);
			if (RtlCompareMemory(&pmd->_Node + offset, &t2, size) == size)
			{
				return pmd->_Node;
			}
		}
	}
public:
	bool Remove(T t)
	{
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			LINK_LIST *pmd = CONTAINING_RECORD(p, LINK_LIST, _List_Entry);
			if (RtlCompareMemory(&pmd->_Node, &t, sizeof(T)) == sizeof(T))
			{
				RemoveEntryList(&pmd->_List_Entry);
				delete pmd;
				return true;
			}
		}
		return false;
	}

	bool Remove(_Iterate_Remove func)
	{
		for (auto p = _List.Flink; p != &_List; p = p->Flink)
		{
			LINK_LIST *pmd = CONTAINING_RECORD(p, LINK_LIST, _List_Entry);
			if (func(pmd->_Node))
			{
				RemoveEntryList(&pmd->_List_Entry);
				delete pmd;
				return true;
			}
		}
		return false;
	}
public:
	void Clear()
	{
		while (!IsListEmpty(&_List))
		{
			ExInterlockedRemoveHeadList(&_List, &_Lock);
		}
	}

private:
	LIST_ENTRY _List;
	KSPIN_LOCK _Lock;
};

