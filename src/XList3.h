#pragma once
#include "Counted.h"
#include "ImplList.h"

#ifdef _DEBUG
//#define _DEBUG_LISTS
#endif

template <class K, class P, class C> class XList3 : public ImplList
{
public:
    C* head()
    {
#ifdef _DEBUG_LISTS
        C* p = dynamic_cast<C*> (m_pFirst);
#else
        C* p = static_cast<C*> (m_pFirst);
#endif

        return p;
    }

    C* tail()
    {
#ifdef _DEBUG_LISTS
        C* p = dynamic_cast<C*> (m_pLast);
#else
        C* p = static_cast<C*> (m_pLast);
#endif

        return p;
    }

    K* next()
    {
#ifdef _DEBUG_LISTS
        K* p = dynamic_cast<K*> (m_pNext);
#else
        K* p = static_cast<K*> (m_pNext);
#endif

        return p;
    }

    K* previous()
    {
#ifdef _DEBUG_LISTS
        K* p = dynamic_cast<K*> (m_pPrev);
#else
        K* p = static_cast<K*> (m_pPrev);
#endif

        return p;
    }

    P* parent()
    {
#ifdef _DEBUG_LISTS
        P* p = dynamic_cast<P*> (m_pParent);
#else
        P* p = static_cast<P*> (m_pParent);
#endif

        return p;
    }

    void pushBack(C* pX)
	{
	   insertNodeInList(NULL, pX, false);
	}

	void pushFront(C* pX)
	{
	   insertNodeInList(NULL, pX, true);
	}
};

// eof
