namespace SergeM
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
            label2 = new Label();
            buttonCancel = new Button();
            buttonOK = new Button();
            checkBoxLog = new CheckBox();
            label4 = new Label();
            comboBoxReceiverModel = new ComboBox();
            labelCOMPortNumber = new Label();
            comboBoxCOMPort = new ComboBox();
            comboBoxCOMBaudrate = new ComboBox();
            SuspendLayout();
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new Point(12, 44);
            label2.Name = "label2";
            label2.Size = new Size(116, 15);
            label2.TabIndex = 3;
            label2.Text = "COM Port &Baud rate:";
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
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new Point(12, 73);
            label4.Name = "label4";
            label4.Size = new Size(91, 15);
            label4.TabIndex = 5;
            label4.Text = "Receiver Model:";
            // 
            // comboBoxReceiverModel
            // 
            comboBoxReceiverModel.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBoxReceiverModel.FormattingEnabled = true;
            comboBoxReceiverModel.Items.AddRange(new object[] { "EB-800A", "GSU-7x", "LR9548S", "SC872-A" });
            comboBoxReceiverModel.Location = new Point(150, 70);
            comboBoxReceiverModel.Name = "comboBoxReceiverModel";
            comboBoxReceiverModel.Size = new Size(100, 23);
            comboBoxReceiverModel.TabIndex = 6;
            comboBoxReceiverModel.KeyDown += FormSettings_KeyDown;
            // 
            // labelCOMPortNumber
            // 
            labelCOMPortNumber.AutoSize = true;
            labelCOMPortNumber.Location = new Point(12, 15);
            labelCOMPortNumber.Name = "labelCOMPortNumber";
            labelCOMPortNumber.Size = new Size(110, 15);
            labelCOMPortNumber.TabIndex = 1;
            labelCOMPortNumber.Text = "COM &Port Number:";
            // 
            // comboBoxCOMPort
            // 
            comboBoxCOMPort.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBoxCOMPort.FormattingEnabled = true;
            comboBoxCOMPort.Location = new Point(150, 12);
            comboBoxCOMPort.Name = "comboBoxCOMPort";
            comboBoxCOMPort.Size = new Size(100, 23);
            comboBoxCOMPort.TabIndex = 2;
            comboBoxCOMPort.KeyDown += FormSettings_KeyDown;
            // 
            // comboBoxCOMBaudrate
            // 
            comboBoxCOMBaudrate.DropDownStyle = ComboBoxStyle.DropDownList;
            comboBoxCOMBaudrate.FormattingEnabled = true;
            comboBoxCOMBaudrate.Items.AddRange(new object[] { "2400", "4800", "9600", "19200", "38400", "57600", "115200" });
            comboBoxCOMBaudrate.Location = new Point(150, 41);
            comboBoxCOMBaudrate.Name = "comboBoxCOMBaudrate";
            comboBoxCOMBaudrate.Size = new Size(100, 23);
            comboBoxCOMBaudrate.TabIndex = 4;
            comboBoxCOMBaudrate.KeyDown += FormSettings_KeyDown;
            // 
            // FormSettings
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(334, 211);
            Controls.Add(comboBoxCOMBaudrate);
            Controls.Add(comboBoxCOMPort);
            Controls.Add(labelCOMPortNumber);
            Controls.Add(comboBoxReceiverModel);
            Controls.Add(label4);
            Controls.Add(checkBoxLog);
            Controls.Add(buttonOK);
            Controls.Add(buttonCancel);
            Controls.Add(label2);
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
        private Label label2;
        private Button buttonCancel;
        private Button buttonOK;
        private CheckBox checkBoxLog;
        private Label label4;
        private ComboBox comboBoxReceiverModel;
        private Label labelCOMPortNumber;
        private ComboBox comboBoxCOMPort;
        private ComboBox comboBoxCOMBaudrate;
    }
}