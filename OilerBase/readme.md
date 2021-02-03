# Oilerbase
Die sogenannte Oilerbase ist ein Raspberry Pi mit einem Apache Web-Server, der zwei Funktionen zur Verfügung stellt.
- Vom Öler aufgezeichnete Fahrstrecken (Tracks) können per Knopfdruck hochgeladen werden, um die Tracks zuhause zu sichern und Speicherplatz auf dem Öler freizugeben. Bei intensiver Nutzung sollte man das alle 1-2 Wochen machen - es können aber mehr als 40 Stunden Fahrzeit aufgezeichnet werden, ohne die Tracks hochzuladen. Ist der Speicherplatz erschöpft, wird die Aufzeichnungsfunktion automatisch deaktiviert.

- Außerdem können auf der Oilerbase Updates bereitgestellt werden, die mit der integrierten Update-Funktion des Ölers heruntergeladen und eingespielt werden können. So können selbst Anpassungen am Öler vorgenommen und diese leicht eingespielt werden.

## Inhalt  
- [Benötigtes Zubehör](#Benötigtes-Zubehör)
- [Betriebssystem installieren](#Betriebssystem-installieren)
- [Apache Web Server installieren](#Apache-Web-Server-installieren)
	- [Installation des Apache Web-Servers](#Installation-des-Apache-Web-Servers)
	- [Apache Web Server absichern](#Apache-Web-Server-absichern)
	- [Verzeichnisstruktur einrichten](#Verzeichnisstruktur-einrichten)
- [Installation von Samba](#Installation-von-Samba)
	- [Verzeichnis unter Windows einbinden](#Verzeichnis-unter-Windows-einbinden)
- [Öler testen](#Öler-testen)
- [Oilerbase veröffentlichen](#Oilerbase-veröffentlichen)

### Benötigtes Zubehör

Folgende Dinge werden benötigt, um eine Oilerbase aufzubauen und zu betreiben:

- 1 Raspberry Pi, (z.B. ein RPi 3 Modell B+) plus passendes Netzteil.
- eine MicroSD Card, Empfehlung: 16GB.
- möchte man die Oilerbase nutzen, wenn man länger auf Tour ist, ist eine öffentliche IP-Adresse oder ein DynDNS Dienstleister (Dynamic DNS Service) erforderlich. Hinweise zur Einrichtung findet man weiter unten. Sind der Öler und die Oilerbase gewöhnlich im gleichen Netz eingebucht, kann dieser Schritt ausgelassen werden. 

Um mit dem Öler zu kommunizieren, wurden einige PHP Scripte erstellt, die hier zu finden sind. Die Funktionen auf dem Öler und die Scripte auf dem Web-Server sind eng miteinander verzahnt.  Z.B. ziehen Änderungen an den Öler Funktionen sehr wahrscheinlich Änderungen an den Skripten nach sich und umgekehrt.

Die Konfiguration der Oilerbase ist recht einfach - im Grunde werden nur Standardfunktionen genutzt. Nachfolgend die einzelnen Schritte, um einen Rasberry Pi (z.B. das Modell Raspberry Pi 3 Modell B+) in Betrieb zu nehmen.

### Betriebssystem installieren

- Download eines Betriebssystem-Image, **Raspberry Pi OS Lite** reicht. Am besten von der Heimatseite https://www.raspberrypi.org/software/operating-systems
- Schreiben des Images auf die MicroSD Karte (z.B. mit [balenaEtcher](https://www.balena.io/etcher/) oder [Win32 Disk Imager](https://sourceforge.net/projects/win32diskimager/))
- SD-Karte noch für Windows im Leser stecken lassen. Kann man nicht auf die Karte zugreifen, diese evtl. neu einstecken. Windows ist u.U. der Meinung, dass die Karte formatiert werden muss, da Windows das Format nicht erkennt. Diese Meldungen bitte ignorieren (abbrechen). Eine auch für Windows lesbare Partition (FAT32) ist allerdings dabei. Dort findet man Verzeichnisse und Dateien (deren Namen z.B. mit "kernel", "fixup" oder "start" anfangen). 
  Dort konfigurieren wir WLAN vor dem Start des Raspberry Pis mit der frisch beschriebenen Karte - und zwar so ([Quelle](https://www.raspberrypi.org/documentation/configuration/wireless/headless.md)):
- Auf der FAT32 Partition der SD-Karte eine Datei mit Namen *wpa_supplicant.conf* mit folgendem Inhalt anlegen und die Daten des eigenen WLANs eintragen:

  ```
  country=DE
  ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
  update_config=1
  network={
  	ssid="hier SSID des WLAN eintragen, z.B. FBoxWLAN01"
  	psk="hier passwort eintragen"
  }
  ```

* Um Zugriff per SSH zu erlauben, wird an gleicher Stelle noch eine Datei mit Namen ssh angelegt (ohne Endung und ohne Inhalt - muss einfach nur da sein, [Quelle](https://www.raspberrypi.org/documentation/remote-access/ssh/README.md)).

* von dieser SD-Karte gestartet, sollte der RPi nun eine Anmeldung akzeptieren. Dies kann man unter Windows entweder mit [putty](https://www.putty.org/) oder auch mit der in Windows 10 eingebauten OpenSSH (=Open Secure Shell) bewerkstelligen. Hier mal die OpenSSH Variante (dazu Konsole oder Eingabeaufforderung starten oder mit der Tastenkombination \<Windows>+r den "Ausführen" Dialog). Dann folgenden Befehl eingeben:

  `ssh pi@raspberrypi` (also ssh Zugang zum Host *raspberrypi* mit User *pi*)

  Eventuelle Warnmeldungen bezüglich ECDSA host keys mit 'yes' beantworten.

  Das Standardpasswort des Users pi ist *raspberry*.

  Nach der Anmeldung zunächst mal die letzten Updates einspielen:

  ```
  sudo apt update
  sudo apt upgrade
  ```

* Starten der Konfigurationsoberfläche:

  `sudo raspi-config`

  Folgende Punkte anwählen und ändern:

  * System Options -> **Hostname**: z.B. 'oilerbase'

  * System Options -> **Password**: neues Passwort für User pi wählen

  * [Optional] Localisation Options -> **Locale**: 'de_DE.UTF-8 UTF-8' auswählen, anschließende Frage nach 'Default Locale' mit 'de_DE.UTF-8' bestätigen.

  * Localisation Options -> **Timezone**: 'Europe', dann 'Berlin' wählen.

  * Localisation Options -> **WLAN Country**: 'DE' wählen.

  * Advanced Options -> **Expand Filesystem**

  * Localisation Options -> **Keyboard** (funktioniert aktuell leider nicht). Kann auch manuell erledigt werden, wenn raspi-config wieder beendet (und der RPi ggf. durchgestartet) wurde. Dazu die Datei **/etc/default/keyboard** modifizieren. Dazu den Editor starten:

    `sudo nano /etc/default/keyboard`

    Der Inhalt sollte so aussehen (entsprechend ändern):

    ```
    XKBMODEL="pc105"
    XKBLAYOUT="de"
    XKBVARIANT=""
    XKBOPTIONS=""
    BACKSPACE="guess"
    ```

    Danach mit **\<Strg>+O** (und **\<Enter>**)speichern und mit **\<Strg>+X** verlassen.
    
    ACHTUNG: wenn oben der Hostname geändert wurde, muss bei erneuter Anmeldung der ssh Befehl natürlich entsprechend angepasst werden. Wurde z.B. der Hostname auf 'oilerbase' geändert, lautet die Angabe für den ssh Befehl jetzt:
    
    `ssh pi@oilerbase`
  
* [**Optional**] Willkommensbildschirm ändern, damit wir bei Anmeldung entsprechend begrüßt werden. Dazu Datei "/home/pi/.profile" bearbeiten. Also: `nano ~/.profile`, dann ans Ende der Datei und dort mit rechter Maustaste die folgenden Zeilen aus der Zwischenablage einfügen (andere "Schriftarten" s. [hier](http://patorjk.com/software/taag)):

  ```
  cat << 'EOF'
  
                      _  __ _  ____  _ _           ____
        Welcome to   (_)/ _(_)/ __ \(_) |         |  _ \
            __      ___| |_ _| |  | |_| | ___ _ __| |_) | __ _ ___  ___
            \ \ /\ / / |  _| | |  | | | |/ _ \ '__|  _ < / _` / __|/ _ \
             \ V  V /| | | | | |__| | | |  __/ |  | |_) | (_| \__ \  __/
              \_/\_/ |_|_| |_|\____/|_|_|\___|_|  |____/ \__,_|___/\___|
  
  EOF
  ```

  
### Apache Web Server installieren

#### Installation des Apache Web-Servers

`sudo apt install apache2`

Nach erfolgreicher Installation kann man schon die Standardseite des Webservers aufrufen. Hier den oben gewählten Hostnamen nutzen, z.B.

`http://oilerbase`

#### Apache Web Server absichern

`sudo nano /etc/apache2/apache2.conf`

Dort folgende Änderungen vornehmen. Sind die Schlüsselwörter schon vorhanden, dort ändern oder wenn sie nicht existieren, am Ende der Datei hinzufügen. Der Servername kann frei gewählt werden und muss nicht mit dem oben gewählten Hostnamen übereinstimmen:

```
# Keine Details zum Server anzeigen
ServerSignature Off
ServerTokens Prod
ServerName oilerbase
```

Folgenden Abschnitt löschen, da wir das Verzeichnis /usr/share nicht verwenden:

```
<Directory /usr/share>
    AllowOverride None
    Require all granted
</Directory>
```

Folgenden Abschnitt ändern. Wichtig sind die Minuszeichen vor den Optionen. "-Indexes" verhindert z.B. die automatische Anzeige von Verzeichnislisten, wenn im angewählten Ordner keine "index.html" Datei gefunden wird. "FollowSymLinks" schließen wir aus, da wir keine Links verwenden. Wir könnten auch die ganze Zeile ersetzen mit: `Options none`. Allerdings ist es oft sinnvoll, bestimmte Optionen bewusst auszuschließen (z.B. wenn sie beim übergeordneten Verzeichnis erlaubt wurden):

```
<Directory /var/www/>
    Options -Indexes -FollowSymLinks
    AllowOverride None
    Require all granted
</Directory>
```

Datei speichern und Editor verlassen. 

Die Apache Konfiguration kann mit folgendem Befehl geprüft werden:

`sudo apachectl configtest`

#### Verzeichnisstruktur einrichten

Als nächstes bauen wir die Verzeichnisstruktur für unseren Webserver auf:

```
sudo rm /var/www/* -R
sudo mkdir /var/www/html
sudo mkdir /var/www/update
sudo mkdir /var/www/update/WemosMini
sudo mkdir /var/www/uploads
sudo chmod 755 /var/www -R
sudo chmod 757 /var/www/uploads
```

### Installation von Samba

Um einfacher auf die Dateien zugreifen zu können, installieren wir noch Samba. Außerdem benötigen wir PHP für den Web-Server (bzw. zur Ausführung der Scripte hier im Verzeichnis):

`sudo apt install php samba`

Samba muss konfiguriert werden:

`sudo nano /etc/samba/smb.conf`

In dieser Datei werden am Ende einige Zeilen hinzugefügt, um das Verzeichnis des Webservers 'zugänglich' zu machen:

```
[oilerweb]
  path=/var/www
  read only = no
```

Editor beenden und mit folgendem Befehl ein Passwort für die Freigabe setzen. Dieses Passwort wird benötigt, um das Verzeichnis unter Windows einzubinden (zu mounten):

`smbpasswd -a root` (und zweimal Passwort eingeben)

#### Verzeichnis unter Windows einbinden

Danach dann unter Windows (in einer Eingabeaufforderung oder Konsole) mit dem oben gewählten Passwort (entsprechend ändern):

`net use * \\oilerbase\oilerweb passwort /user:root /persistent:yes`

Die Ausgabe sollte so oder ähnlich lauten:

> ```
> Laufwerk Z: ist jetzt mit \\oilerbase\oilerweb verbunden.
> 
> Der Befehl wurde erfolgreich ausgeführt.
> ```

nun können wir die hier aufgelisteten Dateien in das entsprechende Verzeichnis kopieren (nach unserem Beispiel Z:\html). 

Wenn Z: im weiteren Verlauf verwendet wird, steht es beispielhaft für den Laufwerksbuchstaben, der beim net use Befehl (s.o.) zugewiesen wurde.

Und nun der erste Test: Browser starten und Adresse der Oilerbase eingeben:

`http://oilerbase` oder `http://oilerbase.fritz.box`(sollte auch gehen, wenn eine Fritz.box im Einsatz ist). Als Ergebnis wird ein einfaches OK oben links auf einer leeren Seite angezeigt.

### Öler testen

Um den Öler zu testen (wenn dieser schon betriebsbereit und konfiguriert ist) können folgende Aktionen durchgeführt werden:

**Test der "Update Oiler" Funktion**: Ordner im Update Verzeichnis anlegen, der eine höhere Versionsnummer hat, als der Öler. Also z.B. Z:\update\WemosMini\4.9.999

Ein/zwei kleinere Dateien in diesen Ordner kopieren (ist eigentlich egal, sollte aber nicht den bestehenden Dateien auf dem Öler namentlich in die Quere kommen). Also z.B. "test.txt" oder "beispiel.dat".

Dann die Update Funktion des Ölers aufrufen. Die Dateien sollten ohne Fehlermeldung transferiert werden. Ist dies der Fall, dann die Dateien über den File Browser des Ölers wieder löschen (sie stören eigentlich nicht, nehmen aber Platz weg).

Im Falle eines Fehlers sollten die Logs des Webservers überprüft werden. Man findet sie normalerweise in /var/log/apache2. Zusätzlich kann die Ausgabe des Ölers helfen. Dazu im Öler ggf. die Funktion "Logging to file?" anschalten (s. Konfiguration) und die Aktion wiederholen. Danach findet man im File Browser die Datei "myLogger.txt" und ein Klick darauf zeigt den Inhalt im Editor Fenster an. ACHTUNG: nicht vergessen, die "Logging to file" Option wieder auszuschalten!

**Test der "Upload Tracks" Funktion**: im File Browser des Ölers zwei Dateien erzeugen mit Namensmuster der Track Dateien (z.B. "/20210614-1523.dat" und "/20210615-1245.dat"). Dazu den Namen in das Eingabefeld eintragen, dann "New file" drücken, etwas in das Editorfenster eintragen und "Save" drücken). Nun im Hauptmenü die Funktion "Upload Tracks" wählen.

### Oilerbase veröffentlichen

"Veröffentlichen" hört sich etwas hochtrabend an, gemeint ist damit die Bereitstellung der Oilerbase im großen, weiten Internet, damit der Öler von unterwegs darauf zugreifen kann. Dazu benötigt die Oilerbase eine eindeutige IP-Adresse, damit sie eindeutig identifiziert werden kann. Da fest zugewiesene IP Adressen knapp sind und man als Otto Normalverbraucher auch keine gebucht hat (ja, das kostet Geld), kann man einen sogenannten DynDNS Service in Anspruch nehmen (der durchaus kostenlos nutzbar ist). Dieser Service bekommt von unserem "Internet Router", der von unserem Provider täglich mit einer anderen IP-Adresse versorgt wird, diese IP-Adresse jedes Mal nach Änderung mitgeteilt. So kann er immer die richtige IP-Adresse rausgeben, wenn er im Internet danach gefragt wird. Der so gefundene Internet Router leitet die Anfrage des Ölers dann weiter an die Oilerbase und schon haben sich beide gefunden...

Wie dieses DynDNS nun auf dem eigenen Internet Router eingerichtet wird, ist vom Hersteller abhängig. 

Sie ähneln sich aber in den meisten Fällen darin, dass entweder ein DynDNS Anbieter aus einer Liste ausgewählt wird oder eine Update URL angegeben werden muss. Dazu werden noch die Anmeldedaten eingegeben, die man zur Erstellung eines Kontos beim Anbieter verwendet hat. Fertig.

Beim jeweiligen Hersteller des eigenen Routers sollte eine Beschreibung zur Einrichtung eines DynDNS Dienstes zu finden sein. Manche Router Hersteller bieten sogar selber einen kostenlosen DynDNS Service für ihre Kunden an. Ansonsten findet man diese Dienste im Internet, wenn man z.B. nach "DynDNS kostenlos" sucht. Bei dem von mir anfangs gewählten Anbieter musste einmal im Monat (Erinnerung per E-Mail) die freie Nutzung bestätigt/verlängert werden (dafür gab's coole Domain Namen).
