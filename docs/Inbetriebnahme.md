# Inbetriebnahme

Um die Software zu kompilieren und auf die MCU (Micro Controller Unit, NICHT "Marvel Cinematic Universe") zu laden, sind folgende Schritte notwendig. Eventuell könnt ihr einige Schritte überspringen, je nachdem, ob ihr schon mal mit einem D1 Mini oder einem anderen Microcontroller gearbeitet habt, der auf einem ESP8266 (ursprünglich von der Firma espressif) basiert.

## Arduino Entwicklungsumgebung

Die Entwicklungsumgebung wird ab jetzt der Einfachheit halber IDE genannt. Hier wird die Installation und Inbetriebnahme der portablen Version beschrieben. Benutzt man die für Windows vorgesehene Installationsversion sind ggf. andere Verzeichnisse wichtig, die hier nicht beschrieben werden.

- IDE herunterladen und dabei Windows ZIP file wählen ([Link](https://www.arduino.cc/en/software)) 
- den in der Zip-Datei enthaltenen Ordner an einen sicheren Platz kopieren, mein Beispiel: "F:\apps". Nach dem entpacken sollte es also einen Ordner "F:\apps\arduino-1.8.13" geben (bzw. mit einer höheren Versionsnummer).
- In diesem Verzeichnis, neben den anderen bereits enthaltenen Ordnern, einen Ordner mit Namen "portable" anlegen. Nach unserem Beispiel also "F:\apps\arduino-1.8.13\portable". Damit wird der IDE gesagt, dass es sich um eine sogenannte portable Installation handelt. 
- Die im Verzeichnis "F:\apps\arduino-1.8.13" enthaltene arduino.exe starten. Eventuell kommt eine Meldung der Windows Firewall, dass "einige Features dieser App blockiert wurden". Unter Pfad wird der Windows Java Interpreter angegeben, der mit der Arduino IDE gekommen ist (nach unserem Beispiel: "F:\apps\arduino-1.8.13\java\bin\javaw.exe"). Diese Meldung kann mit "Zugriff zulassen" beantwortet werden.
- Um die Arduino IDE für die Programmierung von ESP8266 basierten MCUs vorzubereiten, starten wir in der IDE den Einstellungsdialog, im Menü zu finden unter "Datei" -> "Voreinstellungen". Im unteren Teil des Fensters findet sich ein Eingabefeld "Zusätzliche Boardverwalter-URLs:". Hier wird folgender Link eingetragen: https://arduino.esp8266.com/stable/package_esp8266com_index.json. Dann den Dialog mit "OK" bestätigen.
- Dann die Boardverwaltung aufrufen, zu finden im Menü unter "Werkzeuge" -> "Board: ..." -> "Boardverwalter". Im oberen Eingabefeld "ESP8266" eingeben. Es sollte ein Treffer "esp8266" und darunter "by ESP8266 Community" angezeigt werden. Diesen Treffer anwählen, bzw. "Installieren" wählen. Die Installation kann ein wenig dauern. Wenn man danach einen Blick in das "portable" (s.o.) Verzeichnis wirft, sollten dort einige Unterverzeichnisse und Dateien zu finden sein, wie "sketchbook" oder "packages".

#### LittleFS Uploader

Um ein LittleFS Dateisystem auf der MCU einzurichten und zu nutzen, bzw. Dateien dorthin hochzuladen, kann ein Upload Tool in die Arduino IDE integriert werden. Es lädt auf Knopfdruck alle Dateien hoch, die sich im Unterverzeichnis "data" eines Sketches befinden.

- Im Verzeichnis "sketchbook" ein Unterverzeichnis mit Namen "tools" anlegen (nach unserem Beispiel: "F:\apps\arduino-1.8.13\sketchbook\tools").
- Von der github Seite [LittleFS Upload Tool](https://github.com/earlephilhower/arduino-esp8266littlefs-plugin/releases) das aktuellste Release herunterladen und das in der Zip-Datei enthaltene Verzeichnis in das tools Verzeichnis kopieren. Das Ergebnis sollte nach unserem Beispiel etwa so aussehen: "F:\apps\arduino-1.8.13\portable\sketchbook\tools\ESP8266LittleFS\tool". In diesem Verzeichnis befindet sich das eigentliche tool "esp8266littlefs.jar" (Stand Februar 2021).
- Nun die Arduino IDE beenden und wieder starten. Im Menü sollte der Eintrag "Werkzeuge" -> "ESP8266 LittleFS Data Upload" zu finden sein.

#### Benötigte Bibliotheken installieren

Um die Öler Software erfolgreich zu kompilieren, benötigen wir noch einige Bibliotheken:

- Adafruit GFX Library: in der Arduino IDE über das Menü "Werkzeuge" -> "Bibliotheken verwalten" suchen nach "adafruit_gfx" und diese installieren. Erscheint eine Meldung, dass noch andere abhängige Bibliotheken existieren ("Would you like to install also all the missing dependencies?)", bitte "Install all" wählen.
- TinyGPS++ Library: diese Lib ist aktuell nicht über die Bibliotheksverwaltung installierbar. Herunterladen kann man sie von der [github Seite von Mikal Hart](https://github.com/mikalhart/TinyGPSPlus) Habt ihr die Zip-Datei heruntergeladen, extrahiert ihr den enthaltenen Ordner in das "portable\sketchbook\libraries" Verzeichnis. Danach müsste das nach unserem Beispiel so aussehen: "F:\apps\arduino-1.8.13\portabe\sketchbook\libraries\TinyGPSPlus-master" (das "-master", welches von github angehängt wird und den branch bezeichnet, bitte entfernen).
- Wird die IDE nun beendet und wieder gestartet, können im Menü unter "Datei" -> "Beispiel" -> "TinyGPS++" einige Beispiel Sketches gefunden werden.

#### Software kompilieren und hochladen

Nun sind alle Voraussetzungen erfüllt und wir können die MCU mit einem Micro-USB Kabel anschließen. In der Arduino IDE sind nun folgende Werte festzulegen:

- Werkzeuge -> Board -> ESP8266 Boards (...) -> LOLIN (WEMOS) D1 R2 & mini

- Werkzeuge -> Flash Size -> 4MB (FS:**3MB** OTA:~512KB)
- Werkzeuge -> Port: (hier den richtigen USB Port wählen)

Wird kein Port zur Auswahl angeboten, muss wahrscheinlich noch ein "USB to Serial Port" Treiber installiert werden. Auch daran zu erkennen, dass im Windows Gerätemanager unter "Andere Geräte" ein "USB2.0-Serial" Gerät mit gelbem Warndreieck angezeigt wird.

Den Treiber findet man z.B. hier beim [Hersteller](http://www.wch-ic.com/downloads/CH341SER_ZIP.html). Nach erfolgreicher Installation sollte im Gerätemanager unter "Anschlüsse (COM & LPT)" ein Gerät mit der Bezeichnung "USB-Serial CH340 (COMx)" angezeigt werden. Den angezeigten COM-Port in Klammern auch in der Arduino IDE wählen.

Jetzt kann kompiliert werden: Sketch -> Überprüfen/Kompilieren. Sollte fehlerfrei durchlaufen, ansonsten Meldungen im unteren Teil der IDE beachten.

Vor dem Hochladen der Dateien und des Programms, bitte die Datei **wifi.ini** im Unterverzeichnis **data** des Sketches anpassen und dort die eigenen WiFi/WLAN Daten eingeben. Der Sketch Ordner lässt sich einfach über die IDE Funktion "Sketch" -> "Sketch-Ordner anzeigen" öffnen.

Nun die folgenden Schritte ausführen, um das LittleFS zu füllen und die Firmware, bzw. das Programm zu übertragen:

- Werkzeuge -> ESP8266 LittleFS Data Upload (häufiger Fehler: serieller Monitor ist geöffnet)
- Sketch -> Hochladen

Nachdem der Sketch hochgeladen wurde, wird der Sketch zwar sofort gestartet, man sieht aber nicht so viel. Dazu nun den seriellen Monitor starten und unten auf 9600 Baud einstellen. Den seriellen Monitor findet man in der IDE über "Werkzeuge" -> "Serieller Monitor". Die MCU eventuell über den Reset Knopf neu starten. Die Ausgabe im seriellen Monitor sollte dann etwa so aussehen:

> ```
> ⸮$N⸮h⸮HdCZ⸮⸮B`⸮⸮
> 166:********** Starting...
> 166:[checkforUpdate] Looking for firmware file /wifiOiler.ino.d1_mini.bin
> 170:[checkforUpdate] no new firmware found
> 220:LittleFS free: 2820096 bytes = 52224 GPS records (43:31)
> 609:[setupGPS] calling configureGPS()
> 609:[configureGPS] still under construction...
> 609:reading gps device config file /Neo-M6.ini
> [...]
> 942:This is wifiOiler version 4.2.011 on board WemosMini
> 943:starting WiFi ...
> 1124:Versuche, mit bekanntem Wifi Netz zu verbinden...
> 1211:Add wifi to wifiMulti: HandyHotspot
> 1212:Add wifi to wifiMulti: SweetHomeWLAN
> 1213:Connecting Wifi .....
> 5077:WiFi connected to 'SweetHomeWLAN' - IP address: 192.168.178.42
> 5179:mDNS responder started, please connect to "http://wifiOiler.local"
> 5599:-------- Initialisierung beendet.
> ```

Mit der Software können nun die ersten Versuche gestartet werden. Dazu den Browser öffnen und mit "http://wifiOiler.local" verbinden. 

## Platine

Tipps zur Erstellung der Platine sind im Dokument "ListeBauteile" zu finden. Wer noch nicht so oft gelötet hat, hier einige Tipps:

- die Oberfläche der Platine sollte blank, sauber und korrosionsfrei sein. Wenn z.B. eine kupferbeschichtete Platine etwas länger liegt, bilden sich Ablagerungen, die die Annahme von Lötzinn erschweren. Ggf. mit einem Topfschwamm oder leicht schleifenden Mitteln bearbeiten (aber Vorsicht: man will ja nicht gleich das ganze Kupfer abschleifen!).
- in gut belüfteten Räumen arbeiten. Auch wenn das Lötzinn heutzutage nicht mehr mit Blei versetzt ist (oder sein sollte), sind die entstehenden Dämpfe bestimmt nicht gesund.
- als erstes die flachen Teile auflöten (in diesem Fall die Gleichrichterdioden und die Widerstände) und sich dann langsam "hocharbeiten". Zum Schluss die höchsten Teile, sollte in diesem Fall der Elko sein.
- Es muss kein teurer Lötkolben sein, geregelt wäre aber schön und die Lötspitze sollte nicht zu dick sein (also nicht der Breitmeißel für die Dachrinne).
- Das GPS Modul kann auf zwei Arten angeschlossen werden, Chip-up oder Chip-down, je nachdem, wo die Antenne verbaut wird. Am besten klebt man sie mit doppelseitigem Klebeband auf die Rückseite des GPS-Moduls und verbaut dieses Chip-down, also so, dass der u-blox Chip auf dem GPS-Modul nach unten zeigt. Wenn dies so geplant ist, dann die zweite Steckbuchsenleiste für Chip-Up Installation weglassen (kollidiert sonst etwas mit dem Antennenanschluss).