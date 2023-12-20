#include "camara.h"
// #include "socket.h"
#include <base64.h>
#include<string.h>
#include "soc/soc.h"             // disable brownout problems
#include "soc/rtc_cntl_reg.h"    // disable brownout problems
#include "handlePwm.h"
#include "WiFiConnect.h"


// const IPAddress serverIp(10,177,13,153);  //你自己的公网服务器ip地址
IPAddress serverIp(8,130,121,18);
uint16_t serverPort = 80; 

// 如果 configSUPPORT_STATIC_ALLOCATION 宏被定义为 1，则使用内部 SRAM
//#define configSUPPORT_STATIC_ALLOCATION         1
// 如果 configSUPPORT_STATIC_ALLOCATION 宏被定义为 2，则使用 PSRAM
//#define configSUPPORT_STATIC_ALLOCATION           2



camera_fb_t * fb;
size_t jpg_buf_len = 0;
uint8_t * jpg_buf = NULL;
const char* ap_ssid = "ESP-32";
const char* ap_pwd = "";

WiFiClient client;

void HandleHttp();
void HandleSocket();

WiFiMulti WiFiMulti;

void setup() {
  // WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);


  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid,ap_pwd);
  wifiServer();
  while(WiFi.status() != WL_CONNECTED){
    Serial.printf("..");
    delay(300);
  }
  
  Serial.printf("connect...");
  
  Serial.println(WiFi.localIP());

  // 初始化pwm
  initPwm();
  
   // 初始化摄像头，并进行配置
  if(CaramaInit() != true){
    return;
  }


}

void loop() {
  HandleHttp();
}



/*
 * 连接服务器，并不断发送图片
*/
void HandleHttp(){
  Serial.println("http::::");
  if(client.connect(serverIp, serverPort)){
    
    while(1){
        fb = esp_camera_fb_get();
        jpg_buf_len = fb->len;
        jpg_buf = fb->buf;
        if(!fb){
          return;
        }else{
            String aft = base64::encode(jpg_buf,jpg_buf_len);
            client.print(aft);
            client.print("\n");
            esp_camera_fb_return(fb);
            fb = NULL;
            jpg_buf_len = 0;
            jpg_buf = NULL;
        }
        xTaskCreate(handlePwm,"pwm",10000,NULL,1,NULL);
    }
    Serial.println("connect close!!");
  }else{
      Serial.println("Connect To Tcp Server Failed!After 10 Seconds Try Again!");
      client.stop(); //关闭客户端
    }
}



