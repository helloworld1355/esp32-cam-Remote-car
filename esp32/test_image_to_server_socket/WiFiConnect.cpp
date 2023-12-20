#include "WiFiConnect.h"

httpd_handle_t stream_httpd = NULL;


/*
 * 处理esp32连接wifi函数
 */
void wifiServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t connect_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = connect_handler,
    .user_ctx  = NULL
  };
  httpd_uri_t submit_uri = {
    .uri       = "/submit",
    .method    = HTTP_POST,
    .handler   = submit_handler,
    .user_ctx  = NULL
  };
  
  //Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &connect_uri);
    httpd_register_uri_handler(stream_httpd, &submit_uri);
  }
}

static esp_err_t connect_handler(httpd_req_t *req){
  httpd_resp_set_type(req,"text/html");
     static const char indexHtml[]=R"ral(
        <!DOCTYPE html>
          <html lang="en">
          <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Connect to WiFi</title>
            <style>
              body {
                font-family: Arial, sans-serif;
                text-align: center;
                padding: 20px;
              }
              input {
                padding: 10px;
                margin: 10px;
              }
              .button {
                padding: 10px 20px;
                margin: 10px;
                cursor: pointer;
              }
            </style>
          </head>
          <body>
          <h2>WiFi连接页面</h2>
          <form id="wifiForm" action="/submit" method="post">
            <label for="ssid">WiFi SSID:</label>
            <input type="text" id="ssid" name="ssid" required placeholder="输入WiFi名">
            <br>
            <label for="password">WiFi PWD:</label>
            <input type="password" id="password" name="password" required placeholder="输入WiFi密码">
            <br>
            <label for="hostip">host IP:</label>
            <input type="hostip" id="hostip" name="hostip" required placeholder="服务器ip,默认8.130.121.18">
            <br>
            <label for="hostport">port:</label>
            <input type="hostport" id="hostport" name="hostport" placeholder="服务器端口,默认81">
            <br>
            <input type="hidden" id="hidden" name="hidden">
            <input class="button" type="submit" value="Submit">
          </form>
          <p id="statusMessage"></p>
          </body>
          </html>
       )ral";
    
    httpd_resp_send(req, indexHtml, strlen(indexHtml));
    return ESP_OK;

}

static esp_err_t submit_handler(httpd_req_t *req){
     char content[100];
     // 获取的内容
     if (httpd_req_recv(req, content, sizeof(content)) <= 0) {
         httpd_resp_send_500(req);
         return ESP_FAIL;
     }

     // 提取表单字段的值
     // 这里出错
     char ssid[64],password[64],hostip[20],hostport[10];
    
     if (httpd_query_key_value(content, "ssid", ssid, sizeof(ssid)) != ESP_OK ||
         httpd_query_key_value(content, "password", password, sizeof(password)) != ESP_OK ||
         httpd_query_key_value(content, "hostip", hostip, sizeof(hostip)) != ESP_OK ||
         httpd_query_key_value(content, "hostport", hostport, sizeof(hostport)) != ESP_OK ) {
         httpd_resp_send_500(req);
        return ESP_FAIL;
     }

     // 注意wifi 连接的密码，可能会有莫名其妙的位出来，导致连接不上
     ssid[sizeof(ssid) - 1] = '\0';
     password[sizeof(password) - 1] = '\0';
     hostip[sizeof(hostip) - 1] = '\0';
     hostport[sizeof(hostport) - 1] = '\0';
     serverIp.fromString(hostip);
     try {
          serverPort = std::stoi(hostport);
          // std::cout << "Parsed serverPort: " << serverPort << std::endl;
          Serial.println("Parsed serverPort: ");
          Serial.print(serverPort);
      } catch (const std::invalid_argument& e) {
          
      } catch (const std::out_of_range& e) {
          
      }

     Serial.println("\nReceived SSID: " + String(ssid));
     Serial.println("Received Password: " + String(password));
     Serial.println("Received hostip: " + serverIp.toString());
     
     int ret = WiFiStart(ssid,password);
     if(ret == 0){
         httpd_resp_set_type(req,"text/html");
         static const char* SuccessHtml = R"ral(
            <!DOCTYPE html>
              <html lang="en">
              <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>Connection Success</title>
                <style>
                  body {
                    font-family: Arial, sans-serif;
                    text-align: center;
                    padding: 20px;
                  }
                  button {
                    padding: 10px 20px;
                    margin: 10px;
                    cursor: pointer;
                  }
                </style>
              </head>
              <body>
              <h2>Connection Success</h2>
              <p id="statusMessage">连接成功</p>
              <button type="button" onclick="showVideo()">点击查看视频</button>
              <script>
                function showVideo() {
                  window.location.href = "http://8.130.121.18:81/";
                }
              </script>
              </body>
              </html>
            )ral";
            httpd_resp_send(req,SuccessHtml, strlen(SuccessHtml));
     }else{
        static const char* ErrorHtml = R"ral(
          <!DOCTYPE html>
          <html lang="en">
          <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>Connection Success</title>
            <style>
              body {
                font-family: Arial, sans-serif;
                text-align: center;
                padding: 20px;
              }
              button {
                padding: 10px 20px;
                margin: 10px;
                cursor: pointer;
              }
            </style>
          </head>
          <body>
          <h2>Connection False</h2>
          <p id="statusMessage">连接失败！</p>
          <button type="button" onclick="returnBack()">返回连接页面</button>
          <script>
            function returnBack() {
              window.location.href = "/";
            }
          </script>
          </body>
          </html>
        )ral";
        httpd_resp_send(req,ErrorHtml, strlen(ErrorHtml));
        return ESP_FAIL;
     }
     return ESP_OK;
 }

int WiFiStart(const char * ssid,const char* password){
    int ret = -1;
    Serial.print("ssid:");Serial.print(String(ssid));Serial.print("\tlen:");Serial.println(strlen(ssid));
    Serial.print("password:");Serial.print(String(password));Serial.print("\tlen:");Serial.println(strlen(password));

    WiFi.begin(String(ssid),String(password));
    Serial.print("wait WiFi");
    int temp=0;
    while( WiFi.status() != WL_CONNECTED && temp <= 15){
            delay(500);
            Serial.print(". ");  
            temp++;
    }
    if(WiFi.status() == WL_CONNECTED){
        Serial.print("WiFi ip:");
        Serial.println(WiFi.localIP());
        ret = 0;
    }else{
        Serial.println("WiFi connect false");
    }
    return ret;
}
