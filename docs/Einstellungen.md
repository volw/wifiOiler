# Einstellungen
Hier werden die Einstellungen des Ölers beschrieben, die über die Web-Oberfläche geändert werden können. 
Die Einstellungen werden in der Datei **oiler.ini** in lesbarer Form gespeichert. 
Die Datei kann auch mit einem Editor bearbeitet werden, bevor sie hochgeladen wird. 
Geänderte Einstellungen, die über die Web Oberfläche geändert wurden, sind nach dem Speichern sofort aktiv. 
Dies gilt nicht für die Logging Einstellungen, die GPS-Konfigurationsdatei und die Einstellung für automatischen WiFi-Start. 
Um diese Einstellungen zu aktivieren, muss der Öler durchgestartet werden. 
Um geänderte Einstellungen zu aktivieren, die über den integrierten Editor geändert (oder mit der Upload-Funktion hochgeladen) wurden, muss der Öler neu gestartet werden.
<br>Folgende Einstellungen können angepasst werden:
## Wartezeit Simulation
Dieser Wert in Sekunden gibt an, ab wann das Notfallprogramm aktiv werden soll, wenn kein GPS Signal empfangen wird. Bei einem Wert von z.B. 180 wird 3 Minuten nach dem Start des Ölers (wenn bis dahin kein gültiges GPS Signal empfangen werden konnte) eine bestimmte Geschwindigkeit angenommen. Diese simulierte Geschwindigkeit wird durch den Wert **Meter/s bei Simulation** bestimmt. Beispielrechnung: wenn der Wert **Normal Mode Meter** 6000 beträgt und **Meter/s bei Simulation** 15, dann wird alle 400 Sekunden (3:40 min) gepumpt.
## Normal Mode Meter
Dieser Wert gibt in Metern an, nach welcher Strecke die Pumpe im Normalbetrieb aktiviert werden soll. Ein Wert von 6000 gibt also an, dass alle 6 km Öl an die Kette gegeben werden soll (Menge wird bestimmt durch **Anzahl Pumpaktionen pro Ölvorgang**).
## Rain Mode Meter
Wert in Metern, nach welcher Strecke auf nasser Straße die Kette etwas Öl bekommen soll.
## Offroad Mode Meter
Wert in Metern, nach welcher Strecke im Offroad-Betrieb die Kette geölt werden soll.
## Meter/s bei Simulation
Wird kein GPS Signal empfangen, wird nach einer festgelegten Zeit (s. **Wartezeit Simulation**) eine gewisse Geschwindigkeit angenommen, die durch diesen Wert bestimmt wird. Diese Simulation (oder Notfallprogramm) sorgt dafür, dass die Kette auch geölt wird, wenn aus irgendwelchen Gründen kein GPS Signal empfangen werden kann. Ein Wert von 15 z.B. (also 15 Meter pro Sekunde) simuliert eine Geschwindigkeit von 54 km/h (wird hinter dem Eingabefeld angezeigt). Oder anders gerechnet: im Modus Normal bei **Normal Mode Meter** von 6000 wird (6000 / 15 = 400) alle 400 Sekunden (also alle 3:40 Minuten) geölt.
## Anzahl Pumpaktionen pro Ölvorgang
Dieser Wert gibt an, wie oft die Pumpe pro Ölvorgang aktiviert werden soll. Bei der Dellorto Pumpe hat sich z.B. ein Wert von 3 bewährt (sonst kommt kaum was raus).
## GPS Low-Filter (cm)
Wert in cm, ab wann eine Bewegung als solche registriert werden soll. Hintergrund: das Signal vom GPS Modul 'streut' etwas, auch wenn die Karre nicht bewegt wird. Um diese Streuung rauszufiltern, ist dieser Wert gedacht. 500 (also 5 Meter) ist ein guter Wert.
## Öltank Kapazität
Anzahl an Pumpaktionen, bis der Tank leer ist. Dieser Wert ist leider nicht so gut zu schätzen. Bei jeder Pumpaktion wird auf jeden Fall die Anzahl Pumpvorgänge von diesem Wert abgezogen. Ist das Ergebnis unter 0, wird bei Start des Ölers ein LED Signal ausgegeben (mehrmaliges rot-grünes Blinken). Dies ist als Aufforderung zu verstehen, mal den Füllstand des Öltanks zu prüfen. Siehe auch **...davon verbraucht**.
## ...davon verbraucht
In diesem Feld werden alle Pumpaktionen seit dem letzten **reset** (s. Button neben dem Eingabenfeld) gezählt. Dies ist zusammen mit dem Wert **Öltank Kapazität** (s.o.) dafür gedacht, einen leer gelaufenen Öltank rechtzeitig zu erkennen. Überschreitet der Zähler den Wert **Öltank Kapazität** wird beim Start des Ölers ein LED Signal ausgegeben (mehrmaliges rot-grünes Blinken). Sieht man dieses Signal, sollte also der Füllgrad des Öltanks überprüft werden. Wurde der Öltank aufgefüllt, sollte dieser Zähler hier in den Einstellungen mit dem Button **reset** wieder auf 0 gesetzt werden.
## Name neuer Firmware Datei
Findet der Öler beim Start eine Datei mit diesem Namen, wird automatisch ein Update mit dieser Firmware Datei durchgeführt. Auch nachdem eine Datei manuell hochgeladen wurde, wird diese Prüfung (und ggf. ein Update) durchgeführt.
## Sek. zw. GPS Trackpoints
Dieser Wert gibt an, wie oft bei einer Streckenaufzeichnung ein Wegpunkt geschrieben werden soll. Einerseits möchte man die Strecke so genau wie möglich aufzeichnen - auf der anderen Seite ist der Speicherplatz halt begrenzt.
<br>Eigentlich ist es nicht unbedingt sinnvoll, jede Sekunde einen Wegpunkt aufzuzeichnen. Dadurch wird die Genauigkeit der aufgezeichneten Strecke auch nicht höher. Je schneller man fährt, um so gerader wird die Strecke wohl sein und nur bei vielen kleinen Kurven wäre eine höhere Genauigkeit wünschenswert (aber da fährt man im Allgemeinen auch keine 150).
<br>Ein Wert von 3 (also alle 3 Sekunden einen Wegpunkt schreiben) hat sich bewährt. Damit kann man ca. 46 Stunden Fahrt aufzeichnen. Ist der Speicherplatz vollständig belegt, wird die Aufzeichung automatisch gestoppt.
<br>Sollen die Touren überhaupt nicht aufgezeichnet werden, hier eine 0 einsetzen.
## eigener Host/Access Point Name
Name des Ölers und gleichzeitig des Access Points. Meldet sich der Öler in einem bekannten WLAN Netz an, ist das der Host Name, unter dem der Öler erreichbar ist, also z.B. **http://wifiOiler.local**. Kann der Öler sich an keinem WLAN anmelden, wird ein eigener Access Point/Hotspot mit diesem Namen erstellt. Auch hier wird nach erfolgter Anmeldung eine Verbindung über **http://<name>.local** hergestellt.
## Access Point Passwort
Passwort zur Verbindung mit Access Point.
## Upload Host Name
Host Name der Oilerbase, muss aus dem Internet erreichbar sein oder sich im gleichen Netz befinden. Siehe Dokumentation zur [Oilerbase](../OilerBase/readme.md).
## Upload Host Port
Port zum Upload der Track Dateien. Siehe Dokumentation zur [Oilerbase](../OilerBase/readme.md).
## Upload Host URL
URL für Upload der Track Dateien. Siehe Dokumentation zur [Oilerbase](../OilerBase/readme.md).
## Logging to file?
Der Öler gibt während des Betriebs einige Diagnosemeldungen aus. Diese können zu Analysezwecken in eine Datei geschrieben werden. Es empfiehlt sich, dies nur kurzzeitig zu machen, da das interne Dateisystem schnell volllaufen kann. Im Regelfall steht hier also **Nein**.
## Logging to Serial?
Die Diagnosemeldungen des Ölers können während der Laufzeit über die serielle Schnittstelle ausgegeben werden. Dies ist ganz nützlich, um sie mit dem Arduino Serial Monitor mitzulesen, wenn man in der Bauphase ist oder die ersten Testläufe im nicht eingebauten Zustand macht. Ist der Öler erst mal eingebaut und sozusagen im Dauerbetrieb, sollte hier ein **Nein** stehen (auch wenn's den Öler nicht sonderlich stört, macht sich das in der Performance und ggf. im Stromverbrauch etwas bemerkbar).
## GPS Logging to File?
Die empfangenen GPS Daten können ebenfalls in eine Datei geschrieben werden (gpslog.txt). Zur kurzzeitigen Analsyse ganz nützlich - auf keinen Fall für längere Zeit auf **Ja** stehen lassen. Das Dateisystem ist ruckzuck voll, da pro Sekunde mindestens zwei Sätze geschrieben werden. Also nicht stundenlang aktiviert lassen!!
## WiFi automatisch starten?
Gibt an, ob beim Starten des Ölers das WLAN automatisch aktiviert werden soll. Es kann auch nachträglich über den Button aktiviert werden (langer Buttondruck).
## GPS config file
Diese Funktion ist noch nicht vollständig implementiert. Die hier angegebene Datei soll Konfigurationsbefehle für das GPS-Modul enthalten, die beim Start des Ölers an das GPS-Modul gesendet werden sollen. Wie gesagt - noch nicht fertig.
<a name="fsbrowser"></a>
## FS Browser & Editor
Es werden verschiedene Browser / einfache Explorer für das integrierte Dateisystem des ESP8266 mitgeliefert, die auch einen Editor zum Bearbeiten der Dateien mitbringen. Diese Browser belegen unterschiedlich viel Speicherplatz auf dem internen Dateisystem, was sich auf die Aufzeichnungszeit auswirkt. Mit dem einfachsten Browser (und löscht man die anderen) hat man ca. 2 Stunden mehr Zeit, um Touren aufzuzeichnen. Man kann sich, wie gesagt, für einen entscheiden, diesen in der Konfiguration hinterlegen und dann die anderen löschen. Diese sind initial vorhanden:
* "/littlefsb.htm": der Kleinste und Einfachste von allen, benötigt keine Verbindung zum Internet.
* "/cmedit.htm": wie littlefsb.htm, als Editor wurde der Open Source Editor "CodeMirror" eingebaut. Benötigt zwar keine Verbindung zum Internet, belegt aber auch den meisten Speicherplatz.
* "/aceedit.htm": nach dem FSBrowser Beispiel von Hristo Gochkov. Hat alles, was man sich denken kann, benötigt aber eine aktive Internet Verbindung, da einige Scripte nachgeladen werden. Benutzt den Editor "ace".
