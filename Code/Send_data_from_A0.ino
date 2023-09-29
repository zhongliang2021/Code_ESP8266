/*
 * 支持天猫、小爱、小度、google Assistent同时控制
 * 2023-06-29
 */
#include <ESP8266WiFi.h>


#define server_ip "bemfa.com" //巴法云服务器地址默认即可
#define server_port "8344" //服务器端口，tcp创客云端口8344

//********************需要修改的部分*******************//

#define wifi_name  "******"     //WIFI名称，区分大小写，不要写错
#define wifi_password   "************"  //WIFI密码
String UID = "******************************";  //用户私钥，可在控制台获取,修改为自己的UID
String TOPIC = "******************************";         //主题名字，可在控制台新建
const int LED_Pin = D0;              //单片机LED引脚值，DX是NodeMcu引脚命名方式，其他esp8266型号将D0改为自己的引脚，比如 const int LED_Pin = 0; 
int a=0;
int b=6;//保留小数点位数
float Sensor=0;
String Sensor1 = "0";
String Sensor2 = "0";
String Sensor3 = "0";
String Sensor4 = "0";
String Sensor5 = "0";
String Sensor6 = "0";
String Sensor7 = "0";
String Sensor8 = "0";
String Sensor9 = "0";
String Sensor10 = "0";
String ssensor;
// float Sensor=0;
// float Sensor=0;
// float Sensor=0;
// float Sensor=0;
//**************************************************//
//最大字节数
#define MAX_PACKETSIZE 512
//设置心跳值60s----------发送数据的时间间隔------采样频率
#define KEEPALIVEATIME 1*1000
//tcp客户端相关初始化，默认即可
;WiFiClient TCPclient;
String TcpClient_Buff = "";//初始化字符串，用于接收服务器发来的数据
unsigned int TcpClient_BuffIndex = 0;
unsigned long TcpClient_preTick = 0;
unsigned long preHeartTick = 0;//心跳
unsigned long preTCPStartTick = 0;//连接
bool preTCPConnected = false;
//相关函数初始化
//连接WIFI
void doWiFiTick();
void startSTA();

//TCP初始化连接
void doTCPClientTick();
void startTCPClient();
void sendtoTCPServer(String p);

//led控制函数，具体函数内容见下方
void turnOnLed();
void turnOffLed();
void readdata();


/*
  *发送数据到TCP服务器
 */
void sendtoTCPServer(String p){
  if (!TCPclient.connected()) 
  {
    Serial.println("Client is not readly");
    return;
  }
  TCPclient.print(p);
  preHeartTick = millis();//心跳计时开始，需要每隔60秒发送一次数据
}


/*
  *初始化和服务器建立连接
*/
void startTCPClient(){
  if(TCPclient.connect(server_ip, atoi(server_port))){
    Serial.print("\nConnected to server:");
    Serial.printf("%s:%d\r\n",server_ip,atoi(server_port));
    
    String tcpTemp="";  //初始化字符串
    tcpTemp = "cmd=1&uid="+UID+"&topic="+TOPIC+"\r\n"; //构建订阅指令
    sendtoTCPServer(tcpTemp); //发送订阅指令
    tcpTemp="";//清空
    /*
     //如果需要订阅多个主题，可发送  cmd=1&uid=xxxxxxxxxxxxxxxxxxxxxxx&topic=xxx1,xxx2,xxx3,xxx4\r\n
    教程：https://bbs.bemfa.com/64
     */
    
    preTCPConnected = true;
    TCPclient.setNoDelay(true);
  }
  else{
    Serial.print("Failed connected to server:");
    Serial.println(server_ip);
    TCPclient.stop();
    preTCPConnected = false;
  }
  preTCPStartTick = millis();
}


