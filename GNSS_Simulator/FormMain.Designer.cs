namespace SergeM
{
    partial class FormMain
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
            buttonStopReceive = new Button();
            buttonSend = new Button();
            textBoxSend = new TextBox();
            textBoxReceive = new RichTextBox();
            buttonReconnect = new Button();
            buttonSettings = new Button();
            label1 = new Label();
            labelUARTBaudrate = new Label();
            groupBox1 = new GroupBox();
            radioButtonSendAsIs = new RadioButton();
            radioButtonSendLine = new RadioButton();
            radioButtonSendNMEANoCRC = new RadioButton();
            radioButtonSendNMEA = new RadioButton();
            buttonSendRestart = new Button();
            groupBox1.SuspendLayout();
            SuspendLayout();
            // 
            // buttonStopReceive
            // 
            buttonStopReceive.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonStopReceive.Location = new Point(577, 426);
            buttonStopReceive.Name = "buttonStopReceive";
            buttonStopReceive.Size = new Size(95, 23);
            buttonStopReceive.TabIndex = 25;
            buttonStopReceive.Text = "Stop &Receive";
            buttonStopReceive.UseVisualStyleBackColor = true;
            buttonStopReceive.Click += buttonStopReceive_Click;
            // 
            // buttonSend
            // 
            buttonSend.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonSend.Location = new Point(518, 317);
            buttonSend.Name = "buttonSend";
            buttonSend.Size = new Size(75, 23);
            buttonSend.TabIndex = 24;
            buttonSend.Text = "&Send";
            buttonSend.UseVisualStyleBackColor = true;
            buttonSend.Click += buttonSend_Click;
            // 
            // textBoxSend
            // 
            textBoxSend.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            textBoxSend.Location = new Point(12, 318);
            textBoxSend.Name = "textBoxSend";
            textBoxSend.Size = new Size(500, 23);
            textBoxSend.TabIndex = 23;
            // 
            // textBoxReceive
            // 
            textBoxReceive.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            textBoxReceive.BorderStyle = BorderStyle.FixedSingle;
            textBoxReceive.Location = new Point(12, 12);
            textBoxReceive.Name = "textBoxReceive";
            textBoxReceive.ReadOnly = true;
            textBoxReceive.ScrollBars = RichTextBoxScrollBars.Vertical;
            textBoxReceive.Size = new Size(500, 300);
            textBoxReceive.TabIndex = 22;
            textBoxReceive.Text = "";
            // 
            // buttonReconnect
            // 
            buttonReconnect.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonReconnect.Location = new Point(93, 426);
            buttonReconnect.Name = "buttonReconnect";
            buttonReconnect.Size = new Size(75, 23);
            buttonReconnect.TabIndex = 27;
            buttonReconnect.Text = "Re&connect";
            buttonReconnect.UseVisualStyleBackColor = true;
            buttonReconnect.Click += buttonReconnect_Click;
            // 
            // buttonSettings
            // 
            buttonSettings.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonSettings.Location = new Point(12, 426);
            buttonSettings.Name = "buttonSettings";
            buttonSettings.Size = new Size(75, 23);
            buttonSettings.TabIndex = 26;
            buttonSettings.Text = "Se&ttings";
            buttonSettings.UseVisualStyleBackColor = true;
            buttonSettings.Click += buttonSettings_Click;
            // 
            // label1
            // 
            label1.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            label1.AutoSize = true;
            label1.Location = new Point(518, 14);
            label1.Name = "label1";
            label1.Size = new Size(60, 15);
            label1.TabIndex = 28;
            label1.Text = "Baud rate:";
            // 
            // labelUARTBaudrate
            // 
            labelUARTBaudrate.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            labelUARTBaudrate.AutoSize = true;
            labelUARTBaudrate.Location = new Point(600, 14);
            labelUARTBaudrate.Name = "labelUARTBaudrate";
            labelUARTBaudrate.Size = new Size(22, 15);
            labelUARTBaudrate.TabIndex = 29;
            labelUARTBaudrate.Text = "---";
            // 
            // groupBox1
            // 
            groupBox1.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            groupBox1.Controls.Add(radioButtonSendAsIs);
            groupBox1.Controls.Add(radioButtonSendLine);
            groupBox1.Controls.Add(radioButtonSendNMEANoCRC);
            groupBox1.Controls.Add(radioButtonSendNMEA);
            groupBox1.Location = new Point(518, 188);
            groupBox1.Name = "groupBox1";
            groupBox1.Size = new Size(154, 123);
            groupBox1.TabIndex = 30;
            groupBox1.TabStop = false;
            groupBox1.Text = "Send format";
            // 
            // radioButtonSendAsIs
            // 
            radioButtonSendAsIs.AutoSize = true;
            radioButtonSendAsIs.Location = new Point(6, 22);
            radioButtonSendAsIs.Name = "radioButtonSendAsIs";
            radioButtonSendAsIs.Size = new Size(49, 19);
            radioButtonSendAsIs.TabIndex = 3;
            radioButtonSendAsIs.TabStop = true;
            radioButtonSendAsIs.Text = "As is";
            radioButtonSendAsIs.UseVisualStyleBackColor = true;
            // 
            // radioButtonSendLine
            // 
            radioButtonSendLine.AutoSize = true;
            radioButtonSendLine.Location = new Point(6, 47);
            radioButtonSendLine.Name = "radioButtonSendLine";
            radioButtonSendLine.Size = new Size(47, 19);
            radioButtonSendLine.TabIndex = 2;
            radioButtonSendLine.TabStop = true;
            radioButtonSendLine.Text = "Line";
            radioButtonSendLine.UseVisualStyleBackColor = true;
            // 
            // radioButtonSendNMEANoCRC
            // 
            radioButtonSendNMEANoCRC.AutoSize = true;
            radioButtonSendNMEANoCRC.Location = new Point(6, 97);
            radioButtonSendNMEANoCRC.Name = "radioButtonSendNMEANoCRC";
            radioButtonSendNMEANoCRC.Size = new Size(102, 19);
            radioButtonSendNMEANoCRC.TabIndex = 1;
            radioButtonSendNMEANoCRC.TabStop = true;
            radioButtonSendNMEANoCRC.Text = "NMEA no CRC";
            radioButtonSendNMEANoCRC.UseVisualStyleBackColor = true;
            // 
            // radioButtonSendNMEA
            // 
            radioButtonSendNMEA.AutoSize = true;
            radioButtonSendNMEA.Location = new Point(6, 72);
            radioButtonSendNMEA.Name = "radioButtonSendNMEA";
            radioButtonSendNMEA.Size = new Size(59, 19);
            radioButtonSendNMEA.TabIndex = 0;
            radioButtonSendNMEA.TabStop = true;
            radioButtonSendNMEA.Text = "NMEA";
            radioButtonSendNMEA.UseVisualStyleBackColor = true;
            // 
            // buttonSendRestart
            // 
            buttonSendRestart.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonSendRestart.Location = new Point(577, 397);
            buttonSendRestart.Name = "buttonSendRestart";
            buttonSendRestart.Size = new Size(95, 23);
            buttonSendRestart.TabIndex = 31;
            buttonSendRestart.Text = "Send Rest&art";
            buttonSendRestart.UseVisualStyleBackColor = true;
            buttonSendRestart.Click += buttonSendRestart_Click;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(684, 461);
            Controls.Add(buttonSendRestart);
            Controls.Add(groupBox1);
            Controls.Add(labelUARTBaudrate);
            Controls.Add(label1);
            Controls.Add(buttonStopReceive);
            Controls.Add(buttonSend);
            Controls.Add(textBoxSend);
            Controls.Add(textBoxReceive);
            Controls.Add(buttonReconnect);
            Controls.Add(buttonSettings);
            MinimumSize = new Size(700, 500);
            Name = "FormMain";
            StartPosition = FormStartPosition.Manual;
            Text = "GNSS Simulator";
            FormClosing += FormMain_FormClosing;
            groupBox1.ResumeLayout(false);
            groupBox1.PerformLayout();
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button buttonStopReceive;
        private Button buttonSend;
        private TextBox textBoxSend;
        private RichTextBox textBoxReceive;
        private Button buttonReconnect;
        private Button buttonSettings;
        private Label label1;
        private Label labelUARTBaudrate;
        private GroupBox groupBox1;
        private RadioButton radioButtonSendNMEA;
        private RadioButton radioButtonSendNMEANoCRC;
        private RadioButton radioButtonSendLine;
        private RadioButton radioButtonSendAsIs;
        private Button buttonSendRestart;
    }
}
