namespace DallasBus
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
            listViewBus = new ListView();
            columnHeaderFamilyCode = new ColumnHeader();
            columnHeaderID = new ColumnHeader();
            columnHeaderValue = new ColumnHeader();
            buttonSearch = new Button();
            buttonGetValue = new Button();
            checkBoxAutoGetValue = new CheckBox();
            SuspendLayout();
            // 
            // buttonSettings
            // 
            buttonSettings.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonSettings.Location = new Point(12, 326);
            buttonSettings.Name = "buttonSettings";
            buttonSettings.Size = new Size(75, 23);
            buttonSettings.TabIndex = 11;
            buttonSettings.Text = "Se&ttings";
            buttonSettings.UseVisualStyleBackColor = true;
            buttonSettings.Click += buttonSettings_Click;
            buttonSettings.KeyDown += FormMain_KeyDown;
            // 
            // buttonReconnect
            // 
            buttonReconnect.Anchor = AnchorStyles.Bottom | AnchorStyles.Left;
            buttonReconnect.Location = new Point(97, 326);
            buttonReconnect.Name = "buttonReconnect";
            buttonReconnect.Size = new Size(75, 23);
            buttonReconnect.TabIndex = 12;
            buttonReconnect.Text = "Re&connect";
            buttonReconnect.UseVisualStyleBackColor = true;
            buttonReconnect.Click += buttonReconnect_Click;
            buttonReconnect.KeyDown += FormMain_KeyDown;
            // 
            // listViewBus
            // 
            listViewBus.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            listViewBus.Columns.AddRange(new ColumnHeader[] { columnHeaderFamilyCode, columnHeaderID, columnHeaderValue });
            listViewBus.Location = new Point(12, 12);
            listViewBus.Name = "listViewBus";
            listViewBus.Size = new Size(460, 308);
            listViewBus.TabIndex = 1;
            listViewBus.UseCompatibleStateImageBehavior = false;
            listViewBus.View = View.Details;
            listViewBus.KeyDown += FormMain_KeyDown;
            // 
            // columnHeaderFamilyCode
            // 
            columnHeaderFamilyCode.Text = "Family Code";
            columnHeaderFamilyCode.Width = 100;
            // 
            // columnHeaderID
            // 
            columnHeaderID.Text = "ID";
            columnHeaderID.Width = 120;
            // 
            // columnHeaderValue
            // 
            columnHeaderValue.Text = "Value";
            columnHeaderValue.Width = 200;
            // 
            // buttonSearch
            // 
            buttonSearch.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonSearch.Location = new Point(209, 326);
            buttonSearch.Name = "buttonSearch";
            buttonSearch.Size = new Size(75, 23);
            buttonSearch.TabIndex = 14;
            buttonSearch.Text = "&Search";
            buttonSearch.UseVisualStyleBackColor = true;
            buttonSearch.Click += buttonSearch_Click;
            buttonSearch.KeyDown += FormMain_KeyDown;
            // 
            // buttonGetValue
            // 
            buttonGetValue.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            buttonGetValue.Location = new Point(290, 326);
            buttonGetValue.Name = "buttonGetValue";
            buttonGetValue.Size = new Size(75, 23);
            buttonGetValue.TabIndex = 15;
            buttonGetValue.Text = "&GetValue";
            buttonGetValue.UseVisualStyleBackColor = true;
            buttonGetValue.Click += buttonGetValue_Click;
            buttonGetValue.KeyDown += FormMain_KeyDown;
            // 
            // checkBoxAutoGetValue
            // 
            checkBoxAutoGetValue.Anchor = AnchorStyles.Bottom | AnchorStyles.Right;
            checkBoxAutoGetValue.AutoSize = true;
            checkBoxAutoGetValue.Location = new Point(371, 330);
            checkBoxAutoGetValue.Name = "checkBoxAutoGetValue";
            checkBoxAutoGetValue.Size = new Size(101, 19);
            checkBoxAutoGetValue.TabIndex = 16;
            checkBoxAutoGetValue.Text = "&Auto GetValue";
            checkBoxAutoGetValue.UseVisualStyleBackColor = true;
            checkBoxAutoGetValue.CheckedChanged += checkBoxAutoGetValue_CheckedChanged;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(484, 361);
            Controls.Add(checkBoxAutoGetValue);
            Controls.Add(buttonGetValue);
            Controls.Add(buttonSearch);
            Controls.Add(listViewBus);
            Controls.Add(buttonReconnect);
            Controls.Add(buttonSettings);
            MinimumSize = new Size(500, 400);
            Name = "FormMain";
            StartPosition = FormStartPosition.CenterScreen;
            Text = "DALLAS BUS";
            FormClosing += FormMain_FormClosing;
            KeyDown += FormMain_KeyDown;
            ResumeLayout(false);
            PerformLayout();
        }

        #endregion
        private Button buttonSettings;
        private Button buttonReconnect;
        private ListView listViewBus;
        private ColumnHeader columnHeaderFamilyCode;
        private ColumnHeader columnHeaderID;
        private ColumnHeader columnHeaderValue;
        private Button buttonSearch;
        private Button buttonGetValue;
        private CheckBox checkBoxAutoGetValue;
    }
}