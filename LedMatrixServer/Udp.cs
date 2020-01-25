using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace LedMatrixServer
{
    class Udp : ICommunicationLayer
    {
        public int MaxPacketSize { get; set; } = 80;
        private UdpClient udpClient { get; set; }
        private IPEndPoint RemoteIpEndPoint { get; set; }
        public Udp(string hostname, int port)
        {
            udpClient = new UdpClient(hostname, port);
        }
        public void Dispose()
        {
            udpClient.Close();
        }

        public void PrintIncoming()
        {
            //Console.WriteLine($"{PortStream.ReadByte().ToString("X2")} ");
        }

        private List<byte> Buffer { get; set; } = new List<byte>();

        public void Queue(byte b)
        {
            Buffer.Add(b);
            if (Buffer.Count >= MaxPacketSize) Transmit();
        }

        public void Queue(IEnumerable<byte> b)
        {
            Buffer.AddRange(b);
            if (Buffer.Count >= MaxPacketSize) Transmit();
        }

        public void Transmit()
        {
            udpClient.Send(Buffer.ToArray(), Buffer.Count);
            Buffer = new List<byte>();
            Thread.Sleep(10);
        }
    }
}
