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

#ifndef TNTDB_SQLITE_IMPL_CONNECTION_H
#define TNTDB_SQLITE_IMPL_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <sqlite3.h>

namespace tntdb
{
  namespace sqlite
  {
    class Connection : public IStmtCacheConnection
    {
        sqlite3* db;

      public:
        explicit Connection(const char* conninfo);
        ~Connection();

        void beginTransaction();
        void commitTransaction();
        void rollbackTransaction();

        size_type execute(const std::string& query);
        tntdb::Result select(const std::string& query);
        tntdb::Row selectRow(const std::string& query);
        tntdb::Value selectValue(const std::string& query);
        tntdb::Statement prepare(const std::string& query);

        sqlite3* getSqlite3() const  { return db; }
    };
  }
}

#endif // TNTDB_SQLITE_IMPL_CONNECTION_H
