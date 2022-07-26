import numpy as np
import cv2
import socket
import base64
import struct
addr = ('106.14.215.37',3390)
cap = cv2.VideoCapture(0)
cap.set(3, 320)
cap.set(4, 240)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


def image_to_base64(image_np):
    image = cv2.imencode('.jpg', image_np)[1]
    image_code = str(base64.b64encode(image))[2:-1]

    return image_code

def split_img(stringdata,maxsize):

    data = stringdata
    len = data.__sizeof__()
    print("总包大小为：",len)
    while len > maxsize:
        pack = data[0:maxsize]
        rlen= s.sendto(pack, addr)
        data = data[maxsize:]
        len -= maxsize
        print("本次拆分后剩余的字节长度为", len,"发送了",rlen)
    if len // maxsize == 0:
        pack = data[0:len]+"end".encode()
        llen = s.sendto(pack, addr)

        print("本次结束",llen)



while True:
    _,img = cap.read()
    img = cv2.flip(img,1)
    #img = cv2.resize(img,(80,80))
    encode_param = [int(cv2.IMWRITE_JPEG_QUALITY), 40]
    img_code = cv2.imencode('.jpg', img,encode_param)[1] # 视频帧编码
    #img_new = cv2.imdecode(img_code, cv2.IMREAD_COLOR)
    data = np.array(img_code)
    stringData = data.tobytes()
    split_img(stringData,1000)
    #s.sendto("begin".encode()+stringData+"end".encode(),addr)
    #s.sendto("begin".encode()+stringData,addr)
    size = stringData.__sizeof__()
    #print("原始",stringData.__sizeof__())
    # while size > 0:
    #     sendsize = s.sendto(stringData,stringData+size,0)
    #     size = size - sendsize

    #cv2.imshow('server', img_new)
    #size = s.sendto(stringData,addr)
    #print("发送了",size)
    #a = "hello app"
    #x =a.encode()
    #s.sendto(x,addr)
    if cv2.waitKey(1000) & 0xFF == ord('q'):
        break
s.close()
# while True:
#     img = cv2.imread('test.jpg')
#     img_code = cv2.imencode('.jpg', img)[1]
#     data = np.array(img_code)
#     stringData = data.tobytes()
#     cv2.imshow('server', img)
#     s.sendto(stringData,addr)
#     cv2.waitKey(0)
