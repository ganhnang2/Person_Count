  **Project 13: Đếm số người (Person count)**
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
  
  **Lưu ý**: 
  - Mỗi chân điều khiển độ sáng từng digit của LED 7 đoạn khi mắc vào chân Arduino thì
  phải cắm một con điện trở 2kOhm để tránh gây cháy!

  ## LED-RGB (Anode-Common)
  | Pin (LED RGB)          | Arduino Pin       |
  |------------------------|-------------------|
  | Common Pin             | VCC (5V)          |
  | Red Pin                | 9                 |
  | Green Pin              | 10                |
  | Blue Pin               | 11                |
  
  **Lưu ý**: 
  - Mỗi chân điều khiển một màu của LED RGB phải cắm một con điện trở 220Ohm khi nối vào
  chân Arduino!

  ## Sensor HC-SR04
  | Pin (Sensor)           | Arduino Pin       |
  |------------------------|-------------------|
  | VCC                    | 5V                |
  | GND                    | GND               |
  | Trigger Pin            | 7                 |
  | Echo Pin               | 8                 |

  ## Button
  Button - Pin 12 Arduino

  # Mô tả thuật toán:
  - Ban đầu, chúng ta xem xét số lượng người đã đếm được sau vòng lặp loop() trước đó.
  - Tiếp theo, kiểm tra xem liệu rằng lần lặp trước đã hoàn thành việc người đã đi qua cảm biến chưa 
  (hay có nghĩa  là việc thực hiện đếm 1 người đã diễn ra xong) bằng biến bool isConfirm. Nếu hoàn 
  thành xong rồi thì ta thực hiện việc chạy vòng lặp while (!(millis() > lastUpdate + period)) để delay
  gian trên một khoảng period cho lần kiểm tra sau và in ra số người đã qua cảm biến lên bảng điều 
  khiển Serial monitor!
  - Tiếp đến, thực hiện việc tính khoảng cách giữa cảm biến và con người: distance. Và ta có biến 
  state để thể hiện 2 trạng thái:
      - waitingForArrival: đợi người tiếp theo đến cảm biến
      - waitingForDeparture: đợi người đi ra khỏi cảm biến và vào bên trong
  - Ở đây, if (state == waitingForArrival) là xem có phải là cảm biến đang đợi để kiểm tra người tiếp theo 
  hay ko? Nếu đúng như vậy thì lại kiểm tra tiếp:
      - Nếu khoảng cách distance là <= khoảng cách ngưỡng (threshold) thì có nghĩa là người đã bắt đầu ở
      đối diện cảm biến -> Lúc này thì ta chuyển trạng thái state = waitingForDeparture, điều này có
      nghĩa là trạng thái chuyển đổi sang chế độ chờ người rời khỏi cảm biến và đi vào trong.
      - Nếu distance > threshold thì ngoài vùng cảm biến xác nhận thì trạng thái vẫn là waitingForArrival
      và setRgb(1, 0, 0) -> Đỏ sáng: Ko có hiện tg gì xảy ra
  - Thêm 1 câu lệnh if (state == waitingForDeparture), nếu thỏa mãn thì thực hiện if (distance >= threshold):
      - Nếu đúng: lúc này con người rời khỏi cảm biến và đi vào bên trong hay có nghĩa 
      là thực hiện xong 1 lần kiểm tra! Ta cập nhật tổng số người đã đếm được (personCount++), biến xác nhận
      khi hoàn thành kiểm tra 1 người (isConfirm = true), đặt setRgb(0, 0, 1) -> LED xanh sáng: Báo hiệu hoàn
      thành xong 1 lần đếm
      - Nếu sai thì có nghĩa là con người vẫn đang trên cảm biến, đặt setRgb(0, 0, 1) -> LED xanh lá: Báo hiệu
      con người vẫn đang đứng trên cảm biến (chưa hoàn thành xong lần kiểm tra)
  - Cuối cùng, kiểm tra xem nút nhấn có bấm hay không? Nếu có thì resetCount(), reset tất cả về ban đầu.
  Thực hiện vòng lặp while (!digitalRead(BUTTON_PIN)) { } khi mà ta thực hiện nhấn giữ nút và delay(1) 
  để thực hiện vòng lặp loop() tiếp theo
  - Hết 1 quy trình lặp

  **DONE!**

  Soure: Nguyễn Hải Nam
