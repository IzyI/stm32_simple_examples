[<< НАЗАД](/README.md)   
Это простая программа которая может принять по  UART1  данные  например от компьютора 
по переходнику usb-uart, и в зависимости от  переданной команды включает или выключает пины A1,A2,A3,A4.
Команды которые принемает  программа:
- 1-0 - выключает пин А1
- 1-1 - включает пин А1
- 2-0 - выключает пин А2
- ...
- 4-1 - включает пин А4
   
 Для отправки данных можно воспользоватся скриптом на python send.py (изменив только путь к UART если он отличается
 *port = serial.Serial('/dev/ttyUSB0', 115200)*):   
 python3 send.py 1-1.   
