using System.Text.RegularExpressions;

namespace ExtensionMethods
{
    public static class StringExtension
    {
        public static bool IsKeyGroup(this string str)
        {
            var KeyIDRaw = str.Where(char.IsLetterOrDigit).ToArray()!;
            return KeyIDRaw?.Length > 0;
        }

        public static bool IsKeyLabel(this string str)
        {
            var KeyIDRaw = str.Where(char.IsLetterOrDigit).ToArray()!;
            return KeyIDRaw?.Length > 0;
        }

        public static bool IsKeyID(this string str)
        {
            var KeyIDRaw = str.Where((char a) => { return char.IsDigit(a) || (a >= 'a' && a <= 'f') || (a >= 'A' && a <= 'F'); }).ToArray()!;
            return KeyIDRaw?.Length == 12;
        }

        public static bool IsKeyFieldChanged(this string str, string value)
        {
            return str.Length == 0 || str != value;
        }
    }
}
