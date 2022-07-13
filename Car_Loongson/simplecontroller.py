import time
import tkinter as tk

import cmd2serial as cmd


class controller:
    def __init__(self):
        self.root = tk.Tk()
        self.parser = cmd.cmdparser()
        self.setwindows()

        self.parser.openserialport()

    def setwindows(self):
        root = self.root
        commandframe = tk.Frame(root)
        commandframe.grid(row=0, column=1, sticky='n')
        controlframe = tk.LabelFrame(commandframe, text='基础控制', padx=5)
        controlframe.grid(row=0, column=0, padx=2, pady=2, sticky='n')
        keyframe = tk.Frame(controlframe)
        keyframe.grid(row=0, column=0, padx=2, pady=2)
        tk.Button(keyframe, text='Front', width=8, height=4, command=lambda: self.keyboard_callback('Front')).grid(
            row=0,
            column=0,
            padx=2,
            pady=2)
        tk.Button(keyframe, text='Back', width=8, height=4, command=lambda: self.keyboard_callback('Back')).grid(row=1,
                                                                                                                 column=0,
                                                                                                                 padx=2,
                                                                                                                 pady=2)
        tk.Button(keyframe, text='Left', width=8, height=4, command=lambda: self.keyboard_callback('Left')).grid(row=0,
                                                                                                                 column=1,
                                                                                                                 padx=2,
                                                                                                                 pady=2)
        tk.Button(keyframe, text='Right', width=8, height=4, command=lambda: self.keyboard_callback('Right')).grid(
            row=1,
            column=1,
            padx=2,
            pady=2)
        tk.Button(keyframe, text='STOP', width=8, height=4, command=lambda: self.keyboard_callback('stop')).grid(row=0,
                                                                                                                 column=2,
                                                                                                                 padx=2,
                                                                                                                 pady=2)
        tk.Button(keyframe, text='EM_STOP', width=8, height=4, command=lambda: self.keyboard_callback('em_stop')).grid(
            row=1,
            column=2,
            padx=2,
            pady=2)

        keyframe2 = tk.Frame(controlframe)
        keyframe2.grid(row=1, column=0)
        tk.Button(keyframe2, text='开启键盘映射', width=10, height=2, command=self.keyboard_detect).grid(row=0, column=0,
                                                                                                   padx=2, pady=2)
        tk.Button(keyframe2, text='关闭键盘映射', width=10, height=2, command=self.cancel_keyboard_detect).grid(row=0,
                                                                                                          column=1,
                                                                                                          padx=2,
                                                                                                          pady=2)
        keyframe3 = tk.Frame(controlframe)
        keyframe3.grid(row=2, column=0)
        speed = tk.Scale(keyframe3, from_=0, to=100, resolution=1, orient="horizontal", length=150)
        speed.grid(row=0)
        tk.Label(keyframe3, text="速度（1-100Hz/s）").grid(row=1)

        self.speed = speed

    def keyboard_detect(self):
        frame = self.root
        print("键盘映射已打开")
        frame.focus_set()
        frame.bind("<KeyPress-W>", lambda event: self.keyboard_callback('Front', event))
        frame.bind("<KeyPress-S>", lambda event: self.keyboard_callback('Back', event))
        frame.bind("<KeyPress-A>", lambda event: self.keyboard_callback('Left', event))
        frame.bind("<KeyPress-D>", lambda event: self.keyboard_callback('Right', event))
        frame.bind("<KeyPress-Q>", lambda event: self.keyboard_callback('stop', event))
        frame.bind("<KeyPress-E>", lambda event: self.keyboard_callback('em_stop', event))

        frame.bind("<KeyPress-w>", lambda event: self.keyboard_callback('Front', event))
        frame.bind("<KeyPress-s>", lambda event: self.keyboard_callback('Back', event))
        frame.bind("<KeyPress-a>", lambda event: self.keyboard_callback('Left', event))
        frame.bind("<KeyPress-d>", lambda event: self.keyboard_callback('Right', event))
        frame.bind("<KeyPress-q>", lambda event: self.keyboard_callback('stop', event))
        frame.bind("<KeyPress-e>", lambda event: self.keyboard_callback('em_stop', event))

    def cancel_keyboard_detect(self):
        print("键盘映射已关闭")
        frame = self.root
        frame.unbind("<KeyPress-W>")
        frame.unbind("<KeyPress-S>")
        frame.unbind("<KeyPress-A>")
        frame.unbind("<KeyPress-D>")
        frame.unbind("<KeyPress-Q>")
        frame.unbind("<KeyPress-E>")
        frame.unbind("<KeyPress-w>")
        frame.unbind("<KeyPress-s>")
        frame.unbind("<KeyPress-a>")
        frame.unbind("<KeyPress-d>")
        frame.unbind("<KeyPress-q>")
        frame.unbind("<KeyPress-e>")

    def keyboard_callback(self, command, event=None):
        speed = 10 * self.speed.get()

        if command == 'Front':
            self.parser.setspeed(speed, speed)
            time.sleep(0.05)
        elif command == 'Back':
            self.parser.setspeed(-speed, -speed)
            time.sleep(0.05)
        elif command == 'Left':
            self.parser.setspeed(speed - 800, speed)
            time.sleep(0.05)
        elif command == 'Right':
            self.parser.setspeed(speed, speed - 800)
            time.sleep(0.05)
        elif command == 'stop':
            self.parser.stopcar(0)
            time.sleep(0.05)
        elif command == 'em_stop':
            self.parser.stopcar(1)
            time.sleep(0.05)


def main():
    a = controller()
    a.root.title("小车简易调试器")
    a.root.resizable(0, 0)
    a.root.mainloop()


if __name__ == "__main__":
    main()
