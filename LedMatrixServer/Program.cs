using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

namespace LedMatrixServer {
    class Program {

        static void Main(string[] args) {
            //using var matrix = new LedMatrixServer(15, 15, new Serial("COM7", 115200));
            using var matrix = new LedMatrixServer(15, 15, new Tcp("10.10.10.237", 4321));
            //using var matrix = new LedMatrixServer(15, 15, new Udp("10.10.10.237", 1234));
            //var matrix = new LedMatrixServer(15, 15, "COM7", 115200);

            //(new Thread(i => {
            //    while (true) {
            //        matrix.PrintIncoming();
            //    }
            //})).Start();


            matrix.SetColor(Color.Black);
            //while(true)
            //{
            //    matrix.SetPixel(5, 5, 10, 0, 0);
            //    matrix.Draw();
            //    matrix.SetPixel(5, 5, 0, 10, 0);
            //    matrix.Draw();
            //    matrix.SetPixel(5, 5, 0, 0, 10);
            //    matrix.Draw();
            //}

            matrix.Flush();
            matrix.RenderGif("banana.gif", 0, 10, false);
            //matrix.RenderGif("banana.gif", 0, 10, false);
            //while (true)
            //{
            //    matrix.RenderGif("banana.gif", 3, 10, false);
            //}
            matrix.SetPixel(5, 5, 255, 255, 255);
            matrix.Draw();

        }
    }
}
