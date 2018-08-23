using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;

namespace WpfTest01
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public Data.CustomData DataStore = new Data.CustomData();

        public ObservableCollection<string> Data1
        {
            get { return DataStore.Contacts; }
        }

        public ObservableCollection<KeyValuePair<int, string>> Data2
        {
            get { return DataStore.Contacts2; }
        }

        public MainWindow()
        {
            InitializeComponent();
        }
    }
}
