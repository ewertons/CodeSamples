namespace GUI
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
            textBox1 = new TextBox();
            dataGridView1 = new DataGridView();
            menuStrip1 = new MenuStrip();
            connectionToolStripMenuItem = new ToolStripMenuItem();
            keyVaultSettingsToolStripMenuItem = new ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)dataGridView1).BeginInit();
            menuStrip1.SuspendLayout();
            SuspendLayout();
            // 
            // textBox1
            // 
            textBox1.Location = new Point(12, 43);
            textBox1.Name = "textBox1";
            textBox1.Size = new Size(2049, 39);
            textBox1.TabIndex = 0;
            textBox1.TextChanged += TextBox1_TextChanged;
            // 
            // dataGridView1
            // 
            dataGridView1.AccessibleRole = AccessibleRole.None;
            dataGridView1.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            dataGridView1.Location = new Point(12, 88);
            dataGridView1.Name = "dataGridView1";
            dataGridView1.RowHeadersWidth = 82;
            dataGridView1.Size = new Size(2049, 949);
            dataGridView1.TabIndex = 1;
            dataGridView1.MouseClick += DataGridView1_MouseClick;
            // 
            // menuStrip1
            // 
            menuStrip1.ImageScalingSize = new Size(32, 32);
            menuStrip1.Items.AddRange(new ToolStripItem[] { connectionToolStripMenuItem });
            menuStrip1.Location = new Point(0, 0);
            menuStrip1.Name = "menuStrip1";
            menuStrip1.Size = new Size(2073, 40);
            menuStrip1.TabIndex = 2;
            menuStrip1.Text = "menuStrip1";
            // 
            // connectionToolStripMenuItem
            // 
            connectionToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { keyVaultSettingsToolStripMenuItem });
            connectionToolStripMenuItem.Name = "connectionToolStripMenuItem";
            connectionToolStripMenuItem.Size = new Size(157, 36);
            connectionToolStripMenuItem.Text = "&Connection";
            // 
            // keyVaultSettingsToolStripMenuItem
            // 
            keyVaultSettingsToolStripMenuItem.Name = "keyVaultSettingsToolStripMenuItem";
            keyVaultSettingsToolStripMenuItem.Size = new Size(354, 44);
            keyVaultSettingsToolStripMenuItem.Text = "&Key Vault Settings...";
            keyVaultSettingsToolStripMenuItem.Click += keyVaultSettingsToolStripMenuItem_Click;
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(13F, 32F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(2073, 1049);
            Controls.Add(dataGridView1);
            Controls.Add(textBox1);
            Controls.Add(menuStrip1);
            MainMenuStrip = menuStrip1;
            Name = "Form1";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "Azure Key Vault Manager";
            ((System.ComponentModel.ISupportInitialize)dataGridView1).EndInit();
            menuStrip1.ResumeLayout(false);
            menuStrip1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        private void DataGridView1_MouseClick(object sender, MouseEventArgs e)
        {
            int currentMouseOverRow = dataGridView1.HitTest(e.X, e.Y).RowIndex;
            var targetRow = dataGridView1.Rows[currentMouseOverRow];
            dataGridView1.CurrentCell = targetRow.Cells[0];

            var contextMenuItem = new ToolStripLabel();
            contextMenuItem.Text = "Get secret";
            contextMenuItem.AutoSize = true;
            contextMenuItem.Click += (cmiSender, cmiEvent) =>
            {
                var secretValue = secretClient.GetSecret(targetRow.Cells[0].Value.ToString());
                Clipboard.SetText(secretValue.Value.Value);
            };

            ContextMenuStrip contextMenu = new ContextMenuStrip();
            contextMenu.AutoSize = true;
            contextMenu.Items.Add(contextMenuItem);
            contextMenu.PerformLayout(); // This is not working.
            contextMenu.Show(dataGridView1, new Point(e.X, e.Y));
        }

        private void TextBox1_TextChanged(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in dataGridView1.Rows)
            {
                if (row.Cells.Count > 0 &&
                    row.Cells[0].Value != null &&
                    row.Cells[0].Value.ToString().StartsWith(textBox1.Text))
                {
                    dataGridView1.CurrentCell = row.Cells[0];
                    break;
                }
            }
        }

        #endregion

        private TextBox textBox1;
        private DataGridView dataGridView1;
        private MenuStrip menuStrip1;
        private ToolStripMenuItem connectionToolStripMenuItem;
        private ToolStripMenuItem keyVaultSettingsToolStripMenuItem;
    }
}