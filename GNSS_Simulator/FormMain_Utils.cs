// In *.csproj.user the following statement:
// 
//     <Compile Update="FormMain_Utils.cs">
//       <SubType>Form</SubType>
//     </Compile>
// 
// shall be replaces with the next one:
// 
//     <Compile Update="FormMain_Utils.cs">
//       <DependentUpon>FormMain.cs</DependentUpon>
//       <SubType>Form</SubType>
//     </Compile>

namespace SergeM
{
    public partial class FormMain
    {
        void InitializeFormSizes()
        {
            Size FormSize = Properties.Settings.Default.FormMainSize;
            if (FormSize.Height < this.MinimumSize.Height || FormSize.Width < this.MinimumSize.Width)
                FormSize = this.MinimumSize;
            if (FormSize.Height > SystemInformation.VirtualScreen.Height || FormSize.Width > SystemInformation.VirtualScreen.Width)
                FormSize = this.MinimumSize;
            if (IsLocationOnScreen(Properties.Settings.Default.FormMainPosition))
            {
                this.Location = Properties.Settings.Default.FormMainPosition;
            }
            else
            {
                this.StartPosition = FormStartPosition.CenterScreen;
            }
            this.Size = FormSize;
        }

        void SaveFormSizes()
        {
            if (this.WindowState != FormWindowState.Normal)
                return;
            if (Properties.Settings.Default.FormMainPosition == this.Location &&
                Properties.Settings.Default.FormMainSize == this.Size)
                return;
            Properties.Settings.Default.FormMainPosition = this.Location;
            Properties.Settings.Default.FormMainSize = this.Size;
            Properties.Settings.Default.Save();
        }

        bool IsLocationOnScreen(Point location)
        {
            foreach (Screen screen in Screen.AllScreens)
            {
                Rectangle screenBounds = screen.Bounds;
                Rectangle formBounds = new Rectangle(location, Application.OpenForms.Count > 0 ? Application.OpenForms[0]!.Size : new Size(800, 600));
                if (screenBounds.Contains(formBounds))
                    return true;
            }
            return false;
        }
    }
}
