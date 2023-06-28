/****************************************************
 * Simple log file rotation
 * file should have an extension (a dot in the file name somewhere)
 * a number between 1 and 9 will be inserted before the dot (separated by a dot)
 * i.e.  logfile.txt will become logfile.1.txt ...
 * don't try to keep/rotate more than 9 versions - the code will fail!
 */
// #define debug_out(...) Serial.printf(__VA_ARGS__)
// no debug messages:
#define debug_out(...)
#define _ROTATE_MAX_ 9    // you have to adjust the code if > 9

/****************************************
 * find char c from the right in str
 ****************************************/
int rfind(const char* str, char c) {
  int _nLen = strlen(str)-1;
  while (_nLen >= 0 && c != str[_nLen]) _nLen--;
  return (_nLen);
}

/****************************************
 * recursive call if more than 1 version
 * will rename log file or (if already exists)
 * try to rename next version
 * 1 = latest version (youngest file)
 ****************************************/
void renameOrNext(const char* fOld, const char* fNew, int counterPos, uint8_t maxFiles) {
  int _nLen = strlen(fNew);
  int _counter = fNew[counterPos]-'0';

  debug_out("calling renameOrNext with old=%s, new=%s, counterpos=%d, maxFiles=%d\n",fOld,fNew,counterPos,maxFiles);
  if (_counter >= maxFiles) {
    if (LittleFS.exists(fNew)) {
      debug_out(PSTR("Das Maximum an files ist erreicht - letzte wird gelöscht (maxFiles==%d)\n"), maxFiles);
      LittleFS.remove(fNew);
    }
  } else {
    if (LittleFS.exists(fNew)) {
      char fNewer[strlen(fNew)+1];
      strncpy(&fNewer[0], fNew, strlen(fNew)+1);
      fNewer[counterPos]++;
      renameOrNext(fNew, &fNewer[0], counterPos, maxFiles);
    }
  }
  debug_out(PSTR("Datei '%s' wird umbenannt in '%s'...\n"), fOld, fNew);
  if (_counter <= maxFiles) LittleFS.rename(fOld, fNew);
}

void rotateFile(const char* path, uint8_t maxFiles = _ROTATE_MAX_) {
  int _pathLen = strlen(path);
  char _newName[_pathLen + 2];
  int _nPos = rfind(path,'.');

  debug_out(PSTR("File to rotate: %s, maxFiles == %d\n"), path, maxFiles);
  assert(maxFiles <= 9);  // to be sure...
  if (_nPos > 0) {
    strncpy(&_newName[0], path, _nPos+1);   // incl. Punkt
    _newName[_nPos+1] = '1';  // counterPos
    strncpy(&_newName[_nPos+2],path+_nPos,_pathLen-_nPos);
    _newName[_pathLen+2] = '\0';
    renameOrNext(path, &_newName[0], _nPos+1, maxFiles);
  } else debug_out(PSTR("[rotateFile] FEHLER: ein Punkt konnte nicht gefunden werden - keine Endung?\n"));
  debug_out("leaving rotateFile()\n");
  return;
}
