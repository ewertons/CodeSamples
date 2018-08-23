using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Speech.Recognition;

namespace SpeechRecognitionTest01
{
    class Program
    {
        static void Main(string[] args)
        {
            SpeechRecognitionEngine recognitionEngine = new SpeechRecognitionEngine();
            recognitionEngine.SetInputToDefaultAudioDevice();
            recognitionEngine.LoadGrammar(new DictationGrammar());
            RecognitionResult result = null;

            // Recognition using DictationGrammar may be very unaccurate. To get "end", spell E-N-D, if the whole word is not recognized.
            while (result == null || String.Compare(result.Text, "end", true) != 0)
            {
                result = recognitionEngine.Recognize();

                Console.WriteLine(result == null ? "<null>" : result.Text);
            }
        }
    }
}
