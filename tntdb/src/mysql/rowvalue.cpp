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

#include <tntdb/mysql/impl/rowvalue.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <cxxtools/log.h>
#include <sstream>

log_define("tntdb.mysql.rowvalue");

namespace tntdb
{
  namespace mysql
  {
    bool RowValue::isNull() const
    {
      return row[col] == 0;
    }

    bool RowValue::getBool() const
    {
      return row[col][0] == '1'
          || row[col][0] == 'y'
          || row[col][0] == 'Y'
          || row[col][0] == 't'
          || row[col][0] == 'T';
    }

    int RowValue::getInt() const
    {
      int ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    unsigned RowValue::getUnsigned() const
    {
      unsigned ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    float RowValue::getFloat() const
    {
      float ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    double RowValue::getDouble() const
    {
      double ret = 0;
      std::istringstream v(getString());
      v >> ret;
      return ret;
    }

    char RowValue::getChar() const
    {
      return row[col][0];
    }

    std::string RowValue::getString() const
    {
      return std::string(row[col], len);
    }

    Date RowValue::getDate() const
    {
      return Date::fromIso(getString());
    }

    Time RowValue::getTime() const
    {
      return Time::fromIso(getString());
    }

    Datetime RowValue::getDatetime() const
    {
      return Datetime::fromIso(getString());
    }

  }
}