package com.example.carcontroller

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.ImageDecoder
import android.graphics.drawable.Drawable
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Message
import android.widget.Button
import android.widget.ImageButton
import android.widget.ImageView
import android.widget.Toast
import androidx.annotation.RequiresApi
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.io.IOException
import java.io.OutputStream
import java.io.PrintWriter
import java.lang.Exception
import java.lang.ref.WeakReference
import java.net.*
import java.nio.ByteBuffer
import java.time.LocalDate
import java.util.*
import kotlin.experimental.inv


class MainActivity : AppCompatActivity() {
    private lateinit var ds: DatagramSocket
    private lateinit var vs: DatagramSocket
    private var img: Drawable? = null
    public var picture: Bitmap? = null
    private var lastindex = intArrayOf(-1, 1)
    private lateinit var data_total0: ByteArray
    private lateinit var data_total1: ByteArray
    private var pic_index = intArrayOf(0, 0)
    private var pic_cnt = intArrayOf(0, 0)
    private var cnt_lost = 0

    @RequiresApi(Build.VERSION_CODES.S)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //小车连接服务器
        var carspeed = 300; // default speed(默认速度）
        var speedlist: String = ""
        var button_setspeed: Button = findViewById(R.id.changespeed)
        button_setspeed.setOnClickListener {
            var str: String = setspeed.text.toString()
            if (str.isNotEmpty()) {
                try {
                    carspeed = Integer.parseInt(str)
                } catch (e: Exception) {
                    println("输入的速度格式非法！")
                    Toast.makeText(
                        getApplicationContext(), "输入的速度格式非法！",
                        Toast.LENGTH_SHORT
                    ).show();
                } finally {
                    Toast.makeText(
                        getApplicationContext(), "速度设置为 " + carspeed.toString(),
                        Toast.LENGTH_SHORT
                    ).show();
                }
            } else {
                Toast.makeText(
                    getApplicationContext(), "输入为空！",
                    Toast.LENGTH_SHORT
                ).show();
            }
        }


        //视频连接
        val buttonvideo: Button = findViewById(R.id.remotevideo)
        buttonvideo.setOnClickListener {
            initThread_video()
            //var source:ImageDecoder.Source = ImageDecoder.createSource()
            //var source = ImageDecoder.createSource(assets, "")
            //var source = ImageDecoder.createSource(resources, R.drawable.kunku025)
            //img = ImageDecoder.decodeDrawable(source)
            if (picture != null) {
                videoshow.setImageBitmap(picture)
                println("已经设置picture")
            } else {
                println("未设置")
            }
        }

