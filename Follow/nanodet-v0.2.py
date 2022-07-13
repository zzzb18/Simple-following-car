# Tencent is pleased to support the open source community by making ncnn available.
#
# Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
#
# Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
# in compliance with the License. You may obtain a copy of the License at
#
# https://opensource.org/licenses/BSD-3-Clause
#
# Unless required by applicable law or agreed to in writing, software distributed
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
import sys
import cv2
import time
import numpy as np
import ncnn
from ncnn.model_zoo import get_model
from ncnn.utils import draw_detection_objects
import cmd2serial as cmd
def mask(img):
    length = img.shape[1]
    width = img.shape[0]
    point1 = (length*1/3,0)#左下顶点
    point2 = (length*1/3,width)#左上顶点
    point3 = (length*2/3,width)#右上顶点
    point4 = (length*2/3,0)#右下顶点
    points = np.array([[point1,point2,point3,point4]],dtype=np.int32)#梯形区域
    newmask = np.zeros_like(img)
    newmask = cv2.fillPoly(newmask,points,color=(255,255,255,0))
    img = cv2.bitwise_and(img,newmask)#将划定的区域与原图进行叠加
    return img
def dynamicmask(img,x,y):
    epsx = 150
    epsy = 150
    point1 = (x-epsx,y-epsy)
    point2 = (x+epsx,y-epsy)
    point4 = (x-epsx,y+epsy)
    point3 = (x+epsx,y+epsy)
    points = np.array([[point1,point2,point3,point4]],dtype=np.int32)#梯形区域 
    newmask = np.zeros_like(img)
    newmask = cv2.fillPoly(newmask,points,color=(255,255,255,0))
    img = cv2.bitwise_and(img,newmask)#将划定的区域与原图进行叠加
    return img

if __name__ == "__main__":
    car = cmd.cmdparser()
    car.openserialport()
    speed = 300
    capture = cv2.VideoCapture(0)

    width = 640  #定义摄像头获取图像宽度
    height = 480  #定义摄像头获取图像长度

    #width = 320
    #height = 240

    capture.set(cv2.CAP_PROP_FRAME_WIDTH, width)  #设置宽度
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height)  #设置长度
    #capture.set(3,320)
    #capture.set(4,240)
    net = get_model(
            "nanodet",
            target_size=320,
            prob_threshold=0.4,
            nms_threshold=0.5,
            num_threads=4,
            use_gpu=True,
            )
    eps = 70
    roix = 0
    roiy = 0
    while True:
        loop_start = cv2.getTickCount()
        ret, m = capture.read()
        #m = cv2.cvtColor(m,cv2.COLOR_RGB2GRAY)
        #a = time.time()
        if roix==0 or roiy==0:
            #m = mask(m)
            pass
        else:
            m = dynamicmask(m,roix,roiy)
        objects = net(m)
        has_detected = False
        max_prob_object = None
        for object in objects:
            if object.label==0 and not has_detected:
                has_detected = True
                if max_prob_object==None or object.prob > max_prob_object.prob:
                    max_prob_object = object
        if not has_detected:
            roix = 0
            roiy = 0
        else:
            rectx = max_prob_object.rect.x
            recty = max_prob_object.rect.y
            recth = max_prob_object.rect.h
            rectw = max_prob_object.rect.w
            midx = (2*rectx+rectw)/2
            midy = (2*recty+recth)/2
            roix = midx
            roiy = midy
            leftspeed=rightspeed=0
            if midx - width / 2 > eps:
                leftspeed = speed
                rightspeed = speed-400
                eps = 50
                print('you should turn right')
            elif midx - width/2 <-eps:
                leftspeed = speed-400
                rightspeed = speed
                eps = 50
                print('you shoud turn left')
            else:
                leftspeed =  speed
                rightspeed = speed
                eps= 30
                print('go ahead')
            car.setspeed(leftspeed,rightspeed)
            time.sleep(2)
            car.stopcar(0)

        draw_detection_objects(m, net.class_names, objects)
        loop_time = cv2.getTickCount()-loop_start
        total_time = loop_time/(cv2.getTickFrequency())
        running_FPS = 1/total_time
        print("running_fps:",running_FPS)
        if cv2.waitKey(10) == ord('q'):
            break
            
    car.stopcar(0)
    cv2.destroyAllWindows()	

