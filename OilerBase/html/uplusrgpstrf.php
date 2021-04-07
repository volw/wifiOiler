<?php
// DO NOT COPY CONTENT VIA CLIPBOARD TO EDITOR IN TERMINAL
// YOU WILL PROBABLY GET TROUBLE WITH LINE END / LINE FEED CHARS

// In PHP < 4.1.0 sollte $HTTP_POST_FILES anstatt $_FILES verwendet werden

include 'global.php';

function main() {
  global $USER_AGENT, $uploaddir;
  
	/* DEBUG
	foreach (getallheaders() as $name => $value) {
    mylog("Header: $name: $value");
	}    	
	foreach ($_POST as $name => $value) {
    mylog("POST: $name: $value");
	}
	foreach ($_FILES['userfile'] as $name => $value) {
    mylog("File: $name: $value");
	}
	mylog("-------------------------");
	mylog(file_get_contents('php://input'));
	*/

  if (!startsWith($_SERVER['HTTP_USER_AGENT'], $USER_AGENT)) {
    mylog("Zugriff mit nicht autorisiertem Client");
    // 401: unauthorized
    http_response_code(404);
    return;
  } else {
    $filecount = count($_FILES);
    if ($filecount > 0) {
      $uploadfile = $uploaddir . basename($_FILES['userfile']['name']);
      $fileexists = file_exists($uploadfile);
    } else {
      mylog("No filename given (may be unauthorized use)");
			
    	http_response_code(404);
      return;
    }
  
    //echo "<pre>";
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
}

main();
?>
