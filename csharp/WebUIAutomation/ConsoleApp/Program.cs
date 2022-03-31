using OpenQA.Selenium;
using OpenQA.Selenium.Firefox;
using System.Threading;

/*
 * 1. Create a .Net solution
 * 2. Install the nuget package ""
 * 3. Go to https://firefox-source-docs.mozilla.org/testing/geckodriver/Support.html
 * 4. Click on "geckodriver releases" (https://github.com/mozilla/geckodriver/releases)
 * 5. Download the geckodriver specific to OS and platform (e.g., "geckodriver-v0.30.0-win64.zip" for Windows 11)
 * 6. Extract the content of the zip file, save the path to the directory** (not the executable).
 * 7. Update the path below in webDriverPath.
 * 8. Run this application.
 */

namespace Selenium_Automation
{
    class Program
    {
        static void Main(string[] args)
        {
            const string webDriverPath = "C:\\Users\\ewertons\\Downloads\\geckodriver-v0.30.0-win64";

            IWebDriver driver = new FirefoxDriver(webDriverPath);

            driver.Navigate().GoToUrl("https://www.bing.com/");

            IWebElement searchBox = driver.FindElement(By.Id("sb_form_q"));
            searchBox.SendKeys("selenium webdriver");

            IWebElement searchButton = driver.FindElement(By.Id("search_icon"));
            searchButton.Click();

            Thread.Sleep(3000);

            // Close the firefox window.
            driver.Close();
        }
    }
}

