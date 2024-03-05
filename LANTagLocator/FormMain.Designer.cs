namespace LANTagLocator
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
            this.listViewTags = new System.Windows.Forms.ListView();
            this.columnIPAddress = new System.Windows.Forms.ColumnHeader();
            this.columnPlatform = new System.Windows.Forms.ColumnHeader();
            this.columnHost = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // listViewTags
            // 
            this.listViewTags.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listViewTags.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnIPAddress,
            this.columnPlatform,
            this.columnHost});
            this.listViewTags.Location = new System.Drawing.Point(12, 12);
            this.listViewTags.Name = "listViewTags";
            this.listViewTags.Size = new System.Drawing.Size(360, 137);
            this.listViewTags.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listViewTags.TabIndex = 0;
            this.listViewTags.UseCompatibleStateImageBehavior = false;
            this.listViewTags.View = System.Windows.Forms.View.Details;
            // 
            // columnIPAddress
            // 
            this.columnIPAddress.Text = "IP-address";
            this.columnIPAddress.Width = 130;
            // 
            // columnPlatform
            // 
            this.columnPlatform.Text = "Platform";
            this.columnPlatform.Width = 100;
            // 
            // columnHost
            // 
            this.columnHost.Text = "Host name";
            this.columnHost.Width = 100;
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(384, 161);
            this.Controls.Add(this.listViewTags);
            this.MinimumSize = new System.Drawing.Size(400, 200);
            this.Name = "FormMain";
            this.Text = "LANTag Locator";
            this.ResumeLayout(false);

        }

        #endregion

        private ListView listViewTags;
        private ColumnHeader columnPlatform;
        private ColumnHeader columnHost;
        private ColumnHeader columnIPAddress;
    }
}