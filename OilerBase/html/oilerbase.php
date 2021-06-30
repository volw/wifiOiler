<?php
// DO NOT COPY CONTENT VIA CLIPBOARD TO EDITOR IN TERMINAL
// YOU WILL PROBABLY GET TROUBLE WITH LINE END / LINE FEED CHARS

define("DEBUG",       false);	// file download only when false
define("QUIET_MODE",  false);	// good, when running in docker and logs are re-directed to standard out/error

define("USER_AGENT",  "wifiOiler");
define("MY_LOG_FILE", "/var/www/log/myphp.log");
define("UPLOAD_DIR",  "/var/www/uploads/");
define("CHUNK_SIZE",  1024*1024); // Size (in bytes) of tiles chunk
define("BASE_PATH",   "/var/www/update/");
define("MAX_VERSION", "zzzzzzzzzz");

function mylog($meldung) {
	if (QUIET_MODE) return;

  //global MY_LOG_FILE;
  //error_log(date("d.m.Y-H:i:s") . ": " . $meldung . "\n", 3, MY_LOG_FILE);
  //using apache log:
	if (DEBUG)
  	echo $meldung . "<br>";
  else
  	error_log(date("d.m.Y-H:i:s") . ": " . $meldung . "\n", 3, MY_LOG_FILE);
  	//error_log($meldung, 4); // 4 = using SAPI logging handler
}

function startsWith($haystack, $needle) {
  if (strlen($haystack) < strlen($needle)) return false;
  $length = strlen($needle);
  return (substr($haystack, 0, $length) === $needle);
}
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
    //ob_flush();  // gibt Fehlermeldung, dass hier nichts zu flushen ist...
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
// "board", "version"
function updateinfo($board, $version) {
	if (DEBUG) {
		mylog("board = " . $board);
		mylog("version = " . $version);
	}
	// $version = aktuelle Version des Oilers
  $basedir = BASE_PATH.$board;
  mylog("update: update info requested; Board=".$board."; Version=".$version."; base path=".$basedir);

	if (!file_exists($basedir)) {
		if (DEBUG){
			mylog("ERROR: can't find requested board directory for update, check 'board' in url");
      http_response_code(200);
    } else {
      http_response_code(404);
    }
    return;
	}
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
}
// "file"
function download($file) {	// download request from client
  $filename = BASE_PATH.$file;
  mylog("update: file requested: " . $filename . " basename=" . basename($file));
  if (file_exists( $filename ))
  {
    // $mimetype = "mime/type";
    $mimetype = "application/octet-stream";
    http_response_code(200);
    header("Content-Type: ".$mimetype );
    header("Content-Length: " . filesize($filename));
    header("Content-Disposition: filename=".basename($file).";");
    readfile_chunked($filename);
  } else {
    http_response_code(404);
  }
}
// "filename"
function fileexists($filename) {
	$uploadfile = UPLOAD_DIR . $filename;

	if (file_exists($uploadfile)) {
	   mylog("File check initiated for: '" . $uploadfile . "' and :-)");
	   http_response_code(200);
	} else {
	   mylog("File check initiated for: '" . $uploadfile . "' and :-(");
	   http_response_code(404);    // "Not Found"
	}
}
// count($_FILES) > 0
function upload() {	// get files from client
  $filecount = count($_FILES);
  $uploadfile = UPLOAD_DIR . basename($_FILES['userfile']['name']);
  $fileexists = file_exists($uploadfile);

  mylog("upload file temp name/location: " . $_FILES['userfile']['tmp_name']);
  mylog("... target name of file: " . $uploadfile);
  if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
    if ($fileexists) {
      mylog("Erfolgreicher Upload, existierende Datei wurde überschrieben: '{$uploadfile}'");
    } else {
      mylog("Erfolgreicher Upload, neue Datei wurde angelegt: '{$uploadfile}'");
    }
  	http_response_code(200);
  } else {
  	http_response_code(404);
    mylog("Fehler beim Verschieben der Datei: '{$uploadfile}'");
  }
}

if (!startsWith($_SERVER['HTTP_USER_AGENT'], USER_AGENT)) {
   mylog("No oiler agent: " . $_SERVER['HTTP_USER_AGENT']);
   http_response_code(200);
   echo "OK";
} elseif (isset($_GET["board"]) && isset($_GET["version"])) {
	updateinfo($_GET["board"], $_GET["version"]);
} elseif (isset($_GET["file"])) {
	download($_GET["file"]);
} elseif (isset($_GET["filename"])) {
	fileexists($_GET["filename"]);
} elseif (count($_FILES) > 0) {
	upload();
} else {	// simple alive confirmation
	http_response_code(200);
	echo "OK";
}
?>
