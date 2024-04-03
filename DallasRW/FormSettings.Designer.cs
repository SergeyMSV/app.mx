namespace DallasRW
{
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
            textBoxCOMPortNumber = new TextBox();
            label4 = new Label();
            SuspendLayout();
            // 
            // textBoxUDPPortLocal
            // 
            textBoxUDPPortLocal.Location = new Point(123, 12);
            textBoxUDPPortLocal.MaxLength = 5;
            textBoxUDPPortLocal.Name = "textBoxUDPPortLocal";
            textBoxUDPPortLocal.Size = new Size(100, 23);
            textBoxUDPPortLocal.TabIndex = 0;
            textBoxUDPPortLocal.TextAlign = HorizontalAlignment.Center;
            textBoxUDPPortLocal.KeyDown += FormSettings_KeyDown;
            textBoxUDPPortLocal.KeyPress += textBox_Digit_KeyPress;
            // 
            // textBoxUDPPortRemote
            // 
            textBoxUDPPortRemote.Location = new Point(123, 41);
            textBoxUDPPortRemote.MaxLength = 5;
            textBoxUDPPortRemote.Name = "textBoxUDPPortRemote";
            textBoxUDPPortRemote.Size = new Size(100, 23);
            textBoxUDPPortRemote.TabIndex = 1;
            textBoxUDPPortRemote.TextAlign = HorizontalAlignment.Center;
            textBoxUDPPortRemote.KeyDown += FormSettings_KeyDown;
            textBoxUDPPortRemote.KeyPress += textBox_Digit_KeyPress;
            // 
            // textBoxIPAddressRemote
            // 
            textBoxIPAddressRemote.Location = new Point(123, 70);
            textBoxIPAddressRemote.MaxLength = 15;
            textBoxIPAddressRemote.Name = "textBoxIPAddressRemote";
            textBoxIPAddressRemote.Size = new Size(100, 23);
            textBoxIPAddressRemote.TabIndex = 2;
            textBoxIPAddressRemote.TextAlign = HorizontalAlignment.Center;
            textBoxIPAddressRemote.KeyDown += FormSettings_KeyDown;
            textBoxIPAddressRemote.KeyPress += textBoxIPAddressRemote_KeyPress;
            // 
            // label1
            // 
            label1.AutoSize = true;
            label1.Location = new Point(12, 15);
            label1.Name = "label1";
            label1.Size = new Size(80, 15);
            label1.TabIndex = 3;
            label1.Text = "UDPPortLocal";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(12, 44);
            label2.Name = "label2";
            label2.Size = new Size(93, 15);
            label2.TabIndex = 4;
            label2.Text = "UDPPortRemote";
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new Point(12, 73);
            label3.Name = "label3";
            label3.Size = new Size(99, 15);
            label3.TabIndex = 5;
            label3.Text = "IPAddressRemote";
            // 
            // buttonCancel
            // 
            buttonCancel.Location = new Point(247, 176);
            buttonCancel.Name = "buttonCancel";
            buttonCancel.Size = new Size(75, 23);
            buttonCancel.TabIndex = 7;
            buttonCancel.Text = "&Cancel";
            buttonCancel.UseVisualStyleBackColor = true;
            buttonCancel.Click += buttonCancel_Click;
            buttonCancel.KeyDown += FormSettings_KeyDown;
            // 
            // buttonOK
            // 
            buttonOK.Location = new Point(166, 176);
            buttonOK.Name = "buttonOK";
            buttonOK.Size = new Size(75, 23);
            buttonOK.TabIndex = 6;
            buttonOK.Text = "&OK";
            buttonOK.UseVisualStyleBackColor = true;
            buttonOK.Click += buttonOK_Click;
            buttonOK.KeyDown += FormSettings_KeyDown;
            // 
            // checkBoxLog
            // 
            checkBoxLog.AutoSize = true;
            checkBoxLog.Location = new Point(12, 176);
            checkBoxLog.Name = "checkBoxLog";
            checkBoxLog.Size = new Size(46, 19);
            checkBoxLog.TabIndex = 5;
            checkBoxLog.Text = "&Log";
            checkBoxLog.UseVisualStyleBackColor = true;
            checkBoxLog.KeyDown += FormSettings_KeyDown;
            // 
            // checkBoxIPAddressRemoteLocalhost
            // 
            checkBoxIPAddressRemoteLocalhost.AutoSize = true;
            checkBoxIPAddressRemoteLocalhost.Location = new Point(229, 72);
            checkBoxIPAddressRemoteLocalhost.Name = "checkBoxIPAddressRemoteLocalhost";
            checkBoxIPAddressRemoteLocalhost.Size = new Size(74, 19);
            checkBoxIPAddressRemoteLocalhost.TabIndex = 3;
            checkBoxIPAddressRemoteLocalhost.Text = "localhost";
            checkBoxIPAddressRemoteLocalhost.UseVisualStyleBackColor = true;
            checkBoxIPAddressRemoteLocalhost.CheckedChanged += checkBoxAPAddressRemoteLocalhost_CheckedChanged;
            checkBoxIPAddressRemoteLocalhost.KeyDown += FormSettings_KeyDown;
            // 
            // textBoxCOMPortNumber
            // 
            textBoxCOMPortNumber.Location = new Point(123, 99);
            textBoxCOMPortNumber.MaxLength = 3;
            textBoxCOMPortNumber.Name = "textBoxCOMPortNumber";
            textBoxCOMPortNumber.Size = new Size(100, 23);
            textBoxCOMPortNumber.TabIndex = 4;
            textBoxCOMPortNumber.TextAlign = HorizontalAlignment.Center;
            textBoxCOMPortNumber.KeyDown += FormSettings_KeyDown;
            textBoxCOMPortNumber.KeyPress += textBox_Digit_KeyPress;
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(12, 102);
            label4.Name = "label4";
            label4.Size = new Size(101, 15);
            label4.TabIndex = 7;
            label4.Text = "COMPortNumber";
            // 
            // FormSettings
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(334, 211);
            Controls.Add(label4);
            Controls.Add(textBoxCOMPortNumber);
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
        private TextBox textBoxCOMPortNumber;
        private Label label4;
    }
}