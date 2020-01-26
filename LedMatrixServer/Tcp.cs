using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace LedMatrixServer
{
    class Tcp : ICommunicationLayer
    {
        public int MaxPacketSize { get; set; } = 800;
        private TcpClient Client { get; set; }
        private NetworkStream NetworkStream { get; set; }
        private IPEndPoint RemoteIpEndPoint { get; set; }

        public Tcp(string hostname, int port)
        {
            Client = new TcpClient(hostname, port);
            NetworkStream = Client.GetStream();
        }
        public void Dispose()
        {
            Client.Close();
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

        public void Queue(IEnumerable<byte> b) {
            Buffer.AddRange(b);
            if (Buffer.Count >= MaxPacketSize) Transmit();
        }

        public void Transmit() {
            NetworkStream.Write(Buffer.ToArray(), 0, Buffer.Count);
            Buffer = new List<byte>();
            Thread.Sleep(10);
        }
    }
}
