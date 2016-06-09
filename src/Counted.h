#pragma once

class Counted
{
   mutable N32 _nRefCount;

public:

   Counted() : _nRefCount(0) 
   {
   };
   
   void grabReference() const
   {
        ++_nRefCount;
   };

   void releaseReference() const
   { 
      --_nRefCount;
      
      if (_nRefCount <= 0) 
         delete this; 
  };

   virtual ~Counted() 
   {
   };

};

//eof
