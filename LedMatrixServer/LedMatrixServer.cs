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
            Invalid     = 0xFF,
            Render      = 0xF0,
            BitMap      = 0xF1,
            ClearBuffer = 0xF2
        };

        public Serial Serial = new Serial();
        
        public readonly int Width  = 15;
        public readonly int Height = 15;

        private void SendByte(byte b) {
            byte[] buffer = { b };
            Serial.Queue(b);
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

            Serial.Queue(buffer);
        }


        public void SetColor(Color color) {
            for (int y = 0; y < Height; y++) {
                for (int x = 0; x < Width; x++) {
                    SetPixel(x, y, color);
                }
            }
            Draw();
        }

        public LedMatrixServer() {

        }
        public void RenderBitmap(Bitmap bmp) {
            RenderBitmap(bmp, null);
        }
        public void RenderBitmap(Bitmap bmp, Bitmap differential) {
            for (int x = 0; x < bmp.Width; x++) {
                for (int y = 0; y < bmp.Height; y++) {
                    var color = bmp.GetPixel(x, y);
                    if (color == differential?.GetPixel(x, y)) continue;

                    SetPixel(x, y, Color.FromArgb(color.R, color.G, color.B));
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
            foreach (var pixel in pixels)
            {
                SetPixel(pixel.Point, pixel.Color);
            }
        }
        public void SetPixels(IEnumerable<Pixel> pixels, int divisor) {
            foreach (var pixel in pixels) {
                SetPixel(pixel.Point, Color.FromArgb(pixel.Color.R / divisor, pixel.Color.G / divisor, pixel.Color.B / divisor));
            }
        }


        /// <summary>
        /// Resize the image to the specified width and height.
        /// </summary>
        /// <param name="image">The image to resize.</param>
        /// <returns>The resized image.</returns>
        private Bitmap FitImage(Image image) {
            var destImage = new Bitmap(this.Width, this.Height);

            destImage.SetResolution(image.HorizontalResolution, image.VerticalResolution);

            // Figure out the ratio
            double ratioX = (double)this.Width / (double)image.Width;
            double ratioY = (double)this.Height / (double)image.Height;
            // use whichever multiplier is smaller
            double ratio = ratioX < ratioY ? ratioX : ratioY;

            // now we can get the new height and width
            int newWidth = Convert.ToInt32(image.Width * ratio);
            int newHeight = Convert.ToInt32(image.Height * ratio);

            using (var graphics = Graphics.FromImage(destImage)) {
                graphics.CompositingMode = CompositingMode.SourceCopy;
                graphics.CompositingQuality = CompositingQuality.HighQuality;
                graphics.InterpolationMode = InterpolationMode.HighQualityBicubic;
                graphics.SmoothingMode = SmoothingMode.HighQuality;
                graphics.PixelOffsetMode = PixelOffsetMode.HighQuality;

                using (var wrapMode = new ImageAttributes()) {
                    wrapMode.SetWrapMode(WrapMode.TileFlipXY);
                    graphics.DrawImage(image, new Rectangle(0, 0, newWidth, newHeight), 0, 0, image.Width, image.Height, GraphicsUnit.Pixel, wrapMode);
                }
            }

            return destImage;
        }
        
        public void DisplayBmp(Bitmap bmp) {
            for (int x = 0; x < bmp.Width; x++) {
                for (int y = 0; y < bmp.Height; y++) {
                    var color = bmp.GetPixel(x, y);
                    SetPixel(x, y, color.R / 5, color.G / 5, color.B / 5);
                }
            }
        }

        public void DisplayBmpScaled(Bitmap bmp) {
            DisplayBmp(FitImage(new Bitmap(bmp)));
        }

        public void DisplayComputerMonitor()
        {

            var captureBmp = new Bitmap(1920, 1080, PixelFormat.Format32bppArgb);
            using (var captureGraphic = Graphics.FromImage(captureBmp)) {
                //captureGraphic.CopyFromScreen(0, 0, 0, 0, captureBmp.Size);
                captureGraphic.CopyFromScreen(1920, 0, 0, 0, captureBmp.Size);
                DisplayBmpScaled(captureBmp);
                //captureBmp.Save("capture.jpg", ImageFormat.Jpeg);
            }
        }

        public List<Frame> CreateFrames(Image gif) {
            var frames = new List<Frame>();

            FrameDimension frameDimension = new FrameDimension(gif.FrameDimensionsList[0]);


            for (int f = 0; f < gif.GetFrameCount(frameDimension); f++) {
                Frame frame = new Frame();

                gif.SelectActiveFrame(frameDimension, f);
                Bitmap bmp = FitImage(new Bitmap(gif));


                // https://stackoverflow.com/questions/3785031/getting-the-frame-duration-of-an-animated-gif
                //var gifDelay = gif.GetPropertyItem(0x5100);
                frame.DelayMS = 0;
                foreach(var item in gif.PropertyItems) {
                    if(item.Id == 0x5100) {
                        frame.DelayMS = (item.Value[0] + item.Value[1] * 256) * 10;
                    }
                }

                for (int x = 0; x < bmp.Width; x++) {
                    for (int y = 0; y < bmp.Height; y++) {
                        var color = bmp.GetPixel(x, y);

                        frame.AllPixels.Add(new Pixel() {
                            Point = new Point(x, y),
                            Color = Color.FromArgb(color.R, color.G, color.B)
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


        public void RenderGif(string filename, int iterations = 0, int colorDivisor = 1, bool keepDelay = true) {
            Image img = Image.FromFile(filename);
            var frames = CreateFrames(img);

            Stopwatch stopwatch = Stopwatch.StartNew();
            SetPixels(frames[0].AllPixels, colorDivisor);
            Draw();
            if(frames.Count > 1) {
                int i = 1;
                while (true) {
                    while (i < frames.Count)
                    {
                        stopwatch.Restart();
                        SetPixels(frames[i].ChangedPixels, colorDivisor);
                        if(keepDelay) Thread.Sleep(Math.Max(frames[i].DelayMS - (int)stopwatch.ElapsedMilliseconds, 0));
                        Draw();
                        i++;
                    }
                    i = 0;
                    if (iterations == 1) break;
                    if (iterations != 0) iterations--;
                }
            }
        }


        public void Dispose() {
            Serial.Dispose();
        }
    }
}
