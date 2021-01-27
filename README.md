# wifiOiler
Automatischer streckenabhängiger Kettenöler für's Motorrad.<br>
Folgende Features wurden umgesetzt:<br>
- Ermittlung der zurückgelegten Strecke per GPS Chip (Neo-6M).<br>
- Aufzeichnung der Strecken im Speicher (SPIFFS-Dateisystem) des verwendeten ESP8266 Chips (ca. 45 Stunden Fahrzeit).<br>
- Übertragen (und löschen) der aufgezeichneten Strecken per WLAN auf einen zentralen Web-Server.<br>
- Automatisches Verbinden mit einem bekannten WLAN (z.B. Hotspot des Handys).<br>
- Ist keine WLAN Verbindung vorhanden, wird ein Acces Point erstellt.<br>
- Konfiguration des Ölers über eine Web-Oberfläche (integrierter Web-Server).<br>
- Vier verschiedene Pumpen Modi (Normal, Regen, Offroad, Permanent) einstellbar.<br>
- Notlaufprogramm, falls kein GPS Signal emfpangen werden kann.<br>
- Herunterladen von Updates von einem zentralen Web-Server.<br>
- Bedienung per Button oder Web-Oberfläche. Feedback per LED (zweifarbig).<br>
- Software zur Konvertierung der aufgezeichneten Tracks in das .gpx Format.<br>
- PHP Scripte und html Dateien für den zentralen Web-Server (Apache auf Raspberry Pi)
- Hochladen der aufgezeichneten Tracks auf Web-Server (Löschen nach Empfangsbestätigung)
