#if !LOCALTEST

using System;
using System.Runtime.InteropServices;

namespace System.IO {

	public abstract class TextReader : MarshalByRefObject, IDisposable {

		protected TextReader() { }

		public static readonly TextReader Null;

		public virtual void Close() {
			Dispose(true);
		}

		public void Dispose ()
		{
			Dispose(true);
		}

		protected virtual void Dispose(bool disposing) {
			return;
		}

		public virtual int Peek() {
			return -1;
		}

		public virtual int Read() {
			return -1;
		}


		public virtual int Read([In, Out] char[] buffer, int index, int count) {
			int c, i;

			for (i = 0; i < count; i++) {
				if ((c = Read()) == -1)
					return i;
				buffer[index + i] = (char)c;
			}

			return i;
		}

		public virtual int ReadBlock([In, Out] char[] buffer, int index, int count) {
			int total_read_count = 0;
			int current_read_count = 0;

			do {
				current_read_count = Read(buffer, index, count);
				index += current_read_count;
				total_read_count += current_read_count;
				count -= current_read_count;
			} while (current_read_count != 0 && count > 0);

			return total_read_count;
		}

		public virtual string ReadLine() {
			return String.Empty;
		}

		public virtual string ReadToEnd() {
			return String.Empty;
		}

		public static TextReader Synchronized(TextReader reader) {
			if (reader == null)
				throw new ArgumentNullException("reader is null");
			if (reader is SynchronizedReader)
				return reader;

			return new SynchronizedReader(reader);
		}
	}

	//
	// Synchronized Reader implementation, used internally.
	//
	internal class SynchronizedReader : TextReader {
		TextReader reader;

		public SynchronizedReader(TextReader reader) {
			this.reader = reader;
		}

		public override void Close() {
			lock (this) {
				reader.Close();
			}
		}

		public override int Peek() {
			lock (this) {
				return reader.Peek();
			}
		}

		public override int ReadBlock(char[] buffer, int index, int count) {
			lock (this) {
				return reader.ReadBlock(buffer, index, count);
			}
		}

		public override string ReadLine() {
			lock (this) {
				return reader.ReadLine();
			}
		}

		public override string ReadToEnd() {
			lock (this) {
				return reader.ReadToEnd();
			}
		}

		public override int Read() {
			lock (this) {
				return reader.Read();
			}
		}

		public override int Read(char[] buffer, int index, int count) {
			lock (this) {
				return reader.Read(buffer, index, count);
			}
		}

	}
}

#endif