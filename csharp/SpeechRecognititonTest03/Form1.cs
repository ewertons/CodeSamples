using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Speech.Recognition;

namespace SpeechRecognititonTest03
{
    public partial class Form1 : Form
    {
        static Dictionary<string, Color> colorTable = new Dictionary<string, Color>();
        static SpeechRecognitionEngine speechRecognitionEngine;

        public Form1()
        {
            InitializeComponent();

            colorTable.Add("yellow", Color.Yellow);
            colorTable.Add("red", Color.Red);
            colorTable.Add("black", Color.Black);
            colorTable.Add("blue", Color.Blue);
            colorTable.Add("green", Color.Green);
            colorTable.Add("white", Color.White);
            colorTable.Add("orange", Color.Orange);
            colorTable.Add("magenta", Color.Magenta);
            colorTable.Add("purple", Color.Purple);

            List<string> grammarList = new List<string>();
            grammarList.AddRange(colorTable.Keys);
            grammarList.Add("close");
            grammarList.Add("left");
            grammarList.Add("right");
            grammarList.Add("center");
            grammarList.Add("top");
            grammarList.Add("bottom");

            Choices speechChoices = new Choices(grammarList.ToArray());
            GrammarBuilder grammarBuilder = new GrammarBuilder(speechChoices);
            Grammar grammar = new Grammar(grammarBuilder);

            speechRecognitionEngine = new SpeechRecognitionEngine();
            speechRecognitionEngine.SetInputToDefaultAudioDevice();
            speechRecognitionEngine.LoadGrammar(grammar);
            speechRecognitionEngine.SpeechDetected += new EventHandler<SpeechDetectedEventArgs>(speechRecognizer_SpeechDetected);
            speechRecognitionEngine.SpeechRecognized += new EventHandler<SpeechRecognizedEventArgs>(speechRecognizer_SpeechRecognized);
            speechRecognitionEngine.RecognizeAsync(RecognizeMode.Multiple);
        }

        void speechRecognizer_SpeechDetected(object sender, SpeechDetectedEventArgs e)
        {
            lbSpeechDetected.Text = DateTime.Now.ToString();
        }


        void speechRecognizer_SpeechRecognized(object sender, SpeechRecognizedEventArgs e)
        {
            Form form = Application.OpenForms[0];

            if (!this.TopMost)
            {
                this.TopMost = true;
                this.TopMost = false;
            }

            lbRecognizedText.Text = e.Result.Text;

            if (colorTable.ContainsKey(e.Result.Text))
            {
                form.BackColor = colorTable[e.Result.Text];
            }
            else
            {
                switch (e.Result.Text)
                { 
                    case "close":
                        form.Close();
                        break;
                    case "left":
                        form.Location = new Point(0, (Screen.PrimaryScreen.Bounds.Height - form.Height) >> 1);
                        break;
                    case "center":
                        form.Location = new Point((Screen.PrimaryScreen.Bounds.Width - form.Width) >> 1, (Screen.PrimaryScreen.Bounds.Height - form.Height) >> 1);
                        break;
                    case "right":
                        form.Location = new Point(Screen.PrimaryScreen.Bounds.Width - form.Width, (Screen.PrimaryScreen.Bounds.Height - form.Height) >> 1);
                        break;
                    case "top":
                        form.Location = new Point((Screen.PrimaryScreen.Bounds.Width - form.Width) >> 1, 0);
                        break;
                    case "bottom":
                        form.Location = new Point((Screen.PrimaryScreen.Bounds.Width - form.Width) >> 1, Screen.PrimaryScreen.Bounds.Height - form.Height);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}
