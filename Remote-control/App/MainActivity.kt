package com.example.carcontroller

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.ImageButton
import kotlinx.android.synthetic.main.activity_main.*
import kotlinx.android.synthetic.main.activity_main.view.*
import java.io.IOException
import java.io.OutputStream
import java.io.PrintWriter
import java.lang.Exception
import java.net.*


class MainActivity : AppCompatActivity() {
    private lateinit var ds:DatagramSocket
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        //小车连接服务器


        val button_connect:Button = findViewById(R.id.connectcar)
        button_connect.setOnClickListener {
            initThread()
        }
        val buttonleft:ImageButton = findViewById(R.id.TurnLeft)
        buttonleft.setOnClickListener {
            var str = "left -300 300 "
            sendMessage(str)
            print("Turn left\n")
        }
        val buttonRight:ImageButton = findViewById(R.id.TurnRight)
        buttonRight.setOnClickListener {
            var str = "Right 300 -300 "
            sendMessage(str)
            print("Turn Right\n")
        }
        val buttonFront:ImageButton = findViewById(R.id.TurnFront)
        buttonFront.setOnClickListener {
            var str = "front 300 300 "
            sendMessage(str)
            print("Go ahead\n")
        }
        val buttonBack:ImageButton = findViewById(R.id.TurnBack)
        buttonBack.setOnClickListener {
            var str = "back -300 -300 "
            sendMessage(str)
            print("Go back\n")
        }
        val buttonstop:ImageButton = findViewById(R.id.stop)
        buttonstop.setOnClickListener {
            var str = "stop 0 0 "
            sendMessage(str)
            print("stop\n")
        }

    }

    private fun initThread(){
        Thread(Runnable {
            initConnect()
        }).start()
    }
    fun initConnect(){
        var serveraddr:InetAddress = InetAddress.getByName("106.14.215.37")
        ds = DatagramSocket()
        ds.connect(serveraddr,3391)
        println("success")
    }
    fun sendMessage(msg:String){
        var data_send:ByteArray = msg.toByteArray()
        var dp:DatagramPacket = DatagramPacket(data_send,data_send.size-1)
        ds.send(dp)
    }
}