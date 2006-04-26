/* 
   Copyright (C) 2006 Tommi Maekitalo

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

#include <tntdb/connectionpool.h>
#include <tntdb/connect.h>
#include <tntdb/impl/poolconnection.h>
#include <cxxtools/log.h>

log_define("tntdb.connectionpool");

namespace tntdb
{
  Connection* ConnectionPool::Connector::operator() ()
  {
    return new Connection(tntdb::connect(url));
  }

  ConnectionPool::~ConnectionPool()
  {
    cxxtools::MutexLock lock(mutex);
    for (PoolsType::iterator it = pools.begin(); it != pools.end(); ++it)
      delete it->second;
  }

  Connection ConnectionPool::connect(const std::string& url)
  {
    log_debug("ConnectionPool::connect(\"" << url << "\")");

    PoolsType::iterator it;

    {
      cxxtools::MutexLock lock(mutex);
      it = pools.find(url);
      if (it == pools.end())
      {
        log_debug("create pool for url \"" << url << "\"");
        PoolType* pool = new PoolType(maxcount, Connector(url));
        it = pools.insert(PoolsType::value_type(url, pool)).first;
      }
      else
        //log_debug("pool for url \"" << url << "\" found; current size=" << it->second->getCurrentSize());
        log_debug("pool for url \"" << url << "\" found");
    }

    log_debug("current pool-size " << it->second->getCurrentSize());

    return Connection(new PoolConnection(it->second->get()));
  }

  void ConnectionPool::drop(unsigned keep)
  {
    log_debug("drop(" << keep << ')');

    cxxtools::MutexLock lock(mutex);
    for (PoolsType::iterator it = pools.begin(); it != pools.end(); ++it)
    {
      log_debug("pool \"" << it->first << "\"; current size " << it->second->getCurrentSize());
      it->second->drop();
      log_debug("connections released " << it->second->getCurrentSize() << " kept");
    }
  }

  void ConnectionPool::drop(const std::string& url, unsigned keep)
  {
    log_debug("drop(\"" << url << "\", " << keep << ')');

    cxxtools::MutexLock lock(mutex);

    PoolsType::iterator it = pools.find(url);
    if (it != pools.end())
    {
      log_debug("pool \"" << url << "\" found; current size " << it->second->getCurrentSize());
      it->second->drop(keep);
      log_debug("connections released " << it->second->getCurrentSize() << " kept");

      if (it->second->getCurrentSize() == 0)
      {
        log_debug("delete connectionpool for \"" << url << "\"");
        delete it->second;
        pools.erase(it);
      }
    }
    else
      log_debug("pool \"" << url << "\" not found");
  }

  unsigned ConnectionPool::getCurrentSize(const std::string& url) const
  {
    cxxtools::MutexLock lock(mutex);

    PoolsType::const_iterator it = pools.find(url);
    return it == pools.end() ? 0
                             : it->second->getCurrentSize();
  }
}