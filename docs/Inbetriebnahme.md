# Inbetriebnahme und Einbau

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

Vor dem Hochladen der Dateien und des Programms, bitte die Datei **wifi.ini** im Unterverzeichnis **data** des Sketches anpassen und dort die eigenen WiFi/WLAN Daten eingeben. Der Sketch Ordner lässt sich einfach über das Menü in der IDE öffnen: "Sketch" -> "Sketch-Ordner anzeigen".

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

Mit der Software können nun die ersten Ve1rsuche gestartet werden. Dazu den Browser öffnen und mit "http://wifiOiler.local" verbinden. 

## Platine

Tipps zur Erstellung der Platine sind im Dokument "ListeBauteile" zu finden. Wer noch nicht so oft gelötet hat, hier einige Tipps:

- die Oberfläche der Platine sollte blank, sauber und korrosionsfrei sein. Wenn z.B. eine kupferbeschichtete Platine etwas länger liegt, bilden sich Ablagerungen, die die Annahme von Lötzinn erschweren. Ggf. mit einem Topfschwamm oder leicht schleifenden Mitteln bearbeiten (aber Vorsicht: man will ja nicht gleich das ganze Kupfer abschleifen!).
- in gut belüfteten Räumen arbeiten. Auch wenn das Lötzinn heutzutage nicht mehr mit Blei versetzt ist (oder sein sollte), sind die entstehenden Dämpfe bestimmt nicht gesund.
- als erstes die flachen Teile auflöten (in diesem Fall die Gleichrichterdioden und die Widerstände) und sich dann langsam "hocharbeiten". Zum Schluss die höchsten Teile, sollte in diesem Fall der Elko sein. Ggf. können die Bauteile mit Klebeband fixiert werden, damit sie beim anlöten auch richtig sitzen.
- Es muss kein teurer Lötkolben sein, geregelt wäre aber schön und die Lötspitze sollte nicht zu dick sein (also nicht der Breitmeißel für die Dachrinne).
- Das GPS Modul kann auf zwei Arten angeschlossen werden, Chip-up oder Chip-down, je nachdem, wo die Antenne verbaut wird. Am besten klebt man sie mit doppelseitigem Klebeband auf die Rückseite des GPS-Moduls und verbaut dieses Chip-down, also so, dass der u-blox Chip auf dem GPS-Modul nach unten zeigt. Wenn dies so geplant ist, dann die zweite Steckbuchsenleiste für Chip-Up Installation weglassen (kollidiert sonst etwas mit dem Antennenanschluss).
- Zum Schluss die betankte MCU (s.o. "Software kompilieren und hochladen") in die dafür vorgesehenen Buchsenleisten stecken (USB Buchse zur Anschlussleiste)

Hier noch ein Foto eines Versuchsaufbaus (mit angeschlossenem Display):

![Testaufbau](images\testaufbau.jpg)

## Einbau Motorrad

Jetzt geht es an den Einbau. Schon vorab sollte man sich überlegen, wo Platz für Pumpe, Öltank und Öler sein könnte. Danach richten sich u.U. die benötigten Kabel- und Schlauchlängen.

#### Einbau Button und LED im Cockpit

Für den Einbau könnte ein Ausbau des entsprechenden Cockpit-Teils notwendig, bzw. nützlich sein. Es müssen Löcher für den Einbau der LED Fassung und den Button geschaffen werden. Die LED in der Fassung ggf. mit einem Stück Platine kontern und anlöten. Da die vorgeschlagenen Kabel, bzw. die Litzen im Kabel sehr dünn sind, ist das Befestigen der LED etwas fummelig. Für den Button können evtl. auch kleine Stecker nützlich sein. Bei einer frei schwebenden Verlötung können mechanische Belastungen die dünnen LED Anschlüsse schnell brechen lassen. Die großzügige Verwendung von Klebeband schützt so eine Installation enorm ;-). Das Verbindungskabel zum Öler "schlabberfrei" verlegen.

#### Öltank

Den Öltank so anbringen, dass die Öl-führenden Auslässe im Öl liegen und der Entlüftungsstutzen eben nicht. Eine etwas besser zugängliche Stelle ist nützlich (z.B. unter der Sitzbank, wenn möglich). Der Entlüftungsschlauch sollte höher enden, als die Einbauposition des Tanks. Trotzdem ist ein Ventil für den Entlüftungsschlauch sinnvoll. So kann mit Sicherheit vermieden werden, dass durch den Entlüftungsschlauch austretendes Öl die Karre einsaut. Das Befüllen des Tanks sollte schon vor dem endgültigen Einbau erfolgen. Zum Be- oder auch Nachfüllen eignet sich eine Einmalspritze ganz gut (in jeder Apotheke erhältlich). Vorsicht: mit der Spritze kann gut ein Überdruck im Tank erzeugt werden - und das Öl kommt zurück, sobald es die Gelegenheit hat.

