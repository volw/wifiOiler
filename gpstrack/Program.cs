using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Reflection;

namespace gpsTrack
{
    public class Program
    {
        static void Main(string[] args)
        {
            Configuration conf = new Configuration(args);
            if (conf.IsValid)
            {
                /**
                 * Quelle: 
                 * - Q1: (einzelne) Datei
                 * - Q3: Dateisuchmuster (z.B. "D:\temp\*.dat")
                 * Ziel:
                 * - Z1: (einzelne) Datei (bei Q1, Q3 = alle zusammenfassen)
                 * - Z2: Verzeichnis (Q1 = Dateiname wie Quelle mit .gpx, Q3 = alle Dateien wie Quelle mit .gpx)
                 *
                 * overwrite: bei allen Möglichkeiten
                 * daily: bei Q3, Zieldateiname wird aus Tagesdatum gebildet
                 */

                /**
                  * foreach Sourcefile
                  *   bilde Ausgabedateiname
                  *   wenn anders als aktiver:
                  *     neuen Namen bilden, footer in alte Datei und schließen, neue öffnen und header schreiben
                  */
                String OutFilename = "";
                String oldOutFilename = "";
                StreamWriter outfile = null;
                long inFileCounter = 0;
                long outFileCounter = 0;
                gpsFix oldfix = new gpsFix();

                foreach (BinaryTrackFile trackFile in conf.SourceFiles)
                {
                    if (!conf.TargetIsDirectory)    // feste Ausgabedatei wurde angegeben - könnte also zum konnektieren kommen
                    {
                        OutFilename = conf.TargetPath;
                    }
                    else if (conf.DailyConnect)     // konnektieren gewünscht (auf täglicher Basis)
                    {
                        OutFilename = conf.TargetPath + Path.DirectorySeparatorChar + trackFile.DateString() + ".gpx";
                    }
                    else   // Einzelbehandlung - keine Konnektierung:
                    {
                        OutFilename = conf.TargetPath + Path.DirectorySeparatorChar + trackFile.OnlyName() + ".gpx";
                        //Console.WriteLine("Creating file '{0}'", OutFilename);
                    }
                    //-------------------
                    if (OutFilename != oldOutFilename)
                    {
                        // alte Datei schließen, neue öffnen
                        CloseOutFile(outfile);
                        outfile = OpenOutFile(OutFilename, conf.ForceOverwrite);
                        oldOutFilename = OutFilename;
                        outFileCounter++;
                    }

                    // hier jetzt conversion...
                    using (FileStream s = File.OpenRead(trackFile.CurrentFullPath()))
                    // and BinaryReader to read values
                    using (BinaryReader r = new BinaryReader(s))
                    {
                        inFileCounter++;
                        Console.WriteLine("Converting {0} ({1}) -> {2}", trackFile.Name, trackFile.Startzeit, OutFilename);

                        // stop when reached the file end
                        while (s.Position < s.Length)
                        {
                            try
                            {
                                gpsFix fix = new gpsFix(r);
                                double distance = fix.distanceBetween(oldfix);
                                //TODO: wenn der Abstand zu groß ist, könnte der Track aufgeteilt werden...
                                if (fix.IsValidTimestamp)
                                    outfile.WriteLine(fix.getGPXstring());
                                oldfix = fix;
                            }
                            catch (Exception e)
                            {
                                Console.WriteLine("ERROR READING FILE {0} - unerwartetes Ende des aufgezeichneten Tracks...{1} von {2}", trackFile.CurrentFullPath(), e.Message, e.Source);
                                foreach (String key in e.Data.Keys)
                                {
                                    Console.WriteLine("{0} = {1}", key, e.Data[key]);
                                }
                                Console.WriteLine("outfile = {0}", outfile.ToString());
                                Console.WriteLine(e.StackTrace);
                            }
                        }
                    }
                }
                CloseOutFile(outfile);
                Console.WriteLine("{0} Datei{1} konvertiert. {2} Ausgabedatei{3} erzeugt.", inFileCounter, inFileCounter != 1 ? "en" : "", outFileCounter, outFileCounter != 1 ? "en" : "");
                foreach (BinaryTrackFile trackFile in conf.SourceFiles)
                {
                    // überprüfen, ob Dateiname mit Startzeit übereinstimmt - wenn nicht, dann Datei umbenennen
                    if (!trackFile.CurrentFullPath().Equals(trackFile.StartTimeFullPath()))
                    {
                        if (conf.Rename)
                        {
                            Console.WriteLine("Datei '{0}' wird umbenannt in '{1}'", trackFile.CurrentFullPath(), trackFile.StartTimeFullPath());
                            if (File.Exists(trackFile.StartTimeFullPath()))
                            {
                                Console.WriteLine("Fehler: Datei schon vorhanden - Umbennung wird nicht vorgenommen... (geht ja auch nicht)");
                            }
                            else
                            {
                                File.Move(trackFile.CurrentFullPath(), trackFile.StartTimeFullPath());
                            }
                        }
                        else
                        {
                            Console.WriteLine("Warnung: die Datei '{0}' sollte so heißen: '{1}' - ggf. /rename Option nutzen...", trackFile.CurrentFullPath(), trackFile.StartTimeFullPath());
                        }
                    }
                }
            }
        }

        static StreamWriter OpenOutFile(String outfilename, bool overwrite)
        {
            
            if (File.Exists(outfilename) && !overwrite)
            {
                throw new IOException("FEHLER: Datei '"+outfilename+"' existiert bereits und Option /o(verwrite) war nicht angegeben!");
            }
            else
            {
                StreamWriter result = new StreamWriter(outfilename, false);
                writeXMLHeader(result);
                return result;
            }
        }

        static void CloseOutFile(StreamWriter sw)
        {
            if (sw != null)
            {
                writeXMLFooter(sw);
                sw.Flush();
                sw.Close();
                sw.Dispose();
            }
        }

        static void writeXMLHeader(StreamWriter fout)
        {
            fout.WriteLine("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>");
            fout.WriteLine("<gpx xmlns=\"http://www.topografix.com/GPX/1/1\" version=\"1.1\" creator=\"Wikipedia\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"");
            fout.WriteLine("    xsi:schemaLocation=\"http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd\">");
            fout.WriteLine("<metadata><desc>GPX-Track created with wifiOiler data</desc></metadata>");
            fout.WriteLine("<trk><name>Trackname1</name><desc>Trackbeschreibung</desc><trkseg>");
        }

        static void writeXMLFooter(StreamWriter fout)
        {
            fout.WriteLine("</trkseg></trk></gpx>");
        }
    }
}
