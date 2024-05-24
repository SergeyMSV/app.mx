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
            components = new System.ComponentModel.Container();
            listViewTags = new ListView();
            columnIPAddress = new ColumnHeader();
            columnPlatform = new ColumnHeader();
            columnHost = new ColumnHeader();
            columnUptime = new ColumnHeader();
            contextMenuStripTags = new ContextMenuStrip(components);
            toolStripMenuItemReboot = new ToolStripMenuItem();
            toolStripMenuItemHalt = new ToolStripMenuItem();
            contextMenuStripTags.SuspendLayout();
            SuspendLayout();
            // 
            // listViewTags
            // 
            listViewTags.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
            listViewTags.Columns.AddRange(new ColumnHeader[] { columnIPAddress, columnPlatform, columnHost, columnUptime });
            listViewTags.ContextMenuStrip = contextMenuStripTags;
            listViewTags.FullRowSelect = true;
            listViewTags.Location = new Point(12, 12);
            listViewTags.MultiSelect = false;
            listViewTags.Name = "listViewTags";
            listViewTags.Size = new Size(429, 137);
            listViewTags.Sorting = SortOrder.Ascending;
            listViewTags.TabIndex = 0;
            listViewTags.UseCompatibleStateImageBehavior = false;
            listViewTags.View = View.Details;
            // 
            // columnIPAddress
            // 
            columnIPAddress.Text = "IP-address";
            columnIPAddress.Width = 100;
            // 
            // columnPlatform
            // 
            columnPlatform.Text = "Platform";
            columnPlatform.Width = 100;
            // 
            // columnHost
            // 
            columnHost.Text = "Host name";
            columnHost.Width = 100;
            // 
            // columnUptime
            // 
            columnUptime.Text = "Uptime";
            columnUptime.TextAlign = HorizontalAlignment.Right;
            columnUptime.Width = 120;
            // 
            // contextMenuStripTags
            // 
            contextMenuStripTags.Items.AddRange(new ToolStripItem[] { toolStripMenuItemReboot, toolStripMenuItemHalt });
            contextMenuStripTags.Name = "contextMenuStripTags";
            contextMenuStripTags.Size = new Size(113, 48);
            // 
            // toolStripMenuItemReboot
            // 
            toolStripMenuItemReboot.Name = "toolStripMenuItemReboot";
            toolStripMenuItemReboot.Size = new Size(112, 22);
            toolStripMenuItemReboot.Text = "&Reboot";
            toolStripMenuItemReboot.Click += toolStripMenuItemReboot_Click;
            // 
            // toolStripMenuItemHalt
            // 
            toolStripMenuItemHalt.Name = "toolStripMenuItemHalt";
            toolStripMenuItemHalt.Size = new Size(112, 22);
            toolStripMenuItemHalt.Text = "&Halt";
            toolStripMenuItemHalt.Click += toolStripMenuItemHalt_Click;
            // 
            // FormMain
            // 
            AutoScaleDimensions = new SizeF(7F, 15F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(453, 161);
            Controls.Add(listViewTags);
            MinimumSize = new Size(400, 200);
            Name = "FormMain";
            Text = "LANTag Locator";
            contextMenuStripTags.ResumeLayout(false);
            ResumeLayout(false);
        }

        #endregion

        private ListView listViewTags;
        private ColumnHeader columnPlatform;
        private ColumnHeader columnHost;
        private ColumnHeader columnIPAddress;
        private ContextMenuStrip contextMenuStripTags;
        private ToolStripMenuItem toolStripMenuItemReboot;
        private ToolStripMenuItem toolStripMenuItemHalt;
        private ColumnHeader columnUptime;
    }
}