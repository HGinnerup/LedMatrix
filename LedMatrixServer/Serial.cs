//using RJCP.IO.Ports;
using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using System.Threading;

namespace LedMatrixServer
{
    public class Serial : IDisposable {

        public int MaxPacketSize { get; set; } = 1;


        private List<byte> Buffer { get; set; } = new List<byte>();

        private SerialPort PortStream { get; set; }

        public void Queue(byte b) {
            var array = new byte[] { b };
            PortStream.Write(array, 0, array.Length);
            //Thread.Sleep(10);
        }
        public void Queue(IEnumerable<byte> b) {
            Buffer.AddRange(b);
            if (Buffer.Count >= MaxPacketSize) Transmit();
        }

        public void Transmit() {
            PortStream.Write(Buffer.ToArray(), 0, Buffer.Count);
            Buffer = new List<byte>();
            Thread.Sleep(100);
        }

        public Serial(string comPort, int baudRate) {
            PortStream = new SerialPort(comPort, baudRate);

            PortStream.DataBits  = 8;
            PortStream.Parity    = Parity.Even;
            PortStream.StopBits  = StopBits.One;

            PortStream.Handshake = Handshake.RequestToSend;
            //PortStream.Handshake = Handshake.RequestToSend;
            //PortStream.Handshake = Handshake.XOnXOff;

            PortStream.DtrEnable = true;
            PortStream.RtsEnable = true;

            if (PortStream.IsOpen) {
                PortStream.Close();
                Thread.Sleep(1000);
            }

            PortStream.Open();
            Thread.Sleep(1000);
        }

        public void PrintSerialInput()
        {
            if (PortStream.BytesToRead > 0)
            {
                while (PortStream.BytesToRead > 0)
                {
                    Console.Write($"{PortStream.ReadByte().ToString("X2")} ");
                }
                Console.WriteLine();
            }
        }

        public void Dispose() {
            PortStream.Dispose();
        }
    }
}
