using System;
using System.Globalization;
using System.IO;
using System.Text;

namespace gpsTrack
{
    public class gpsFix
    {
        //           1         2         3         4         5
        // 0123    89012345        4567    2345    0123    89
        //     4567        67890123    8901    6789    4567
        public Int32 datum;   // 4
        public Int32 zeit;    // 4
        public double loclat; // 8
        public double loclng; // 8
        public float hdop;    // 4
        public float speed;   // 4 
        public float alt;     // 4
        public Int32 locage;  // 4
        public Int32 oldage;  // 4
        public float dist;    // 4
        public float msp;     // 4
        public Int16 pmode;   // 2

        public DateTime Timestamp;
        public bool IsValidTimestamp = false;

        CultureInfo Culture;

        public gpsFix()
        {
            this.Culture = new CultureInfo("en-US");
        }

        public gpsFix(BinaryReader r) : this()
        {
            this.datum = r.ReadInt32();     // (t)tmmjj
            this.zeit = r.ReadInt32();      // (h)hmmss
            this.loclat = r.ReadDouble();
            this.loclng = r.ReadDouble();
            this.hdop = r.ReadSingle();     
            this.speed = r.ReadSingle();
            this.alt = r.ReadSingle();
            this.locage = r.ReadInt32();
            this.oldage = r.ReadInt32();
            this.dist = r.ReadSingle();
            this.msp = r.ReadSingle();
            this.pmode = r.ReadInt16();

            int t = datum / 10000;
            int m = (datum - (t * 10000)) / 100;
            int j = 2000 + datum % 100;
            int hh = zeit / 1000000;
            int mm = (zeit - (hh * 1000000)) / 10000;
            int ss = (zeit % 10000) / 100;
            try
            {
                Timestamp = new DateTime(j, m, t, hh, mm, ss);
                IsValidTimestamp = true;
            }
            catch (Exception)
            {
                this.datum = 0;
                //TODO: wenn mehrere Fehler in Folge auftreten, wird u.U. eine falsche Datei (ein falsches Format) ausgewertet.
            }
        }

        public double distanceBetween(gpsFix otherFix)
        {
            // returns distance in meters between two positions, both specified
            // as signed decimal-degrees latitude and longitude. Uses great-circle
            // distance computation for hypothetical sphere of radius 6372795 meters.
            // Because Earth is no exact sphere, rounding errors may be up to 0.5%.
            // Courtesy of Maarten Lamers
            double radians(double degree)
            { return Math.PI * degree / 180.0; }

            double delta = radians(this.loclng - otherFix.loclng);
            double sdlong = Math.Sin(delta);
            double cdlong = Math.Cos(delta);
            double lat1 = radians(this.loclat);
            double lat2 = radians(otherFix.loclat);
            double slat1 = Math.Sin(lat1);
            double clat1 = Math.Cos(lat1);
            double slat2 = Math.Sin(lat2);
            double clat2 = Math.Cos(lat2);
            delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
            delta *= delta; // zum Quadrat
            delta += Math.Pow(clat2 * sdlong, 2);
            delta = Math.Sqrt(delta);
            double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
            delta = Math.Atan2(delta, denom);
            return delta * 6372795;
        }

        public String getGPXstring()
        {
            /*  Punkt wird als Track-punkt zurückgegeben (<trkpt>) - geht auch anders. Quelle: https://de.wikipedia.org/wiki/GPS_Exchange_Format
             *x <ele> xsd:decimal </ele>                     <!-- Höhe in m -->
             *x <time> xsd:dateTime </time>                  <!-- Datum und Zeit (UTC/Zulu) in ISO 8601 Format: yyyy-mm-ddThh:mm:ssZ -->
             *  <magvar> degreesType </magvar>               <!-- Deklination / magnetische Missweisung vor Ort in Grad -->
             *  <geoidheight> xsd:decimal </geoidheight>     <!-- Höhe bezogen auf Geoid -->
             *  <name> xsd:string </name>                    <!-- Eigenname des Elements -->
             *  <cmt> xsd:string </cmt>                      <!-- Kommentar -->
             *  <desc> xsd:string </desc>                    <!-- Elementbeschreibung -->
             *  <src> xsd:string </src>                      <!-- Datenquelle/Ursprung -->
             *  <link> linkType </link>                      <!-- Link zu weiterführenden Infos -->
             *  <sym> xsd:string </sym>                      <!-- Darstellungssymbol -->
             *  <type> xsd:string </type>                    <!-- Klassifikation -->
             *  <fix> fixType </fix>                         <!-- Art der Positionsfeststellung: none, 2d, 3d, dgps, pps -->
             *  <sat> xsd:nonNegativeInteger </sat>          <!-- Anzahl der zur Positionsberechnung herangezogenen Satelliten -->
             *x <hdop> xsd:decimal </hdop>                   <!-- HDOP: Horizontale Streuung der Positionsangabe -->
             *  <vdop> xsd:decimal </vdop>                   <!-- VDOP: Vertikale Streuung der Positionsangabe -->
             *  <pdop> xsd:decimal </pdop>                   <!-- PDOP: Streuung der Positionsangabe -->
             *  <ageofdgpsdata> xsd:decimal </ageofdgpsdata> <!-- Sekunden zwischen letztem DGPS-Empfang und Positionsberechnung -->
             *  <dgpsid> dgpsStationType:integer </dgpsid>   <!-- ID der verwendeten DGPS Station -->
             *  <extensions> extensionsType </extensions>    <!-- GPX Erweiterung -->
             */

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("<trkpt lat=\"{0}\" lon=\"{1}\">", Math.Round(this.loclat, 6).ToString(Culture), Math.Round(this.loclng, 6).ToString(Culture));
            sb.AppendFormat("<ele>{0}</ele>", this.alt.ToString(Culture));
            sb.AppendFormat("<hdop>{0}</hdop>", this.hdop.ToString(Culture));

            /*
            var output = Timestamp.ToString("yyyy-MM-ddTHH:mm:ssZ");
            String sDate = this.datum.ToString();
            //TODO: use Calendar.ToFourDigitYear(Int32) - not "20"
            sDate = "20" + sDate.Substring(sDate.Length - 2) + '-'
                 + sDate.Substring(sDate.Length - 4, 2) + '-'
                 + sDate.Substring(0, sDate.Length - 4);
            String sTime = this.zeit.ToString();
            sTime = sTime.Substring(0, sTime.Length - 6) + ':' + sTime.Substring(sTime.Length - 6, 2) + ':' + sTime.Substring(sTime.Length - 4, 2);

            sb.AppendFormat("<time>{0}T{1}Z</time>", sDate, sTime);
            */
            sb.AppendFormat("<time>{0}</time>", Timestamp.ToString("yyyy-MM-ddTHH:mm:ssZ"));
            //sb.AppendFormat("<time>{0}</time>", Timestamp.ToString("yyyy-MM-dd, HH:mm:ss"));
            sb.AppendFormat("<cmt>Msp={0:0.0};mode={1}</cmt>", msp, pmode);
            sb.Append("</trkpt>");

            return sb.ToString();
        }

    }
}
