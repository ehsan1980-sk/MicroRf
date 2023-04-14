// Placed in the public domain by Earle F. Philhower, III, 2022

#include <WiFi.h>
#include <Servo.h>

#ifndef STASSID
#define STASSID "HOME"
#define STAPSK "castrowifi"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

int port = 8080;

int pins[] = {12, 11, 10, 9, 8};
Servo s[5]; 

WiFiServer server(port);

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setHostname("PicoW2");
  Serial.printf("Connecting to '%s' with '%s'\n", ssid, password);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.printf("\nConnected to WiFi\n\nConnect to server at %s:%d\n", WiFi.localIP().toString().c_str(), port);

  server.begin();
  for(int i=0;i<5; i++)
    s[i].attach(pins[i]); 

    test();
}
void ser(int id, int v){
  if(id >= 5 or id < 0) return;
  if(v < 0 or v > 180) return;
   s[id].write(v);     
}

void test(){
    for(int i=0;i<5; i++){
      ser(i, 90);
    }
    delay(111);
    for(int i=0;i<5; i++){
      ser(i, 170);
      
    delay(111);
    }
    delay(111);
    for(int i=0;i<5; i++){
      ser(i, 30);
    delay(111);
    }
    delay(111);
}

void process(String dat){
  Serial.print("Input: ");
  Serial.println( dat);
  char in[] = {'a', 'b', 'c', 'd', 'e', 'f'};
   for(int i=0;i<5; i++){
    int st = dat.indexOf(in[i]);
    int en = dat.indexOf(in[i+1]);
      String v = dat.substring(st+1, en); 
      int vi = v.toInt();
  Serial.print(i);
  Serial.print(" : ");
  Serial.println( vi);
      ser(i, vi);
  }
}
void loop() {
  static int i;
  delay(1000);
  Serial.printf("--loop %d\n", ++i);
  delay(10);
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  client.println("Type anything and hit return");
  while (!client.available()) {
    delay(10);
  }
  String req = client.readStringUntil('\n');
  process(req);
  Serial.println(req);
  client.printf("Hello from Pico-W\r\n");
  client.flush();
}
