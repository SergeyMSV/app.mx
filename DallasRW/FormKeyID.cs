using System.Text.Json.Nodes;
using ExtensionMethods;

namespace DallasRW
{
    public partial class FormKeyID : Form
    {
        bool m_IsChanged;
        public bool IsChanged { get => m_IsChanged; }

        string m_FilePath;
        string m_KeyGroup;
        string m_KeyLabel;
        string m_KeyID;
        JsonObject m_JsonObj;

        public FormKeyID(string filePath, string keyGroup, string keyLabel, string keyID)
        {
            InitializeComponent();

            m_FilePath = filePath;

            m_KeyGroup = keyGroup;
            m_KeyLabel = keyLabel;
            m_KeyID = keyID;

            using (StreamReader sr = new StreamReader(m_FilePath))
            {
                string DataJSON = sr.ReadToEnd();
                m_JsonObj = JsonNode.Parse(DataJSON)?.AsObject()!;
            }

            string[] KeyGroups = FileKeys.GetKeyGroups(m_JsonObj);
            foreach (string KeyGroup in KeyGroups)
                comboBoxKeyGroup.Items.Add(KeyGroup);

            comboBoxKeyGroup.Text = m_KeyGroup;
            textBoxKeyLabel.Text = m_KeyLabel;
            textBoxKeyID.Text = m_KeyID;
        }

        void buttonCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        void buttonOK_Click(object sender, EventArgs e)
        {
            if (!comboBoxKeyGroup.Text.IsKeyFieldChanged(m_KeyGroup) &&
                !textBoxKeyLabel.Text.IsKeyFieldChanged(m_KeyLabel) &&
                !textBoxKeyID.Text.IsKeyFieldChanged(m_KeyID))
            {
                Close();
                return;
            }

            if (!comboBoxKeyGroup.Text.IsKeyGroup())
            {
                MessageBox.Show("Wrong Key Group.", "Edit Key ID");
                return;
            }

            if (!textBoxKeyLabel.Text.IsKeyLabel())
            {
                MessageBox.Show("Wrong Key Label.", "Edit Key ID");
                return;
            }

            if (!textBoxKeyID.Text.IsKeyID())
            {
                MessageBox.Show("Wrong Key ID.", "Edit Key ID");
                return;
            }

            if (!FileKeys.Update(m_JsonObj, m_KeyGroup, comboBoxKeyGroup.Text, m_KeyLabel, textBoxKeyLabel.Text, m_KeyID, textBoxKeyID.Text.ToLower()))
            {
                MessageBox.Show("A Key ID with the same label has already been added.", "Edit Key ID");
                return;
            }

            m_IsChanged = true;

            File.WriteAllText(m_FilePath, m_JsonObj.ToJsonString());
            Close();
        }

        void FormKeyID_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                Close();
        }

        private void textBoxKeyID_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (e.KeyChar >= 'a' && e.KeyChar <= 'f' || e.KeyChar >= 'A' && e.KeyChar <= 'F')
                return;
            e.Handled = !char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar);
        }
    }
}
