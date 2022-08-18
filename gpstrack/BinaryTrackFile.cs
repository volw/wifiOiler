using System;
using System.IO;

namespace gpsTrack
{
    public class BinaryTrackFile
    {
        public String Name = "";
        public String Pfad = "";
        public String Prefix = "";
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

            // Datum und Zeit aus Dateinamen bestimmen
            int nPos = Name.LastIndexOf('-');   // zero based!
            
            if (Name.Length-nPos == 9 && nPos > 5)  // neues Format
            {   // <Prefix>JJMMTT-HHMM.dat
                isNewNameFormat = true;
                Jahr = 2000 + int.Parse(Name.Substring(nPos-6, 2));
                Monat = int.Parse(Name.Substring(nPos-4, 2));
                Tag = int.Parse(Name.Substring(nPos-2, 2));
                Stunde = int.Parse(Name.Substring(nPos+1, 2));
                Minute = int.Parse(Name.Substring(nPos+3, 2));
                Sekunde = 0;
                Prefix = Name.Substring(0, nPos - 6);
            }
            else if (Name.Length - nPos == 11)  // altes Format
            {   // JJMMTT-HHMMSS.dat
                Jahr = 2000 + int.Parse(Name.Substring(nPos-6, 2));
                Monat = int.Parse(Name.Substring(nPos-4, 2));
                Tag = int.Parse(Name.Substring(nPos-2, 2));
                Stunde = int.Parse(Name.Substring(nPos+1, 2));
                Minute = int.Parse(Name.Substring(nPos+3, 2));
                Sekunde = int.Parse(Name.Substring(nPos+5, 2));
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
                return Pfad + Path.DirectorySeparatorChar + OnlyName() + ".dat";
        }

        public String DateString()
        {
            return (Startzeit.Year.ToString("D2").Substring(2) + Startzeit.Month.ToString("D2") + Startzeit.Day.ToString("D2"));
        }

        public String OnlyName()
        {
            if (isNewNameFormat)
                return (Prefix + DateString() + "-" + Startzeit.Hour.ToString("D2") + Startzeit.Minute.ToString("D2"));
            else
                return (DateString() + "-" + Startzeit.Hour.ToString("D2") + Startzeit.Minute.ToString("D2") + Startzeit.Second.ToString("D2"));
        }
    }
}
