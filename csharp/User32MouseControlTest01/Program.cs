using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace User32MouseControlTest01
{
    class Program
    {
        [DllImport("user32.dll")]
        static extern bool SetCursorPos(int X, int Y);  

        [DllImport("user32.dll")]
        static extern void mouse_event(uint dwFlags, uint dx, uint dy, uint dwData,  int dwExtraInfo);

        [Flags]
        public enum MouseEventFlags
        {
            LEFTDOWN   = 0x00000002,
            LEFTUP     = 0x00000004,
            MIDDLEDOWN = 0x00000020,
            MIDDLEUP   = 0x00000040,
            MOVE       = 0x00000001,
            ABSOLUTE   = 0x00008000,
            RIGHTDOWN  = 0x00000008,
            RIGHTUP    = 0x00000010
        }   

        static void Main(string[] args)
        {
            //Set cursor position  
            SetCursorPos(10, 35);
            //Mouse Right Down and Mouse Right Up
            mouse_event((uint)MouseEventFlags.RIGHTDOWN, 0, 0, 0, 0);
            mouse_event((uint)MouseEventFlags.RIGHTUP, 0, 0, 0, 0);
        }
    }
}
