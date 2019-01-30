

#include "wifi_ss.h"



bool	wifi_disconnecting = false;

/*--------------------------------------------------*/

void wifi_begin(){

  WiFi.mode(WIFI_STA);
  WiFi.onEvent( wifi_event );
  // Wait for connection
  for ( int i=3, kill = 0; WiFi.status() != WL_CONNECTED; ++i, ++kill ) {
    if ( i > 2 ) {
      WiFi.begin(ssid, password);
      i = 0;
    }
    delay(500);
    Serial.print(".");
	if ( kill > 20){
		Serial.flush();
		ESP.restart(); 
	}
  }
  wifi_disconnecting = false; 
  
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}

/*--------------------------------------------------*/

void wifi_event ( WiFiEvent_t event ){

	int do_reconnect = 1;

	switch( event ){
       case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Disconnected from WiFi access point");
            break;
       case SYSTEM_EVENT_STA_LOST_IP:
            Serial.println("Lost IP address and IP address is reset to 0");
            break;
	   case 200:
	        Serial.println("Beacon time out");
            break;
	   case 202:
	        Serial.println("WiFi Auth fail");
            break;
	   default:
			do_reconnect = 0;
	}

	if ( do_reconnect && ! wifi_disconnecting ){
		wifi_disconnecting = true;	
		WiFi.disconnect();

		wifi_begin();		
	}		

}

/*--------------------------------------------------*/
