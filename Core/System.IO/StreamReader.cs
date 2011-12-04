#if !LOCALTEST

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace System.IO {
	public class StreamReader : TextReader {

		const int DefaultBufferSize = 1024;
		const int DefaultFileBufferSize = 4096;
		const int MinimumBufferSize = 128;

		//
		// The input buffer
		//
		byte[] input_buffer;

		//
		// The decoded buffer from the above input buffer
		//
		char[] decoded_buffer;

		//
		// Decoded bytes in decoded_buffer.
		//
		int decoded_count;

		//
		// Current position in the decoded_buffer
		//
		int pos;

		//
		// The buffer size that we are using
		//
		int buffer_size;

		int do_checks;

		Encoding encoding;
		Decoder decoder;

		Stream base_stream;
		bool mayBlock;
		StringBuilder line_builder;

		private class NullStreamReader : StreamReader {
			public override int Peek() {
				return -1;
			}

			public override int Read() {
				return -1;
			}

			public override int Read([In, Out] char[] buffer, int index, int count) {
				return 0;
			}

			public override string ReadLine() {
				return null;
			}

			public override string ReadToEnd() {
				return String.Empty;
			}

			public override Stream BaseStream {
				get { return Stream.Null; }
			}

			public override Encoding CurrentEncoding {
				get { return Encoding.Unicode; }
			}
		}

		public new static readonly StreamReader Null = (StreamReader)(new NullStreamReader());

		internal StreamReader() { }

		public StreamReader(Stream stream)
			: this(stream, Encoding.UTF8Unmarked, true, DefaultBufferSize) { }

		public StreamReader(Stream stream, bool detect_encoding_from_bytemarks)
			: this(stream, Encoding.UTF8Unmarked, detect_encoding_from_bytemarks, DefaultBufferSize) { }

		public StreamReader(Stream stream, Encoding encoding)
			: this(stream, encoding, true, DefaultBufferSize) { }

		public StreamReader(Stream stream, Encoding encoding, bool detect_encoding_from_bytemarks)
			: this(stream, encoding, detect_encoding_from_bytemarks, DefaultBufferSize) { }

		public StreamReader(Stream stream, Encoding encoding, bool detect_encoding_from_bytemarks, int buffer_size) {
			Initialize(stream, encoding, detect_encoding_from_bytemarks, buffer_size);
		}

		public StreamReader(string path)
			: this(path, Encoding.UTF8Unmarked, true, DefaultFileBufferSize) { }

		public StreamReader(string path, bool detect_encoding_from_bytemarks)
			: this(path, Encoding.UTF8Unmarked, detect_encoding_from_bytemarks, DefaultFileBufferSize) { }

		public StreamReader(string path, Encoding encoding)
			: this(path, encoding, true, DefaultFileBufferSize) { }

		public StreamReader(string path, Encoding encoding, bool detect_encoding_from_bytemarks)
			: this(path, encoding, detect_encoding_from_bytemarks, DefaultFileBufferSize) { }

		public StreamReader(string path, Encoding encoding, bool detect_encoding_from_bytemarks, int buffer_size) {
			if (null == path) {
				throw new ArgumentNullException("path");
			}
			if (String.Empty == path) {
				throw new ArgumentException("Empty path not allowed");
			}
			if (path.IndexOfAny(Path.InvalidPathChars) != -1) {
				throw new ArgumentException("path contains invalid characters");
			}
			if (null == encoding) {
				throw new ArgumentNullException("encoding");
			}
			if (buffer_size <= 0) {
				throw new ArgumentOutOfRangeException("buffer_size", "The minimum size of the buffer must be positive");
			}

			Stream stream = File.OpenRead(path);
			Initialize(stream, encoding, detect_encoding_from_bytemarks, buffer_size);
		}

		internal void Initialize(Stream stream, Encoding encoding, bool detect_encoding_from_bytemarks, int buffer_size) {
			if (null == stream) {
				throw new ArgumentNullException("stream");
			}
			if (null == encoding) {
				throw new ArgumentNullException("encoding");
			}
			if (!stream.CanRead) {
				throw new ArgumentException("Cannot read stream");
			}
			if (buffer_size <= 0) {
				throw new ArgumentOutOfRangeException("buffer_size", "The minimum size of the buffer must be positive");
			}

			if (buffer_size < MinimumBufferSize) {
				buffer_size = MinimumBufferSize;
			}

			base_stream = stream;
			input_buffer = new byte[buffer_size];
			this.buffer_size = buffer_size;
			this.encoding = encoding;
			decoder = encoding.GetDecoder();

			byte[] preamble = encoding.GetPreamble();
			do_checks = detect_encoding_from_bytemarks ? 1 : 0;
			do_checks += (preamble.Length == 0) ? 0 : 2;

			decoded_buffer = new char[encoding.GetMaxCharCount(buffer_size)];
			decoded_count = 0;
			pos = 0;
		}

		public virtual Stream BaseStream {
			get {
				return base_stream;
			}
		}

		public virtual Encoding CurrentEncoding {
			get {
				if (encoding == null) {
					throw new Exception();
				}
				return encoding;
			}
		}


		public bool EndOfStream {
			get { return Peek() < 0; }
		}

		public override void Close() {
			Dispose(true);
		}

		protected override void Dispose(bool disposing) {
			if (disposing && base_stream != null) {
				base_stream.Close();
			}
			input_buffer = null;
			decoded_buffer = null;
			encoding = null;
			decoder = null;
			base_stream = null;
			base.Dispose(disposing);
		}

		//
		// Provides auto-detection of the encoding, as well as skipping over
		// byte marks at the beginning of a stream.
		//
		int DoChecks(int count) {
			if ((do_checks & 2) == 2) {
				byte[] preamble = encoding.GetPreamble();
				int c = preamble.Length;
				if (count >= c) {
					int i;

					for (i = 0; i < c; i++) {
						if (input_buffer[i] != preamble[i]) {
							break;
						}
					}

					if (i == c) {
						return i;
					}
				}
			}

			if ((do_checks & 1) == 1) {
				if (count < 2) {
					return 0;
				}

				/*if (input_buffer[0] == 0xfe && input_buffer[1] == 0xff) {
					this.encoding = Encoding.BigEndianUnicode;
					return 2;
				}*/

				if (count < 3) {
					return 0;
				}

				if (input_buffer[0] == 0xef && input_buffer[1] == 0xbb && input_buffer[2] == 0xbf) {
					this.encoding = Encoding.UTF8Unmarked;
					return 3;
				}

				if (count < 4) {
					if (input_buffer[0] == 0xff && input_buffer[1] == 0xfe && input_buffer[2] != 0) {
						this.encoding = Encoding.Unicode;
						return 2;
					}
					return 0;
				}

				/*if (input_buffer[0] == 0 && input_buffer[1] == 0
					&& input_buffer[2] == 0xfe && input_buffer[3] == 0xff) {
					this.encoding = Encoding.BigEndianUTF32;
					return 4;
				}

				if (input_buffer[0] == 0xff && input_buffer[1] == 0xfe) {
					if (input_buffer[2] == 0 && input_buffer[3] == 0) {
						this.encoding = Encoding.UTF32;
						return 4;
					}

					this.encoding = Encoding.Unicode;
					return 2;
				}*/
			}

			return 0;
		}

		public void DiscardBufferedData() {
			pos = decoded_count = 0;
			mayBlock = false;
			// Discard internal state of the decoder too.
			decoder = encoding.GetDecoder();
		}

		// the buffer is empty, fill it again
		private int ReadBuffer() {
			pos = 0;
			int cbEncoded = 0;

			// keep looping until the decoder gives us some chars
			decoded_count = 0;
			int parse_start = 0;
			do {
				cbEncoded = base_stream.Read(input_buffer, 0, buffer_size);

				if (cbEncoded <= 0) {
					return 0;
				}

				mayBlock = (cbEncoded < buffer_size);
				if (do_checks > 0) {
					Encoding old = encoding;
					parse_start = DoChecks(cbEncoded);
					if (old != encoding) {
						decoder = encoding.GetDecoder();
					}
					do_checks = 0;
					cbEncoded -= parse_start;
				}

				decoded_count += decoder.GetChars(input_buffer, parse_start, cbEncoded, decoded_buffer, 0);
				parse_start = 0;
			} while (decoded_count == 0);

			return decoded_count;
		}

		public override int Peek() {
			if (base_stream == null) {
				throw new ObjectDisposedException("StreamReader", "Cannot read from a closed StreamReader");
			}
			if (pos >= decoded_count && (mayBlock || ReadBuffer() == 0)) {
				return -1;
			}

			return decoded_buffer[pos];
		}

		public override int Read() {
			if (base_stream == null) {
				throw new ObjectDisposedException("StreamReader", "Cannot read from a closed StreamReader");
			}
			if (pos >= decoded_count && ReadBuffer() == 0) {
				return -1;
			}

			return decoded_buffer[pos++];
		}

		public override int Read([In, Out] char[] dest_buffer, int index, int count) {
			if (base_stream == null) {
				throw new ObjectDisposedException("StreamReader", "Cannot read from a closed StreamReader");
			}
			if (dest_buffer == null) {
				throw new ArgumentNullException("dest_buffer");
			}
			if (index < 0) {
				throw new ArgumentOutOfRangeException("index", "< 0");
			}
			if (count < 0) {
				throw new ArgumentOutOfRangeException("count", "< 0");
			}
			// re-ordered to avoid possible integer overflow
			if (index > dest_buffer.Length - count) {
				throw new ArgumentException("index + count > dest_buffer.Length");
			}

			int chars_read = 0;
			while (count > 0) {
				if (pos >= decoded_count && ReadBuffer() == 0) {
					return chars_read > 0 ? chars_read : 0;
				}

				int cch = Math.Min(decoded_count - pos, count);
				Array.Copy(decoded_buffer, pos, dest_buffer, index, cch);
				pos += cch;
				index += cch;
				count -= cch;
				chars_read += cch;
				if (mayBlock) {
					break;
				}
			}
			return chars_read;
		}

		bool foundCR;
		int FindNextEOL() {
			char c = '\0';
			for (; pos < decoded_count; pos++) {
				c = decoded_buffer[pos];
				if (c == '\n') {
					pos++;
					int res = (foundCR) ? (pos - 2) : (pos - 1);
					if (res < 0) {
						res = 0; // if a new buffer starts with a \n and there was a \r at
					}
					// the end of the previous one, we get here.
					foundCR = false;
					return res;
				} else if (foundCR) {
					foundCR = false;
					return pos - 1;
				}

				foundCR = (c == '\r');
			}

			return -1;
		}

		public override string ReadLine() {
			if (base_stream == null) {
				throw new ObjectDisposedException("StreamReader", "Cannot read from a closed StreamReader");
			}

			if (pos >= decoded_count && ReadBuffer() == 0) {
				return null;
			}

			int begin = pos;
			int end = FindNextEOL();
			if (end < decoded_count && end >= begin) {
				return new string(decoded_buffer, begin, end - begin);
			}

			if (line_builder == null) {
				line_builder = new StringBuilder();
			} else {
				line_builder.Length = 0;
			}

			while (true) {
				if (foundCR) { // don't include the trailing CR if present
					decoded_count--;
				}

				line_builder.Append(decoded_buffer, begin, decoded_count - begin);
				if (ReadBuffer() == 0) {
					if (line_builder.Capacity > 32768) {
						StringBuilder sb = line_builder;
						line_builder = null;
						return sb.ToString(0, sb.Length);
					}
					return line_builder.ToString(0, line_builder.Length);
				}

				begin = pos;
				end = FindNextEOL();
				if (end < decoded_count && end >= begin) {
					line_builder.Append(decoded_buffer, begin, end - begin);
					if (line_builder.Capacity > 32768) {
						StringBuilder sb = line_builder;
						line_builder = null;
						return sb.ToString(0, sb.Length);
					}
					return line_builder.ToString(0, line_builder.Length);
				}
			}
		}

		public override string ReadToEnd() {
			if (base_stream == null) {
				throw new ObjectDisposedException("StreamReader", "Cannot read from a closed StreamReader");
			}

			StringBuilder text = new StringBuilder();

			int size = decoded_buffer.Length;
			char[] buffer = new char[size];
			int len;

			while ((len = Read(buffer, 0, size)) > 0) {
				text.Append(buffer, 0, len);
			}

			return text.ToString();
		}
	}
}

#endif
