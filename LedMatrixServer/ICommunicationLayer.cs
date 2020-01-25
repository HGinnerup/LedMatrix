using System;
using System.Collections.Generic;
using System.Text;

namespace LedMatrixServer
{
    interface ICommunicationLayer : IDisposable
    {
        public void Queue(byte b);
        public void Queue(IEnumerable<byte> b);
        public void Transmit();
        public void PrintIncoming();
    }
}
