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

#ifndef TNTDB_SQLITE_IMPL_CURSOR_H
#define TNTDB_SQLITE_IMPL_CURSOR_H

#include <tntdb/iface/icursor.h>
#include <tntdb/smartptr.h>
#include <sqlite3.h>

namespace tntdb
{
  namespace sqlite
  {
    class Statement;

    class Cursor : public ICursor
    {
        SmartPtr<Statement, InternalRefCounted> statement;
        sqlite3_stmt* stmt;

      public:
        Cursor(Statement* statement, sqlite3_stmt* stmt);
        ~Cursor();

        // method for ICursor
        Row fetch();

        // specific methods of sqlite-driver
        sqlite3_stmt* getStmt() const   { return stmt; }
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_CURSOR_H