Welches Öl man verwendet, ist Geschmacks- und Glaubenssache. Persönlich verwende ich gutes Sägekettenöl. Viskosität und Haftung ist gut und es kostet nicht so viel.

#### Pumpe

Die Pumpe am besten in der Nähe des Tanks installieren, um die Länge der ölgefüllten Schläuche zu verringern. Nur sicherstellen, dass die Schläuche nicht geknickt werden.

#### Auslassdüse

Die Verlegung des Schlauchs von der Pumpe zum Auslass unbedingt **sicher** gestalten. Der Schlauch hat ohne eine sichere Befestigung nichts in der Nähe von beweglichen Teilen zu suchen. Manchmal besteht die Möglichkeit, den Schlauch **durch** die Schwinge direkt bis zum Kettenrad zu verlegen - dort ist er natürlich gut aufgehoben. Ansonsten helfen kleb- oder schraubbare Kabelführungen, Schlauchschellen, Kabelbinder, etc.. Die Schläuche selber müssen natürlich Öl-beständig sein. Der Schlauch zum Auslass sollte etwas steifer ausfallen (Pneumatik Schlauch, Durchmesser s. Auslass Ölpumpe). 

Die Auslassdüse so anbringen, dass sie direkt am Kettenrad kurz über der Kette endet, und zwar so, dass das austretende Öl per Fliehkraft an/in der Kette landet (am besten also mittig unten "bei 6 Uhr"). Die angeschrägte Austrittsöffnung dabei nach außen drehen, damit beim rückwärts rangieren nicht so schnell Dreck in die Düse geschoben wird.

Sitzt der Ausgangsschlauch, diesen oben an der Pumpe nochmal lösen und mit Öl füllen, bis es unten raustropft (etwas unterstellen). Nicht zu viel Luft im Schlauch lassen.

#### Öler

ACHTUNG: hier wird mit Strom hantiert - bitte unbedingt Sicherheitshinweise eures Fahrzeugs beachten. Die hier beschriebene Vorgehensweise geschieht auf eigene Gefahr. Es wird selbstverständlich keine Haftung in jeglicher Art übernommen. Diese Anleitung ist nur ein Vorschlag.

Der Stromanschluss des Ölers wird an Zündung plus abgenommen (oft vorne und hinten vorhanden, ggf. Schaltplan konsultieren, Sicherung zwischenschalten). Das Gehäuse des Ölers muss nicht unbedingt angeschraubt werden - ein Regen- und Spritzwasser geschütztes Plätzchen ist aber sinnvoll. Zur Vorsicht kann der Öler z.B. auch in einen Gefrierbeutel gesteckt/gewickelt werden, da (den Einsatz des vorgeschlagenen Spannungswandlers vorausgesetzt) im Betrieb so gut wie keine Hitze entsteht. Zusätzlich schützt etwas Schaumstoff gegen zu heftige Erschütterungen. Vor dem Einbau sollte aber unbedingt die Erreichbarkeit über WLAN sichergestellt werden. Die erstmalige Programmierung **muss im ausgebauten Zustand** erfolgen (s.o.).

Der Empfang des GPS Moduls ist mit der verwendeten Keramik Aktivantenne recht ordentlich. Unter dem Sozius verbaut, dauert es bis zum ersten Fixpunkt wenige (bis max. ca. 30) Sekunden.

#### Einschalten und Test

Ist alles an Ort und Stelle, kann nun die Zündung eingeschaltet werden. Die LED im Cockpit sollte grün für einige Sekunden leuchten und dann entweder mehrmals kurz grün blinken (mit konfiguriertem WLAN verbunden) oder erst dreimal kurz rot, dann dreimal kurz grün blinken (Access Point erstellt).

Ist jetzt z.B. das Handy mit dem gleichen WLAN verbunden, kann per Browser gleich auf den Öler zugegriffen werden. Wurde der Name des Ölers in der Konfigurationsdatei "oiler.ini" nicht geändert, dann lautet die Adresse "http://wifioiler.local".

Wurde eine Access Point erstellt, dann erst mit diesem verbinden und ebenfalls o.g. Adresse verwenden. Damit sollte man im Hauptmenü des Ölers landen und kann nun einen LED- oder Pumpentest auslösen.

#### Entlüften

Zum Schluss die Pumpe auf Dauerbetrieb stellen, bis unten Öl aus dem Auslass kommt.

