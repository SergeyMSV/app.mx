namespace DallasRW
{
    partial class FormKeyID
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
            this.buttonOK = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxKeyLabel = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxKeyID = new System.Windows.Forms.TextBox();
            this.comboBoxKeyGroup = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // buttonOK
            // 
            this.buttonOK.Location = new System.Drawing.Point(97, 106);
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.Size = new System.Drawing.Size(75, 23);
            this.buttonOK.TabIndex = 1;
            this.buttonOK.Text = "&OK";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            this.buttonOK.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Location = new System.Drawing.Point(178, 106);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(75, 23);
            this.buttonCancel.TabIndex = 2;
            this.buttonCancel.Text = "&Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            this.buttonCancel.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(40, 15);
            this.label1.TabIndex = 20;
            this.label1.Text = "Group";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 38);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 15);
            this.label2.TabIndex = 21;
            this.label2.Text = "Label";
            // 
            // textBoxKeyLabel
            // 
            this.textBoxKeyLabel.Location = new System.Drawing.Point(76, 35);
            this.textBoxKeyLabel.MaxLength = 40;
            this.textBoxKeyLabel.Name = "textBoxKeyLabel";
            this.textBoxKeyLabel.Size = new System.Drawing.Size(177, 23);
            this.textBoxKeyLabel.TabIndex = 4;
            this.textBoxKeyLabel.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 67);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(40, 15);
            this.label3.TabIndex = 22;
            this.label3.Text = "Key ID";
            // 
            // textBoxKeyID
            // 
            this.textBoxKeyID.CharacterCasing = System.Windows.Forms.CharacterCasing.Lower;
            this.textBoxKeyID.Location = new System.Drawing.Point(76, 64);
            this.textBoxKeyID.MaxLength = 12;
            this.textBoxKeyID.Name = "textBoxKeyID";
            this.textBoxKeyID.PlaceholderText = "XXXXXXXXXXXX";
            this.textBoxKeyID.Size = new System.Drawing.Size(177, 23);
            this.textBoxKeyID.TabIndex = 5;
            this.textBoxKeyID.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.textBoxKeyID.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            this.textBoxKeyID.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBoxKeyID_KeyPress);
            // 
            // comboBoxKeyGroup
            // 
            this.comboBoxKeyGroup.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxKeyGroup.FormattingEnabled = true;
            this.comboBoxKeyGroup.Location = new System.Drawing.Point(76, 6);
            this.comboBoxKeyGroup.Name = "comboBoxKeyGroup";
            this.comboBoxKeyGroup.Size = new System.Drawing.Size(177, 23);
            this.comboBoxKeyGroup.TabIndex = 3;
            this.comboBoxKeyGroup.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            // 
            // FormKeyID
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(265, 141);
            this.Controls.Add(this.comboBoxKeyGroup);
            this.Controls.Add(this.textBoxKeyID);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.textBoxKeyLabel);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.Name = "FormKeyID";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Key ID";
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.FormKeyID_KeyDown);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Button buttonOK;
        private Button buttonCancel;
        private Label label1;
        private Label label2;
        private TextBox textBoxKeyLabel;
        private Label label3;
        private TextBox textBoxKeyID;
        private ComboBox comboBoxKeyGroup;
    }
}