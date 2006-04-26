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

#ifndef TNTDB_IFACE_ICURSOR_H
#define TNTDB_IFACE_ICURSOR_H

#include <tntdb/refcounted.h>
#include <tntdb/noncopyable.h>

namespace tntdb
{
  class Row;

  class ICursor : public RefCounted, private NonCopyable
  {
    public:
      virtual Row fetch() = 0;
  };
}

#endif // TNTDB_IFACE_ICURSOR_H
