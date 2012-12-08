using System.Collections.Generic;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Text;

namespace System
{
    public struct Char : IComparable, IComparable<char>, IEquatable<char>
    {
        // Note that this array must be ordered, because binary searching is used on it.
        internal static readonly char[] WhiteChars = {
			(char) 0x9, (char) 0xA, (char) 0xB, (char) 0xC, (char) 0xD,
			(char) 0x85, (char) 0x1680, (char) 0x2028, (char) 0x2029,
			(char) 0x20, (char) 0xA0, (char) 0x2000, (char) 0x2001,
			(char) 0x2002, (char) 0x2003, (char) 0x2004, (char) 0x2005,
			(char) 0x2006, (char) 0x2007, (char) 0x2008, (char) 0x2009,
			(char) 0x200A, (char) 0x200B, (char) 0x3000, (char) 0xFEFF };

#pragma warning disable 0649
        private char mValue;
#pragma warning restore 0649

        public override string ToString() { return new string(mValue, 1); }

        public override bool Equals(object obj) { return (obj is char && ((char)obj).mValue == mValue); }

        public override int GetHashCode() { return (int)mValue; }

        public static UnicodeCategory GetUnicodeCategory(char c)
        {
            ushort val = (ushort)c;
            if (val >= 0x0000 && val <= 0x001F) return UnicodeCategory.Control;
            if (val == 0x0020) return UnicodeCategory.SpaceSeparator;
            if (val >= 0x0021 && val <= 0x0023) return UnicodeCategory.OtherPunctuation;
            if (val == 0x0024) return UnicodeCategory.CurrencySymbol;
            if (val >= 0x0025 && val <= 0x0027) return UnicodeCategory.OtherPunctuation;
            if (val == 0x0028) return UnicodeCategory.OpenPunctuation;
            if (val == 0x0029) return UnicodeCategory.ClosePunctuation;
            if (val == 0x002A) return UnicodeCategory.OtherPunctuation;
            if (val == 0x002B) return UnicodeCategory.MathSymbol;
            if (val == 0x002C) return UnicodeCategory.OtherPunctuation;
            if (val == 0x002D) return UnicodeCategory.DashPunctuation;
            if (val == 0x002E || val == 0x002F) return UnicodeCategory.OtherPunctuation;
            if (val >= 0x0030 && val <= 0x0039) return UnicodeCategory.DecimalDigitNumber;
            if (val == 0x003A || val == 0x003B) return UnicodeCategory.OtherPunctuation;
            if (val >= 0x003C && val <= 0x003E) return UnicodeCategory.MathSymbol;
            if (val == 0x003F || val == 0x0040) return UnicodeCategory.OtherPunctuation;
            if (val >= 0x0041 && val <= 0x005A) return UnicodeCategory.UppercaseLetter;
            if (val == 0x005B || val == 0x005C) return UnicodeCategory.OtherPunctuation;
            if (val == 0x005D) return UnicodeCategory.ClosePunctuation;
            if (val == 0x005E) return UnicodeCategory.ModifierSymbol;
            if (val == 0x005F) return UnicodeCategory.ConnectorPunctuation;
            if (val == 0x0060) return UnicodeCategory.ModifierSymbol;
            if (val >= 0x0061 && val <= 0x007A) return UnicodeCategory.LowercaseLetter;
            if (val == 0x007B) return UnicodeCategory.OtherPunctuation;
            if (val == 0x007C) return UnicodeCategory.MathSymbol;
            if (val == 0x007D) return UnicodeCategory.ClosePunctuation;
            if (val == 0x007E) return UnicodeCategory.MathSymbol;
            if (val >= 0x007F && val <= 0x009F) return UnicodeCategory.Control;
            if (val == 0x00A0) return UnicodeCategory.SpaceSeparator;
            if (val == 0x00A1) return UnicodeCategory.OtherPunctuation;
            if (val >= 0x00A2 && val <= 0x00A5) return UnicodeCategory.CurrencySymbol;
            if (val == 0x00A6 || val == 0x00A7) return UnicodeCategory.OtherSymbol;
            if (val == 0x00A8) return UnicodeCategory.ModifierSymbol;
            if (val == 0x00A9) return UnicodeCategory.OtherSymbol;
            if (val == 0x00AA) return UnicodeCategory.LowercaseLetter;
            if (val == 0x00AB) return UnicodeCategory.InitialQuotePunctuation;
            if (val == 0x00AC) return UnicodeCategory.MathSymbol;
            if (val == 0x00AD) return UnicodeCategory.DashPunctuation;
            if (val == 0x00AE) return UnicodeCategory.OtherSymbol;
            if (val == 0x00AF) return UnicodeCategory.ModifierSymbol;
            if (val == 0x00B0) return UnicodeCategory.OtherSymbol;
            if (val == 0x00B1) return UnicodeCategory.MathSymbol;
            if (val == 0x00B2 || val == 0x00B3) return UnicodeCategory.OtherNumber;
            if (val == 0x00B4) return UnicodeCategory.ModifierSymbol;
            if (val == 0x00B5) return UnicodeCategory.LowercaseLetter;
            if (val == 0x00B6) return UnicodeCategory.OtherSymbol;
            if (val == 0x00B7) return UnicodeCategory.OtherPunctuation;
            if (val == 0x00B8) return UnicodeCategory.ModifierSymbol;
            if (val == 0x00B9) return UnicodeCategory.OtherNumber;
            if (val == 0x00BA) return UnicodeCategory.LowercaseLetter;
            if (val == 0x00BB) return UnicodeCategory.FinalQuotePunctuation;
            if (val >= 0x00BC && val <= 0x00BE) return UnicodeCategory.OtherNumber;
            if (val == 0x00BF) return UnicodeCategory.OtherPunctuation;
            if (val >= 0x00C0 && val <= 0x00D6) return UnicodeCategory.UppercaseLetter;
            if (val == 0x00D7) return UnicodeCategory.MathSymbol;
            if (val >= 0x00D8 && val <= 0x00DE) return UnicodeCategory.UppercaseLetter;
            if (val >= 0x00DF && val <= 0x00F6) return UnicodeCategory.LowercaseLetter;
            if (val == 0x00F7) return UnicodeCategory.MathSymbol;
            if (val >= 0x00F8 && val <= 0x00FF) return UnicodeCategory.LowercaseLetter;
            return UnicodeCategory.OtherNotAssigned;
        }

