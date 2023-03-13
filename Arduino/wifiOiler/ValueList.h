/************************************************************************
 * wifiOiler, an automatic distance depending motorbike chain oiler
 * Copyright (C) 2019-2023, volw
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ************************************************************************/
#define _LIST_LEN_ 22

class ValueList {
  
  struct Item {
    char Name[4];
    bool isSet = false;
    bool get() { return isSet; }
  };

  struct Item ValueList[_LIST_LEN_] = {
    { "apn", false },{ "app", false },{ "bac", false },{ "wts", false },{ "ffn", false },
    { "fpw", false },{ "gdl", false },{ "glf", false },{ "gts", false },{ "lgf", false },
    { "lgs", false },{ "nmm", false },{ "omm", false },{ "otk", false },{ "pac", false },
    { "rmm", false },{ "sim", false },{ "uhn", false },{ "uhp", false },{ "url", false },
    { "use", false },{ "wso", false }
  };

  public:
    void setAll(bool _value) {
      for (uint8_t i = 0; i < _LIST_LEN_; i++) {
        ValueList[i].isSet = _value;
      }
    }
    bool set(const char * name, bool value) {
      uint8_t i = 0;
      while (i < _LIST_LEN_ && strcmp(ValueList[i].Name, name) != 0) i++;
      if (i >= _LIST_LEN_) return false;
      ValueList[i].isSet = value;
      return true;
    }

    bool getFirst(bool _value, char **foundItem) {
      uint8_t i = 0;
      while (i < _LIST_LEN_ && ValueList[i].isSet != _value) i++;
      bool result = (i < _LIST_LEN_);
      if (result) *foundItem = ValueList[i].Name;
      return result;
    }
} static valueList;
