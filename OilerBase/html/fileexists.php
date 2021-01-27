<?php
// DO NOT COPY CONTENT VIA CLIPBOARD TO EDITOR IN TERMINAL
// YOU WILL PROBABLY GET TROUBLE WITH LINE END / LINE FEED CHARS

include 'global.php';

if (!startsWith($_SERVER['HTTP_USER_AGENT'], $USER_AGENT)) {
   mylog("Zugriff mit nicht autorisiertem Client: " . $_SERVER['HTTP_USER_AGENT']);
   // 401: unauthorized
   http_response_code(404);
   return;
}

$uploadfile = $uploaddir . $_GET['filename'];
$fileexists = file_exists($uploadfile);

if ($fileexists) {
   mylog("File check initiated for: '" . $_GET['filename'] . "' and :-)");
   http_response_code(200);
} else {
   mylog("File check initiated for: '" . $_GET['filename'] . "' and :-(");
   http_response_code(404);    // "Not Found"
}
?>
