namespace SergeM{
    partial class FormSettings
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
            textBoxUDPPortLocal = new TextBox();
            textBoxUDPPortRemote = new TextBox();
            textBoxIPAddressRemote = new TextBox();
            label1 = new Label();
            label2 = new Label();
            label3 = new Label();
            buttonCancel = new Button();
            buttonOK = new Button();
            checkBoxLog = new CheckBox();
            checkBoxIPAddressRemoteLocalhost = new CheckBox();
            label5 = new Label();
            textBoxAutoGetValuePeriod = new TextBox();
            label4 = new Label();
            SuspendLayout();
            // 
            // textBoxUDPPortLocal
            // 
            textBoxUDPPortLocal.Location = new Point(150, 12);
            textBoxUDPPortLocal.MaxLength = 5;
            textBoxUDPPortLocal.Name = "textBoxUDPPortLocal";
            textBoxUDPPortLocal.Size = new Size(100, 23);
            textBoxUDPPortLocal.TabIndex = 2;
            textBoxUDPPortLocal.TextAlign = HorizontalAlignment.Center;
            textBoxUDPPortLocal.KeyDown += FormSettings_KeyDown;
            textBoxUDPPortLocal.KeyPress += textBox_Digit_KeyPress;
            // 
            // textBoxUDPPortRemote
            // 
            textBoxUDPPortRemote.Location = new Point(150, 41);
            textBoxUDPPortRemote.MaxLength = 5;
            textBoxUDPPortRemote.Name = "textBoxUDPPortRemote";
            textBoxUDPPortRemote.Size = new Size(100, 23);
            textBoxUDPPortRemote.TabIndex = 4;
            textBoxUDPPortRemote.TextAlign = HorizontalAlignment.Center;
            textBoxUDPPortRemote.KeyDown += FormSettings_KeyDown;
            textBoxUDPPortRemote.KeyPress += textBox_Digit_KeyPress;
            // 
            // textBoxIPAddressRemote
            // 
            textBoxIPAddressRemote.Location = new Point(150, 70);
            textBoxIPAddressRemote.MaxLength = 15;
            textBoxIPAddressRemote.Name = "textBoxIPAddressRemote";
            textBoxIPAddressRemote.Size = new Size(100, 23);
            textBoxIPAddressRemote.TabIndex = 6;
            textBoxIPAddressRemote.TextAlign = HorizontalAlignment.Center;
            textBoxIPAddressRemote.KeyDown += FormSettings_KeyDown;
            textBoxIPAddressRemote.KeyPress += textBoxIPAddressRemote_KeyPress;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(12, 15);
            label1.Name = "label1";
            label1.Size = new Size(89, 15);
            label1.TabIndex = 1;
            label1.Text = "UDP Port Local:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(12, 44);
            label2.Name = "label2";
            label2.Size = new Size(102, 15);
            label2.TabIndex = 3;
            label2.Text = "UDP Port Remote:";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(12, 73);
            label3.Name = "label3";
            label3.Size = new Size(109, 15);
            label3.TabIndex = 5;
            label3.Text = "IP Address Remote:";
            // 
            // buttonCancel
            // 
            buttonCancel.Location = new Point(247, 177);
            buttonCancel.Name = "buttonCancel";
            buttonCancel.Size = new Size(75, 23);
            buttonCancel.TabIndex = 92;
            buttonCancel.Text = "&Cancel";
            buttonCancel.UseVisualStyleBackColor = true;
            buttonCancel.Click += buttonCancel_Click;
            buttonCancel.KeyDown += FormSettings_KeyDown;
            // 
            // buttonOK
            // 
            buttonOK.Location = new Point(166, 177);
            buttonOK.Name = "buttonOK";
            buttonOK.Size = new Size(75, 23);
            buttonOK.TabIndex = 91;
            buttonOK.Text = "&OK";
            buttonOK.UseVisualStyleBackColor = true;
            buttonOK.Click += buttonOK_Click;
            buttonOK.KeyDown += FormSettings_KeyDown;
            // 
            // checkBoxLog
            // 
            checkBoxLog.AutoSize = true;
            checkBoxLog.Location = new Point(12, 180);
            checkBoxLog.Name = "checkBoxLog";
            checkBoxLog.Size = new Size(46, 19);
            checkBoxLog.TabIndex = 90;
            checkBoxLog.Text = "&Log";
            checkBoxLog.UseVisualStyleBackColor = true;
            checkBoxLog.KeyDown += FormSettings_KeyDown;
            // 
            // checkBoxIPAddressRemoteLocalhost
            // 
            checkBoxIPAddressRemoteLocalhost.AutoSize = true;
            checkBoxIPAddressRemoteLocalhost.Location = new Point(254, 72);
            checkBoxIPAddressRemoteLocalhost.Name = "checkBoxIPAddressRemoteLocalhost";
            checkBoxIPAddressRemoteLocalhost.Size = new Size(74, 19);
            checkBoxIPAddressRemoteLocalhost.TabIndex = 7;
            checkBoxIPAddressRemoteLocalhost.Text = "localhost";
            checkBoxIPAddressRemoteLocalhost.UseVisualStyleBackColor = true;
            checkBoxIPAddressRemoteLocalhost.CheckedChanged += checkBoxAPAddressRemoteLocalhost_CheckedChanged;
            checkBoxIPAddressRemoteLocalhost.KeyDown += FormSettings_KeyDown;
            // 
            // label5
            // 
            label5.AutoSize = true;
            label5.Location = new Point(12, 131);
            label5.Name = "label5";
            label5.Size = new Size(124, 15);
            label5.TabIndex = 10;
            label5.Text = "Auto get value period:";
            // 
            // textBoxAutoGetValuePeriod
            // 
            textBoxAutoGetValuePeriod.Location = new Point(148, 128);
            textBoxAutoGetValuePeriod.MaxLength = 3;
            textBoxAutoGetValuePeriod.Name = "textBoxAutoGetValuePeriod";
            textBoxAutoGetValuePeriod.Size = new Size(100, 23);
            textBoxAutoGetValuePeriod.TabIndex = 11;
            textBoxAutoGetValuePeriod.TextAlign = HorizontalAlignment.Center;
            textBoxAutoGetValuePeriod.KeyPress += textBox_Digit_KeyPress;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(254, 131);
            label4.Name = "label4";
            label4.Size = new Size(12, 15);
            label4.TabIndex = 12;
            label4.Text = "s";
            // 
            // FormSettings
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(334, 211);
            Controls.Add(label4);
            Controls.Add(label5);
            Controls.Add(textBoxAutoGetValuePeriod);
            Controls.Add(checkBoxIPAddressRemoteLocalhost);
            Controls.Add(checkBoxLog);
            Controls.Add(buttonOK);
            Controls.Add(buttonCancel);
            Controls.Add(label3);
            Controls.Add(label2);
            Controls.Add(label1);
            Controls.Add(textBoxIPAddressRemote);
            Controls.Add(textBoxUDPPortRemote);
            Controls.Add(textBoxUDPPortLocal);
            FormBorderStyle = FormBorderStyle.FixedToolWindow;
            MinimumSize = new Size(350, 250);
            Name = "FormSettings";
            StartPosition = FormStartPosition.CenterParent;
            Text = "Settings";
            KeyDown += FormSettings_KeyDown;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox textBoxUDPPortLocal;
        private TextBox textBoxUDPPortRemote;
        private TextBox textBoxIPAddressRemote;
        private Label label1;
        private Label label2;
        private Label label3;
        private Button buttonCancel;
        private Button buttonOK;
        private CheckBox checkBoxLog;
        private CheckBox checkBoxIPAddressRemoteLocalhost;
        private Label label5;
        private TextBox textBoxAutoGetValuePeriod;
        private Label label4;
    }
}