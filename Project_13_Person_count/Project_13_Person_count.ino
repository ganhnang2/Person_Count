/*
  Project 13: Đếm số người (Person count)
  # Hardware:
    |Linh kiện                |Số lượng      |
    |-------------------------|--------------|
    |Arduino Atmega           |1             |
    |Điện trở 220 ohm         |3             |
    |Điện trở 2k ohm          |4             |
    |LED-7-đoạn-4-số (A-chung)|1             |
    |Breadboard cỡ lớn        |2             |

  # Pin Connections:
  ## LED-7-Segments-4-digits (Anode-Common)
    | Pin (LED 7 Segemnts)   | Arduino Pin       |
    |------------------------|-------------------|
    | 11 (seg-A)             | A0                |
    | 7  (seg-B)             | A1                |
    | 4  (seg-C)             | A2                |
    | 2  (seg-D)             | A3                |
    | 1  (seg-E)             | A4                |
    | 10 (seg-F)             | A5                |
    | 5  (seg-G)             | 13                |
    | 3  (seg-DP)            | 2                 |
    | 12 (digit 1)           | 3                 |
    | 9  (digit 2)           | 4                 |
    | 8  (digit 3)           | 5                 |
    | 6  (digit 4)           | 6                 |

  ## LED-RGB (Anode-Common)
    | Pin (LED RGB)          | Arduino Pin       |
    |------------------------|-------------------|
    | Common Pin             | VCC (5V)          |
    | Red Pin                | 9                 |
    | Green Pin              | 10                |
    | Blue Pin               | 11                |

  ## Sensor HC-SR04
    | Pin (Sensor)           | Arduino Pin       |
    |------------------------|-------------------|
    | VCC                    | 5V                |
    | GND                    | GND               |
    | Trigger Pin            | 7                 |
    | Echo Pin               | 8                 |

  ## Button - Pin 12 (Arduino)

  Mô tả thuật toán:
  +) Ban đầu, chúng ta xem xét số lượng người đã đếm được sau vòng lặp loop() trước đó.
  +) Tiếp theo, kiểm tra xem liệu rằng lần lặp trước đã hoàn thành việc người đã đi qua cảm biến chưa 
  (hay có nghĩa  là việc thực hiện đếm 1 người đã diễn ra xong) bằng biến bool isConfirm. Nếu hoàn 
  thành xong rồi thì ta thực hiện việc chạy vòng lặp while (!(millis() > lastUpdate + period)) để delay
  gian trên một khoảng period cho lần kiểm tra sau và in ra số người đã qua cảm biến lên bảng điều 
  khiển Serial monitor!
  +) Tiếp đến, thực hiện việc tính khoảng cách giữa cảm biến và con người: distance. Và ta có biến 
  state để thể hiện 2 trạng thái:
      - waitingForArrival: đợi người tiếp theo đến cảm biến
      - waitingForDeparture: đợi người đi ra khỏi cảm biến và vào bên trong
  Ở đây, if (state == waitingForArrival) là xem có phải là cảm biến đang đợi để kiểm tra người tiếp theo 
  hay ko? Nếu đúng như vậy thì lại kiểm tra tiếp:
      - Nếu khoảng cách distance là <= khoảng cách ngưỡng (threshold) thì có nghĩa là người đã bắt đầu ở
      đối diện cảm biến -> Lúc này thì ta chuyển trạng thái state = waitingForDeparture, điều này có
      nghĩa là trạng thái chuyển đổi sang chế độ chờ người rời khỏi cảm biến và đi vào trong.
      - Nếu distance > threshold thì ngoài vùng cảm biến xác nhận thì trạng thái vẫn là waitingForArrival
      và setRgb(1, 0, 0) -> Đỏ sáng: Ko có hiện tg gì xảy ra
  +) Thêm 1 câu lệnh if (state == waitingForDeparture), nếu thỏa mãn thì thực hiện if (distance >= threshold):
      - Nếu đúng: lúc này con người rời khỏi cảm biến và đi vào bên trong hay có nghĩa 
      là thực hiện xong 1 lần kiểm tra! Ta cập nhật tổng số người đã đếm được (personCount++), biến xác nhận
      khi hoàn thành kiểm tra 1 người (isConfirm = true), đặt setRgb(0, 0, 1) -> LED xanh sáng: Báo hiệu hoàn
      thành xong 1 lần đếm
      - Nếu sai thì có nghĩa là con người vẫn đang trên cảm biến, đặt setRgb(0, 0, 1) -> LED xanh lá: Báo hiệu
      con người vẫn đang đứng trên cảm biến (chưa hoàn thành xong lần kiểm tra)
  +) Cuối cùng, kiểm tra xem nút nhấn có bấm hay không? Nếu có thì resetCount(), reset tất cả về ban đầu.
  Thực hiện vòng lặp while (!digitalRead(BUTTON_PIN)) { } khi mà ta thực hiện nhấn giữ nút và delay(1) 
  để thực hiện vòng lặp loop() tiếp theo
  +) Hết 1 quy trình 
  
  -> Quá mệt mỏi, mất nguyên ngày thứ 7!

  Soure: Nguyễn Hải Nam
*/


