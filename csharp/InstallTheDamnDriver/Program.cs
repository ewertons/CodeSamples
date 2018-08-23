using System;
using System.Windows.Automation;
using DeviceHelper.Mouse;
using System.Threading;

namespace InstallTheDamnDriver
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                while (true)
                {
                    Condition condition = new AndCondition(
                        new PropertyCondition(AutomationElement.ControlTypeProperty, ControlType.Pane),
                        new PropertyCondition(AutomationElement.NameProperty, "Install this driver software anyway"));

                    AutomationElement button = AutomationElement.RootElement.FindFirst(TreeScope.Descendants, condition);

                    if (button != null)
                    {
                        int x = (int)(button.Current.BoundingRectangle.X + button.Current.BoundingRectangle.Width / 2);
                        int y = (int)(button.Current.BoundingRectangle.Y + button.Current.BoundingRectangle.Height / 2);

                        Console.WriteLine("{0} Clicking at x={1},y={2}", DateTime.Now.ToString(), x, y);

                        Mouse.Click(x, y);

                        button = null;
                    }

                    Thread.Sleep(10000);
                }
            }
            catch { }
        }
    }
}
