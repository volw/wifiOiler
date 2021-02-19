# Bedienung des Ölers
Der Öler kann über eine Web-Oberfläche bedient werden. Dafür wurde auf dem ESP8266 Chip ein kleiner Web-Server implementiert. 

Kleiner Hinweis zur Performance: der Öler ist ständig mit dem Empfang und der Auswertung der GPS Daten beschäftigt, so dass manchmal etwas Geduld erforderlich ist, da eine Reaktion des Web-Servers schon mal einige Sekunden dauern kann - also nicht nervös werden und Reaktion des Browsers abwarten. Tippt man hektisch auf den Buttons herum, bringt das den kleinen Rechenknecht nur in Schwierigkeiten... Insgesamt sind die Antwortzeiten aber durchaus anständig.

## WLAN Aktivierung
Nachdem der Öler gestartet wurde, kann er über eine Web-Oberfläche bedient werden. Ist der Öler so konfiguriert (s. Einstellungen), dass das WLAN automatisch aktiviert wird, ist sonst nach dem Start keine Aktion erforderlich. Muss dagegen das WLAN manuell aktiviert werden, wird der Button so lange gedrückt, bis die LED rot leuchtet (dies ist das Zeichen dafür, dass ein langer Tastendruck erkannt wurde).
<br>Bei Aktivierung des WLANs versucht der Öler, sich mit einem der in der Datei "wifi.ini" konfigurierten WLAN Netze zu verbinden. Ist dies nicht möglich, wird ein eigener Access Point aktiviert, an dem man sich anmelden kann. Name und Passwort des Access Points wird in den Einstellungen konfiguriert (s. Konfiguration). <br>Ist ein Display angeschlossen, zeigt die WLAN Statusanzeige nun ein 'w' für ein verbundenes WLAN oder ein 'A' für einen erstellten Access Point. Ohne Display wird das Ergebnis durch ein LED Feedback angezeigt (mehrmaliges grünes Flackern für WLAN, erst rotes dann grünes Flackern für einen Access Point).
<br>Bei Auslieferung heißt der Access Point "wifiOiler" und das Passwort "alteoile". Dies sollte in den Einstellungen bald geändert werden, damit nicht jemand anderes am Öler Einstellungen ändern kann.
## Hauptmenü der Web-Oberfläche
Im oberen Teil wird der Name des Ölers (ist identisch mit dem Namen des Access Points) und die Version angezeigt. Danach sind mehrere Buttons zu sehen:
<br>![Main Menu](images/mnumain.png)
### Modus Pumpe
Ein Untermenü mit den einstellbaren Modi erscheint. Die aktuelle Einstellung wird durch einen roten Button angezeigt. Wählen kann man
<br>![Pump Menu](images/mnupmp.png)
<br>Nach welchen Entfernungen bei den einzelnen Modi geölt wird, kann in den Einstellungen konfiguriert werden. Üblicherweise wird bei **Regen** deutlich öfter geölt, als bei trockener Straße (also **Normal**), da die Nässe das Öl von der Kette wäscht. Bewährt hat sich bei mir die zweifache Menge. Wenn im Normalzustand also alle 6000 Meter geölt wird, ist bei Regen ein Wert von 3000 Meter ein guter Richtwert.
<br>Bei **Offroad** Fahrten sollte noch öfter geölt werden, damit an der Kette haftender Dreck nicht wie Schmirgelpapier wirkt.
<br>Der Modus **Permanent** kann verwendet werden, wenn das System enlüftet werden soll (z.B. nach dem Einbau). Eine manuelle Entlüftung sollte allerdings zuerst vorgenommen werden, damit die Pumpe nicht stundenlang im Dauerbetrieb ist.
<br>Den Modus **Deaktiviert** verwende ich eigentlich nur zum Testen oder Debuggen.
<br>Da man während der Fahrt nicht das Handy aus der Tasche ziehen kann (bzw. sollte!), wird der Pumpenmodus dann über den eingebauten Button umgestellt. Dabei wird jeweils pro Knopfdruck ein Modus weitergeschaltet, von **Normal** zu **Regen**, dann zu **Offroad**, dann **Permanent**, dann aus und danach wieder zu **Normal**. 
<br>Nach jedem Knopfdruck wird die gewählte Pumpeneinstellung durch ein LED Signal angezeigt
* 1 x grün = **Normal**
* 2 x grün = **Regen**
* 3 x grün = **Offroad**
* 4 x grün = **permanent**
* 1 x rot = **aus**
<br>Genau 1,5 Sekunden nach dem letzten Knopfdruck wird die gewählte Einstellung aktiv. Durch diese Verzögerung können die Modi durchgeschaltet werden, ohne dass gleich die Pumpe ausgelöst wird.
### Konfiguration
In diesem Dialog lassen sich die Einstellungen des Ölers dauerhaft ändern. Die einzelnen Einstellungen sind nochmal separat [hier](Einstellungen.md) beschrieben.
### File Browser
Ein File Browser für die Dateien im Dateisystem des ESP8266, welches für Konfigurationsdateien, HTML-Sourcen für den Web-Server und für die aufgezeichneten Touren (Track files) verwendet wird. Es sind auch Editierfunktionen vorhanden, um z.B. die WiFi-Konfiguration zu ändern, Track Files zu löschen, etc. Trotz Sicherheitsabfrage sollte man hier sehr vorsichtig sein. Löscht man versehentlich die Konfiguration oder HTML Dateien, muss das Dateisystem u.U. wieder extern 'beladen' werden (wozu der Öler ggf. ausgebaut werden muss - also Vorsicht!).
<br>Es stehen drei Varianten für den File Browser zur Verfügung (kann in den Einstellungen geändert werden). Der Standard Browser (/littlefsb.htm) verbraucht am wenigsten Platz und kann ohne Verbindung zum Internet genutzt werden. Weitere Infos s. [Einstellungen - FSBrowser & Editor](Einstellungen.md/#fsbrowser)

In der oberen Funktionsleiste sind einige Funktionen über Buttons anwählbar. Im linken Bereich sieht man eine Liste mit allen Dateien, die im Filesystem enthalten sind. Im rechten Teil, der größte Bereich im File Browser, sieht man ein Eingabefenster, in dem Dateien angezeigt und/oder geändert werden können. Wählt man mit der Maus eine Datei in der linken Liste aus, wird der Inhalt im Eingabefenster angezeigt, sofern es sich um eine editierbare Datei handelt (wird anhand der Dateiendung entschieden). Wählt man z.B. die Datei "wifi.ini" wird im Eingabefenster der Inhalt der wifi.ini, also die wifi Konfiguration angezeigt.

Vorsicht: hat man im Eingabefenster eine Datei geändert und diese noch nicht gespeichert, wird mit einem Klick auf eine Datei in der Liste die Änderung ohne Rückfrage verworfen.

Die einzelnen Funktionen im File Browser:

- #### Upload

  Über den Button **[Datei auswählen]** kann eine Datei aus dem lokalen Dateisystem (also PC oder Notebook) ausgewählt werden. Der Name der ausgewählten Datei erscheint nun neben dem **[Datei auswählen]** Button und im Eingabefeld in der Funktionsleiste. Mit dem Button **[Upload]** kann diese Datei jetzt in das Dateisystem auf dem Öler hochgeladen werden. Wenn erforderlich, kann der Name der Datei vor dem Hochladen im Eingabefeld noch geändert werden - die hochgeladene Datei wird dann unter diesem neuen Namen auf dem Öler abgespeichert. 

  Während des Uploads blinkt die LED des Ölers. Je schneller die LED blinkt, desto schneller ist der Upload. Auf einem angeschlossenen Display werden ebenfalls Statusmeldungen zum Upload angezeigt. Ist die Datei komplett hochgeladen, wird die Eingabe in der Funktionsleiste zurückgesetzt und der Name der hochgeladenen Datei ist in der Liste sichtbar und wird hervorgehoben.

  Mit **[Upload]** kann auch eine neue **Firmware** eingespielt werden. Dazu in der Arduino IDE den wifiOiler Sketch laden und mit der Funktion "Sketch" -> "Kompilierte Binärdatei exportieren" die Firmware Datei erzeugen. Die Datei wird im Sketch Verzeichnis abgelegt, also in der IDE die Funktion "Sketch" -> "Sketch Ordner anzeigen" wählen. Im angezeigten Ordner findet man nun eine Datei mit dem Namen "wifiOiler.ino.d1_mini.bin". 

  Lädt man diese Datei mit der **[Upload]** Funktion hoch, wird automatisch erkannt, dass eine Firmware hochgeladen wurde und der Updateprozess wird automatisch gestartet. Die Firmware wird anhand des Namens erkannt (s. Konfiguration, dort wird der Name angegeben, wonach der Öler Ausschau halten soll).

- #### Save

  Sichern der Datei, die im oberen Rahmen des Eingabebereichs angezeigt wird. Gesichert wird der Inhalt, der im Eingabebereich angezeigt wird. [Save] funktioniert im Grunde wie ein Upload (LED und Displayanzeige wie beim Upload, s.o.)

- #### New File

  Mit diesem Button kann eine neue Datei angelegt werden, wenn der Name zuvor im Eingabefeld in der Funktionsleiste angegeben wurde. Nach der Anlage erscheint der Name links in der Liste, das Eingabefeld wird geleert und die neue Datei kann editiert werden. Dies funktioniert auch mit Dateien, deren Endung nicht auf eine Textdatei schließen und sich sonst nicht editieren lassen. 

- #### Rename

  Im Dateisystem existierende Dateien können auch umbenannt werden. Dazu die Datei auswählen, die umbenannt werden soll, den neuen Namen in das Eingabefeld in der Funktionsleiste eintragen und anschließend **[Rename]** drücken.

- #### Delete

  Wie die Beschriftung vermuten lässt, kann man mit dieser Funktion Dateien aus dem Dateisystem des Ölers löschen. Dazu die Datei links in der Liste auswählen und danach **[Delete]** drücken. Nach einer Sicherheitsabfrage wird die Datei endgültig gelöscht (wenn die Aktion nicht abgebrochen wird).

  Eine letzte Rettung besteht nur für Dateien, deren Inhalt noch im Eingabefenster angezeigt wird. Da der Inhalt und der Dateiname (im oberen Rahmen des Eingabebereichs) noch existieren, kann mit der [Save] Funktion die Datei mit dem aktuell angezeigten Inhalt gesichert und damit wiederhergestellt werden.

- #### Download

  Eine links in der Liste ausgewählte Datei kann mit dieser Funktion auf den PC oder anderes Endgerät, mit dem man gerade den Öler bedient, heruntergeladen werden. Es wird immer der gesicherte Zustand genutzt. Eventuelle Änderungen im Eingabebereich werden nicht heruntergeladen - dazu die Datei vorher sichern.

- #### Extract

  Viele oder alle der bereitgestellten HTML-Dateien für den Öler sind mit 'gzip' komprimiert worden, um Speicherplatz zu sparen. Mit dieser Funktion können diese komprimierten Dateien dekomprimiert werden. Dabei wird davon ausgegangen, dass nur jeweils EINE Datei im Archiv vorhanden ist, die unter dem Namen des Archivs (ohne abschließendes ".gz") im Dateisystem des Ölers gesichert wird.

  Eine dekomprimierte Datei wird bei der Verwendung durch den Öler immer der komprimierten Datei vorgezogen. Wenn also z.B. "upload.htm.gz" per **[Extract]** dekomprimiert wird und danach zusätzlich eine Datei "upload.htm" existiert, wird bei Anwahl der Upload Funktion im Hauptmenü des Ölers die "upload.htm" Datei genutzt und nicht mehr die "upload.htm.gz". Dies hat den Vorteil, dass gewünschte Änderungen sofort aktiv sind. 

### Debug Info
Hier werden Laufzeit Informationen, die aktuelle Konfiguration und der Inhalt des Dateisystems angezeigt. Außerdem wird hier die restliche Speicherkapazität angezeigt und die (mit der aktuellen Konfiguration) verbleibende Aufzeichnungszeit.
### Test Pumpe
Hiermit kann man testen, ob der Öler richtig aufgebaut und angeschlossen wurde. Die Pumpe wird so oft ausgelöst, wie in der Konfiguration vorgegeben (Anzahl Pumpaktionen pro Ölvorgang).
### Test LED
Die LED blinkt dreimal grün und dreimal rot. So kann festgestellt werden, ob die zweifarbige LED richtig angeschlossen wurde.
### Upload Tracks
Ist ein Upload Server vorhanden, können die aufgezeichneten Tracks hochgeladen werden. Nach erfolgreichem Upload werden sie im Öler Dateisystem gelöscht, um den Speicherplatz wieder freizugeben. Siehe Dokumentation zur [Oilerbase](../OilerBase/readme.md).
### Update Oiler
Ist die Oilerbase implementiert, kann von dieser ein Update geladen und installiert werden (es gibt allerdings auch andere Wege). Siehe Dokumentation zur [Oilerbase](../OilerBase/readme.md).
### Soft Reboot
Mit dieser Funktion kann der Öler neu gestartet werden. Normalerweise überflüssig, kann man aber mal verwenden, wenn man die Einstellungen per Editor geändert hat und diese aktiv werden sollen.