#!/usr/bin/env python
# -*- coding: utf-8 -*-
import socket
import cv2
import numpy as np
import datetime
import time
import struct

'''使用udp协议发送视频帧'''

cnt = 0
ip_port = ('106.14.215.37', 3390)



class img_send:
    def __init__(self):
        self.bytes_size = 60000
        self.server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.IMAGE_SIZE_X, self.IMAGE_SIZE_Y = 320, 240
        self.cap = cv2.VideoCapture(0)
        self.cap.set(3, self.IMAGE_SIZE_X)
        self.cap.set(4, self.IMAGE_SIZE_Y)

    def code_send(self, data):
        global ip_port
        self.server.sendto(data, ip_port)

    def img_encode(self, img):
        global cnt
        cnt += 1
        if cnt > 200:
            cnt = 0
        encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 40]
        img_code = cv2.imencode('.jpg', img, encode_param)[1]  # 视频帧编码
        data = np.array(img_code)
        stringData = data.tobytes()
        length = len(stringData)
        length_num = length // 1400 + 1
        # print(length, length_num)
        i = length - (length // 100) * 100  # 取编码长度的个位和十位
        for k in range(length_num):
            j = length_num
            if k == length_num - 1:
                self.code_send((struct.pack('B', k)) + (struct.pack('B', j)) + (struct.pack('B', i))
                               + (struct.pack('B', cnt)) + stringData[k * 1400:length])
            else:
                self.code_send((struct.pack('B', k)) + (struct.pack('B', j)) + (struct.pack('B', i))
                               + (struct.pack('B', cnt)) + stringData[k * 1400:(k + 1) * 1400])

    def img_split(self, img):
        a = datetime.datetime.now()
        self.img_encode(img)
        print(datetime.datetime.now() - a)

    def main(self):
        try:
            while True:
                success, image = self.cap.read()
                self.img_encode(image)
        except:
            self.__del__()

    def __del__(self):
        self.cap.release()
        self.server.close()


def main():
    try:
        a = img_send()
        a.main()
    except:
        img_send().__del__()


if __name__ == '__main__':
    main()
