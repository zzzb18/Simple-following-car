# 操控界面，windows端口的操控界面
# coding=utf-8
import socket
import threading
import time
import datetime
import PIL
import cv2
from PIL import Image, ImageTk
import tkinter as tk
import client
import os

data_cache = 'stop'
speed = 0  # 规定速度
vid_add = ("127.0.0.1", 3390)  # 视频用ip及端口
ctrl_add = ("106.14.215.37", 3391)  # 控制用ip及端口

# vid 8848
# ctrl 8888
'''
192.168.43.128
    局域网：vid："192.168.10.15", 8888 ctrl:"192.168.10.15", 8081
    本机测试用： vid: "127.0.0.1", 8848 ctrl:"127.0.0.1", 8081
'''


# 控制用服务器端口：'124.222.199.134', 8081


class Control_Object:
    def __init__(self):
        self.speedlist=["Disconnected","0","0"]
        self.window = tk.Tk()
        self.link = RemoteVideoCapture()
        self.ui()
        self.window.resizable(1, 1)

        self.window.mainloop()

    def camera_f(self):
        """设置camera容器"""
        self.cameraframe = tk.Frame(self.window)
        self.cameraframe.grid(row=0, column=0)
        '''设置canvas画布'''
        self.canvas = tk.Canvas(self.cameraframe, width=640, height=480)
        self.canvas.grid(row=0, column=0)
        self.delay = 5  # 延迟15ms
        self.update()  # 调用图像更新函数

    def update(self):
        ret, frame = self.link.get_frame()
        print(1)
        if ret:
            self.photo = PIL.ImageTk.PhotoImage(image=PIL.Image.fromarray(frame))
            self.canvas.create_image(0, 0, image=self.photo, anchor=tk.NW)
        self.window.after(self.delay, self.update)  # 整个界面在delay事件后重复刷新，并重复调用update函数

    def ui(self):
        window = self.window  # 生成界面对象
        window.title('breeze waiting')
        '''相机页面容器及页面设置'''
        cameraframe = tk.Frame(window)
        cameraframe.grid(row=0, column=0)
        canvas = tk.Canvas(cameraframe, bg='gray', width=640, heigh=480)
        canvas.grid(row=0, column=0)

        '''生成控制容器'''
        controlframe = tk.Frame(window)
        controlframe.grid(row=0, column=1)

        '''
            button 参数： 在contolframe容器上生成，
            repeatdelay：按下10ms之后开始重复执行函数，
            repeatinterval：每300ms执行一次
        '''
        bn0 = tk.Button(controlframe, text='↑', repeatdelay=10, repeatinterval=300,
                        command=lambda: self.control('front'), heigh=2, width=5)
        bn1 = tk.Button(controlframe, text='↓', repeatdelay=10, repeatinterval=300,
                        command=lambda: self.control('back'), heigh=2, width=5)
        bn2 = tk.Button(controlframe, text='→', repeatdelay=10, repeatinterval=300,
                        command=lambda: self.control('right'), heigh=2, width=5)
        bn3 = tk.Button(controlframe, text='←', repeatdelay=10, repeatinterval=300,
                        command=lambda: self.control('left'), heigh=2, width=5)
        bn4 = tk.Button(controlframe, text='□', repeatdelay=10, repeatinterval=300,
                        command=lambda: self.control('em_stop'), heigh=2, width=5)
        bn5 = tk.Button(controlframe, text='light_on', command=lambda: self.control('light1'), heigh=2, width=18)
        bn6 = tk.Button(controlframe, text='camera', command=lambda: self.camera_f(), heigh=2, width=18)
        bn7 = tk.Button(controlframe, text='connect', command=lambda: self.link_b(), heigh=2, width=18)
        bn8 = tk.Button(controlframe, text='V1', command=lambda: self.gear(500), heigh=2, width=5)
        bn9 = tk.Button(controlframe, text='V2', command=lambda: self.gear(700), heigh=2, width=5)
        bn10 = tk.Button(controlframe, text='V3', command=lambda: self.gear(1100), heigh=2, width=5)
        bn11 = tk.Button(controlframe, text='V4', command=lambda: self.gear(1400), heigh=2, width=5)
        bn12 = tk.Button(controlframe, text='V5', command=lambda: self.gear(1700), heigh=2, width=5)
        bn13 = tk.Button(controlframe, text='V6', command=lambda: self.gear(2001), heigh=2, width=5)
        bn14 = tk.Button(controlframe, text='light_off', command=lambda: self.control('light0'), heigh=2, width=18)
        #bn15 = tk.Button(controlframe, text='auto', command=lambda: self.control('auto'), heigh=2, width=18)#自动控制开启
        bn0.grid(row=2, column=1)
        bn1.grid(row=4, column=1)
        bn2.grid(row=3, column=2)
        bn3.grid(row=3, column=0)
        bn4.grid(row=3, column=1)
        bn5.grid(row=5, column=0, columnspan=3)  # light_on
        bn6.grid(row=7, column=0, columnspan=3)  # camera
        bn7.grid(row=8, column=0, columnspan=3)  # connect
        bn8.grid(row=0, column=0)
        bn9.grid(row=0, column=1)
        bn10.grid(row=0, column=2)
        bn11.grid(row=1, column=0)
        bn12.grid(row=1, column=1)
        bn13.grid(row=1, column=2)
        bn14.grid(row=6, column=0, columnspan=3)  # light_off
        #bn15.grid(row=10, column=0, columnspan=3)
        '''<ButtonRelease-1>:鼠标左键弹起时执行相应的函数'''
        bn0.bind("<ButtonRelease-1>", lambda event: self.control('em_stop', event))
        bn1.bind("<ButtonRelease-1>", lambda event: self.control('em_stop', event))
        bn2.bind("<ButtonRelease-1>", lambda event: self.control('em_stop', event))
        bn3.bind("<ButtonRelease-1>", lambda event: self.control('em_stop', event))
        '''在控制容器中绑定键盘事件'''
        window.focus_set()
        window.bind("<KeyPress-Up>", lambda event: self.control('front', event))
        window.bind("<KeyPress-Down>", lambda event: self.control('back', event))
        window.bind("<KeyPress-Right>", lambda event: self.control('right', event))
        window.bind("<KeyPress-Left>", lambda event: self.control('left', event))
        window.bind("<KeyPress-space>", lambda event: self.control('em_stop', event))
        window.bind("<KeyPress-f>", lambda event: self.control('light', event))
        window.bind("<KeyPress-1>", lambda event: self.gear(500))
        window.bind("<KeyPress-2>", lambda event: self.gear(810))
        window.bind("<KeyPress-3>", lambda event: self.gear(1100))
        window.bind("<KeyPress-4>", lambda event: self.gear(1400))
        window.bind("<KeyPress-5>", lambda event: self.gear(1700))
        window.bind("<KeyPress-6>", lambda event: self.gear(2000))
        '''键盘释放时，执行相应的函数'''
        window.bind("<KeyRelease-Up>", lambda event: self.control('em_stop', event))
        window.bind("<KeyRelease-Down>", lambda event: self.control('em_stop', event))
        window.bind("<KeyRelease-Right>", lambda event: self.control('em_stop', event))
        window.bind("<KeyRelease-Left>", lambda event: self.control('em_stop', event))

        infoframe=tk.Frame(window)
        infoframe.grid(row=0, column=2)
        info = tk.Text(infoframe, width=20, height=24)
        info.grid(row=0, column=0, padx=10, pady=5)
        infobn = tk.Button(infoframe, text='Info', command=self.showinfo, heigh=2, width=16)
        infobn.grid(row=1, column=0, padx=10, pady=5)
        info.insert('insert', '未连接')
        self.info = info

    def showinfo(self):
        self.info.delete(1.0, 'end')
        datestr=str(datetime.datetime.now().strftime('%H:%M:%S'))
        speedlist2 = self.speedlist.split()
        infolist = [datestr,speedlist2[0],speedlist2[1],speedlist2[2]]
        #print(infolist)
        infolistname = ['Time','Car state:','L_speed:','R_speed:']
        for i in range(0, len(infolist)):
            self.info.insert('end', str(infolistname[i])+str(infolist[i])  + '\n')
        self.window.after(5, self.showinfo)

    def control(self, data, event=None):
        """过滤程序内容，减少发生的重复信息"""
        global data_cache
        if data_cache != data:
            print('control message:' + data)
            self.send_data(data)  # socket发送信息
            data_cache = data

    def send_data(self, mode):
        L_k = 0
        R_k = 0
        if mode == 'front':
            L_k = 1
            R_k = 1
        elif mode == 'back':
            L_k = -1
            R_k = -1
        elif mode == 'left':
            L_k = -1
            R_k = 1
        elif mode == 'right':
            L_k = 1
            R_k = -1
        elif mode == 'em_stop':
            L_k = 0
            R_k = 0
        elif mode == 'stop':
            L_k = 0
            R_k = 0
        else:
            pass
        speed_list = mode+" "+str(L_k * speed)+" "+str(R_k * speed)
        self.speedlist = speed_list
        print('send data:' + speed_list)
        try:
            self.cl.con_send(speed_list)
        except:
            print('con_send error')


    def gear(self, speed_args):
        """控制全局变量speed的值"""
        global speed
        speed = speed_args

    def link_b(self):
        global ctrl_add
        time.sleep(0.5)
        self.link.link()
        self.cl = RemoteControl(ctrl_add)
        a = threading.Thread(target=self.cl.link, args=())
        a.start()
        a.join()


