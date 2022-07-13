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
#import server as sv
if __name__ == '__main__':
    car = cmd.cmdparser()
    car.openserialport()
    speed = 300
    capture = cv2.VideoCapture(0)

    width = 640  #定义摄像头获取图像宽度
    height = 480   #定义摄像头获取图像长度

    capture.set(cv2.CAP_PROP_FRAME_WIDTH, width)  #设置宽度
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height)  #设置长度

    net = get_model(
            "nanodet",
            target_size=320,
            prob_threshold=0.4,
            nms_threshold=0.5,
            num_threads=4,
            use_gpu=True,
            )
    eps = 50
    while True:
        loop_start = cv2.getTickCount()
        ret, m = capture.read()
        #a = datetime.datetime.now()
        objects = net(m)
        has_detected = False
        for object in objects:
            if object.label==0 and not has_detected:
                has_detected = True
                rectx = object.rect.x
                recty = object.rect.y
                recth = object.rect.h
                rectw = object.rect.w
                mid = (2*rectx+rectw)/2
                leftspeed=rightspeed=0
                if mid - width / 2 > eps:
                    leftspeed = speed
                    rightspeed = -speed
                    print("right,speed = ",speed)
                elif mid - width/2 <-eps:
                    leftspeed = -speed
                    rightspeed = speed
                    print('you shoud turn left,speed= ',speed)
                else:
                    leftspeed =  speed
                    rightspeed = speed
                    print('go ahead')
                car.setspeed(leftspeed,rightspeed)
                time.sleep(0.05)

        draw_detection_objects(m, net.class_names, objects)
        loop_time = cv2.getTickCount()-loop_start
        total_time = loop_time/(cv2.getTickFrequency())
        running_FPS = 1/total_time
        print("running_fps:",running_FPS)
        #a = sv.img_send()
        #a.img_encode(m)

        
        if cv2.waitKey(10) == ord('q'):
            break

    car.stopcar(0)
    cv2.destroyAllWindows()	
           

    
    
