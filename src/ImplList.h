#pragma once
#include "Counted.h"

// must be somewhere in client side
//void* globalAllocatorXListGetBlockGetBlock( size_t stAllocateBlock);
//void  globalAllocatorXListGetBlockFreeBlock( void *pvMem);

class ImplList : public Counted
{
protected:
    ImplList* m_pFirst;
    ImplList* m_pLast;
    ImplList* m_pPrev;
    ImplList* m_pNext;
    ImplList* m_pParent;

public:
    int m_nCount;

    ImplList()
    {
        m_pFirst = m_pLast = m_pPrev = m_pNext = m_pParent = NULL;
        m_nCount = 0;
    }

    ~ImplList()
    {
       emptyList();
    }

    bool isInList(ImplList* pX)
    {
        //debug use only

        if (m_pFirst)
            for(ImplList* pTest = m_pFirst; pTest; pTest = pTest->m_pNext)
            {
                if (pTest == pX)
                    return true;
            }

        return false;
    }

    bool empty()
    {
        return m_pFirst == NULL;
    }

    void implListPushFront(ImplList* pX)
    {
        insertNodeInList(NULL, pX, true);
    }

   void popFront()
    {
        xUnlink(m_pFirst);
    }

    ImplList* head()
    {
#ifdef _DEBUG
        ImplList* p = dynamic_cast<ImplList*> (m_pFirst);
#else
        ImplList* p = static_cast<ImplList*> (m_pFirst);
#endif

        return p;
    }

    ImplList* tail()
    {
        /* #ifdef _DEBUG
         ImplList* p = dynamic_cast<ImplList*> (m_pLast);
         #else
         ImplList* p = static_cast<ImplList*> (m_pLast);
         #endif

         return p;*/
        return m_pLast;
    }

#ifdef _DEBUG
    void checkIntegrityDebug()
    {
        int nControl = 0;

        if (m_pFirst)
        {
            for(ImplList* pTest = m_pFirst; pTest; )
            {
                ++nControl;
                XK(pTest->m_pParent == this);


				ImplList* pNext = pTest->m_pNext;

				if (pNext)
				{
					XK(pNext->m_pPrev == pTest);
				}
				pTest = pNext;
            }

            XK(m_pLast != NULL);
        }

        XK(nControl == m_nCount);
    }
#endif

    int count()
    {
#ifdef _DEBUG
        checkIntegrityDebug();
#endif
        return m_nCount;
    }

    void implListPushBack(ImplList* pX)
    {
        insertNodeInList(NULL, pX, false);
    }

    void popBack()
    {
        xUnlink(m_pLast);
    }

    void insertNodeInList(ImplList* pWhere, ImplList* pX, bool bInsertBeforeNode)
    {
		#ifdef _DEBUG
        checkIntegrityDebug();
		#endif
        //inserts after pWhere node
        ++m_nCount;
        pX->m_pParent = this;

		#ifdef _DEBUG

		#if 1 // is it already in ?
	    XK(!isInList(pX));
		#endif

		#endif

        ImplList* pBefore, *pAfter;

        /*if (bAppend)
        {
           pBefore = m_pLast;
           pAfter = NULL;
        }*/

        if (bInsertBeforeNode)
        {
            pBefore = pWhere ? pWhere->m_pPrev : NULL;
            pAfter = pWhere ? pWhere : m_pFirst;
        }
        else
        {
            pBefore = pWhere ? pWhere : m_pLast;
            pAfter = pWhere ? pWhere->m_pNext : NULL;
        }

        pX->m_pNext = pAfter;
        pX->m_pPrev = pBefore;

        if (pBefore)
            pBefore->m_pNext = pX;
        else
            m_pFirst = pX;

        if (pAfter)
            pAfter->m_pPrev = pX;
        else
            m_pLast = pX;

        pX->grabReference();

#ifdef _DEBUG
        checkIntegrityDebug();
#endif
    }

    void selfUnlink()
	{
		parent()->xUnlink(this);
	}

    void xUnlink(ImplList* pX)
    {
        XK(m_pFirst);
        XK(pX);
        XK(isInList(pX));
        XK(pX->m_pParent);
        XK(pX->m_pParent == this);

        if (pX->m_pParent != this)
            return;

        --m_nCount;

        if(pX->m_pPrev)
            pX->m_pPrev->m_pNext = pX->m_pNext;
        else
            m_pFirst = pX->m_pNext;

        if(pX->m_pNext)
            pX->m_pNext->m_pPrev = pX->m_pPrev;
        else
            m_pLast = pX->m_pPrev;

        pX->m_pPrev = NULL;
        pX->m_pNext = NULL;
        pX->m_pParent = NULL;

        pX->releaseReference();

#ifdef _DEBUG
        checkIntegrityDebug();
#endif
    }

