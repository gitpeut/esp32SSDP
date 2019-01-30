#include "update_server.h"

/*-----------------------------------------------------------------------------------*/

const char* updatepage = 
"<script> function submit_file(oFormElement){\n\\
  var progdiv = document.getElementById('progdiv');\n\\
  var xhr = new XMLHttpRequest();\n\\   
  progdiv.innerHTML = 'Starting upload...';\n\\
    \n\\
    xhr.upload.addEventListener('progress', function(evt) {\n\\
    if (evt.lengthComputable) {\n\\
      var per = evt.loaded / evt.total;\n\\
      progdiv.innerHTML = 'progress: ' + Math.round(per*100) + '%';\n\\
    }\n\\
   }, false);\n\\
  xhr.onload = function(){ \n\\
    if ( xhr.status == 200) {\n\\
      progdiv.innerHTML = 'Upload finished';\n\\
    } else {\n\\
      progdiv.innerHTML = 'Error uploading file';\n\\
    }\n\\ 
  }\n\\ 
  xhr.open (oFormElement.method, oFormElement.action , true);\n\\
  xhr.send (new FormData (oFormElement));\n\\
  return false;\n\\
}\n\\
</script>\n\\
<form method='POST' action='/update' enctype='multipart/form-data' id='upload_form' onSubmit='return submit_file( this);'>\n\\
   <input type='file' name='update'>\n\\
   <input type='submit' value='Update'>\n\\
   </form>\n\\
   <div id='progdiv'></div>\n";

/*-----------------------------------------------------------------------------------*/
WebServer *update_server;

void update_begin(WebServer *server){
  update_server = server;
  update_server->on("/update", HTTP_GET,  update_ask  );
  update_server->on("/update", HTTP_POST, update_end, update_progress );

}

/*-----------------------------------------------------------------------------------*/

void update_ask(){
    update_server->sendHeader("Connection", "close");
    update_server->send(200, "text/html", updatepage );
}

/*-----------------------------------------------------------------------------------*/

void update_progress(){
    static int written_size=0;
    
    HTTPUpload& upload = update_server->upload();
    
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE ) {
      /* flashing firmware to ESP*/
      //written_size += upload.currentSize;
      //Serial.printf("Writing %d bytes, %d/%d\n", upload.currentSize, upload.totalSize, written_size );
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("\nUpdate Success: %u\nRebooting...\n", upload.totalSize);
        update_server->sendHeader("Connection", "close");
        update_server->send(200, "text/plain", "Upload successful, rebooting");
      } else {
        Update.printError(Serial);
        update_server->send(200, "text/plain", "Upload failed");
      }
    }  
}

void update_end(){
    update_server->sendHeader("Connection", "close");
    update_server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    Serial.printf("---Update %s\n", Update.hasError() ? "FAIL" : "OK");
    Serial.flush();
    ESP.restart(); 
}
