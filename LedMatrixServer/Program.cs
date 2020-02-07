using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

namespace LedMatrixServer {
    class Program {

        static void Main(string[] args) {
            //using var matrix = new LedMatrixServer(15, 15, new Serial("COM7", 115200));
            //using var matrix = new LedMatrixServer(15, 15, new Tcp("10.10.10.237", 4321));
            using var matrix = new LedMatrixServer(15, 15, new Udp("10.10.10.237", 1234));
            //var matrix = new LedMatrixServer(15, 15, "COM7", 115200);

            //(new Thread(i => {
            //    while (true) {
            //        matrix.PrintIncoming();
            //    }
            //})).Start();

            //while (true)
            //{
            //    matrix.DisplayComputerMonitor();
            //    matrix.Draw();
            //}

            matrix.SetColor(Color.Black);
            //while (true)
            //{
            //    matrix.SetPixel(5, 5, 10, 0, 0);
            //    matrix.Draw();
            //    matrix.SetColor(Color.FromArgb(10, 10, 0));
            //    matrix.SetPixel(5, 5, 0, 10, 0);
            //    matrix.Draw();
            //    matrix.SetPixel(5, 5, 0, 0, 10);
            //    matrix.Draw();
            //    matrix.SetColor(Color.FromArgb(10, 0, 0));
            //}


            //while (true)
            //{
            //    int delay = 0;
            //    int brightness = 200;
            //    matrix.SetColor(Color.FromArgb(brightness, 0, 0));
            //    Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(brightness, brightness, 0));
            //    Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(0, brightness, 0));
            //    Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(0, brightness, brightness));
            //    Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(0, 0, brightness));
            //    Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(brightness, 0, brightness));
            //    Thread.Sleep(delay);
            //}
            //while (true)
            //{
            //    int delay = 0;
            //    int brightness = 200;
            //    matrix.SetColor(Color.FromArgb(brightness, 0, 0));
            //    Thread.Sleep(delay);
            //    //matrix.SetColor(Color.FromArgb(brightness, brightness, 0));
            //    //Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(0, brightness, 0));
            //    Thread.Sleep(delay);
            //    //matrix.SetColor(Color.FromArgb(0, brightness, brightness));
            //    //Thread.Sleep(delay);
            //    matrix.SetColor(Color.FromArgb(0, 0, brightness));
            //    Thread.Sleep(delay);
            //    //matrix.SetColor(Color.FromArgb(brightness, 0, brightness));
            //    //Thread.Sleep(delay);
            //}

            //matrix.Flush();
            matrix.RenderGif("banana.gif", 0, 10, true);
            //matrix.RenderGif("banana.gif", 0, 100, false);
            //while (true)
            //{
            //    matrix.RenderGif("banana.gif", 3, 100, false);
            //}
            matrix.SetPixel(5, 5, 255, 255, 255);
            matrix.Draw();

        }
    }
}
