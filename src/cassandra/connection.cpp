/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#include <tntdb/cassandra/impl/connection.h>
//#include <tntdb/cassandra/impl/statement.h>
#include <tntdb/impl/row.h>
#include <tntdb/impl/value.h>
#include <tntdb/impl/result.h>
#include <tntdb/cassandra/error.h>
#include <tntdb/bits/result.h>
#include <tntdb/bits/row.h>
#include <tntdb/bits/value.h>
#include <tntdb/bits/statement.h>
#include <cxxtools/log.h>

log_define("tntdb.cassandra.connection")

namespace tntdb
{
  namespace cassandra
  {
    Connection::Connection(const char* conninfo)
    {

//      CassFuture* connect_future = NULL;
//      CassCluster* cluster = cass_cluster_new();
//      cass_cluster_set_contact_points(cluster, "127.0.0.1,127.0.0.2,127.0.0.3");

/*
      log_debug("cassandra_open(\"" << conninfo << "\")");
      int errcode = ::cassandra_open(conninfo, &db);

      if (db == 0)
        throw Execerror("cassandra_open", db, errcode);

      log_debug("cassandra = " << db);

      log_debug("cassandra_busy_timeout(\"" << db << "\", 60000)");
      errcode = ::cassandra_busy_timeout(db, 60000);
      if (errcode != CASS)
        throw Execerror("cassandra_busy_timeout", db, errcode);
    */
    }

    Connection::~Connection()
    {
      /*
      if (db)
      {
        clearStatementCache();

        log_debug("cassandra_close(" << db << ")");
        ::cassandra_close(db);
      }*/
    }
  }
}
