#define _LIST_LEN_ 23

class ValueList {
  
  struct Item {
    char Name[4];
    bool isSet = false;
    bool get() { return isSet; }
  };

  struct Item ValueList[_LIST_LEN_] = {
    { "apn", false },{ "app", false },{ "bac", false },{ "fbe", false },{ "ffn", false },
    { "fpw", false },{ "gdl", false },{ "glf", false },{ "gts", false },{ "lgf", false },
    { "lgs", false },{ "nmm", false },{ "omm", false },{ "otk", false },{ "pac", false },
    { "rmm", false },{ "sim", false },{ "uhn", false },{ "uhp", false },{ "url", false },
    { "use", false },{ "wso", false },{ "wts", false }
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
