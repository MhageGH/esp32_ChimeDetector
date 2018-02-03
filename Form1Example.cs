using System;
using System.Windows.Forms;

namespace ChimeDetector {
    public partial class Form1 : Form {
        public Form1() {
            InitializeComponent();
            serialPort1.Open();
        }

        private delegate void AlertDelegate(String str);

        private void Alert(String str) {
            if (str.Equals("Chime detect\r")) {
                var player = new System.Media.SoundPlayer("nc2039.wav");
                player.Play();
                label1.Text = "!";
            }
        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e) {
            Invoke(new AlertDelegate(Alert), serialPort1.ReadLine());
        }
    }
}
