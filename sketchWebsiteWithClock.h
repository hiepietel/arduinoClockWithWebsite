#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <DS3231.h>

const char* ssid = "ssid";
const char* password = "password";
bool showHour;
bool showDate;
bool showTemp;
int setBrightness;
int setChangeTime;
int alarmTab[5][2]={{-1,-1},{-1,-1},{-1,-1},{-1,-1},{-1,-1}}; 
DS3231 clock;
RTCDateTime dt;
ESP8266WebServer server(80);

const int led = 2;
const int led0 =0;
String prepareHtmlPageFirst(bool refresh)
{
  String htmlPage =
            String ("<!DOCTYPE HTML>") +
            "<html lang=\"en\">"
            "<head>"+
            "<title>kupa</title>"+
             "<meta charset=\"utf-8\">"+
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
             if(refresh==true)htmlPage+=String("<meta http-equiv='Refresh' content='1' />");
             //htmlPage+=(refresh==true)?"<meta http-equiv='Refresh' content='5' />":"";
             htmlPage+=String("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">")+
             "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.2.0/jquery.min.js\"></script>"+
             "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>"+
            "</head>";
  return htmlPage;
}
String prepareHtmlPageSecond()
{
  String htmlPage =
            String("</html>") +
            "\r\n";
            return htmlPage;
}
String prepareIndexPage()
{
    dt = clock.getDateTime();
    //htmlPage="Refresh: 5"+htmlPage;
    String htmlPage =
            String("<body>")+
            "<div class='container-fluid' style='height:100%;'>"+
            "<nav class=\"navbar navbar-default\">"+
            "<div class='navbar-header'>"+
            "<a class=\"navbar-brand\" href=\"#\">MainPage</a>"+
            "<a class=\"navbar-brand\" href=\"Alarms\">Alarms</a>"+
            "<a class=\"navbar-brand\" href=\"Settings\">Settings</a>"+
            "</nav>"+
            "</div>"+
            "<div style='text-align:center; padding: 1%;'><h1>My First ESP8266 clock project</h1></div>"+
            //"<p>This is some text.</p> "+
            "<div class='panel panel-default' style='padding:10%;'>"+
            "<div class='panel-heading'>Time</div>"+
            "<div class='panel-body' style='text-align:center'><h4>";
            htmlPage+=(dt.hour>9)?String(dt.hour)+":":("0"+String(dt.hour))+":";
            htmlPage+=(dt.minute>9)?String(dt.minute)+":":"0"+String(dt.minute)+":";
            htmlPage+=(dt.second>9)?String(dt.second):("0"+String(dt.second));
            htmlPage+=String("</h4></div>")+
            "<div class='panel-heading'>Date</div>"+
            "<div class='panel-body' style='text-align:center'><h4>";
            htmlPage+=(dt.day>9)?String(dt.day)+".":("0"+String(dt.day))+".";
            htmlPage+=(dt.month>9)?String(dt.month)+".":"0"+String(dt.month)+".";
            htmlPage+=String(dt.year)+
            "</h4></div>"+
            "<div class='panel-heading'>Temperature</div>"+
            "<div class='panel-body' style='text-align:center'><h4>";
            clock.forceConversion();
            //int t1=clockTemp.readTemperature()/10;
            htmlPage+=String(clock.readTemperature())+"&degC"+
            "</h4></div>"+
            "</div>"+
            //String(dt.hour)+":"+String(dt.minute)+":"+(dt.second)+"<br>"+
            //String(dt.day)+"."+String(dt.month)+"."+(dt.year)+"<br>"+
            //String(
            "</div>"+
            "</body>";
            return htmlPage;
}
String prepareAlarmsPage(String message)
{String htmlPage=String("<body>")+
            "<div class=\"container-fluid style='height:100%;\">"+
            "<nav class=\"navbar navbar-default\">"+
            "<div class=\"navbar-header\">"+
            "<a class=\"navbar-brand\" href=\"index\">MainPage</a>"+
            "<a class=\"navbar-brand\" href=\"Alarms\">Alarms</a>"+
            "<a class=\"navbar-brand\" href=\"Settings\">Settings</a>"+
            "</nav>"+
            "</div>"+
            "<div style='text-align:center; padding: 1%;'><h1>Alarms</h1></div>"+
            "<div class='row' style='padding:5%;'>"+
            "<form action='Alarms' method='POST'>"+
            "<div class='col-xs-12' style='text-align:center'><h4><b>Add alarm:</b></h4></div>"+
            "<div class='col-xs-6'><div class='input-group'><span class='input-group-addon'>Hour:</span><input id='hour' type='number'  min='0' max='23' step='1' class='form-control' name='hourName'></div></div>"+
            "<div class='col-xs-6'><div class='input-group'><span class='input-group-addon'>Minute:</span><input id='minute' type='number' min='0' max='59' step='1' class='form-control' name='minuteName'></div></div>"+
            "<div class='col-xs-12'><button type='button submit' name='' value='' class='btn btn-success btn-lg btn-block'>Set active</button></div>"+ 
            "</form>"+
             "</div>"+
             "<div class='row' style='padding:5%;'>"+
             "<div class='col-xs-12' style='text-align:center'><h4><b>Your alarms: [max 5]</b></h4></div>";
             
             for (int i=0; i<5; i++)
             {
              if(alarmTab[i][0]==-1 && alarmTab[i][1]==-1){;}
              else {
                htmlPage+=String("<div class='col-xs-7'><h5 style='text-align:right;'><b>")+alarmTab[i][0]+":"+alarmTab[i][1]+"</b></h5></div>"+
                "<div class='col-xs-5'><form action='Alarms' method='POST'><button type='button submit' name='button"+i+"' value='del' class='btn btn-danger btn-sm btnoc-blk'>delete</button></form></div>";
              }
              
             }
             htmlPage+=String("</div>")+
            "</div>"+
            "</body>";
             
        return htmlPage;  
}

String prepareSettingsPage()
{           String htmlPage =
            String("<body>")+
            "<div class=\"container-fluid style='height:100%;\">"+
            "<nav class=\"navbar navbar-default\">"+
            "<div class=\"navbar-header\">"+
            "<a class=\"navbar-brand\" href=\"index\">MainPage</a>"+
            "<a class=\"navbar-brand\" href=\"Alarms\">Alarms</a>"+
            "<a class=\"navbar-brand\" href=\"Settings\">Settings</a>"+
            "</nav>"+
            "</div>"+
            "<div style='text-align:center; padding: 1%;'>Settings</h1></div>"+
            //"<p>Settings</p> "+
            "<div style='padding:10%;'><form action='Settings' method='POST'>"+
            "<div class='row'><div class='col-xs-12'>Check what will be shwon: <br></div></div>"+
            "<div class='row'>";
            //"Hour:";
            if(showHour==true){
                 htmlPage+=String("<div class='col-xs-4'>Hour:</div>")+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showHour' value='yes' checked>ON</label></div>"+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showHour' value='no'>OFF</label></div>";
                 //"</div>";
            }
            else{
            htmlPage+=String("<div class='col-xs-4'>Hour:</div>")+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showHour' value='yes'>ON</label></div>"+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showHour' value='no' checked>OFF</label></div>";
            //"</div>";
            }
            htmlPage+=String("</div>")+
            "<div class='row'>";
            //htmlPage+=String("</div>")
            if(showDate==true){
                 htmlPage+=String("<div class='col-xs-4'>Date:</div>")+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showDate' value='yes' checked>ON</label></div>"+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showDate' value='no'>OFF</label></div>";
                 //"</div>";
            }
            else{
            htmlPage+=String("<div class='col-xs-4'>Date:</div>")+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showDate' value='yes'>ON</label></div>"+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showDate' value='no' checked>OFF</label></div>";
            //"</div>";
            }
            htmlPage+=String("</div>")+
            "<div class='row'>";
            //"Temp:";
            if(showTemp==true){
                 htmlPage+=String("<div class='col-xs-4'>Temperaure:</div>")+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showTemp' value='yes' checked>ON</label></div>"+
                 "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showTemp' value='no'>OFF</label></div>";
                 //"</div>";
            }
            else{
            htmlPage+=String("<div class='col-xs-4'>Temperature:</div>")+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showTemp' value='yes'>ON</label></div>"+
            "<div class='col-xs-4'><label class='radio-inline'><input type='radio' name='showTemp' value='no' checked>OFF</label></div>";
            //"</div>";
            }
            htmlPage+=String("</div>")+
            "<div class='row'><div class='col-xs-12'>Set Brightness:<br></div></div>"+
            "<div class='row'>"+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setBrightness' value='0'"; htmlPage+=(setBrightness==0)?String(" checked"):String(""); htmlPage+=String(">50%</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setBrightness' value='1'"; htmlPage+=(setBrightness==1)?String(" checked"):String(""); htmlPage+=String(">75%</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setBrightness' value='2'"; htmlPage+=(setBrightness==2)?String(" checked"):String(""); htmlPage+=String(">100%</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setBrightness' value='3'"; htmlPage+=(setBrightness==3)?String(" checked"):String(""); htmlPage+=String(">auto</label></div>")+
            "</div>"+
            "<div class='row'><div class='col-xs-12'>Set change time: [sek]<br></div></div>"+
            "<div class='row'>"+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setChangeTime' value='0'"; htmlPage+=(setChangeTime==0)?String(" checked"):String(""); htmlPage+=String(">5</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setChangeTime' value='1'"; htmlPage+=(setChangeTime==1)?String(" checked"):String(""); htmlPage+=String(">10</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setChangeTime' value='2'"; htmlPage+=(setChangeTime==2)?String(" checked"):String(""); htmlPage+=String(">15</label></div>")+
            "<div class='col-xs-3'><label class='radio-inline'><input type='radio' name='setChangeTime' value='3'"; htmlPage+=(setChangeTime==3)?String(" checked"):String(""); htmlPage+=String(">20</label></div>")+
            "</div>"+
            /*"<div class=\"checkbox\">";
            if(showHour==true)
            {
              htmlPage+=String("<label><input type=\"checkbox\" name='showHour' value='yes' checked>Hour</label>");
            }
            else{ htmlPage+=String("<label><input type=\"checkbox\" name='showHour' value='yes' checked>Hour</label>");}
            //"<div class=\"checkbox\">"+
            //"<label><input type=\"checkbox\" name='showHour' value='yes' >Hour</label>"+
            */
           // htmlPage+=String("</div>")+
            //"<button type=\"button\" class=\"btn\" name=\"confirm\">Confirm</button>"+
            //"<form action='Settings' method='POST'>"+
            "<div style='padding:10%;'><button type='button submit' name='' value='' class='btn btn-success btn-lg btnoc-blk'>Confirm</button></div>"+
            "</div>"+
            "</body>";
       

  return htmlPage;
}
void handleRoot() {
 // digitalWrite(led, 1);
  server.send(200, "text/html", (prepareHtmlPageFirst(true)+prepareIndexPage()+prepareHtmlPageSecond()));
 // digitalWrite(led, 0);
}
void showAlarms()
{
   String hour="", minute="", message="";
   if ( server.hasArg("hourName") && server.hasArg("minuteName")) {
    hour = server.arg("hourName");
     minute = server.arg("minuteName");

     
     Serial.println(hour+":"+minute);
     if (hour=="" || minute=="") {;}
     else{
     int h=hour.toInt();
     int m=minute.toInt();
      for (int i=0; i<5; i++)
    {
      if(alarmTab[i][0]==-1 && alarmTab[i][1]==-1)
        {alarmTab[i][0]=h;
        alarmTab[i][1]=m;
        message="alarmAdded";
        break;
        }
      else {message="no more alarms avaible";}
      Serial.println(message);  
    }
     }
   }
   //
      if ( server.hasArg("button0") ) {
        if (server.arg("button0")=="del")
      {
        Serial.println("usuwamy button0"); 
        alarmTab[0][0]=-1;
        alarmTab[0][1]=-1;
      }
       }
        if ( server.hasArg("button1") ) {
        if (server.arg("button1")=="del")
      {
        Serial.println("usuwamy button1"); 
        alarmTab[1][0]=-1;
        alarmTab[1][1]=-1;
      }
       }
        if ( server.hasArg("button2") ) {
        if (server.arg("button2")=="del")
      {
        Serial.println("usuwamy button2"); 
        alarmTab[2][0]=-1;
        alarmTab[2][1]=-1;
      }
       }
        if ( server.hasArg("button3") ) {
        if (server.arg("button3")=="del")
      {
        Serial.println("usuwamy button3"); 
        alarmTab[3][0]=-1;
        alarmTab[3][1]=-1;
      }
       }
    if ( server.hasArg("button4") ) {
        if (server.arg("button4")=="del")
      {
        Serial.println("usuwamy button4"); 
        alarmTab[4][0]=-1;
        alarmTab[4][1]=-1;
      }
       }
    //int r=1;
    //if(typeId(m) ==typeId(r)){ Serial.println("isInt";}
    
    //if(h<24 && h>=0 && m<60 && m>=0)
     //{Serial.print(h+":"+m);
     // message="Alarm added"; }  
     //else {message="try again";} 
    
   
   
  server.send(200, "text/html", (prepareHtmlPageFirst(false)+prepareAlarmsPage(message)+prepareHtmlPageSecond()));
}
void showSettings()
{
    if ( server.hasArg("showHour") ) {
      //Serial.println("showHour");
      String checkbox = server.arg("showHour");
     if (checkbox=="yes"){
      showHour=true;
      //digitalWrite(led0, HIGH);
      //Serial.println("h_YES");
      }
     else if (checkbox=="no"){
      //Serial.println("h_NO");
      //digitalWrite(led0, LOW);
      showHour=false;
      } 
     }
     if ( server.hasArg("showDate") ) {
      //Serial.println("showDate");
      String checkbox = server.arg("showDate");
     if (checkbox=="yes"){
      showDate=true;
      //digitalWrite(led0, HIGH);
      //Serial.println("h_YES");
      }
     else if (checkbox=="no"){
      //Serial.println("h_NO");
      //digitalWrite(led0, LOW);
      showDate=false;
      } 
     }
      if ( server.hasArg("showTemp") ) {
      //Serial.println("showTemp");
      String checkbox = server.arg("showTemp");
     if (checkbox=="yes"){
      showTemp=true;
      //digitalWrite(led0, HIGH);
      //Serial.println("h_YES");
      }
     else if (checkbox=="no"){
      //Serial.println("h_NO");
      //digitalWrite(led0, LOW);
      showTemp=false;
      } 
     }
     if ( server.hasArg("setBrightness") ) {
        int checkbox =(server.arg("setBrightness")).toInt();
        //int check=int(checkbox);
        switch(checkbox)
        {
          case 0:
          setBrightness=0;
          break;
          case 1:
          setBrightness=1;
          break;
          case 2:
          setBrightness=2;
          break;
          case 3:
          setBrightness=3;
          break;
        }
     }
          if ( server.hasArg("setChangeTime") ) {
        int checkbox =(server.arg("setChangeTime")).toInt();
        //int check=int(checkbox);
        switch(checkbox)
        {
          case 0:
          setChangeTime=0;
          break;
          case 1:
          setChangeTime=1;
          break;
          case 2:
          setChangeTime=2;
          break;
          case 3:
          setChangeTime=3;
          break;
        }
     }
     /*
    if ( server.hasArg("D6") ) {
      Serial.print("hasArg d6");
      Serial.println(String(server.arg("D6")));
    const char* D6= "0";
      if (server.arg("D6")=="0")
      {
        Serial.println("checked db6"); 
        digitalWrite(led, 1);
      }
      }
      */
    
  //else {digitalWrite(led, 1);}
  server.send(200, "text/html", (prepareHtmlPageFirst(false)+prepareSettingsPage()+prepareHtmlPageSecond()));
}



void setup(void){
  pinMode(led, OUTPUT);
  pinMode(led0, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Initialize DS3231");;
  clock.begin();
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  server.on("/", handleRoot);
  server.on("/index", handleRoot);
  server.on("/Alarms", showAlarms);
  server.on("/Settings", showSettings);
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  //dt = clock.getDateTime();
  server.handleClient();
  if(showHour==true)
  { 
    digitalWrite(led, HIGH);
  }
  else {digitalWrite(led, LOW);}
    if(showDate==true)
  { 
    digitalWrite(led0, HIGH);
  }
  else {digitalWrite(led0, LOW);}
}
void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}
