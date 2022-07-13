# -*- coding:utf8 -*-
import os
import threading
import time
from configparser import ConfigParser

import serial
import serial.tools.list_ports

debug = True

if os.name == "posix":
    print("Cmd parser in Linux")
elif os.name == "nt":
    print("Cmd parser in Windows")


class cmdparser:
    serialport = None
    buadrate = None
    databit = None
    stopbit = None
    checkbit = None
    version = 0.1
    leftspeed=0
    rightspeed=0
    datalist = []

    def __init__(self):
        self.readconfig()
        self.portcheck()


    def readconfig(self):
        con = ConfigParser()
        con.read('config.cfg')
        if os.name == "posix":
            self.serialport = con.get('Seriall', 'port')
            self.buadrate = int(con.get('Seriall', 'baudrate'))
            self.databit = int(con.get('Seriall', 'databit'))
            self.stopbit = int(con.get('Seriall', 'stopbit'))
            self.checkbit = con.get('Seriall', 'checkbit')
        elif os.name == "nt":
            self.serialport = con.get('Serialw', 'port')
            self.buadrate = int(con.get('Serialw', 'baudrate'))
            self.databit = int(con.get('Serialw', 'databit'))
            self.stopbit = int(con.get('Serialw', 'stopbit'))
            self.checkbit = con.get('Serialw', 'checkbit')

        if debug == True:
            print("小车命令调试器版本:{}".format(self.version))
            print("默认串口:{}".format(self.serialport))
            print("默认波特率:{}".format(self.buadrate))
            print("使用数据位:{}".format(self.databit))
            print("使用停止位:{}".format(self.stopbit))
            print("使用校验位:{}".format(self.checkbit))
            print("软件作者：RXF")
        else:
            print("Serial settings:", self.serialport, self.buadrate, self.databit, self.checkbit, self.stopbit)

    def portcheck(self):
        portlist = list(serial.tools.list_ports.comports())
        if debug == True:
            for port in portlist:
                print(port)
            if len(portlist) == 0:
                print("No port has been detected")
        if os.name == "posix":
            pass
        elif os.name == "nt":
            for i in range(0, len(portlist)):
                portlist[i] = str(portlist[i])[0:4]
            if self.serialport in portlist:
                print("port check over")
                return 0
            else:
                print("config port is not in the list")
                return -1

    def openserialport(self):
        try:
            self.serial = serial.Serial(self.serialport, self.buadrate, self.databit, self.checkbit, self.stopbit)
            flag = self.serial.is_open
            self.serial.timeout = 0.03
            # 开启接收线程
            threading.Thread(target=self.serialreceive_thread, daemon=True).start()
            if flag:
                print('open serial successfully\n')
        except:
            RuntimeError('openserial error')

    def closeserialport(self):
        try:
            self.serial.close()
        except:
            RuntimeError('CLOSE ERROR')

    def senddata_hex(self, data_hex):
        self.serial.write(bytes.fromhex(data_hex))

    def setspeed(self, leftspeed, rightspeed):
        senddata = [0xaa, 0x01, 0x04]
        senddata.extend(dec2hex_16bits(leftspeed))
        senddata.extend(dec2hex_16bits(rightspeed))
        senddata.extend(sendchecksum_add8(senddata))
        self.senddata_hex(numlist2str(senddata))
        if debug == True:
            print("设定速度(Hz/s) L:{0} R:{1}".format(leftspeed, rightspeed))

    def stopcar(self, mode):
        senddata = [0xaa, 0x02, 0x01]
        senddata.extend(dec2hex_8bits(mode))
        senddata.extend(sendchecksum_add8(senddata))
        self.senddata_hex(numlist2str(senddata))
        if debug == True:
            if mode == 0:
                print("设定停车方式为正常停止")
            elif mode == 1:
                print("设定停车方式为紧急停止")
            elif mode == 2:
                print("设定停车方式为自由停止")

    def serialreceive_thread(self):
        if debug == True:
            print("接收线程已经启动")
        while (self.serial.is_open):
            try:
                num = self.serial.in_waiting
                if num > 0:
                    self.datalist = []
                    received = self.serial.read_all()
                    dataflag = False
                    for i in range(0, len(received)):
                        print(i,hex(received[i]))
                        if received[i] == 0xaa:  # 从帧头开始记录数据帧
                            # print(i,hex(received[i]))
                            dataflag = True
                        if dataflag == True:
                            self.datalist.append(received[i])

                    #print(self.datalist)
                    self.parser(self.datalist)
            except:
                RuntimeError('serial thread error')
                pass

    def parser(self, list):
        if receivechecksum_add8(list) == True:
            #print('correct serial command')
            command=list[1]
            if command==0x01:
                self.leftspeed=(list[3]<<8)+list[4]
                self.rightspeed=(list[5]<<8)+list[6]
                #print('leftspeed(Hz/s):',leftspeed,'rightspeed(Hz/s)'rightspeed)
                return (self.leftspeed,self.rightspeed)
            elif command==0x02:
                self.mode=list[3]
                return (self.mode)
            elif command==0x03:
                pass


        else:
            print('invalid serial command')
            return -1


'''
该函数用于根据10进制数计算16bits的16进制数
'''


def dec2hex_16bits(dec):
    if dec < 0:
        dec &= 0xffff
    data_l = dec & 0x00ff
    data_h = dec >> 8
    return [data_h, data_l]


'''
该函数用于根据10进制数计算8bits的16进制数
'''


def dec2hex_8bits(dec):
    if dec < 0:
        dec &= 0xff
    return [dec]


'''
该函数用于根据数据帧计算checksum校验位值
'''


def sendchecksum_add8(list):
    sum = 0
    for i in list:
        sum += i
    sum &= 0xff
    sum = (0 - sum) & 0xff
    # print(hex(sum))
    return [sum]


'''
该函数用于检验接收到的数据帧
'''


def receivechecksum_add8(list):
    sum = 0
    for i in list:
        sum += i
    sum &= 0xff
    if sum == 0:
        return True
    else:
        return False


'''
该函数用于将数据帧转换成字符串以便发送
'''


def numlist2str(numlist):
    sendstr = ""
    for i in numlist:
        sendstr += str(hex(i)).strip('0x').zfill(2)
    return sendstr


def main():
    a = cmdparser()
    a.openserialport()

    for i in range(0, 2001,5):
        a.setspeed(-i, i)
        time.sleep(0.05)
    a.stopcar(0)
    time.sleep(5)
    for i in range(2000,-1,-5):
        a.setspeed(-i, i)
        time.sleep(0.05)


if __name__ == "__main__":
    main()
