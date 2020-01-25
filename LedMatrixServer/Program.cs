using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

namespace LedMatrixServer {
    class Program {

        static void Main(string[] args) {

            var matrix = new LedMatrixServer(15, 15, "COM7", 1000000);
            //var matrix = new LedMatrixServer(15, 15, "COM7", 115200);

            (new Thread(i => {
                while (true) {
                    matrix.Serial.PrintSerialInput();
                }
            })).Start();


            matrix.SetColor(Color.Black);

            matrix.RenderGif("banana.gif");

        }
    }
}
