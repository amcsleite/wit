#pragma once
#include "Counted.h"

template <class T> class SmartPtr 
{
protected:

   T* _p;

public:
   SmartPtr()
   {
      _p = NULL;
   }
   
   SmartPtr(T* p) : _p(p)
   {
      if (p)
         p->grabReference();
   }

   void zero()
   {
      _p = NULL;
   }

   SmartPtr(const SmartPtr& ptr) : _p(ptr._p) 
   { 
      if (_p) 
         _p->grabReference(); 
   }

   ~SmartPtr(void) 
   {
      if (_p) 
         _p->releaseReference(); 
   }
   
   void New()
   {
      _p = new T;
      _p->grabReference();
   }

   void newFromRoot(N32 nRoot)
   {
      _p = new T;
      _p->grabReference();
   }

   operator T*(void) const   
   {
      return _p; 
   }
   
   T& operator*(void) const  
   {
      return *_p;
   }

   T* operator->(void) const 
   { 
      return _p; 
   }
   
   SmartPtr& operator=(T* p) 
   {
      if (p)
         p->grabReference(); 
      
      if (_p) 
         _p->releaseReference(); 

      _p = p; 
      
      return *this;
   }

   SmartPtr& operator=(const SmartPtr& ptr) 
   {
      if (_p != ptr._p)
      {
         if (_p) 
            _p->releaseReference(); 
       
         _p = ptr._p; 

         if (_p) 
            _p->grabReference(); 
      }
      return *this;
   }
   bool operator==(T* p) const { return _p == p; };
   bool operator!=(T* p) const { return _p != p; };
};

typedef SmartPtr<Counted> CountedPtr;

//eof
