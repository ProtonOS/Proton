using System.Runtime.InteropServices;
using System.Threading;

namespace System.IO
{
	public abstract class Stream : MarshalByRefObject, IDisposable
	{
		public static readonly Stream Null = new NullStream();

		protected Stream() { }

		public abstract bool CanRead { get; }

		public abstract bool CanSeek { get; }

		public abstract bool CanWrite { get; }

		public virtual bool CanTimeout { get { return false; } }

		public abstract long Length { get; }

		public abstract long Position { get; set; }


		public void Dispose() { Close(); }

		protected virtual void Dispose(bool disposing) { }

		public virtual void Close() { Dispose(true); }

		public virtual int ReadTimeout
		{
			get { throw new InvalidOperationException("Timeouts are not supported on this stream."); }
			set { throw new InvalidOperationException("Timeouts are not supported on this stream."); }
		}

		public virtual int WriteTimeout
		{
			get { throw new InvalidOperationException("Timeouts are not supported on this stream."); }
			set { throw new InvalidOperationException("Timeouts are not supported on this stream."); }
		}

		//public static Stream Synchronized(Stream stream) { return new SynchronizedStream(stream); }

		//protected virtual WaitHandle CreateWaitHandle() { return new ManualResetEvent(false); }

		public abstract void Flush();

		public abstract int Read([In, Out] byte[] buffer, int offset, int count);

		public virtual int ReadByte()
		{
			byte[] buffer = new byte[1];
			if (Read(buffer, 0, 1) == 1) return buffer[0];
			return -1;
		}

		public abstract long Seek(long offset, SeekOrigin origin);

		public abstract void SetLength(long value);

		public abstract void Write(byte[] buffer, int offset, int count);

		public virtual void WriteByte(byte value)
		{
			byte[] buffer = new byte[1];
			buffer[0] = value;
			Write(buffer, 0, 1);
		}

		public void CopyTo (Stream destination) { CopyTo (destination, 16*1024); }

		public void CopyTo (Stream destination, int bufferSize)
		{
			if (destination == null) throw new ArgumentNullException ("destination");
			if (!CanRead) throw new NotSupportedException ("This stream does not support reading");
			if (!destination.CanWrite) throw new NotSupportedException ("This destination stream does not support writing");
			if (bufferSize <= 0) throw new ArgumentOutOfRangeException ("bufferSize");

			var buffer = new byte [bufferSize];
			int nread;
			while ((nread = Read (buffer, 0, bufferSize)) != 0) destination.Write (buffer, 0, nread);
		}

		protected virtual void ObjectInvariant() { }
	}

	class NullStream : Stream
	{
		public override bool CanRead { get { return true; } }

		public override bool CanSeek { get { return true; } }

		public override bool CanWrite { get { return true; } }

		public override long Length { get { return 0; } }

		public override long Position { get { return 0; } set { } }

		public override void Flush() { }

		public override int Read(byte[] buffer, int offset, int count) { return 0; }

		public override int ReadByte() { return -1; }

		public override long Seek(long offset, SeekOrigin origin) { return 0; }

		public override void SetLength(long value) { }

		public override void Write(byte[] buffer, int offset, int count) { }

		public override void WriteByte(byte value) { }
	}

	//class SynchronizedStream : Stream
	//{
	//    Stream source;
	//    object slock;

	//    internal SynchronizedStream(Stream source)
	//    {
	//        this.source = source;
	//        slock = new object();
	//    }

	//    public override bool CanRead { get { lock (slock) return source.CanRead; } } 

	//    public override bool CanSeek { get { lock (slock) return source.CanSeek; } }

	//    public override bool CanWrite { get { lock (slock) return source.CanWrite; } }

	//    public override long Length { get { lock (slock) return source.Length; } }

	//    public override long Position { get { lock (slock) return source.Position; } set { lock (slock) source.Position = value; } }

	//    public override void Flush() { lock (slock) source.Flush(); }

	//    public override int Read(byte[] buffer, int offset, int count) { lock (slock) return source.Read(buffer, offset, count); }

	//    public override int ReadByte() { lock (slock) return source.ReadByte(); }

	//    public override long Seek(long offset, SeekOrigin origin) { lock (slock) return source.Seek(offset, origin); }

	//    public override void SetLength(long value) { lock (slock) source.SetLength(value); }

	//    public override void Write(byte[] buffer, int offset, int count) { lock (slock) source.Write(buffer, offset, count); }

	//    public override void WriteByte(byte value) { lock (slock) source.WriteByte(value); }
	//}
}
