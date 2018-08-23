using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Automation;
using System.Windows.Automation.Provider;


namespace UIAutomationTest01
{
    class Program
    {
        static void Main(string[] args)
        {
            Condition condition = new PropertyCondition(AutomationElement.ProcessIdProperty, Int32.Parse(args[0]));

            foreach (AutomationElement child in AutomationElement.RootElement.FindAll(TreeScope.Descendants, condition))
            {
                Console.WriteLine("<AutomationElement Name=\"{0}\" ControlType=\"{1}\" AutomationId=\"{2}\" />", child.Current.Name, child.Current.ControlType.LocalizedControlType, child.Current.AutomationId);
            }
        }
    }
}