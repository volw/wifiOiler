<?php
// DO NOT COPY CONTENT VIA CLIPBOARD TO EDITOR IN TERMINAL
// YOU WILL PROBABLY GET TROUBLE WITH LINE END / LINE FEED CHARS

$USER_AGENT = 'wifiOiler';
$MYLOG      = "/var/www/log/myphp.log";
$uploaddir  = '/var/www/uploads/';

function mylog($meldung) {
  global $MYLOG;
  //error_log(date("d.m.Y-H:i:s") . ": " . $meldung . "\n", 3, $MYLOG);
  error_log($meldung, 4);
}

function startsWith($haystack, $needle)
{
  if (strlen($haystack) < strlen($needle)) return false;
  $length = strlen($needle);
  return (substr($haystack, 0, $length) === $needle);
}
?>