#include <SevSeg.h>

// Chân của LED RGB
const uint8_t ledPins[] = {9, 10, 11};

// Chân Trigger, Echo của HCSR04
const int TRIGGER_PIN = 7;
const int ECHO_PIN = 8;

// Số chữ số của tổng số người
// 6: digit6, 5: digit3, 4: digit2, 3: digit1
const uint8_t digit1 = 3;
const uint8_t digit2 = 4;
const uint8_t digit3 = 5;
const uint8_t digit4 = 6;
const int BUTTON_PIN = 12;
const int period = 1000;

uint8_t personCount = 0;        // Số người đã đi qua
bool isConfirm = false;         // Xác nhận một người đã qua cảm biến hay lần kiểm tra 1 người xong chưa

int waitingForArrival = 0;      // Đợi vật thể đến
int waitingForDeparture = 1;    // Đợi vật thể đi
int threshold = 50;             // Khoảng cách ngưỡng để tính vật thể đã đi qua cảm biến
int state = waitingForArrival;  // Trạng thái của vật thể, ban đầu mặc định là đang đợi

int mode = COMMON_ANODE;      // LED ở chế độ Anode chung
// Số chân của 7 thanh trong 1 LED
const int segA = A0;
const int segB = A1;
const int segC = A2;
const int segD = A3;
const int segE = A4;
const int segF = A5;
const int segG = 2;
const int segDP = 13;
const int numberOfDigits = 4;

// Khai báo lớp Sevseg để phục vụ cho việc in ra kết quả
SevSeg myDisplay;

void setup() {
  // Chân nút nhấn
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Chân của cảm biến HCSR04
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Đặt các chân LED RGB, chân của từng chữ số
  for (int i = 0; i < 3; i++)
    pinMode(ledPins[i], OUTPUT);

  myDisplay.Begin(mode, numberOfDigits, digit1, digit2, digit3, digit4, segA, segB, segC, segD, segE, segF, segG, segDP);
  myDisplay.SetBrightness(100);      // Đặt độ sáng 100 %

  resetCount();       // Reset về ban đầu
  Serial.begin(9600); // Bật cửa sổ Serial
  while(!Serial)
  delay(4);
}

// Bật màu của LED Rgb lên
void setRgb(int red, int green, int blue)
{
  digitalWrite(ledPins[0], red ? LOW : HIGH);
  digitalWrite(ledPins[1], green ? LOW : HIGH);
  digitalWrite(ledPins[2], blue ? LOW : HIGH);
}

// Reset khi mà bấm nút nhấn
void resetCount()
{
    setRgb(0, 0, 0);
    personCount = 0;
    isConfirm = false;
    state = waitingForArrival;
} 

// In ra số người đã đi qua
void displayPersonCount()
{
  char termString[10];
  sprintf(termString, "%4d", personCount);      // Chuyển thành chuỗi ký tự
  myDisplay.DisplayString(termString, 0);       // In ra chuỗi ký tự, 0 :ko đặt dấu thập phân
}

// Nhận khoảng cách từ cảm biến đến người
int takeDistance()
{
  unsigned long pulseWidth;
  int distance;
  digitalWrite(TRIGGER_PIN,0);   // tắt chân trig
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN,1);   // phát xung từ chân trig
  delayMicroseconds(5);          // xung có độ dài 5 microSeconds
  digitalWrite(TRIGGER_PIN,0);   // tắt chân trig
  pulseWidth = pulseIn(ECHO_PIN , HIGH);           //Đọc độ rộng xung trả về microsecond
  distance = long(pulseWidth*0.5*0.034);           //s = v*t = pulse_width*0.5*10^(-6)*340*100 = pulse_width*0.5*0.03

  return distance;
}

void loop() {
    unsigned long lastUpdate = millis();

    // In ra tổng số người đã đếm được
    displayPersonCount();

    if (isConfirm)     // Sau khi thực hiện đếm thành công
    {
      // Delay một khoảng tgian period
      while (!(millis() > lastUpdate + period)) 
      { 
        displayPersonCount(); 
      }

      Serial.print("Person Count: ");
      Serial.println(personCount);
      isConfirm = false;      // Reset trở về false
    }

    int distance = takeDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println("cm");
    if (state == waitingForArrival)   // Đang đợi người đến
    {
      if (distance < threshold)       // Nếu người đến thì cập nhật sang trạng thái chờ
        state = waitingForDeparture;
      else                            // Nếu ko có ai thì led Red sáng
        setRgb(1, 0, 0);
    }

    if (state == waitingForDeparture)
    {
      if (distance >= threshold)      // Khi người đã đi qua cảm biến
      {
        state = waitingForArrival;    // Cập nhật trạng thái
        personCount++;                // Tăng số đếm người lên
        isConfirm = true;             
        setRgb(0, 0, 1);              // Đèn blue sáng
      }
      else                            // Nếu người vẫn còn trên cảm biến thì đèn Green sáng
        setRgb(0, 1, 0);
    }

    if (!digitalRead(BUTTON_PIN))
      resetCount();

    // Khi nhấn giữ nút bấm
    while (!digitalRead(BUTTON_PIN)) { }
    delay(1);
}