/*
  *检查数据，发送心跳
*/
void doTCPClientTick(){
 //检查是否断开，断开后重连
   if(WiFi.status() != WL_CONNECTED) return;
  if (!TCPclient.connected()) {//断开重连
  if(preTCPConnected == true){
    preTCPConnected = false;
    preTCPStartTick = millis();
    Serial.println();
    Serial.println("TCP Client disconnected.");
    TCPclient.stop();
  }
  else if(millis() - preTCPStartTick > 1*1000)//重新连接
    startTCPClient();
  }
  else
  {
    if (TCPclient.available()) {//收数据
      char c =TCPclient.read();
      TcpClient_Buff +=c;
      TcpClient_BuffIndex++;
      TcpClient_preTick = millis();
      
      if(TcpClient_BuffIndex>=MAX_PACKETSIZE - 1){
        TcpClient_BuffIndex = MAX_PACKETSIZE-2;
        TcpClient_preTick = TcpClient_preTick - 200;
      }
 
    }
    if(millis() - preHeartTick >= KEEPALIVEATIME){//保持心跳
      if(a==1){
        readdata();
        Sensor1=ssensor;
 

        readdata();
        Sensor2=ssensor;
        

        readdata();
        Sensor3=ssensor;


        readdata();
        Sensor4=ssensor;


        readdata();
        Sensor5=ssensor;


        readdata();
        Sensor6=ssensor;


        readdata();
        Sensor7=ssensor;


        readdata();
        Sensor8=ssensor;


        readdata();
        Sensor9=ssensor;


        readdata();
        Sensor10=ssensor;

        }
      preHeartTick = millis();
      Serial.println("--Keep alive:");
      // sendtoTCPServer("ping\r\n"); //发送心跳，指令需\r\n结尾，详见接入文档介绍
      String upstr = "";
      upstr = "cmd=2&uid="+UID+"&topic="+TOPIC+"&msg=#"+Sensor1+"#"+Sensor2+"#"+Sensor3+"#"+Sensor4+"#"+Sensor5+"#"+Sensor6+"#"+Sensor7+"#"+Sensor8+"#"+Sensor9+"#"+Sensor10+"#\r\n";
      sendtoTCPServer(upstr);
      upstr = "";
    }
    
  }
  if((TcpClient_Buff.length() >= 1) && (millis() - TcpClient_preTick>=200))
  {
    TCPclient.flush();
    Serial.print("Rev string: ");
    TcpClient_Buff.trim(); //去掉首位空格
    Serial.println(TcpClient_Buff); //打印接收到的消息
    String getTopic = "";
    String getMsg = "";
    if(TcpClient_Buff.length() > 15){//注意TcpClient_Buff只是个字符串，在上面开头做了初始化 String TcpClient_Buff = "";
          //此时会收到推送的指令，指令大概为 cmd=2&uid=xxx&topic=light002&msg=off
          int topicIndex = TcpClient_Buff.indexOf("&topic=")+7; //c语言字符串查找，查找&topic=位置，并移动7位，不懂的可百度c语言字符串查找
          int msgIndex = TcpClient_Buff.indexOf("&msg=");//c语言字符串查找，查找&msg=位置
          getTopic = TcpClient_Buff.substring(topicIndex,msgIndex);//c语言字符串截取，截取到topic,不懂的可百度c语言字符串截取
          getMsg = TcpClient_Buff.substring(msgIndex+5);//c语言字符串截取，截取到消息
          Serial.print("topic:");
          Serial.print(getTopic); //打印截取到的主题值
          Serial.println();
          Serial.print("message:");
          Serial.print(getMsg);   //打印截取到的消息值
          Serial.println();
   }
   if(getMsg  == "on"){       //如果是消息==打开
     turnOnLed();
      a=1;
   }else if(getMsg == "off"){ //如果是消息==关闭
      turnOffLed();
      a=0;
    }

   TcpClient_Buff="";
   TcpClient_BuffIndex = 0;
  }
}
/*
  *初始化wifi连接
*/
void startSTA(){
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_name, wifi_password);
}



/**************************************************************************
                                 WIFI
***************************************************************************/
/*
  WiFiTick
  检查是否需要初始化WiFi
  检查WiFi是否连接上，若连接成功启动TCP Client
  控制指示灯
*/
void doWiFiTick(){
  static bool startSTAFlag = false;
  static bool taskStarted = false;
  static uint32_t lastWiFiCheckTick = 0;

  if (!startSTAFlag) {
    startSTAFlag = true;
    startSTA();
  }

  //未连接1s重连
  if ( WiFi.status() != WL_CONNECTED ) {
    if (millis() - lastWiFiCheckTick > 1000) {
      lastWiFiCheckTick = millis();
    }
  }
  //连接成功建立
  else {
    if (taskStarted == false) {
      taskStarted = true;
      Serial.print("\r\nGet IP Address: ");
      Serial.println(WiFi.localIP());
      startTCPClient();
    }
  }
}
//打开灯泡
void turnOnLed(){
  Serial.println("Turn ON");
  digitalWrite(LED_Pin,LOW);
  digitalWrite(LED_BUILTIN,LOW);
}
//关闭灯泡
void turnOffLed(){
  Serial.println("Turn OFF");
    digitalWrite(LED_Pin,HIGH);
    digitalWrite(LED_BUILTIN,HIGH);
}
//读取传感器
void readdata(){
  const int analogInPin = A0;  //ESP8266模拟引脚ADC0,也就是A0
  int ADCValue = 0;  
  float voltage = 0;
  float Ref=20.1925;//单位K欧姆
  float vcc=3;//单位伏特

  // // 初始化串口的波特率为115200
  // Serial.begin(115200);
  //读取模拟数值
  ADCValue = analogRead(analogInPin);
  voltage=ADCValue*vcc/1024;
  Sensor=voltage*Ref/(vcc-voltage);
  ssensor = String(Sensor,b);
  // 打印串行监视器中的读数
  Serial.print("sensor = ");
  Serial.print(ssensor);
  Serial.print("K");
  Serial.println();

}

// 初始化，相当于main 函数
void setup() {
  Serial.begin(115200);
  pinMode(LED_Pin,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println("Beginning...");

}

//循环
void loop() {
  doWiFiTick();
  doTCPClientTick();
}
