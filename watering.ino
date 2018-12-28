nclude <Timer.h>

Timer timer; //正常浇水定时器
Timer safeTimer; //安全定时器
int sensorPin = A0;
int Light = 13;
int Relay4 = 7; //水泵继电器引脚
int ButtonPin = 4;
int ButtonState;
int BeforeState;
int sensorValue = 0; //土壤湿度传感器采样值
const int sampleCount = 10; //采样次数
const double maxVal = 1024; //计算上限
const int threshold = 800; //浇水的传感器阈值
bool isWatered = false; //一段时间内最多浇一次水,防止因为传感器损坏而过浇
long waterInerval = 43200000; //正常浇水时间间隔 12*60*60*1000
long safeWaterInterval = 86400000; //安全浇水时间间隔,默认一天最多浇一次水 24*60*60*1000
long waitTime = 0;

void setup() {
    // put your setup code here, to run once:
    pinMode(sensorPin, INPUT);
    pinMode(Light, OUTPUT);
    pinMode(Relay4, OUTPUT);
    pinMode(ButtonPin, INPUT);

    Serial.begin(9600);
    timer.every(waitTime, water, 1);
    safeTimer.every(safeWaterInterval, changeFlag);
}

void loop() {
    // put your main code here, to run repeatedly:
    ButtonState = digitalRead(ButtonPin);
    Serial.print("ButtonState: ");
    Serial.println(ButtonState);
    Serial.print("Sensor Value: ");
    Serial.println(analogRead(sensorPin));

    if (ButtonState) {
        digitalWrite(Light, HIGH);
        digitalWrite(Relay4, LOW);
    }
    else {
        digitalWrite(Light, LOW);
        digitalWrite(Relay4, HIGH);
    }
    if (ButtonState != BeforeState) {
        delay(20);
    }
    BeforeState = ButtonState;
    timer.update();
    safeTimer.update();
}

void water(){
    int sum = 0;
    int val = 0;
    for(int i=0; i < sampleCount; i++){
        val = analogRead(sensorPin);
        Serial.print("Sample Moisture Sensor Value:");
        Serial.print(val);
        Serial.print("\n");
        sum += val;
    }

    sensorValue = sum / sampleCount;

    //如果大于浇水阈值,浇水
    if(sensorValue > threshold){
        if(!isWatered){
            digitalWrite(Relay4, LOW);
            digitalWrite(Light, HIGH);
            delay(10000);
            digitalWrite(Relay4, HIGH);
            digitalWrite(Light, LOW);
            isWatered = true;
        }
        sensorValue = threshold;
    }

    //根据土壤湿度设定下次检测浇水的时间间隔
    waitTime = (long)2*(maxVal-sensorValue)/maxVal*waterInerval;

    Serial.print("Waiting water time:");
    Serial.print(waitTime);
    Serial.print("\n");
    timer.every(waitTime, water, 1);
}

void changeFlag(){
    isWatered = false; 
}
