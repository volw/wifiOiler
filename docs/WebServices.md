# Web Services

Per HTTP Aufruf können einige Funktionen des Ölers aufgerufen werden.

| Link                   | HTTP Funktion      | Rückgabe, Beschreibung                                       |
| ---------------------- | ------------------ | ------------------------------------------------------------ |
| /list?dir              | HTTP_GET           | JSON array mit allen Dateien, die sich im LittleFS befinden (type, name, size) |
| /list?tracks           | HTTP_GET           | JSON array mit allen aufgezeichneten Tracks (type, name, size). Eigentlich alle Dateien, deren Namen mit "2" anfängt. |
| /pumptest              | HTTP_GET           | "OK" (HTTP_RESULT 200, löst Pumpentest aus)                  |
| /ledtest               | HTTP_GET           | "OK" (HTTP_RESULT 200, löst LED-Test aus)                    |
| /config                | HTTP_GET           | JSON Objekt mit allen Konfigurationswerten                   |
| /config?[Werte]        | HTTP_POST HTTP_GET | Änderung des/der angegebenen Werte(s) (keine Fehlerbehandlung). Beispiel:<br />`curl http://wifioiler.local/config?nmm=8000&use=0` <br />ändert die Anzahl Meter für Modus "Normal" und setzt den Ölzähler zurück. |
| /pumpmode              | HTTP_GET           | Gibt aktuellen Modus der Pumpe zurück ("Normal", "Regen", "Offroad", "Permanent" oder "Aus") |
| /pumpmode?mode=\<mode> | HTTP_GET           | Setzt Modus der Pumpe auf angegebenen Modus. "Normal", "Regen", "Offroad", "Permanent" oder "Aus" werden akzeptiert (Groß-/Kleinschreibung wird ignoriert). |
| /version               | HTTP_GET           | Gibt die aktuelle Version des Ölers zurück.                  |
| /edit                  | HTTP_POST          | Hochladen einer Datei. Beispiel mit "curl": <br />`curl -v -F filename=config.htm.gz -F upload=@D:/temp/config.htm.gz http://wifiOiler.local/edit`<br />Download von Dateien mit "curl":<br />`curl http://wifioiler.local/index.htm.gz --output D:/temp/index.htm.gz`<br />Download mit gleichzeitigem unzip:<br />`curl http://wifioiler.local/index.htm.gz --output -|gzip -d > D:/temp/index.htm` |
| /edit?file=\<filename> | HTTP_DELETE        | Löschen der angegebenen Datei aus dem LittleFS Dateisystem. Name des Arguments beliebig. Beispiel:<br />`curl http://wifioiler.local/edit?file=index.htm`<br />VORSICHT: die angegebene Datei wird ohne Rückfrage gelöscht. |
| /info                  | HTTP_GET           | JSON Objekt mit runtime Informationen. Elemente des Objektes und ihre Bedeutung:<br />"ve" : aktuelle Softwareversion<br/>"da" : code für die Ansteuerung von Displays eingebunden?<br/>"rt" : Laufzeit des Ölers<br/>"fr" : freier Hauptspeicher<br/>"cr" : Anzahl Zeichen, die vom GPS bis jetzt gelesen wurden<br/>"gf" : Anzahl GPS Sätze mit gültigem Fix<br/>"pc" : Anzahl GPS Sätze mit gültiger Prüfsumme<br/>"fc" : Anzahl GPS Sätze mit ungültiger Prüfsumme<br/>"mp" : Anzahl zurückgelegter Meter seit dem letzten Pumpvorgang<br/>"pm" : Aktuell eingestellter Modus der Pumpe<br/>"ra" : GPS Aufzeichnung eingeschaltet?<br/>"tf" : Name der aktuellen Track-Datei (nur vorhanden, wenn Aufzeichnung gestartet)<br/>"rw" : Anzahl der aufgezeichneten GPS Sätze<br/>"rs" : Größe eines GPS Satzes in bytes<br/>"we" : Anzahl an Schreibfehlern<br/>"gm" : Anzahl zurückgelegter Meter laut GPS<br/>"mc" : Aktueller Stand des movement Zählers<br/>"xc" : Max. möglicher Stand des movement Zählers<br/>"tc" : Anzahl Pumpvorgänge seit Start<br/>"mm" : Wartungsmodus aktive?<br/>"wc" : SSID des WLANs mit dem der Öler aktuell verbunden ist<br/>"ip" : Aktuelle IP-Addresse<br/>"fs" : Freier Speicherplatz im Dateisystem (in bytes)<br/>"rl" : Anzahl GPS Sätze, die noch geschrieben werden können<br/>"tl" : Verbliebene Zeit für die Aufzeichnung (mit aktuell freiem Speicherplatz) |
| /reboot?bootnow        | HTTP_GET           | Neustart des Ölers. Da keine Rückgabe erfolgt, werden u.U. Fehlermeldungen von curl und ähnlichen Programmen ausgegeben. |

