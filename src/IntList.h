#pragma once
#include "Essential.h"
#include "Lists.h"

class IntList : public XList3<IntList, IntList, IntList>
{
	const static N32 NotSet = -1;
	
	N32 _item;
	
public:

	IntList()
	{
		_item = NotSet;
	}
	
	IntList(N32 item)
	{
		_item = item;
	}

	void set(N32 item)
	{
		_item = item;
	}

	N32 get()
	{
		return _item;
	}
	
	void print();
};

typedef SmartPtr<IntList> IntListPtr;

// eof
