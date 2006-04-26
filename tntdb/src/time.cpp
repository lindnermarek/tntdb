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

#include <tntdb/time.h>
#include <tntdb/error.h>

namespace tntdb
{
  namespace
  {
    inline bool isdigit(char ch)
    { return ch >= '0' && ch <= '9'; }
  }

  std::string Time::getIso() const
  {
    // format hh:mm:ss.sssss
    //        0....+....1....+
    char ret[14];
    ret[0] = '0' + hour / 10;
    ret[1] = '0' + hour % 10;
    ret[2] = ':';
    ret[3] = '0' + minute / 10;
    ret[4] = '0' + minute % 10;
    ret[5] = ':';
    ret[6] = '0' + second / 10;
    ret[7] = '0' + second % 10;
    ret[8] = '.';
    unsigned short n = millis;
    ret[11] = '0' + n % 10;
    n /= 10;
    ret[10] = '0' + n % 10;
    n /= 10;
    ret[9] = '0' + n % 10;
    ret[12] = '0';
    ret[13] = '0';

    return std::string(ret, 14);
  }

  inline unsigned short getNumber2(const char* s)
  {
    if (!isdigit(s[0])
      || !isdigit(s[1]))
      throw TypeError();
    return (s[0] - '0') * 10
         + (s[1] - '0');
  }

  inline unsigned short getNumber3(const char* s)
  {
    if (!isdigit(s[0])
      || !isdigit(s[1])
      || !isdigit(s[2]))
      throw TypeError();
    return (s[0] - '0') * 100
         + (s[1] - '0') * 10
         + (s[2] - '0');
  }

  Time Time::fromIso(const std::string& s)
  {
    if (s.size() < 11
      || s.at(2) != ':'
      || s.at(5) != ':'
      || s.at(8) != '.')
      throw TypeError();
    const char* d = s.data();
    return Time(getNumber2(d), getNumber2(d + 3), getNumber2(d + 6),
                getNumber3(d + 9));
  }

}