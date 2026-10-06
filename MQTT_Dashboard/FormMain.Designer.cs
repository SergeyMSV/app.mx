namespace MQTT_Dashboard
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
            topicTextBox = new TextBox();
            logTextBox = new TextBox();
            buttonConnect = new Button();
            buttonSubscribe = new Button();
            buttonDisconnect = new Button();
            SuspendLayout();
            // 
            // topicTextBox
            // 
            topicTextBox.Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right;
            topicTextBox.Location = new Point(-3, -1);
            topicTextBox.Multiline = true;
            topicTextBox.Name = "topicTextBox";
            topicTextBox.Size = new Size(473, 130);
            topicTextBox.TabIndex = 0;
            // 
            // logTextBox
            // 
            logTextBox.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            logTextBox.Location = new Point(-3, 135);
            logTextBox.Multiline = true;
            logTextBox.Name = "logTextBox";
            logTextBox.Size = new Size(473, 329);
            logTextBox.TabIndex = 1;
            // 
            // buttonConnect
            // 
            buttonConnect.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            buttonConnect.Location = new Point(476, 11);
            buttonConnect.Name = "buttonConnect";
            buttonConnect.Size = new Size(75, 23);
            buttonConnect.TabIndex = 2;
            buttonConnect.Text = "Connect";
            buttonConnect.UseVisualStyleBackColor = true;
            buttonConnect.Click += buttonConnect_Click;
            // 
            // buttonSubscribe
            // 
            buttonSubscribe.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            buttonSubscribe.Location = new Point(476, 40);
            buttonSubscribe.Name = "buttonSubscribe";
            buttonSubscribe.Size = new Size(75, 23);
            buttonSubscribe.TabIndex = 3;
            buttonSubscribe.Text = "Subscribe";
            buttonSubscribe.UseVisualStyleBackColor = true;
            buttonSubscribe.Click += buttonSubscribe_Click;
            // 
            // buttonDisconnect
            // 
            buttonDisconnect.Anchor = AnchorStyles.Top | AnchorStyles.Right;
            buttonDisconnect.Location = new Point(476, 69);
            buttonDisconnect.Name = "buttonDisconnect";
            buttonDisconnect.Size = new Size(75, 23);
            buttonDisconnect.TabIndex = 4;
            buttonDisconnect.Text = "Disconnect";
            buttonDisconnect.UseVisualStyleBackColor = true;
            buttonDisconnect.Click += buttonDisconnect_Click;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(563, 461);
            Controls.Add(buttonDisconnect);
            Controls.Add(buttonSubscribe);
            Controls.Add(buttonConnect);
            Controls.Add(logTextBox);
            Controls.Add(topicTextBox);
            Name = "FormMain";
            Text = "MQTT Dashboard";
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion

        private TextBox topicTextBox;
        private TextBox logTextBox;
        private Button buttonConnect;
        private Button buttonSubscribe;
        private Button buttonDisconnect;
    }
}
