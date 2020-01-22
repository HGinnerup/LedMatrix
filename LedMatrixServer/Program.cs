using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;

namespace LedMatrixServer {
    class Program {

        static void Main(string[] args) {

            var matrix = new LedMatrixServer();

            (new Thread(i => {
                while (true) {
                    while (matrix.Serial.BytesToRead == 0) Thread.Sleep(100);
                    matrix.PrintSerialInput();
                }
            })).Start();


            matrix.SetColor(Color.Black);

            matrix.RenderGif("banana.gif");

        }
    }
}
