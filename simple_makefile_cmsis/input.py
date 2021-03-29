# print("_64______57__56______49__48______41__40______33__32______25__24______16__15______9____8______1__")
# def pl(val):
#     print(type(val))
#     s='{0:064b}'.format(val)
#     print([s[x:x+8] for x in range(0, len(s), 8)])


# print(bin(~(0x00000002 | ~0x0000000C)))
#
# print(0x00000003 | 0x0000000C)


import serial

#init serial port and bound
# bound rate on two ports must be the same
ser = serial.Serial('/dev/ttyUSB0', 9600)
print(ser.portstr)

#send data via serial port
byt_str=b"ASDFGHJK45678"
ser.write(byt_str)
print(f'send {byt_str}')
res=ser.read(13)
print(res)
ser.close()