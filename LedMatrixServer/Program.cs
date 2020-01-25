using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

namespace LedMatrixServer {
    class Program {

        static void Main(string[] args) {

            
            
            using var matrix = new LedMatrixServer(15, 15, new Serial("COM7", 1000000));
            //var matrix = new LedMatrixServer(15, 15, "COM7", 115200);

            (new Thread(i => {
                while (true) {
                    matrix.PrintIncoming();
                }
            })).Start();


            matrix.SetColor(Color.Black);

            matrix.RenderGif("banana.gif");

        }
    }
}
