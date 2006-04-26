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

#include <tntdb/mysql/impl/resultrow.h>
#include <tntdb/mysql/impl/rowvalue.h>
#include <tntdb/bits/value.h>
#include <cxxtools/log.h>

log_define("tntdb.mysql.resultrow");

namespace tntdb
{
  namespace mysql
  {
    ResultRow::ResultRow(const tntdb::Result& result_, MYSQL_RES* res,
      MYSQL_ROW row_)
      : result(result_),
        row(row_)
    {
      log_debug("mysql_fetch_lengths");
      lengths = ::mysql_fetch_lengths(res);
    }

    unsigned ResultRow::size() const
    {
      return result.getFieldCount();
    }

    Value ResultRow::getValue(size_type field_num) const
    {
      return Value(new RowValue(result, row, field_num, lengths[field_num]));
    }
  }
}