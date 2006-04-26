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

#include <exception>
#include <iostream>
#include <algorithm>

#include <cxxtools/loginit.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

#include <tntdb/connect.h>
#include <tntdb/transaction.h>
#include <tntdb/value.h>
#include <tntdb/row.h>
#include <tntdb/result.h>
#include <tntdb/statement.h>

#include "util.h"

log_define("sqlcmd.main");

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> value(argc, argv, 'v');
    cxxtools::Arg<bool> row(argc, argv, 'r');
    cxxtools::Arg<bool> exec(argc, argv, 'e');
    cxxtools::Arg<bool> statement(argc, argv, 'p');
    cxxtools::Arg<bool> cursor(argc, argv, 'c');

    cxxtools::Arg<bool> debug(argc, argv, 'd');
    if (debug)
      log_init_debug();
    else
      log_init();

    if (argc < 2)
    {
      std::cerr << "This is sqlcmd - a commandline-sql-processor. It demonstrates the use of tntdb.\n\n"
                   "Usage:\n"
                   "  sqlcmd [options] db-url [sql-query]\n\n"
                   "Options:\n"
                   "  -e        execute statement without result\n"
                   "  -v        select value\n"
                   "  -r        select row\n"
                   "  -p        select with prepared statement\n"
                   "  -c        select with cursor\n"
                   "  -d        enable debug\n\n"
                   "When no sql-query is passed, sqlcmd reads a sql-query from standard-input.\n"
                << std::endl;
      return -1;
    }

    const std::string database = argv[1];
    std::string cmd;
    if (argc > 2)
      cmd = argv[2];
    else
    {
      // read sql-command from stdin
      std::ostringstream s;
      s << std::cin.rdbuf();   // copy stdin to buffer
      cmd = s.str();
    }

    tntdb::Connection conn = tntdb::connect(database);

    if (exec)
    {
      std::cout << "exec" << std::endl;
      conn.execute(cmd);
    }
    else if (value)
    {
      tntdb::Value v = conn.selectValue(cmd);
      ValuePrinter()(v);
      std::cout << std::endl;
    }
    else if (row)
    {
      tntdb::Row t = conn.selectRow(cmd);
      RowPrinter()(t);
    }
    else if (statement)
    {
      tntdb::Statement stmt = conn.prepare(cmd);
      tntdb::Result r = stmt.select();
      std::for_each(r.begin(), r.end(), RowPrinter());
    }
    else if (cursor)
    {
      // postgresql needs a active transaction for cursors
      tntdb::Transaction trans(conn);

      tntdb::Statement stmt = conn.prepare(cmd);
      std::for_each(stmt.begin(), stmt.end(), RowPrinter());
    }
    else
    {
      tntdb::Result r = conn.select(cmd);
      std::for_each(r.begin(), r.end(), RowPrinter());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << "ERROR: " << e.what() << std::endl;
  }
}
