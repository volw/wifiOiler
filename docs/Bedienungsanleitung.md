# Bedienung des �lers
Der �ler kann �ber eine Web-Oberfl�che bedient werden. Daf�r wurde auf dem ESP8266 Chip ein kleiner Web-Server implementiert. Ich habe bisher noch nicht erlebt, dass bei bestehender Verbindung eine Seite mal nicht geliefert wurde - man sollte aber auf jeden Fall Geduld mitbringen, da es manchmal bis zu 10 Sekunden dauern kann - also nicht nerv�s werden und Reaktion des Browsers abwarten. Tippt man hektisch auf den Buttons herum, bringt das den kleinen Rechenknecht nur in Schwierigkeiten...
## WLAN Aktivierung
Nachdem der �ler gestartet wurde, kann er �ber eine Web-Oberfl�che bedient werden. Ist der �ler so konfiguriert (s. Einstellungen), dass das WLAN automatisch aktiviert wird, ist sonst nach dem Start keine Aktion erforderlich. Muss dagegen das WLAN manuell aktiviert werden, wird der Button so lange gedr�ckt, bis die LED rot leuchtet (dies ist das Zeichen daf�r, dass ein langer Tastendruck erkannt wurde).
<br>Bei Aktivierung des WLANs versucht der �ler, sich mit einem der in der Datei "wifi.ini" konfigurierten WLAN Netze zu verbinden. Ist dies nicht m�glich, wird ein eigener Access Point aktiviert, an dem man sich anmelden kann. Name und Passwort des Access Points wird in den Einstellungen konfiguriert (s. Konfiguration). <br>Ist ein Display angeschlossen, zeigt die WLAN Statusanzeige nun ein 'w' f�r ein verbundenes WLAN oder ein 'A' f�r einen erstellten Access Point. Ohne Display wird das Ergebnis durch ein LED Feedback angezeigt (mehrmaliges gr�nes Flackern f�r WLAN, erst rotes dann gr�nes Flackern f�r einen Access Point).
<br>Bei Auslieferung hei�t der Access Point "wifiOiler" und das Passwort "alteoile". Dies sollte in den Einstellungen bald ge�ndert werden, damit nicht jemand anderes am �ler Einstellungen �ndern kann.
## Hauptmen� der Web-Oberfl�che
Im oberen Teil wird der Name des �lers (ist identisch mit dem Namen des Access Points) und die Version angezeigt. Danach sind mehrere Buttons zu sehen:
<br>![Main Menu](images/mnumain.png)
### Modus Pumpe
Ein Untermen� mit den einstellbaren Modi erscheint. Die aktuelle Einstellung wird durch einen roten Button angezeigt. W�hlen kann man
<br>![Pump Menu](images/mnupmp.png)
<br>Nach welchen Entfernungen bei den einzelnen Modi ge�lt wird, kann in den Einstellungen konfiguriert werden. �blicherweise wird bei **Regen** deutlich �fter ge�lt, als bei trockener Stra�e (also **Normal**), da die N�sse das �l von der Kette w�scht. Bew�hrt hat sich bei mir die zweifache Menge. Wenn im Normalzustand also alle 6000 Meter ge�lt wird, ist bei Regen ein Wert von 3000 Meter ein guter Richtwert.
<br>Bei **Offroad** Fahrten sollte noch �fter ge�lt werden, damit an der Kette haftender Dreck nicht wie Schmirgelpapier wirkt.
<br>Der Modus **Permanent** kann verwendet werden, wenn das System enl�ftet werden soll (z.B. nach dem Einbau). Eine manuelle Entl�ftung sollte allerdings zuerst vorgenommen werden, damit die Pumpe nicht stundenlang im Dauerbetrieb ist.
<br>Den Modus **Deaktiviert** verwende ich eigentlich nur zum Testen oder Debuggen.
<br>Da man w�hrend der Fahrt nicht das Handy aus der Tasche ziehen kann (bzw. sollte!), wird der Pumpenmodus dann �ber den eingebauten Button umgestellt. Dabei wird jeweils pro Knopfdruck ein Modus weitergeschaltet, von **Normal** zu **Regen**, dann zu **Offroad**, dann **Permanent**, dann aus und danach wieder zu **Normal**. 
<br>Nach jedem Knopfdruck wird die gew�hlte Pumpeneinstellung durch ein LED Signal angezeigt
* 1 x gr�n = **Normal**
* 2 x gr�n = **Regen**
* 3 x gr�n = **Offroad**
* 4 x gr�n = **permanent**
* 1 x rot = **aus**
<br>Genau 1,5 Sekunden nach dem letzten Knopfdruck wird die gew�hlte Einstellung aktiv. Durch diese Verz�gerung k�nnen die Modi durchgeschaltet werden, ohne dass gleich die Pumpe ausgel�st wird.
### Konfiguration
In diesem Dialog lassen sich die Einstellungen des �lers dauerhaft �ndern. Die einzelnen Einstellungen sind nochmal separat [hier](Einstellungen.md) beschrieben.
### File Browser
Ein File Browser f�r die Dateien im Dateisystem des ESP8266, welches f�r Konf�gurationsdateien, HTML-Sourcen f�r den Web-Server und f�r die aufgezeichneten Touren (Track files) verwendet wird. Es sind auch Editierfunktionen vorhanden, um z.B. die WiFi-Konfiguration zu �ndern, Track Files zu l�schen, etc. Trotz Sicherheitsabfrage sollte man hier sehr vorsichtig sein. L�scht man versehentlich die Konfiguration oder HTML Dateien, muss das Dateisystem ggf. wieder extern 'beladen' werden (wozu der �ler ggf. ausgebaut werden muss - also Vorsicht!).
<br>Es stehen drei Varianten f�r den File Browser zur Verf�gung (kann in den Einstellungen ge�ndert werden). Der Standard Browser (/littlefsb.htm) verbraucht am wenigsten Platz und kann ohne Verbindung zum Internet genutzt werden. Weitere Infos s. [Einstellungen - FSBrowser & Editor](Einstellungen.md/#fsbrowser)
### Debug Info
Hier werden Laufzeit Informationen, die aktuelle Konfiguration und der Inhalt des Dateisystems angezeigt. Au�erdem wird hier die restliche Speicherkapazit�t angezeigt und die (mit der aktuellen Konfiguration) verbleibende Aufzeichnungszeit.
### Test Pumpe
Hiermit kann man testen, ob der �ler richtig aufgebaut und angeschlossen wurde. Die Pumpe wird so oft ausgel�st, wie in der Konfiguration vorgegeben (Anzahl Pumpaktionen pro �lvorgang).
### Test LED
Die LED blinkt dreimal gr�n und dreimal rot. So kann festgestellt werden, ob die zweifarbige LED richtig angeschlossen wurde.
### Upload Tracks
Ist ein Upload Server vorhanden, k�nnen die aufgezeichneten Tracks hochgeladen werden. Nach erfolgreichem Upload werden sie im �ler Dateisystem gel�scht, um den Speicherplatz wieder freizugeben. Siehe Dokumentation zur [Oilerbase](Oilerbase.md).
### Update Oiler
Ist die Oilerbase implementiert, kann von dieser ein Update geladen und installiert werden (es gibt allerdings auch andere Wege). Siehe Dokumentation zur [Oilerbase](Oilerbase.md).
### Soft Reboot
Mit dieser Funktion kann der �ler neu gestartet werden. Normalerweise �berfl�ssig, kann man aber mal verwenden, wenn man die Einstellungen per Editor ge�ndert hat und diese aktiv werden sollen.