import serial
import  sys
import time
command=sys.argv[1]

for i in range(2):
    try:
        port = serial.Serial('/dev/ttyUSB0', 115200)
        port.timeout=1
    except OSError as e:
        print(f"Ошибка: {e}")
        time.sleep(2)
        continue
    port.write(command.encode('utf-8'))
    port.flush()
    response = port.readall()

    port.close()
    print(f"Ответ: {response}")
    exit()
