using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace gpsTrack
{
    public class BinaryTrackFile
    {
        public String Name = "";
        public String Pfad = "";
        public long Groesse = 0;
        public DateTime Startzeit;
        public bool IsValid = true;
        public bool isNewNameFormat = false;

        public BinaryTrackFile(FileInfo fi)
        {
            Name = fi.Name;
            Pfad = fi.DirectoryName;
            Groesse = fi.Length;
            extractDT();    // versucht das Datum des Tracks zu ermitteln
        }

        void extractDT()
        {
            int Jahr = 0, Monat = 0, Tag = 0, Stunde = 0, Minute = 0, Sekunde = 0;

            // nach Länge kann nicht gefragt werden, da beide Längen gleich sind
            int nPos = Name.IndexOf('-');
            //if (Name.Length == "JJJJMMTT-HHMM.dat".Length)  // neues Format
            if (nPos == 8)  // neues Format
            {   // JJJJMMTT-HHMM.dat
                isNewNameFormat = true;
                Jahr = int.Parse(Name.Substring(0, 4));
                Monat = int.Parse(Name.Substring(4, 2));
                Tag = int.Parse(Name.Substring(6, 2));
                Stunde = int.Parse(Name.Substring(9, 2));
                Minute = int.Parse(Name.Substring(11, 2));
                Sekunde = 0;

            }
            else if (nPos == 6) // altes Format
            {   // JJMMTT-HHMMSS.dat
                Jahr = 2000 + int.Parse(Name.Substring(0, 2));
                Monat = int.Parse(Name.Substring(2, 2));
                Tag = int.Parse(Name.Substring(4, 2));
                Stunde = int.Parse(Name.Substring(7, 2));
                Minute = int.Parse(Name.Substring(9, 2));
                Sekunde = int.Parse(Name.Substring(11, 2));
            }
            else
            {
                Console.WriteLine("FEHLER: Dateiname '{0}' hat ein unbekanntes Format...", Name);
            }

            // Damit versuchen wirs zunächst:
            try
            {
                Startzeit = new DateTime(Jahr, Monat, Tag, Stunde, Minute, Sekunde);
            }
            catch (Exception)
            {
                IsValid = false;
            }
            if (!IsValid)
            {
                gpsFix fix = new gpsFix();
                IsValid = true;
                long counter = 0;
                using (FileStream s = File.OpenRead(Pfad + Path.DirectorySeparatorChar + Name))
                using (BinaryReader r = new BinaryReader(s))
                {
                    try
                    {
                        while (s.Position < s.Length && !fix.IsValidTimestamp)
                        {
                            fix = new gpsFix(r);
                            counter++;
                        }
                    }
                    catch
                    {
                        Console.WriteLine("FEHLER: Datei {0} konnte nicht gelesen werden...", Pfad + Path.DirectorySeparatorChar + Name);
                    }
                    try
                    {
                        Startzeit = new DateTime(fix.Timestamp.Year, fix.Timestamp.Month, fix.Timestamp.Day, Stunde, Minute, Sekunde);
                    }
                    catch (Exception)
                    {
                        IsValid = false;
                    }

                }
                if (!fix.IsValidTimestamp)
                {
                    Console.WriteLine("FEHLER: Datum der Datei {0} konnte nicht bestimmt werden.", Pfad + Path.DirectorySeparatorChar + Name);
                    Console.WriteLine("        Insgesamt {0} Wegpunkt{1} und keine Datumsangabe. Datei wird ignoriert ({2} bytes)", counter, counter > 1 ? "e" : "", Groesse);
                    IsValid = false;
                }
            }
        }

        public String CurrentFullPath()
        {
            return Pfad + Path.DirectorySeparatorChar + Name;
        }

        public String StartTimeFullPath()
        {
            if (isNewNameFormat)
                return Pfad + Path.DirectorySeparatorChar + OnlyName() + ".dat";
            else 
                return Pfad + Path.DirectorySeparatorChar + OnlyName().Substring(2) + ".dat";
        }

        public String DateString()
        {
            if (isNewNameFormat)
                return (Startzeit.Year.ToString("D4") + Startzeit.Month.ToString("D2") + Startzeit.Day.ToString("D2"));
            else
                return (Startzeit.Year.ToString("D2") + Startzeit.Month.ToString("D2") + Startzeit.Day.ToString("D2"));
        }

        public String OnlyName()
        {
            if (isNewNameFormat)
                return (DateString() + "-" + Startzeit.Hour.ToString("D2") + Startzeit.Minute.ToString("D2"));
            else
                return (DateString() + "-" + Startzeit.Hour.ToString("D2") + Startzeit.Minute.ToString("D2") + Startzeit.Second.ToString("D2"));
        }
    }
}