class RemoteVideoCapture:
    """该模块为调用远程摄像头的模块"""

    def __init__(self):
        global vid_add
        #self.connect = client.Camera_Connect_Object(vid_add)
        #self.connect.add_port = tuple(self.connect.add_port)  # 列表转换为元组

    def link(self):
        #a = threading.Thread(target=self.connect.Socket_Connect, args=())
        #a.start()
        pass

    def get_frame(self):
        #try:
        frame = client.udp_receive()
        cv2.flip(frame,0,frame)
        frame = cv2.resize(frame, (640, 480))
        frame = cv2.flip(frame,0,dst=None)
        frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        return True, frame
        #except:
        #print("failed")


class RemoteControl:
    """远程控制的模块"""

    def __init__(self, adds_port=("127.0.0.1", 3390)):
        self.adds_port = adds_port
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    def link(self):
        try:
            self.sock.connect(self.adds_port)  # 控制ip
            print("RemoteControl IP is %s:%d" % (self.adds_port[0], self.adds_port[1]))
        except:
            print('ctrl link failed')

    def con_send(self, data):
        """发送控制信息，字符串转字节发送"""
        self.sock.sendto(data.encode(), self.adds_port)


if __name__ == '__main__':
    b = threading.Thread(target=lambda: Control_Object())
    b.start()
    b.join()
