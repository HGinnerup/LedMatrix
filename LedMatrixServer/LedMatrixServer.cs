using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Text;
using System.Threading;
using RJCP.IO.Ports;

namespace LedMatrixServer {
    partial class LedMatrixServer : IDisposable {
        enum Actions {
            Invalid = 0xFF,
            Render = 0xF0,
            BitMap = 0xF1,
            ClearBuffer = 0xF2
        };

        //public SerialPort Serial;
        public SerialPortStream Serial;
        public readonly int Width = 15;
        public readonly int Height = 15;


        private void SendByte(byte b) {
            byte[] buffer = { b };
            while (!Serial.CanWrite) ;
            Serial.Write(buffer, 0, 1);
        }

        private DateTime LastDraw = DateTime.Now;
        public void Draw() {
            SendByte((byte)Actions.Render);
            Console.WriteLine($"\n{1000 / (DateTime.Now - LastDraw).TotalMilliseconds} draws per second");
            LastDraw = DateTime.Now;
            Thread.Sleep(10);
        }
        public void ClearBuffer() {
            for (int i = 0; i < 10; i++)
                SendByte((byte)Actions.ClearBuffer);
        }

        public void SetPixel(Point point, Color color) {
            SetPixel(point.X, point.Y, color.R, color.G, color.B); ;
        }
        public void SetPixel(int x, int y, Color color) {
            SetPixel(x, y, color.R, color.G, color.B); ;
        }
        public void SetPixel(int x, int y, int r, int g, int b) {
            SetPixel((byte)(((x & 0xF) << 4) + (((Height-1)-y) & 0xF)), (byte)r, (byte)g, (byte)b);
        }
        private void SetPixel(byte position, byte r, byte g, byte b) {
            byte[] buffer = { position, r, g, b };

            while (!Serial.CanWrite) ;
            Serial.Write(buffer, 0, 4);
            Thread.Sleep(1);
        }

        public void SetColor(Color color) {
            for (int y = 1; y < 14; y++) {
                for (int x = 1; x < 14; x++) {
                    SetPixel(x, y, color);
                }
            }
            Draw();
        }

        public void PrintSerialInput() {
            if (Serial.BytesToRead > 0) {
                while (Serial.BytesToRead > 0) {
                    Console.Write($"{Serial.ReadByte().ToString("X2")} ");
                }
                Console.WriteLine();
            }
        }

        public LedMatrixServer() {
            Serial = new SerialPortStream("COM3", 115200);
            Serial.DataBits = 8;

            Serial.Parity = Parity.Even;
            Serial.StopBits = StopBits.One;
            Serial.Handshake = Handshake.DtrRts;

            Serial.DtrEnable = false;
            Serial.RtsEnable = false;

            Serial.Open();
            Thread.Sleep(500);
        }

        public void RenderBitmap(Bitmap bmp) {
            RenderBitmap(bmp, null);
        }
        public void RenderBitmap(Bitmap bmp, Bitmap differential) {
            for (int x = 0; x < bmp.Width; x++) {
                for (int y = 0; y < bmp.Height; y++) {
                    var color = bmp.GetPixel(x, y);
                    if (color == differential?.GetPixel(x, y)) continue;

                    SetPixel(x, y, Color.FromArgb(color.R / 10, color.G / 10, color.B / 10));
                }
            }
        }




        public class Pixel {
            public Color Color { get; set; }
            public Point Point { get; set; }
        }
        public class Frame {
            public List<Pixel> AllPixels { get; set; } = new List<Pixel>();
            public List<Pixel> ChangedPixels { get; set; } = new List<Pixel>();
            public int DelayMS { get; set; } = 0;
        }
        public void SetPixels(IEnumerable<Pixel> pixels) {
            foreach(var pixel in pixels) {
                SetPixel(pixel.Point, pixel.Color);
            }
        }



        //https://stackoverflow.com/questions/1922040/how-to-resize-an-image-c-sharp
        /// <summary>
        /// Resize the image to the specified width and height.
        /// </summary>
        /// <param name="image">The image to resize.</param>
        /// <param name="width">The width to resize to.</param>
        /// <param name="height">The height to resize to.</param>
        /// <returns>The resized image.</returns>
        public static Bitmap ResizeImage(Image image, int width, int height) {
            var destRect = new Rectangle(0, 0, width, height);
            var destImage = new Bitmap(width, height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            using (var graphics = Graphics.FromImage(destImage)) {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                using (var wrapMode = new ImageAttributes()) {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, destRect, 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }

        public List<Frame> CreateFrames(Image gif) {
            var frames = new List<Frame>();

            FrameDimension frameDimension = new FrameDimension(gif.FrameDimensionsList[0]);


            for (int f = 0; f < gif.GetFrameCount(frameDimension); f++) {
                Frame frame = new Frame();

                gif.SelectActiveFrame(frameDimension, f);
                Bitmap bmp = ResizeImage(new Bitmap(gif), Width, Height);


                // https://stackoverflow.com/questions/3785031/getting-the-frame-duration-of-an-animated-gif
                var gifDelay = gif.GetPropertyItem(0x5100);
                frame.DelayMS = (gifDelay.Value[0] + gifDelay.Value[1] * 256) * 10;

                for (int x = 0; x < bmp.Width; x++) {
                    for (int y = 0; y < bmp.Height; y++) {
                        var color = bmp.GetPixel(x, y);

                        frame.AllPixels.Add(new Pixel() {
                            Point = new Point(x, y),
                            Color = Color.FromArgb(color.R / 10, color.G / 10, color.B / 10)
                        });
                    }
                }
                frames.Add(frame);
            }

            for (var i = 0; i < frames.Count; i++) {
                var frame0 = frames[i];
                var frame1 = frames[(i + 1) % frames.Count];

                for(var j=0; j<frame0.AllPixels.Count; j++) {
                    if(frame0.AllPixels[j].Color != frame1.AllPixels[j].Color) {
                        frame1.ChangedPixels.Add(frame1.AllPixels[j]);
                    }
                }
            }

            return frames;
        }


        public void RenderGif(string filename) {
            Image img = Image.FromFile(filename);
            //Image img = Image.FromFile("flag.gif");



            var frames = CreateFrames(img);

            Stopwatch stopwatch = Stopwatch.StartNew();
            SetPixels(frames[0].AllPixels);
            Draw();
            int i=1;
            while(true) {

                while (i<frames.Count) {
                    stopwatch.Restart();
                    SetPixels(frames[i].ChangedPixels);
                    Thread.Sleep(Math.Max(frames[i].DelayMS - (int)stopwatch.ElapsedMilliseconds, 0));
                    Draw();
                    i++;
                }
                i = 0;
            }
        }


        public void Dispose() {
            Serial.Dispose();
        }
    }
}
