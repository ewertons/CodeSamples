using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.Speech.Recognition;

namespace SpeechRecognitionTest02
{
    class Program
    {
        static List<string> grammarList;

        static void Main(string[] args)
        {
            grammarList = new List<string>();
            grammarList.Add("one");
            grammarList.Add("two");
            grammarList.Add("three");
            grammarList.Add("end");

            Choices speechChoices = new Choices(grammarList.ToArray());
            GrammarBuilder grammarBuilder = new GrammarBuilder(speechChoices);
            Grammar grammar = new Grammar(grammarBuilder);

            SpeechRecognitionEngine recognitionEngine = new SpeechRecognitionEngine();
            recognitionEngine.SetInputToDefaultAudioDevice();
            recognitionEngine.LoadGrammar(grammar);
            RecognitionResult result = null;

            // Recognition using DictationGrammar may be very unaccurate. To get "end", spell E-N-D, if the whole word is not recognized.
            while (result == null || String.Compare(result.Text, "end", true) != 0)
            {
                result = recognitionEngine.Recognize();

                if(result == null)
                {
                    Console.WriteLine("Got null result. I probably don't know this word...");
                }
                else
                {
                    if (grammarList.Contains(result.Text))
                    {
                        Console.WriteLine("I know this word, \"{0}\".", result.Text);
                    }
                    else
                    {
                        Console.WriteLine(result.ToString());                    
                    }
                }
            }
        }
    }
}
