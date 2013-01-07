using System.Text;

namespace System.Globalization
{
    public class TextInfo
    {
        internal static TextInfo InvariantInfo { get { return new TextInfo(CultureInfo.InvariantCulture); } }

        private int mANSICodePage;
        private int mEBCDICCodePage;
        private bool mIsRightToLeft;
        private string mListSeparator;
        private int mMacCodePage;
        private int mOEMCodePage;
        private CultureInfo mCultureInfo;

        internal TextInfo(CultureInfo cultureInfo)
        {
            mANSICodePage = 1252;
            mEBCDICCodePage = 37;
            mIsRightToLeft = false;
            mListSeparator = ",";
            mMacCodePage = 10000;
            mOEMCodePage = 437;
            mCultureInfo = cultureInfo;
        }

        public virtual int ANSICodePage
        {
            get
            {
                return this.mANSICodePage;
            }
        }

        public string CultureName
        {
            get
            {
                return this.mCultureInfo.Name;
            }
        }

        public int EBCDICCodePage
        {
            get
            {
                return this.mEBCDICCodePage;
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return true;
            }
        }

        public bool IsRightToLeft
        {
            get
            {
                return this.mIsRightToLeft;
            }
        }

        public int LCID
        {
            get
            {
                return this.mCultureInfo.LCID;
            }
        }

        public string ListSeparator
        {
            get
            {
                return this.mListSeparator;
            }
        }

        public int MacCodePage
        {
            get
            {
                return this.mMacCodePage;
            }
        }

        public int OEMCodePage
        {
            get
            {
                return this.mOEMCodePage;
            }
        }

        public char ToLower(char c)
        {
            // For the moment, ignore culture
            return char.ToLowerInvariant(c);
        }

        public string ToLower(string s)
        {
            // For the moment, ignore culture
            return s.ToLowerInvariant();
        }

        public char ToUpper(char c)
        {
            // For the moment, ignore culture
            return char.ToUpperInvariant(c);
        }

        public string ToUpper(string s)
        {
            // For the moment, ignore culture
            return s.ToUpperInvariant();
        }

        public string ToTitleCase(string str)
        {
            if (str == null)
            {
                throw new ArgumentNullException("str");
            }

            StringBuilder sb = null;
            int i = 0;
            int start = 0;
            while (i < str.Length)
            {
                if (!char.IsLetter(str[i++]))
                {
                    continue;
                }
                i--;
                char t = ToUpper(str[i]);
                bool capitalize = true;
                if (t == str[i])
                {
                    capitalize = false;
                    bool allTitle = true;
                    // if the word is all titlecase,
                    // then don't capitalize it.
                    int saved = i;
                    while (++i < str.Length)
                    {
                        if (char.IsWhiteSpace(str[i]))
                        {
                            break;
                        }
                        t = ToUpper(str[i]);
                        if (t != str[i])
                        {
                            allTitle = false;
                            break;
                        }
                    }
                    if (allTitle)
                    {
                        continue;
                    }
                    i = saved;

                    // still check if all remaining
                    // characters are lowercase,
                    // where we don't have to modify
                    // the source word.
                    while (++i < str.Length)
                    {
                        if (char.IsWhiteSpace(str[i]))
                        {
                            break;
                        }
                        if (ToLower(str[i]) != str[i])
                        {
                            capitalize = true;
                            i = saved;
                            break;
                        }
                    }
                }

                if (capitalize)
                {
                    if (sb == null)
                    {
                        sb = new StringBuilder(str.Length);
                    }
                    sb.Append(str, start, i - start);
                    sb.Append(ToUpper(str[i]));
                    start = i + 1;
                    while (++i < str.Length)
                    {
                        if (char.IsWhiteSpace(str[i]))
                        {
                            break;
                        }
                        sb.Append(ToLower(str[i]));
                    }
                    start = i;
                }
            }
            if (sb != null)
                sb.Append(str, start, str.Length - start);

            return sb != null ? sb.ToString() : str;
        }

    }
}
