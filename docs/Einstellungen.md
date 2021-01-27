# Einstellungen
Hier werden die Einstellungen des �lers beschrieben, die �ber die Web-Oberfl�che ge�ndert werden k�nnen. 
Die Einstellungen werden in der Datei **oiler.ini** in lesbarer Form gespeichert. 
Die Datei kann auch mit einem Editor bearbeitet werden, bevor sie hochgeladen wird. 
Ge�nderte Einstellungen, die �ber die Web Oberfl�che ge�ndert wurden, sind nach dem Speichern sofort aktiv. 
Dies gilt nicht f�r die Logging Einstellungen, die GPS-Konfigurationsdatei und die Einstellung f�r automatischen WiFi-Start. 
Um diese Einstellungen zu aktivieren, muss der �ler durchgestartet werden. 
Um ge�nderte Einstellungen zu aktivieren, die �ber den integrierten Editor ge�ndert (oder mit der Upload-Funktion hochgeladen) wurden, muss der �ler neu gestartet werden.
<br>Folgende Einstellungen k�nnen angepasst werden:
## Wartezeit Simulation
Dieser Wert in Sekunden gibt an, ab wann das Notfallprogramm aktiv werden soll, wenn kein GPS Signal empfangen wird. Bei einem Wert von z.B. 180 wird 3 Minuten nach dem Start des �lers (wenn bis dahin kein g�ltiges GPS Signal empfangen werden konnte) eine bestimmte Geschwindigkeit angenommen. Diese simulierte Geschwindigkeit wird durch den Wert **Meter/s bei Simulation** bestimmt. Beispielrechnung: wenn der Wert **Normal Mode Meter** 6000 betr�gt und **Meter/s bei Simulation** 15, dann wird alle 400 Sekunden (3:40 min) gepumpt.
## Normal Mode Meter
Dieser Wert gibt in Metern an, nach welcher Strecke die Pumpe im Normalbetrieb aktiviert werden soll. Ein Wert von 6000 gibt also an, dass alle 6 km �l an die Kette gegeben werden soll (Menge wird bestimmt durch **Anzahl Pumpaktionen pro �lvorgang**).
## Rain Mode Meter
Wert in Metern, nach welcher Strecke auf nasser Stra�e die Kette etwas �l bekommen soll.
## Offroad Mode Meter
Wert in Metern, nach welcher Strecke im Offroad-Betrieb die Kette ge�lt werden soll.
## Meter/s bei Simulation
Wird kein GPS Signal empfangen, wird nach einer festgelegten Zeit (s. **Wartezeit Simulation**) eine gewisse Geschwindigkeit angenommen, die durch diesen Wert bestimmt wird. Diese Simulation (oder Notfallprogramm) sorgt daf�r, dass die Kette auch ge�lt wird, wenn aus irgendwelchen Gr�nden kein GPS Signal empfangen werden kann. Ein Wert von 15 z.B. (also 15 Meter pro Sekunde) simuliert eine Geschwindigkeit von 54 km/h (wird hinter dem Eingabefeld angezeigt). Oder anders gerechnet: im Modus Normal bei **Normal Mode Meter** von 6000 wird (6000 / 15 = 400) alle 400 Sekunden (also alle 3:40 Minuten) ge�lt.
## Anzahl Pumpaktionen pro �lvorgang
Dieser Wert gibt an, wie oft die Pumpe pro �lvorgang aktiviert werden soll. Bei der Dellorto Pumpe hat sich z.B. ein Wert von 3 bew�hrt (sonst kommt kaum was raus).
## GPS Low-Filter (cm)
Wert in cm, ab wann eine Bewegung als solche registriert werden soll. Hintergrund: das Signal vom GPS Modul 'streut' etwas, auch wenn die Karre nicht bewegt wird. Um diese Streuung rauszufiltern, ist dieser Wert gedacht. 500 (also 5 Meter) ist ein guter Wert.
## �ltank Kapazit�t
Anzahl an Pumpaktionen, bis der Tank leer ist. Dieser Wert ist leider nicht so gut zu sch�tzen. Bei jeder Pumpaktion wird auf jeden Fall die Anzahl Pumpvorg�nge von diesem Wert abgezogen. Ist das Ergebnis unter 0, wird bei Start des �lers ein LED Signal ausgegeben (mehrmaliges rot-gr�nes Blinken). Dies ist als Aufforderung zu verstehen, mal den F�llstand des �ltanks zu pr�fen. Siehe auch **...davon verbraucht**.
## ...davon verbraucht
In diesem Feld werden alle Pumpaktionen seit dem letzten **reset** (s. Button neben dem Eingabenfeld) gez�hlt. Dies ist zusammen mit dem Wert **�ltank Kapazit�t** (s.o.) daf�r gedacht, einen leer gelaufenen �ltank rechtzeitig zu erkennen. �berschreitet der Z�hler den Wert **�ltank Kapazit�t** wird beim Start des �lers ein LED Signal ausgegeben (mehrmaliges rot-gr�nes Blinken). Sieht man dieses Signal, sollte also der F�llgrad des �ltanks �berpr�ft werden. Wurde der �ltank aufgef�llt, sollte dieser Z�hler hier in den Einstellungen mit dem Button **reset** wieder auf 0 gesetzt werden.
## Name neuer Firmware Datei
Findet der �ler beim Start eine Datei mit diesem Namen, wird automatisch ein Update mit dieser Firmware Datei durchgef�hrt. Auch nachdem eine Datei manuell hochgeladen wurde, wird diese Pr�fung (und ggf. ein Update) durchgef�hrt.
## Sek. zw. GPS Trackpoints
Dieser Wert gibt an, wie oft bei einer Streckenaufzeichnung ein Wegpunkt geschrieben werden soll. Einerseits m�chte man die Strecke so genau wie m�glich aufzeichnen - auf der anderen Seite ist der Speicherplatz halt begrenzt.
<br>Eigentlich ist es nicht unbedingt sinnvoll, jede Sekunde einen Wegpunkt aufzuzeichnen. Dadurch wird die Genauigkeit der aufgezeichneten Strecke auch nicht h�her. Je schneller man f�hrt, um so gerader wird die Strecke wohl sein und nur bei vielen kleinen Kurven w�re eine h�here Genauigkeit w�nschenswert (aber da f�hrt man im Allgemeinen auch keine 150).
<br>Ein Wert von 3 (also alle 3 Sekunden einen Wegpunkt schreiben) hat sich bew�hrt. Damit kann man ca. 46 Stunden Fahrt aufzeichnen. Ist der Speicherplatz vollst�ndig belegt, wird die Aufzeichung automatisch gestoppt.
<br>Sollen die Touren �berhaupt nicht aufgezeichnet werden, hier eine 0 einsetzen.
## eigener Host/Access Point Name
Name des �lers und gleichzeitig des Access Points. Meldet sich der �ler in einem bekannten WLAN Netz an, ist das der Host Name, unter dem der �ler erreichbar ist, also z.B. **http://wifiOiler.local**. Kann der �ler sich an keinem WLAN anmelden, wird ein eigener Access Point/Hotspot mit diesem Namen erstellt. Auch hier wird nach erfolgter Anmeldung eine Verbindung �ber **http://<name>.local** hergestellt.
## Access Point Passwort
Passwort zur Verbindung mit Access Point.
## Upload Host Name
Host Name der Oilerbase, muss aus dem Internet erreichbar sein oder sich im gleichen Netz befinden. Siehe Dokumentation zur [Oilerbase](Oilerbase.md).
## Upload Host Port
Port zum Upload der Track Dateien. Siehe Dokumentation zur [Oilerbase](Oilerbase.md).
## Upload Host URL
URL f�r Upload der Track Dateien. Siehe Dokumentation zur [Oilerbase](Oilerbase.md).
## Logging to file?
Der �ler gibt w�hrend des Betriebs einige Diagnosemeldungen aus. Diese k�nnen zu Analysezwecken in eine Datei geschrieben werden. Es empfiehlt sich, dies nur kurzzeitig zu machen, da das interne Dateisystem schnell volllaufen kann. Im Regelfall steht hier also **Nein**.
## Logging to Serial?
Die Diagnosemeldungen des �lers k�nnen w�hrend der Laufzeit �ber die serielle Schnittstelle ausgegeben werden. Dies ist ganz n�tzlich, um sie mit dem Arduino Serial Monitor mitzulesen, wenn man in der Bauphase ist oder die ersten Testl�ufe im nicht eingebauten Zustand macht. Ist der �ler erst mal eingebaut und sozusagen im Dauerbetrieb, sollte hier ein **Nein** stehen (auch wenn's den �ler nicht sonderlich st�rt, macht sich das in der Performance und ggf. im Stromverbrauch etwas bemerkbar).
## GPS Logging to File?
Die empfangenen GPS Daten k�nnen ebenfalls in eine Datei geschrieben werden (gpslog.txt). Zur kurzzeitigen Analsyse ganz n�tzlich - auf keinen Fall f�r l�ngere Zeit auf **Ja** stehen lassen. Das Dateisystem ist ruckzuck voll, da pro Sekunde mindestens zwei S�tze geschrieben werden. Also nicht stundenlang aktiviert lassen!!
## WiFi automatisch starten?
Gibt an, ob beim Starten des �lers das WLAN automatisch aktiviert werden soll. Es kann auch nachtr�glich �ber den Button aktiviert werden (langer Buttondruck).
## GPS config file
Diese Funktion ist noch nicht vollst�ndig implementiert. Die hier angegebene Datei soll Konfigurationsbefehle f�r das GPS-Modul enthalten, die beim Start des �lers an das GPS-Modul gesendet werden sollen. Wie gesagt - noch nicht fertig.
<a name="fsbrowser"></a>
## FS Browser & Editor
Es werden verschiedene Browser / einfache Explorer f�r das integrierte Dateisystem des ESP8266 mitgeliefert, die auch einen Editor zum Bearbeiten der Dateien mitbringen. Diese Browser belegen unterschiedlich viel Speicherplatz auf dem internen Dateisystem, was sich auf die Aufzeichnungszeit auswirkt. Mit dem einfachsten Browser (und l�scht man die anderen) hat man ca. 2 Stunden mehr Zeit, um Touren aufzuzeichnen. Man kann sich, wie gesagt, f�r einen entscheiden, diesen in der Konfiguration hinterlegen und dann die anderen l�schen. Diese sind initial vorhanden:
* "/littlefsb.htm": der Kleinste und Einfachste von allen, ben�tigt keine Verbindung zum Internet.
* "/cmedit.htm": wie littlefsb.htm, als Editor wurde der Open Source Editor "CodeMirror" eingebaut. Ben�tigt zwar keine Verbindung zum Internet, belegt aber auch den meisten Speicherplatz.
* "/aceedit.htm": nach dem FSBrowser Beispiel von Hristo Gochkov. Hat alles, was man sich denken kann, ben�tigt aber eine aktive Internet Verbindung, da einige Scripte nachgeladen werden. Benutzt den Editor "ace".