    void emptyList()
    {
#ifdef _DEBUG
        checkIntegrityDebug();
#endif

        ImplList* p = m_pFirst;

        while(p)
        {
            ImplList* pNext = p->m_pNext;
            p->releaseReference();
            p = pNext;
        }

        m_pFirst = NULL;
        m_pLast = NULL;
        m_nCount = 0;
    }

    void joinBack(ImplList* pList) // no copy
    {
        if(!pList->m_pFirst)
            return;
#ifdef _DEBUG
        pList->checkIntegrityDebug();
#endif

        for(ImplList* p = pList->m_pFirst; p ; p = p->m_pNext)
            p->m_pParent = this;

        if (m_pFirst == NULL)
        {
            XK(m_pLast == NULL);
			XK(m_nCount == 0);
            m_pFirst = pList->m_pFirst;
            m_pLast = pList->m_pLast;
            m_nCount = pList->m_nCount;
        }
        else
        {
            XK(m_pLast != NULL);
            XK(m_pLast->m_pNext == NULL);
            XK(pList->m_pLast != NULL);
			XK(m_nCount != 0);
            m_pLast->m_pNext = pList->m_pFirst;
            pList->m_pFirst->m_pPrev = m_pLast;
            m_nCount += pList->m_nCount;
        }

        pList->m_pFirst = NULL;
        pList->m_pLast = NULL;
        pList->m_nCount = 0;

#ifdef _DEBUG
        checkIntegrityDebug();
#endif

    }

    void joinHead(ImplList* pList) // no copy
    {
        if (pList->m_pFirst == NULL)
            return;

        XK(pList->m_pLast);

        for(ImplList* p = pList->m_pFirst; p ; p = p->m_pNext)
            p->m_pParent = this;

        if (m_pFirst == NULL)
        {
            XK(m_pLast == NULL);
            m_pFirst = pList->m_pFirst;
            m_pLast = pList->m_pLast;
            m_nCount = pList->m_nCount;
        }
        else
        {
            XK(m_pLast != NULL);
            pList->m_pLast->m_pNext = m_pFirst;

            m_pFirst->m_pPrev = pList->m_pLast;

            m_pFirst = pList->m_pFirst;
            XK(m_pFirst->m_pPrev == NULL);

            m_nCount += pList->m_nCount;
        }

        pList->m_pFirst = NULL;
        pList->m_pLast = NULL;
        pList->m_nCount = 0;

#ifdef _DEBUG
        checkIntegrityDebug();
#endif
    }

    void popBack(ImplList* pList, int nToPop) // no copy
    {
        int n = 0;

        while(!empty() && n < nToPop)
        {
            ImplList* p = m_pLast;

            if (p == NULL)
                break;

            xUnlink(p);
            pList->insertNodeInList(pList->m_pLast, p, true);
            n++;
        }

#ifdef _DEBUG
        checkIntegrityDebug();
#endif
    }

    void popFront(ImplList* pList, int nToPop) // no copy
    {
        int n = 0;

        while(n < nToPop)
        {
            ImplList* p = m_pFirst;

            if (p == NULL)
                break;

            xUnlink(p);
            pList->insertNodeInList(pList->m_pFirst, p, false);
            n++;
        }

#ifdef _DEBUG
        checkIntegrityDebug();
#endif
    }

    ImplList* Next()
    {
#ifdef _DEBUG
        ImplList* p = dynamic_cast<ImplList*> (m_pNext);
#else
        ImplList* p = static_cast<ImplList*> (m_pNext);
#endif

        return p;
    }

    ImplList* parent()
    {
#ifdef _DEBUG
        ImplList* p = dynamic_cast<ImplList*> (m_pParent);
#else
        ImplList* p = static_cast<ImplList*> (m_pParent);
#endif

        return p;
    }

   /*static void* operator new( size_t stAllocateBlock)
   {
      return globalAllocatorXListGetBlockGetBlock(stAllocateBlock);
   }

   static void operator delete( void *pvMem )
   {
       globalAllocatorXListGetBlockFreeBlock(pvMem);
       }
   */

};
// eof
