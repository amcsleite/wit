#pragma once

#include "Essential.h"

// primes --> 527507, 2500009, 5450021, 10000019

#define QLSTATS

template <class K, class X, N32 nSize = 527507> class XHash : public Counted
{
   X* m_table[nSize];
   X* pRemovedNode; // used to mark as removed

#ifdef QLSTATS
   int m_nTotalLookups;
   int m_nTotalLookupIterations;
#endif

public:

   XHash()
   {
      pRemovedNode = new X;
      memset(m_table, 0, sizeof(m_table));

#ifdef QLSTATS
      m_nTotalLookups = 0;
      m_nTotalLookupIterations = 0;
#endif
   }

   ~XHash()
   {
      delete pRemovedNode;
   }

   bool isFree(X** pPos)
   {
      return *pPos == NULL || *pPos == pRemovedNode;
   }

   X* hLookup(K* p)
   {
#ifdef QLSTATS
      m_nTotalLookups ++;
#endif
      int nPos = p->getHashValue() % nSize;

      int n = 0;

      for (; m_table[nPos];)
      {
         if (m_table[nPos] != pRemovedNode && p->isTheSame(m_table[nPos]))
         {
#ifdef QLSTATS
            m_nTotalLookupIterations += n;
#endif
            return m_table[nPos];
         }

         ++n;
         nPos += n * n;
         nPos %= nSize;
      }

#ifdef QLSTATS
      m_nTotalLookupIterations += n;
#endif

      return NULL;
   }

   X** hAt(K* p)
   {
      int nPos = p->getHashValue() % nSize;
      int nFirstRemovedPos = -1;

#ifdef QLSTATS
      m_nTotalLookups ++;
#endif

      int n = 0;

      for (; m_table[nPos];)
      {
         if (m_table[nPos] != pRemovedNode && p->isTheSame(m_table[nPos]))
         {
#ifdef QLSTATS
            //p->DebugCollisions(nPos, n, (Node*) m_table[nPos]);
            m_nTotalLookupIterations += n;
#endif

            return &m_table[nPos];
         }

         if (nFirstRemovedPos == -1 && m_table[nPos] == pRemovedNode)
            nFirstRemovedPos = nPos;

         ++n;
         nPos += n * n;
         nPos %= nSize;

         if (n > nSize / 2)
            return NULL;
      }

#ifdef QLSTATS
      m_nTotalLookupIterations += n;
      //p->DebugCollisions(nPos, n, (Node*) m_table[nPos]);
#endif

      if (nFirstRemovedPos >= 0)
         return &m_table[nFirstRemovedPos];

      return &m_table[nPos];
   }

   X* hRemove(const K* p)
   {
      int nPos = p->getHashValue() % nSize;

      X* pRemoved = NULL;

      for (int n = 0; m_table[nPos];)
      {
         if (m_table[nPos] != pRemovedNode && p->isTheSame(m_table[nPos]))
         {
            pRemoved = m_table[nPos];

            m_table[nPos] = pRemovedNode;
            return pRemoved;
         }

         ++n;
         nPos += n * n;
         nPos %= nSize;
      }

      return pRemoved;
   }

   #ifdef QLSTATS
   void count(int& nTotalSize, int& nTotal, int& nRemoved, void** pSamples)
   {
      nTotalSize = nSize;
      nTotal = nRemoved = 0;

      for(int n = 0; n < nSize; ++n)
      {
         if (m_table[n] == pRemovedNode)
            nRemoved++;
         else if (m_table[n])
         {
            if (nTotal < 100)
               pSamples[nTotal] = m_table[n];

            nTotal++;
         }
      }
   }
   #endif
};

//eof
