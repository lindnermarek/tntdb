/*
 * Copyright (C) 2014 Andreas Langer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef TNTDB_CASSANDRA_IMPL_CONNECTION_H
#define TNTDB_CASSANDRA_IMPL_CONNECTION_H

#include <tntdb/iface/iconnection.h>
#include <cassandra.h>

namespace tntdb
{
  /**

   This namespace contains the implementation of the Sqlite driver for tntdb.

   The driver makes it possible to access a Sqlite3 database using tntdb.

   Note that only sqlite api version 3 is supported.

   To get a connection to a Sqlite3 file, the dburl to the tntdb::connect
   function must start with "sqlite:". The remaining string is passed to the
   `sqlite3_open` function of sqlite.

   A typical connection with a Sqlite3 driver looks like that:

   @code
     tntdb::Connection conn = tntdb::connect("sqlite:mydatabase.db");
   @endcode

   */

  namespace cassandra
  {
    /// Implements a connection to a Sqlite3 database file.
    class Connection : public IStmtCacheConnection
    {

      public:
        explicit Connection(const char* conninfo);
        ~Connection();
/*
        void beginTransaction();
        void commitTransaction();
        void rollbackTransaction();

        size_type execute(const std::string& query);
        tntdb::Result select(const std::string& query);
        tntdb::Row selectRow(const std::string& query);
        tntdb::Value selectValue(const std::string& query);
        tntdb::Statement prepare(const std::string& query);
        bool ping();
        void lockTable(const std::string& tablename, bool exclusive);
*/
      private:
        static CassCluster* _cluster;
        static CassSession* _session;
        static CassFuture* _close_future;
    };
  }
}

#endif // TNTDB_CASSANDRA_IMPL_CONNECTION_H

