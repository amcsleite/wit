#pragma once
#include "Counted.h"
#include "ImplList.h"

template <class B> class Stortable : public B
{
public:

	B* findInsertion(B* pNew)
	{
	   for (B* p = head(); p; p = p->next())
	   {
		   if (p->getValue() <= pNew->getValue())
			   return p;
	   }

	   return NULL;
	}
	
    K* head()
    {
#ifdef _DEBUG
        K* p = dynamic_cast<K*> (m_pFirst);
#else
        K* p = static_cast<K*> (m_pFirst);
#endif

        return p;
    }

    K* tail()
    {
#ifdef _DEBUG
        K* p = dynamic_cast<K*> (m_pLast);
#else
        K* p = static_cast<K*> (m_pLast);
#endif

        return p;
    }

    K* next()
    {
#ifdef _DEBUG
        K* p = dynamic_cast<K*> (m_pNext);
#else
        K* p = static_cast<K*> (m_pNext);
#endif

        return p;
    }

    K* previous()
    {
#ifdef _DEBUG
        K* p = dynamic_cast<K*> (m_pPrev);
#else
        K* p = static_cast<K*> (m_pPrev);
#endif

        return p;
    }

    K* parent()
    {
#ifdef _DEBUG
        K* p = dynamic_cast<K*> (m_pParent);
#else
        K* p = static_cast<K*> (m_pParent);
#endif

        return p;
    }
};

// eof
