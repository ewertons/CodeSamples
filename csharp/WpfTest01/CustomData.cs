using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;

namespace WpfTest01.Data
{
    public class CustomData
    {
        public static string GeneralContent = "This is a content from a separate data store.";
        
        public ObservableCollection<string> Contacts = new ObservableCollection<string>();
        public ObservableCollection<KeyValuePair<int, string>> Contacts2 = new ObservableCollection<KeyValuePair<int, string>>();

        public CustomData()
        {
            for (int i = 0; i < 20; i++)
            {
                Contacts.Add(i.ToString().PadLeft(5, '0'));

                Contacts2.Add(new KeyValuePair<int, string>(i, i.ToString().PadLeft(5, '0')));
            }
        }

    }

    public static class StaticData
    {
        public static string GeneralContent = "This is a content from a separate data store.";    
    }
}
