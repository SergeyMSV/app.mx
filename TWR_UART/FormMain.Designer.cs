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
            buttonSettings = new Button();
            buttonReconnect = new Button();
            textBoxReceive = new RichTextBox();
            textBoxSend = new TextBox();
            buttonSend = new Button();
            buttonStopReceive = new Button();
            SuspendLayout();
            // 
            // buttonSettings
            // 
            buttonSettings.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonSettings.Location = new Point(12, 326);
            buttonSettings.Name = "buttonSettings";
            buttonSettings.Size = new Size(75, 23);
            buttonSettings.TabIndex = 20;
            buttonSettings.Text = "Se&ttings";
            buttonSettings.UseVisualStyleBackColor = true;
            buttonSettings.Click += buttonSettings_Click;
            // 
            // buttonReconnect
            // 
            buttonReconnect.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonReconnect.Location = new Point(93, 326);
            buttonReconnect.Name = "buttonReconnect";
            buttonReconnect.Size = new Size(75, 23);
            buttonReconnect.TabIndex = 21;
            buttonReconnect.Text = "Re&connect";
            buttonReconnect.UseVisualStyleBackColor = true;
            buttonReconnect.Click += buttonReconnect_Click;
            // 
            // textBoxReceive
            // 
            textBoxReceive.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            textBoxReceive.BorderStyle = BorderStyle.FixedSingle;
            textBoxReceive.Location = new Point(12, 12);
            textBoxReceive.Name = "textBoxReceive";
            textBoxReceive.ReadOnly = true;
            textBoxReceive.ScrollBars = RichTextBoxScrollBars.Vertical;
            textBoxReceive.Size = new Size(460, 278);
            textBoxReceive.TabIndex = 14;
            textBoxReceive.Text = "";
            // 
            // textBoxSend
            // 
            textBoxSend.Anchor = AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            textBoxSend.Location = new Point(12, 297);
            textBoxSend.Name = "textBoxSend";
            textBoxSend.Size = new Size(379, 23);
            textBoxSend.TabIndex = 15;
            // 
            // buttonSend
            // 
            buttonSend.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonSend.Location = new Point(397, 296);
            buttonSend.Name = "buttonSend";
            buttonSend.Size = new Size(75, 23);
            buttonSend.TabIndex = 16;
            buttonSend.Text = "&Send";
            buttonSend.UseVisualStyleBackColor = true;
            buttonSend.Click += buttonSend_Click;
            // 
            // buttonStopReceive
            // 
            buttonStopReceive.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonStopReceive.Location = new Point(397, 326);
            buttonStopReceive.Name = "buttonStopReceive";
            buttonStopReceive.Size = new Size(75, 23);
            buttonStopReceive.TabIndex = 17;
            buttonStopReceive.Text = "Stop &recv";
            buttonStopReceive.UseVisualStyleBackColor = true;
            buttonStopReceive.Click += buttonStopReceive_Click;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(484, 361);
            Controls.Add(buttonStopReceive);
            Controls.Add(buttonSend);
            Controls.Add(textBoxSend);
            Controls.Add(textBoxReceive);
            Controls.Add(buttonReconnect);
            Controls.Add(buttonSettings);
            MinimumSize = new Size(500, 400);
            Name = "FormMain";
            Text = "UART";
            FormClosing += FormMain_FormClosing;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private Button buttonSettings;
        private Button buttonReconnect;
        private TextBox textBoxSend;
        private Button buttonSend;
        private RichTextBox textBoxReceive;
        private Button buttonStopReceive;
    }
}
