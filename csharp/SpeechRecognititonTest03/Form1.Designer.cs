namespace SpeechRecognititonTest03
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
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
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lbRecognizedText = new System.Windows.Forms.Label();
            this.lbSpeechDetected = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // lbRecognizedText
            // 
            this.lbRecognizedText.AutoSize = true;
            this.lbRecognizedText.Location = new System.Drawing.Point(13, 30);
            this.lbRecognizedText.Name = "lbRecognizedText";
            this.lbRecognizedText.Size = new System.Drawing.Size(35, 13);
            this.lbRecognizedText.TabIndex = 1;
            this.lbRecognizedText.Text = "label1";
            // 
            // lbSpeechDetected
            // 
            this.lbSpeechDetected.AutoSize = true;
            this.lbSpeechDetected.Location = new System.Drawing.Point(13, 14);
            this.lbSpeechDetected.Name = "lbSpeechDetected";
            this.lbSpeechDetected.Size = new System.Drawing.Size(35, 13);
            this.lbSpeechDetected.TabIndex = 2;
            this.lbSpeechDetected.Text = "label1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.lbSpeechDetected);
            this.Controls.Add(this.lbRecognizedText);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbRecognizedText;
        private System.Windows.Forms.Label lbSpeechDetected;
    }
}

