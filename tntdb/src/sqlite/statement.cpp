/* 
 * Copyright (C) 2005 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#include <tntdb/sqlite/impl/statement.h>
#include <tntdb/sqlite/impl/cursor.h>
#include <tntdb/sqlite/impl/connection.h>
#include <tntdb/impl/result.h>
#include <tntdb/impl/row.h>
#include <tntdb/impl/value.h>
#include <tntdb/sqlite/error.h>
#include <tntdb/date.h>
#include <tntdb/time.h>
#include <tntdb/datetime.h>
#include <tntdb/result.h>
#include <tntdb/row.h>
#include <tntdb/value.h>
#include <sstream>
#include <cxxtools/log.h>

log_define("tntdb.sqlite.statement");

namespace tntdb
{
  namespace sqlite
  {
    Statement::Statement(Connection* conn_, const std::string& query_)
      : stmt(0),
        stmtInUse(0),
        conn(conn_),
        query(query_),
        needReset(false)
    {
    }

    Statement::~Statement()
    {
      if (stmt)
      {
        log_debug("sqlite3_finalize(" << stmt << ')');
        ::sqlite3_finalize(stmt);
      }

      if (stmtInUse && stmtInUse != stmt)
      {
        log_debug("sqlite3_finalize(" << stmtInUse << ')');
        ::sqlite3_finalize(stmtInUse);
      }
    }

    sqlite3_stmt* Statement::getBindStmt()
    {
      if (stmt == 0)
      {
        // hostvars don't need to be parsed, because sqlite accepts the hostvar-
        // syntax of tntdb (:vvv)

        // prepare statement
        const char* tzTail;
        log_debug("sqlite3_prepare(" << conn->getSqlite3() << ", \"" << query
          << "\", " << &stmt << ", " << &tzTail << ')');
        int ret = ::sqlite3_prepare(conn->getSqlite3(), query.data(), query.size(), &stmt, &tzTail);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_prepare", conn->getSqlite3(), ret);

        log_debug("sqlite3_stmt = " << stmt);

        if (stmtInUse)
        {
          // get bindings from stmtInUse
          log_debug("sqlite3_transfer_bindings(" << stmtInUse << ", " << stmt << ')');
          ret = ::sqlite3_transfer_bindings(stmtInUse, stmt);
          if (ret != SQLITE_OK)
          {
            log_debug("sqlite3_finalize(" << stmt << ')');
            ::sqlite3_finalize(stmt);
            stmt = 0;
            throw Execerror("sqlite3_finalize", stmtInUse, ret);
          }
        }
      }
      else if (needReset)
        reset();

      return stmt;
    }

    void Statement::putback(sqlite3_stmt* stmt_)
    {
      if (stmt == 0)
      {
        stmt = stmt_; // thank you - we can use it
        if (stmtInUse == stmt_)
          stmtInUse = 0; // it is not in use any more
        needReset = true;
      }
      else
      {
        // we have already a new statement-handle - destroy the old one
        log_debug("sqlite3_finalize(" << stmt_ << ')');
        ::sqlite3_finalize(stmt_);
      }
    }

    int Statement::getBindIndex(const std::string& col)
    {
      sqlite3_stmt* stmt = getBindStmt();

      log_debug("sqlite3_bind_parameter_index(" << stmt << ", :" << col  << ')');
      int idx = ::sqlite3_bind_parameter_index(stmt, (':' + col).c_str());
      if (idx == 0)
        log_warn("hostvariable :" << col << " not found");
      return idx;
    }

    void Statement::reset()
    {
      if (stmt)
      {
        if (needReset)
        {
          log_debug("sqlite3_reset(" << stmt << ')');
          int ret = ::sqlite3_reset(stmt);

          if (ret != SQLITE_OK)
            throw Execerror("sqlite3_reset", stmt, ret);

          needReset = false;
        }
      }
      else
        getBindStmt();
    }

    void Statement::clear()
    {
      sqlite3_stmt* stmt = getBindStmt();
      int count = ::sqlite3_bind_parameter_count(stmt);
      for (int i = 0; i < count; ++i)
      {
        int ret = ::sqlite3_bind_null(stmt, i + 1);
        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_null", stmt, ret);
      }
    }

    void Statement::setNull(const std::string& col)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_null(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_null(stmt, idx);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_null", stmt, ret);
      }
    }

    void Statement::setBool(const std::string& col, bool data)
    {
      setInt(col, data ? 1 : 0);
    }

    void Statement::setInt(const std::string& col, int data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_int(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_int(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_int", stmt, ret);
      }
    }

    void Statement::setUnsigned(const std::string& col, unsigned data)
    {
      if (data > static_cast<unsigned>(std::numeric_limits<int>::max()))
      {
        log_warn("possible loss of precision while converting large unsigned " << data
          << " to double");
        setDouble(col, static_cast<double>(data));
      }
      else
        setInt(col, static_cast<int>(data));
    }

    void Statement::setFloat(const std::string& col, float data)
    {
      setDouble(col, static_cast<double>(data));
    }

    void Statement::setDouble(const std::string& col, double data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_double(" << stmt << ", " << idx << ')');
        int ret = ::sqlite3_bind_double(stmt, idx, data);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_double", stmt, ret);
      }
    }

    void Statement::setChar(const std::string& col, char data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_text(" << stmt << ", " << idx << ", " << data
          << ", 1, SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_text(stmt, idx, &data, 1, SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_text", stmt, ret);
      }
    }

    void Statement::setString(const std::string& col, const std::string& data)
    {
      int idx = getBindIndex(col);
      sqlite3_stmt* stmt = getBindStmt();
      if (idx != 0)
      {
        reset();

        log_debug("sqlite3_bind_blob(" << stmt << ", " << idx << ", " << data
          << ", " << data.size() << ", SQLITE_TRANSIENT)");
        int ret = ::sqlite3_bind_blob(stmt, idx, data.c_str(), data.size(), SQLITE_TRANSIENT);

        if (ret != SQLITE_OK)
          throw Execerror("sqlite3_bind_blob", stmt, ret);
      }
    }

    void Statement::setDate(const std::string& col, const Date& data)
    {
      setString(col, data.getIso());
    }

    void Statement::setTime(const std::string& col, const Time& data)
    {
      setString(col, data.getIso());
    }

    void Statement::setDatetime(const std::string& col, const Datetime& data)
    {
      setString(col, data.getIso());
    }

    Statement::size_type Statement::execute()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
        throw Execerror("sqlite3_step", stmt, ret);
      else if (ret != SQLITE_DONE)
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret << " from sqlite3_step";
        throw SqliteError("sqlite3_step", msg.str());
      }

      int n = ::sqlite3_changes(::sqlite3_db_handle(stmt));

      reset();

      return n;
    }

    Result Statement::select()
    {
      reset();
      needReset = true;

      ResultImpl* r = new ResultImpl();
      Result result(r);
      int ret;
      do
      {
        log_debug("sqlite3_step(" << stmt << ')');
        ret = sqlite3_step(stmt);

        if (ret == SQLITE_ERROR)
          throw Execerror("sqlite3_step", stmt, ret);
        else if (ret == SQLITE_ROW)
        {
          log_debug("sqlite3_column_count(" << stmt << ')');
          int count = ::sqlite3_column_count(stmt);
          RowImpl* row = new RowImpl();
          r->add(Row(row));
          for (int i = 0; i < count; ++i)
          {
            log_debug("sqlite3_column_bytes(" << stmt << ", " << i << ')');
            int n = sqlite3_column_bytes(stmt, i);

            log_debug("sqlite3_column_blob(" << stmt << ", " << i << ')');
            const void* txt = sqlite3_column_blob(stmt, i);
            Value v;
            if (txt)
              v = Value(new ValueImpl(
                std::string(static_cast<const char*>(txt), n)));
            row->add(v);
          }
        }
        else if (ret != SQLITE_DONE)
        {
          std::ostringstream msg;
          msg << "unexpected returncode " << ret;
          throw SqliteError("sqlite3_step", msg.str());
        }

      } while (ret == SQLITE_ROW);

      return result;
    }

    Row Statement::selectRow()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
        throw Execerror("sqlite3_step", stmt, ret);
      else if (ret == SQLITE_DONE)
        throw NotFound();
      else if (ret == SQLITE_ROW)
      {
        log_debug("sqlite3_column_count(" << stmt << ')');
        int count = ::sqlite3_column_count(stmt);
        RowImpl* r = new RowImpl();
        Row row(r);
        for (int i = 0; i < count; ++i)
        {
          log_debug("sqlite3_column_bytes(" << stmt << ", " << i << ')');
          int n = sqlite3_column_bytes(stmt, i);

          log_debug("sqlite3_column_blob(" << stmt << ", " << i << ')');
          const void* txt = sqlite3_column_blob(stmt, i);
          Value v;
          if (txt)
            v = Value(new ValueImpl(
              std::string(static_cast<const char*>(txt), n)));
          r->add(v);
        }
        return row;
      }
      else
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret;
        throw SqliteError("sqlite3_step", msg.str());
      }
    }

    Value Statement::selectValue()
    {
      reset();
      needReset = true;

      log_debug("sqlite3_step(" << stmt << ')');
      int ret = sqlite3_step(stmt);

      if (ret == SQLITE_ERROR)
      {
        log_debug("sqlite3_step returned SQLITE_ERROR");
        throw Execerror("sqlite3_step", stmt, ret);
      }
      else if (ret == SQLITE_DONE)
      {
        log_debug("sqlite3_step returned SQLITE_DONE => NotFound");
        throw NotFound();
      }
      else if (ret == SQLITE_ROW)
      {
        log_debug("sqlite3_column_count(" << stmt << ')');
        int count = ::sqlite3_column_count(stmt);
        if (count == 0)
          throw NotFound();

        log_debug("sqlite3_column_bytes(" << stmt << ", 0)");
        int n = sqlite3_column_bytes(stmt, 0);

        log_debug("sqlite3_column_blob(" << stmt << ", 0)");
        const void* txt = sqlite3_column_blob(stmt, 0);
        Value v;
        if (txt)
          v = Value(new ValueImpl(
            std::string(static_cast<const char*>(txt), n)));
        return v;
      }
      else
      {
        std::ostringstream msg;
        msg << "unexpected returncode " << ret;
        throw SqliteError("sqlite3_step", msg.str());
      }
    }

    ICursor* Statement::createCursor(unsigned fetchsize)
    {
      stmtInUse = getBindStmt();
      stmt = 0;
      return new Cursor(this, stmtInUse);
    }

  }
}