        public static UnicodeCategory GetUnicodeCategory(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return GetUnicodeCategory(str[index]);
        }

        public static bool IsWhiteSpace(char c)
        {
            // TODO: Make this use Array.BinarySearch() when implemented
            for (int i = 0; i < WhiteChars.Length; i++)
            {
                if (WhiteChars[i] == c)
                {
                    return true;
                }
            }
            return false;
        }

        public static bool IsWhiteSpace(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return IsWhiteSpace(str[index]);
        }

        public static bool IsLetter(char c) { return GetUnicodeCategory(c) <= UnicodeCategory.OtherLetter; }

        public static bool IsLetter(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return IsLetter(str[index]);
        }

        public static bool IsDigit(char c) { return GetUnicodeCategory(c) == UnicodeCategory.DecimalDigitNumber; }

        public static bool IsDigit(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return IsDigit(str[index]);
        }

        public static bool IsLower(char c) { return GetUnicodeCategory(c) == UnicodeCategory.LowercaseLetter; }

        public static bool IsLower(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return IsLower(str[index]);
        }

        public static bool IsUpper(char c) { return GetUnicodeCategory(c) == UnicodeCategory.UppercaseLetter; }

        public static bool IsUpper(string str, int index)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }
            if (index < 0 || index >= str.Length)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return IsUpper(str[index]);
        }

        // TODO: This should use CultureInfo.InvariantCulture.TextInfo
        public static char ToLowerInvariant(char c)
        {
            if (c >= 'A' && c <= 'Z')
            {
                c = (char)(((ushort)c) + 32);
            }
            return c;
        }

        public static char ToLower(char c) { return ToLower(c, CultureInfo.CurrentCulture); }

        public static char ToLower(char c, CultureInfo culture)
        {
            if (culture == null)
            {
                throw new ArgumentNullException("culture");
            }
            if (culture.LCID == 0x7f)
            {
                // Invariant culture
                return ToLowerInvariant(c);
            }
            return '?';
            //return culture.TextInfo.ToUpper(c);
        }

        // TODO: This should use CultureInfo.InvariantCulture.TextInfo
        public static char ToUpperInvariant(char c)
        {
            if (c >= 'a' && c <= 'z')
            {
                c = (char)(((ushort)c) - 32);
            }
            return c;
        }

        public static char ToUpper(char c) { return ToUpper(c, CultureInfo.CurrentCulture); }

        public static char ToUpper(char c, CultureInfo culture)
        {
            if (culture == null)
            {
                throw new ArgumentNullException("culture");
            }
            if (culture.LCID == 0x7f)
            {
                // Invariant culture
                return ToUpperInvariant(c);
            }
            return '?';
            //return culture.TextInfo.ToUpper(c);
        }

        public int CompareTo(object obj)
        {
            if (obj == null)
            {
                return 1;
            }
            if (!(obj is char))
            {
                throw new ArgumentException();
            }
            return CompareTo((char)obj);
        }

        public int CompareTo(char value) { return (mValue > value) ? 1 : ((mValue < value) ? -1 : 0); }

        public bool Equals(char obj) { return mValue == obj; }
    }
}
