import time
import cv2
import numpy as np
import socket
from cmd2serial import *
ip_port = ('127.0.0.1', 3391)
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # udp协议
server.bind(ip_port)

lastindex = [-1, -1]
data_total0 = []
data_total1 = []
pic_index = [0, 0]
pic_cnt = [0, 0]
cnt_lost = 0


def udp_receive():
    global imgshow
    BUFSIZE = 100000
    #server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # udp协议
    #server.bind(ip_port)
    global lastindex, data_total0, data_total1, pic_index, pic_cnt, cnt_lost

    my_cmdparser = cmdparser()
    my_cmdparser.openserialport()

    while True:
        data, client_addr = server.recvfrom(BUFSIZE)
        print('server收到的数据', data)
        backup = data.decode()
        speedlist = backup.split(" ")
        mode = speedlist[0]
        leftspeed = int(speedlist[1])
        rightspeed = int(speedlist[2])

        my_cmdparser.setspeed(leftspeed,rightspeed)

        if data:
            # imde = np.zeros((720, 1280, 3), dtype=np.uint8)
            # print(len(data[2:]))

            i = int.from_bytes(data[0:1], byteorder='big')  # 一张中的第几组数据
            j = int.from_bytes(data[1:2], byteorder='big')  # 一张中的共有j组
            k = int.from_bytes(data[2:3], byteorder='big')  # 校验和
            l = int.from_bytes(data[3:4], byteorder='big')
            # print(i, j, k, l)

            if l % 2 == 0:
                if lastindex[0] != l:
                    pic_cnt[0] = 0

                pic_index[0] = int.from_bytes(data[3:4], byteorder='big')
                pic_cnt[0] += 1
                if i == j - 1:
                    data_total0[i * 1400:] = list(data[4:])
                else:
                    data_total0[i * 1400:(i + 1) * 1400] = list(data[4:])
                # print(i,j,k,pic_cnt[0],k,pic_index[0])
                if pic_cnt[0] == j:
                    b = bytes(data_total0)
                    stringdata = np.frombuffer(b, dtype='uint8')
                    length = len(stringdata)
                    if k == length % 100:
                        img = cv2.imdecode(stringdata, 1)
                        if l == 200:
                            print(cnt_lost / 200)
                            cnt_lost = 0
                        cnt_lost += 1
                        lastindex[0] = l
                        return img
                        # img = cv2.resize(img, (640, 480))
                lastindex[0] = l
            else:
                if lastindex[1] != l:
                    pic_cnt[1] = 0
                pic_index[1] = int.from_bytes(data[3:4], byteorder='big')
                pic_cnt[1] += 1
                if i == j - 1:
                    data_total1[i * 1400:] = list(data[4:])
                else:
                    data_total1[i * 1400:(i + 1) * 1400] = list(data[4:])
                # print(i, j, k, pic_cnt[1], k, pic_index[1])
                if pic_cnt[1] == j:
                    b = bytes(data_total1)
                    stringdata = np.frombuffer(b, dtype='uint8')
                    length = len(stringdata)
                    if k == length % 100:
                        img = cv2.imdecode(stringdata, 1)
                        try:
                            # print(l)
                            cnt_lost += 1
                            # img = cv2.resize(img, (640, 480))
                            lastindex[1] = l
                            return img

                        except:
                            print("show error")
                lastindex[1] = l


if __name__ == '__main__':
    while True:
        cv2.imshow('12', udp_receive())
        if cv2.waitKey(1) == ord('q'):
            break
