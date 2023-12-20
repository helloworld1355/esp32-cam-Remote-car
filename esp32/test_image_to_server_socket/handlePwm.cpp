#include "handlePwm.h"

int forward = 0;
int back = 0;

void initPwm(){
    pinMode(leftPin_1, OUTPUT);
    pinMode(leftPin_2, OUTPUT);
    pinMode(rightPin_1, OUTPUT);
    pinMode(rightPin_2, OUTPUT);
    
    ledcSetup(Left_Channel_1, freq, whatbit);
    ledcAttachPin(leftPin_1, Left_Channel_1);
    ledcSetup(Left_Channel_2, freq, whatbit);
    ledcAttachPin(leftPin_2, Left_Channel_2);
    ledcSetup(Right_Channel_1, freq, whatbit);
    ledcAttachPin(rightPin_1, Right_Channel_1);
    ledcSetup(Right_Channel_2, freq, whatbit);
    ledcAttachPin(rightPin_2, Right_Channel_2);
}

/*
 * 在转弯操作时必须有前进或后退操作，使用差速转弯。
*/
void handlePwm(void *pvParameters){
    if (client.available()) {
        String response = client.readStringUntil('\n');
        Serial.println("Received response from server: " + response);

        if(strstr(response.c_str(), "forward") != NULL)
        handleForward();
        if(strstr(response.c_str(), "fStop") != NULL)
        handleForwardStop();
        if(strstr(response.c_str(), "left") != NULL)
        handleLeft();
        if(strstr(response.c_str() ,"lStop") != NULL )
        handleLeftStop();
        if(strstr(response.c_str() ,"right") != NULL )
        handleRight();
        if(strstr(response.c_str() ,"rStop") != NULL )
        handleRightStop();
        if(strstr(response.c_str() ,"back") != NULL )
        handleBack();
        if(strstr(response.c_str() ,"bStop") != NULL )
        handleBackStop();
    }
    vTaskDelete(NULL);
}

/*
 * 左1右1为0； 左2右2 逐渐上升。
*/
void handleForward(){
    Serial.println("forwd!!!!!");
    forward = 1;
    ledcWrite(Left_Channel_1, 0);
    ledcWrite(Right_Channel_1, 0);
    for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
        ledcWrite(Left_Channel_2, dutyCycle);
        ledcWrite(Right_Channel_2, dutyCycle);
        delay(10);
    }
}
void handleForwardStop(){
    forward = 0;
    ledcWrite(Left_Channel_1, 0);
    ledcWrite(Right_Channel_1, 0);
    ledcWrite(Left_Channel_2, 0);
    ledcWrite(Right_Channel_2, 0);
}

/*
 * if(forward) 左1 逐渐上升，不超过200.
 * if(back) 左2 逐渐上升，不超过200.
*/
void handleLeft(){
    Serial.println("left !!!!!");
    if(forward){
        for(int dutyCycle = 0; dutyCycle <= 200; dutyCycle++){   
            ledcWrite(Left_Channel_1, dutyCycle);
            delay(10);
        }
    }
    if(back){
        for(int dutyCycle = 0; dutyCycle <= 200; dutyCycle++){   
            ledcWrite(Left_Channel_2, dutyCycle);
            delay(10);
        }
    }
}
void handleLeftStop(){
    if(forward){
        ledcWrite(Left_Channel_1, 0);
    }
    if(back){
        ledcWrite(Left_Channel_2, 0); 
    }
}

/*
 * if(forward) 右1逐渐上升，不超过200。
 * if(back) 右2 逐渐上升，不超过200.
*/
void handleRight(){
    Serial.println("right !!!!!");
    if(forward){
        for(int dutyCycle = 0; dutyCycle <= 200; dutyCycle++){   
            ledcWrite(Right_Channel_1, dutyCycle);
            delay(10);
        }
    }
    if(back){
        for(int dutyCycle = 0; dutyCycle <= 200; dutyCycle++){   
            ledcWrite(Right_Channel_2, dutyCycle);
            delay(10);
        }
    }
}
void handleRightStop(){
    if(forward){ 
        ledcWrite(Right_Channel_1, 0);
        
    }
    if(back){
        ledcWrite(Right_Channel_2, 0);
    }
}

/*
 * 左2右2为0； 左1右1 逐渐上升。
*/
void handleBack(){
    Serial.println("back !!!!!");
    back = 1;
    ledcWrite(Left_Channel_2, 0);
    ledcWrite(Right_Channel_2, 0);
    for(int dutyCycle = 0; dutyCycle <= 255; dutyCycle++){   
        ledcWrite(Left_Channel_1, dutyCycle);
        ledcWrite(Right_Channel_1, dutyCycle);
        delay(10);
    }
}
void handleBackStop(){
    back = 0;
    ledcWrite(Left_Channel_1, 0);
    ledcWrite(Right_Channel_1, 0);
    ledcWrite(Left_Channel_2, 0);
    ledcWrite(Right_Channel_2, 0);
}