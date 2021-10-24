
using System.Drawing;
using System.Windows.Forms;

namespace DragAndDrop
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.SuspendLayout();

            Panel panel1 = new Panel();
            panel1.BackColor = Color.Red;
            panel1.Width = 200;
            panel1.Height = 200;
            panel1.Location = new Point(10, 10);
            this.Controls.Add(panel1);

            Panel panel2 = new Panel();
            panel2.BackColor = Color.Blue;
            panel2.Width = 200;
            panel2.Height = 200;
            panel2.Location = new Point(250, 10);
            this.Controls.Add(panel2);

            Panel panel3 = new Panel();
            panel3.BackColor = Color.Green;
            panel3.Width = 200;
            panel3.Height = 200;
            panel3.Location = new Point(500, 10);
            this.Controls.Add(panel3);

            Panel panel4 = new Panel();
            panel4.BackColor = Color.Yellow;
            panel4.Width = 200;
            panel4.Height = 200;
            panel4.Location = new Point(750, 10);
            
            this.Controls.Add(panel4);

            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(13F, 32F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1806, 948);
            this.Name = "Form1";
            this.Text = "Form1";

            // Needed for drag&drop.
            this.AllowDrop = true;
            foreach (Control c in this.Controls)
            {
                c.MouseDown += new MouseEventHandler(c_MouseDown);
            }
            this.DragOver += Form1_DragOver;
            this.DragDrop += Form1_DragDrop;

            this.ResumeLayout(false);

        }

        private void c_MouseDown(object sender, MouseEventArgs e)
        {
            Control c = sender as Control;
            c.DoDragDrop(c, DragDropEffects.Move);
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            Control c = e.Data.GetData(e.Data.GetFormats()[0]) as Control;
            if (c != null)
            {
                c.Location = ((Control)this).PointToClient(new Point(e.X, e.Y));
                ((Control)this).Controls.Add(c);
            }
        }

        private void Form1_DragOver(object sender, DragEventArgs e)
        {
            e.Effect = DragDropEffects.All;
        }

        #endregion
    }
}

