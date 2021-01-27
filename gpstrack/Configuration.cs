﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace gpsTrack
{
    public class Configuration
    {
        /**
         * Syntax: <prog> <quelle> <ziel> /o(verwrite) /d(aily)
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

        static readonly string[] usage =
        {
            "",
            "Syntax:",
            "{0} <Eingabedatei[.dat]> [<Ausgabedatei[.gpx]> [/o(verwrite)] [/r(ename)]",
            " - oder - ",
            "{0} <Quellverzeichnis> [Zielverzeichnis] [/o(verwrite)] [/d(aily)] [/r(ename)]",
            "",
            "Hinweise:",
            " - ist die Quelle eine Datei und es wurde keine Ausgabedatei angegeben, so wird eine Datei",
            "   mit dem Namen der Quelldatei und der Endung .gpx angelegt. Existiert bereits eine",
            "   Datei mit diesem Namen, wird sie nicht überschrieben, außer es wurde die Option /f",
            "   angegeben.",
            " - ist die Quelle eine Datei und als Ziel wurde ein Verzeichnis angegeben, wird dort eine",
            "   Datei mit dem Namen der Quelldatei und der Endung .gpx angelegt. Existiert dort bereits eine",
            "   Datei mit diesem Namen, wird sie nicht überschrieben, außer es wurde die Option /f",
            "   angegeben.",
            " - ist die Quelle ein Verzeichnis und es wurde kein Zielverzeichnis angegeben, werden/wird",
            "   die Zieldatei(en) im Quellverzeichnis mit der/den Endung(en) .gpx angelegt. Bereits",
            "   existierende Dateien werden nicht überschrieben, außer es wurde die Option /f angegeben.",
            " - /d(aily): mehrere Dateien eines Tages werden in einer Track-Datei zusammengefasst.",
            "             Der Name der Zieldatei wird aus dem Tagesdatum gebildet.",
            " - /r(ename): Quelldateien, die als Datum sechs Nullen zeigen, werden entsprechend umbenannt,",
            "              wenn das Datum aus den Daten in der jeweiligen Datei ermittelt werden kann."
        };

        public bool SourceIsDirectory = false;
        public bool TargetIsDirectory = false;
        public bool ForceOverwrite = false;
        public string SourcePath = null;
        public string TargetPath = null;
        public bool DailyConnect = false;
        public bool IsValid = false;
        public bool Rename = false;
        public List<BinaryTrackFile> SourceFiles;
        //public BinaryTrackFile[] SourceFiles = { };

        public Configuration(string[] args)
        {
            if (args.Length < 1 || args.Length > 5)
            {
                PrintUsage("Fehler: Falsche Anzahl Parameter.");
                return;
            }
            for (int i = 0; i < args.Length; i++)
            {
                //if (args[i].Equals("/d") || args[i].Equals("/daily")) DailyConnect = true;
                //else if (args[i].Equals("/o") || args[i].Equals("/overwrite")) ForceOverwrite = true;
                //else if (args[i].Equals("/r") || args[i].Equals("/rename")) Rename = true;
                //else if (args[i].StartsWith("/"))


                Console.WriteLine("Argument {0} = {1}", i, args[i]);
                // mit folgender Abfrage ist "/d", "/daily" und jede Zwischenform möglich (also auch "/dai" und "/dail", etc.)
                if (args[i].StartsWith("/d", StringComparison.OrdinalIgnoreCase) && "/daily".StartsWith(args[i], StringComparison.OrdinalIgnoreCase))
                    DailyConnect = true;
                else if (args[i].StartsWith("/o", StringComparison.OrdinalIgnoreCase) && "/overwrite".StartsWith(args[i], StringComparison.OrdinalIgnoreCase))
                    ForceOverwrite = true;
                else if (args[i].StartsWith("/r", StringComparison.OrdinalIgnoreCase) && "/rename".StartsWith(args[i], StringComparison.OrdinalIgnoreCase))
                    Rename = true;
                else if (args[i].StartsWith("/"))
                {
                    PrintUsage("Unbekannte Option: " + args[i]);
                    return;
                }
                else if (String.IsNullOrEmpty(SourcePath)) SourcePath = args[i];
                else TargetPath = args[i];
            }
            if (String.IsNullOrEmpty(SourcePath))
            {
                PrintUsage("Fehler: Quelldatei(en) oder -verzeichnis wurde nicht angegeben.");
                return;
            }

            // Eingabedatei(en) lesen:
            String path = Path.GetDirectoryName(SourcePath);
            if (String.IsNullOrWhiteSpace(path)) path = ".";
            path = Path.GetFullPath(path);

            //String pattern = Path.GetFileName(SourcePath);

            DirectoryInfo di = new DirectoryInfo(path);
            FileInfo[] filelist = di.GetFiles(Path.GetFileName(SourcePath));
            if (filelist.Length == 0)
            {
                PrintUsage("Keine Quelldatei(en) mit angegebenem Namen oder Muster gefunden.");
                return;
            }

            // wenn Ziel nicht angegeben, nutzen wir als Ausgabeverzeichnis das Quellverz.
            if (String.IsNullOrEmpty(TargetPath))
            {
                //TargetPath = Path.GetDirectoryName(SourcePath);
                TargetPath = path;
            }
            if (Directory.Exists(TargetPath))
            {
                TargetIsDirectory = true;
            }
            else if (File.Exists(TargetPath) && !ForceOverwrite)
            {
                PrintUsage("Zieldatei existiert und die Option /o(verwrite) wurde nicht angegeben.");
                return;
            }
            // hier weiß ich, ob Ziel ein Verzeichnis oder Datei ist

            SourceFiles = new List<BinaryTrackFile>();
            foreach(FileInfo fi in filelist)
            {
                BinaryTrackFile btFile = new BinaryTrackFile(fi);
                if (btFile.IsValid)
                    SourceFiles.Add(btFile);
                //SourceFiles.Append(new BinaryTrackFile(fi));
            }

            // aufsteigend sortieren (nach 1. Fixpunkt der Trackdatei (= Name der Datei)):
            SourceFiles.Sort((f1, f2) => f1.Startzeit.CompareTo(f2.Startzeit));
            //Array.Sort(SourceFiles, (f1, f2) => f1.Startzeit.CompareTo(f2.Startzeit));



            // da bis hier ja alles gut gegangen ist...
            IsValid = true;
        }

        public void PrintUsage(string message)
        {
            string AppName = new FileInfo(Assembly.GetExecutingAssembly().Location).Name;

            if (!String.IsNullOrEmpty(message)) Console.WriteLine(message);
            for (int i = 0; i < usage.Length; i++)
                Console.WriteLine(usage[i], AppName);
        }
    }
}
