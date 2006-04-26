/* 
   Copyright (C) 2005 Tommi Maekitalo

This file is part of tntdb.

Tntdb is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Tntdb is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with tntdb; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef TNTDB_SMARTPTR_H
#define TNTDB_SMARTPTR_H

namespace tntdb
{
  template <typename objectType>
  class RefLinked
  {
      mutable const RefLinked* prev;
      mutable const RefLinked* next;

    protected:
      void unlink(objectType* object)
      {
        if (object)
        {
          if (next == this)
          {
            delete object;
            object = 0;
          }
          else
          {
            next->prev = prev;
            prev->next = next;
          }
          next = prev = this;
        }
      }

      void link(const RefLinked& ptr, objectType* object)
      {
        prev = &ptr;
        next = ptr.next;
        prev->next = this;
        next->prev = this;
      }
  };

  template <typename objectType>
  class InternalRefCounted
  {
    protected:
      void unlink(objectType* object)
      {
        if (object)
          object->release();
      }

      void link(const InternalRefCounted& ptr, objectType* object)
      {
        if (object)
          object->addRef();
      }

  };

  template <typename objectType,
            template <class> class ownershipPolicy = InternalRefCounted>
  class SmartPtr : public ownershipPolicy<objectType>
  {
      objectType* object;

    public:
      SmartPtr()
        : object(0)
        {}
      SmartPtr(objectType* ptr)
        : object(ptr)
        { link(*this, ptr); }
      SmartPtr(const SmartPtr& ptr)
        : object(ptr.object)
        { link(ptr, ptr.object); }
      ~SmartPtr()
        { unlink(object); }

      SmartPtr& operator= (const SmartPtr& ptr)
      {
        unlink(object);
        object = ptr.object;
        link(ptr, object);
        return *this;
      }

      /// The object can be dereferenced like the held object
      objectType* operator->() const              { return object; }
      /// The object can be dereferenced like the held object
      objectType& operator*() const               { return *object; }

      bool operator== (const objectType* p) const { return object == p; }
      bool operator!= (const objectType* p) const { return object != p; }
      bool operator< (const objectType* p) const  { return object < p; }
      bool operator! () const { return object == 0; }
      operator bool () const  { return object != 0; }

      objectType* getPointer()              { return object; }
      const objectType* getPointer() const  { return object; }
      operator objectType* ()               { return object; }
      operator const objectType* () const   { return object; }
  };

}

#endif // TNTDB_SMARTPTR_H
