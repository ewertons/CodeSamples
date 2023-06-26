namespace GUI
{
    partial class KeyVaultSettingsForm
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
            label1 = new Label();
            label2 = new Label();
            label3 = new Label();
            keyVaultUrlTextBox = new TextBox();
            tenantIdTextBox = new TextBox();
            clientIdTextBox = new TextBox();
            secretTextBox = new TextBox();
            label4 = new Label();
            button1 = new Button();
            button2 = new Button();
            SuspendLayout();
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(30, 29);
            label1.Name = "label1";
            label1.Size = new Size(150, 32);
            label1.TabIndex = 0;
            label1.Text = "Key Vault Url";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(30, 135);
            label2.Name = "label2";
            label2.Size = new Size(116, 32);
            label2.TabIndex = 1;
            label2.Text = "Tenant ID";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(30, 244);
            label3.Name = "label3";
            label3.Size = new Size(106, 32);
            label3.TabIndex = 2;
            label3.Text = "Client ID";
            // 
            // keyVaultUrlTextBox
            // 
            keyVaultUrlTextBox.Location = new Point(30, 64);
            keyVaultUrlTextBox.Name = "keyVaultUrlTextBox";
            keyVaultUrlTextBox.Size = new Size(1095, 39);
            keyVaultUrlTextBox.TabIndex = 3;
            // 
            // tenantIdTextBox
            // 
            tenantIdTextBox.Location = new Point(30, 170);
            tenantIdTextBox.Name = "tenantIdTextBox";
            tenantIdTextBox.Size = new Size(1095, 39);
            tenantIdTextBox.TabIndex = 4;
            // 
            // clientIdTextBox
            // 
            clientIdTextBox.Location = new Point(30, 279);
            clientIdTextBox.Name = "clientIdTextBox";
            clientIdTextBox.Size = new Size(1095, 39);
            clientIdTextBox.TabIndex = 5;
            // 
            // secretTextBox
            // 
            secretTextBox.Location = new Point(30, 388);
            secretTextBox.Name = "secretTextBox";
            secretTextBox.Size = new Size(1095, 39);
            secretTextBox.TabIndex = 6;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(30, 353);
            label4.Name = "label4";
            label4.Size = new Size(80, 32);
            label4.TabIndex = 7;
            label4.Text = "Secret";
            // 
            // button1
            // 
            button1.DialogResult = DialogResult.OK;
            button1.Location = new Point(352, 490);
            button1.Name = "button1";
            button1.Size = new Size(150, 46);
            button1.TabIndex = 8;
            button1.Text = "OK";
            button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            button2.DialogResult = DialogResult.Cancel;
            button2.Location = new Point(673, 490);
            button2.Name = "button2";
            button2.Size = new Size(150, 46);
            button2.TabIndex = 9;
            button2.Text = "Cancel";
            button2.UseVisualStyleBackColor = true;
            // 
            // KeyVaultSettingsForm
            // 
            AcceptButton = button1;
            AutoScaleDimensions = new SizeF(13F, 32F);
            AutoScaleMode = AutoScaleMode.Font;
            CancelButton = button2;
            ClientSize = new Size(1151, 581);
            Controls.Add(button2);
            Controls.Add(button1);
            Controls.Add(label4);
            Controls.Add(secretTextBox);
            Controls.Add(clientIdTextBox);
            Controls.Add(tenantIdTextBox);
            Controls.Add(keyVaultUrlTextBox);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(label1);
            FormBorderStyle = FormBorderStyle.FixedDialog;
            MaximizeBox = false;
            MinimizeBox = false;
            Name = "KeyVaultSettingsForm";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Key Vault Settings";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Label label1;
        private Label label2;
        private Label label3;
        private TextBox keyVaultUrlTextBox;
        private TextBox tenantIdTextBox;
        private TextBox clientIdTextBox;
        private TextBox secretTextBox;
        private Label label4;
        private Button button1;
        private Button button2;
    }
}