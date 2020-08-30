#define BLYNK_PRINT Serial
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//ด้านบนทั้งหมดคือ lib เอาใว้เรียกใช้งานฟังชั่น
TinyGPSPlus gps;  // ตั้งค่าคำสั่งgps

SoftwareSerial ss(4, 5); // เซ็ตช่องเสียบ gps
char auth[] = "qemf6HXKEK0nCMuaeb8Cw8U_cngDviZd"; //ที่อยู่ของ บลิ๊ง
const char* ssid = "vivo 1904"; // ชื่อ wifi
const char* pass = "12325812"; // รหัส wifi

float latitude , longitude; // สร้างตัวแปรเก็บค่าตำแหน่งลองติจูดละติจูด
int year , month , date, hour , minute , second; // ตัวแปรเก็บค่าเวลา
String date_str , time_str , lat_str , lng_str; // ตัวแปรเก็บค่าต่าวจากชนิดข้อมูลตัวเลขเป็นชนิดตัวอักษร
int pm; // หน่วยเวลา
WidgetMap myMap(V1); //ตัวแปรเก็บค่าตำแหน่งของบริ๊ง

WiFiServer server(80); //เซ็ตwifi
void setup() //ทำงานรอบเดียว
{
  Serial.begin(115200); //เซ็ตความเร็วในการอ่านค่า
  ss.begin(9600); // ความเร็วในการส่งค่า
  Blynk.begin(auth, ssid, pass); //เชื่อมบลิ๊งด้วยข้อมูลด้านบนที่กรอกใว้
  Serial.println(); 
  Serial.print("Connecting to ");
Serial.println(ssid);

  WiFi.begin(ssid, pass); //เชื่อม wifi จากข้อมูลด้านบน

  while (WiFi.status() != WL_CONNECTED) // loop เชื่อม wifi 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP()); //แสดง ip wifi

}

void loop() //ทำงานวนรอบ
{

  while (ss.available() > 0)
    if (gps.encode(ss.read())) //เช็กค่า gps
    {
if (gps.location.isValid()) //ถ้าเจอตำแหน่ง
      {
        latitude = gps.location.lat();
        lat_str = String(latitude , 6);
        longitude = gps.location.lng();
        lng_str = String(longitude , 6);
        //สี่บรรทัดบนคือแปลงค่าตะแหน่งจากรูปแบบตัวเลขเป็นตัวัอกษรเพื่อเตรียมส่งค่า
      }

      if (gps.date.isValid()) //เริ่มแสดงค่าgpsที่เปลี่ยนจากตัวเลขเป็นตัวอักษรจากด้านบน
      {
        date_str = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();
        // ตัวแปรวัน เดือน ปี ด้านบน
        if (date < 10) //นับวัน
          date_str = '0';
        date_str += String(date);

        date_str += " / ";

        if (month < 10) //นับเดือน
          date_str += '0';
        date_str += String(month);
  date_str += " / ";

        if (year < 10) //นับปี
          date_str += '0';
        date_str += String(year);
      }

      if (gps.time.isValid()) //แสดงค่าเวลา
      {
        time_str = "";
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();

        minute = (minute + 30);
        if (minute > 59) //นาที
        {
          minute = minute - 60;
          hour = hour + 1;
        }
        hour = (hour + 5) ;
        if (hour > 23) // นับ ชม
          hour = hour - 24;

        if (hour >= 12)
          pm = 1;
        else
          pm = 0;

        hour = hour % 12;

        if (hour < 10)
          time_str = '0';
        time_str += String(hour);

        time_str += " : ";

        if (minute < 10)
          time_str += '0';
        time_str += String(minute);

        time_str += " : ";

        if (second < 10)
          time_str += '0';
        time_str += String(second);

        if (pm == 1)
          time_str += " PM ";
        else
          time_str += " AM ";

      }

    }
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Prepare the response คำสั่งเฉพาะเพื่อส่งค่ารับค่าในระบบออนไลน์ - ใช้เหมือนกันทุกที่เป็นมาตรฐาน
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n <!DOCTYPE html> <html> <head> <title>GPS Interfacing with NodeMCU</title> <style>";
  s += "a:link {background-color: YELLOW;text-decoration: none;}";
  s += "table, th, td {border: 1px solid black;} </style> </head> <body> <h1  style=";
  s += "font-size:300%;";
  s += " ALIGN=CENTER> GPS Interfacing with NodeMCU</h1>";
  s += "<p ALIGN=CENTER style=""font-size:150%;""";
  s += "> <b>Location Details</b></p> <table ALIGN=CENTER style=";
  s += "width:50%";
  s += "> <tr> <th>Latitude</th>";
  s += "<td ALIGN=CENTER >";
  s += lat_str;
  s += "</td> </tr> <tr> <th>Longitude</th> <td ALIGN=CENTER >";
  s += lng_str;
  s += "</td> </tr> <tr>  <th>Date</th> <td ALIGN=CENTER >";
  s += date_str;
  s += "</td></tr> <tr> <th>Time</th> <td ALIGN=CENTER >";
  s += time_str;
  s += "</td>  </tr> </table> ";
 
  
  if (gps.location.isValid()) //ส่งค่าgpsไปปลิ๊ง
  {
     s += "<p align=center><a style=""color:RED;font-size:125%;"" href=""http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += lat_str;
    s += "+";
    s += lng_str;
    s += """ target=""_top"">Click here!</a> To check the location in Google maps.</p>";
  }

  s += "</body> </html> \n";

  client.print(s);
  delay(100);
  myMap.location(index, lat, lon, "value"); //โชว์ตำแหน่งบนแผนที่

}
