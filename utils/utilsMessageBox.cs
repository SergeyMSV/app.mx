namespace utils
{
    public class MessageBox
    {
        bool m_AutoClear = false;
        volatile bool m_Shown = false;

        public MessageBox(bool autoClear)
        {
            m_AutoClear = autoClear;
        }

        void Show(string msg, string caption)
        {
            if (m_Shown)
                return;
            m_Shown = true;
            System.Windows.Forms.MessageBox.Show(msg, caption);
            if (m_AutoClear)
                m_Shown = false;
        }
        public void ShowFailure(string msg) => Show(msg, "Failure");
        public void ShowInform(string msg) => Show(msg, "Information");

        public void Clear()
        {
            m_Shown = false;
        }
    }
}
