<?php
// DO NOT COPY CONTENT VIA CLIPBOARD TO EDITOR IN TERMINAL
// YOU WILL PROBABLY GET TROUBLE WITH LINE END / LINE FEED CHARS

include 'global.php';

define("CHUNK_SIZE", 1024*1024); // Size (in bytes) of tiles chunk
define("BASE_PATH", "/var/www/update/");
define("MAX_VERSION", "zzzzzzzzzz");

// Read a file and display its content chunk by chunk
function readfile_chunked($filename, $retbytes = TRUE) {
  $buffer = "";
  $cnt =0;

  $handle = fopen($filename, "rb");
  if ($handle === false) {
    return false;
  }
  while (!feof($handle)) {
    $buffer = fread($handle, CHUNK_SIZE);
    echo $buffer;
    ob_flush();
    flush();
    if ($retbytes) {
      $cnt += strlen($buffer);
    }
  }
  $status = fclose($handle);
  if ($retbytes && $status) {
    return $cnt; // return num. bytes delivered like readfile() does.
  }
  return $status;
}

function main() {
  global $USER_AGENT;
  if (!startsWith($_SERVER['HTTP_USER_AGENT'], $USER_AGENT)) {
  //if (false) {
    mylog("update: Zugriff mit nicht autorisiertem Client");
    // 401: unauthorized
    http_response_code(404);
    return;
  } elseif (isset($_GET["board"]) && isset($_GET["version"])) {
    mylog("update: update info requested");
    $board   = $_GET["board"];
    $version = $_GET["version"];  // aktuelle Version des Oilers
    $basedir = BASE_PATH.$board;
    mylog("update: update info requested; Board=".$board."; Version=".$version."; base path=".$basedir);

    // Verzeichnisse checken, ob eine höhere Version vorhanden ist.
    // es muss die nächsthöhere sein, nicht die höchste:
    $topversion = MAX_VERSION;
    $dirHandle = dir($basedir);
    while (($f = $dirHandle->read()) != false) {
      if ($f != "." && $f != ".." && is_dir($basedir."/".$f)
      && ($f > $version && $f < $topversion)) {
        $topversion = $f;
      }
    }
    $dirHandle->close();
    if ($topversion != MAX_VERSION) {
      mylog("update: higher version found: ".$topversion);
      // creating response:
      $response = "";
      $firmwarefile = "";
      $dirHandle = dir($basedir . '/' . $topversion);
      mylog("update: collecting file names from: ".$basedir.'/'.$topversion);
      while (($f = $dirHandle->read()) != false) {
        if ($f != "." && $f != "..") {
          if (pathinfo($f, PATHINFO_EXTENSION) == "bin") {
            $firmwarefile = $board."/".$topversion."/".$f;
          }else{
            if (strlen($response) > 0) $response = $response.";";
            $response = $response.$board."/".$topversion."/".$f;
          }
        }
      }
      if (strlen($firmwarefile) > 0) {
        if (strlen($response) > 0) $response = $response.";";
        $response = $response.$firmwarefile;
      }
      if (strlen($response) > 0) {
        mylog("update: sending response: ".$response);
        http_response_code(200);
        echo $response;
      } else {
        http_response_code(404);
      }
    } else {
      mylog("update: keine neuere Version gefunden");
      http_response_code(404);
    }
  } elseif (isset($_GET["file"])) {
    $filename = BASE_PATH.$_GET["file"];
    mylog("update: file requested: " . $filename . " basename=" . basename($_GET["file"]));
    if (file_exists( $filename ))
    {
      // $mimetype = "mime/type";
      $mimetype = "application/octet-stream";
      http_response_code(200);
      header("Content-Type: ".$mimetype );
      header("Content-Length: " . filesize($filename));
      header("Content-Disposition: filename=".basename($_GET["file"]).";");
      readfile_chunked($filename);
    } else {
      http_response_code(404);
    }
  } else {
    // altes Verfahren:
    //$filename = "/var/www/update/wifiOiler.ino.d1_mini.bin";
    mylog("update nach altem Verfahren...");
    $filename = BASE_PATH."/wifiOiler.ino.nodemcu.bin";
    if (file_exists( $filename ))
    {
      // $mimetype = "mime/type";
      $mimetype = "application/octet-stream";
      http_response_code(200);
      header("Content-Type: ".$mimetype );
      header("Content-Length: " . filesize($filename));
      readfile_chunked($filename);
    }
  }
}

main();
?>