        val button_connect: Button = findViewById(R.id.connectcar)
        button_connect.setOnClickListener {
            initThread_control()
            Toast.makeText(
                getApplicationContext(), "连接成功！",
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonleft: ImageButton = findViewById(R.id.TurnLeft)
        buttonleft.setOnClickListener {
            speedlist = "left " + (carspeed - 800).toString() + " " + carspeed.toString() + " "
            sendMessage(speedlist)
            print("Turn left\n")
            Toast.makeText(
                getApplicationContext(), "左转 速度" + carspeed.toString(),
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonRight: ImageButton = findViewById(R.id.TurnRight)
        buttonRight.setOnClickListener {
            speedlist = "Right " + carspeed.toString() + " " + (carspeed - 800).toString() + " "
            sendMessage(speedlist)
            print("Turn Right\n")
            Toast.makeText(
                getApplicationContext(), "右转 速度" + carspeed.toString(),
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonFront: ImageButton = findViewById(R.id.TurnFront)
        buttonFront.setOnClickListener {
            speedlist = "Front " + carspeed.toString() + " " + carspeed.toString() + " "
            sendMessage(speedlist)
            print("Go ahead\n")
            Toast.makeText(
                getApplicationContext(), "直行 速度" + carspeed.toString(),
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonBack: ImageButton = findViewById(R.id.TurnBack)
        buttonBack.setOnClickListener {
            speedlist = "Back " + (-carspeed).toString() + " " + (-carspeed).toString() + " "
            sendMessage(speedlist)
            print("Go back\n")
            Toast.makeText(
                getApplicationContext(), "后退 速度" + carspeed.toString(),
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonstop: ImageButton = findViewById(R.id.stop)
        buttonstop.setOnClickListener {
            speedlist = "stop 0 0 "
            sendMessage(speedlist)
            print("stop\n")
            Toast.makeText(
                getApplicationContext(), "停车！",
                Toast.LENGTH_SHORT
            ).show();
        }
        val buttonimstop: Button = findViewById(R.id.imstop)
        buttonimstop.setOnClickListener {
            speedlist = "imstop 0 0 "
            sendMessage(speedlist)
            Toast.makeText(
                getApplicationContext(), "紧急停车！",
                Toast.LENGTH_SHORT
            ).show();
        }
    }

    //控制线程
    private fun initThread_control() {
        Thread(Runnable {
            initConnect_control()
        }).start()
    }

    //图传线程
    @RequiresApi(Build.VERSION_CODES.S)
    private fun initThread_video() {
        Thread(Runnable {
//            var data: ByteArray = ByteArray(10000)
//            try {
//                vs = DatagramSocket(3390)
//                println(vs.toString())
//                println("success！")
//            } catch (e: Exception) {
//                println("has connected")
//            }
//            while(true) {
//                if (data.isNotEmpty()) {
//                    var packet:DatagramPacket = DatagramPacket(data,data.size)
//                    var result:String = String(packet.data,packet.offset,10)
//                    println("data is not empty" +result)
//                    //var source = ImageDecoder.createSource(data)
//                    //img = ImageDecoder.decodeDrawable(source)
//                } else {
//                    println("is empty")
//                }
//            }
            initConnect_video()
        }).start()


    }

    //控制函数
    fun initConnect_control() {
        var serveraddr: InetAddress = InetAddress.getByName("106.14.215.37")
        ds = DatagramSocket()
        ds.connect(serveraddr, 3391)
        println("success")
    }


    //图传函数
    @RequiresApi(Build.VERSION_CODES.S)
    fun initConnect_video() {
        var socket: DatagramSocket;

        try {

            socket = DatagramSocket(3390)
            var cnt = 0
            var packet: DatagramPacket
            var is_first = true
            var data1: ByteArray = ByteArray(1024 * 8)
            while (true) {

                println("cnt=" + cnt)
                packet = DatagramPacket(data1, cnt*1024, 1024)
                socket.receive(packet)

                /*var i = data1.get(0).toInt()
                //println("i=="+i)
                var j = data1.get(1).toInt()
                var k = data1.get(2).toInt()
                var l = data1.get(3).toInt()

                if (l%2 == 0){
                    if (lastindex[0] != l){
                        pic_cnt[0] = 0
                    }
                    pic_index[0] = data1[3].toInt()
                    pic_cnt[0]+=1
                    if (i == j - 1){
                        data1.toList()
                        var t1 = 4
                        var t2 = i*1400
                        while(t1<data1.size){
                            data_total0[t2] = data1[t1]
                            t2++
                            t1++
                        }
                    }
                    else{
                        data1.toList()
                        var t1 = 4
                        var t2 = i*1400
                        while(t1<data1.size&&t2<(i+1)*1400){
                            data_total0[t2] = data1[t1]
                            t2++
                            t1++
                        }
                    }

                    if(pic_cnt[0]==j){
                        var b:ByteArray = data_total0
                        if(k==b.size%100){
                            picture = BitmapFactory.decodeByteArray(b, 0,b.size)
                            println("asdasdasd")
                            if(l==200){
                                println(cnt_lost/200)
                                cnt_lost=0
                            }
                            cnt_lost+=1
                            lastindex[0]=1
                        }
                    lastindex[0] = l
                    }
                }

                else{
                    if (lastindex[1] != l){
                        pic_cnt[1] = 0
                    }
                    pic_index[1] = data1[3].toInt()
                    pic_cnt[1]+=1
                    if (i == j - 1){
                        data1.toList()
                        var t1 = 4
                        var t2 = i*1400
                        while(t1<data1.size){
                            data_total1[t2] = data1[t1]
                            t2++
                            t1++
                        }
                    }
                    else{
                        data1.toList()
                        var t1 = 4
                        var t2 = i*1400
                        while(t1<data1.size&&t2<(i+1)*1400){
                            data_total1[t2] = data1[t1]
                            t2++
                            t1++
                        }
                    }

                    if(pic_cnt[0]==j){
                        var b:ByteArray = data_total1
                        if(k==b.size%100){
                            picture = BitmapFactory.decodeByteArray(b, 0,b.size)
                            if(l==200){
                                println(cnt_lost/200)
                                cnt_lost=0
                            }
                            cnt_lost+=1
                            lastindex[1]=1
                        }
                        lastindex[1] = l
                    }
                }*/

                var result: String = String(packet.data, cnt * 1024, 1024)
                cnt++
                println("包的长度为" + packet.length)
                println("包头信息" + result[0] + result[1] + result[2])
                println("包尾信息" + result[result.length - 3] + result[result.length - 2] + result[result.length - 1])
                println(result)

                if (result.contains("end")) {

                    picture =
                        BitmapFactory.decodeByteArray(
                            data1,
                            0,
                            cnt*1024
                        )

                    cnt = 0
                }
                if (picture != null) {
                    println("非空")
                } else {
                    println("空")
                }

                println(LocalDate.now())
                //var source:ImageDecoder.Source = ImageDecoder.createSource(data)
                //var source = ImageDecoder.createSource(assets, "")
                //var source = ImageDecoder.createSource(resources, R.drawable.kunku025)
                //img = ImageDecoder.decodeDrawable(source)
                //videoshow.setImageDrawable(img)


            }
        } catch (e: Exception) {
            println("fail to connect")
        }


    }

    fun sendMessage(msg: String) {
        var data_send: ByteArray = msg.toByteArray()
        var dp: DatagramPacket = DatagramPacket(data_send, data_send.size - 1)
        ds.send(dp)
    }

}