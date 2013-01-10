using System.Runtime.InteropServices;

namespace System.IO
{
	public class MemoryStream : Stream
	{
		bool canWrite;
		bool allowGetBuffer;
		int capacity;
		int length;
		byte[] internalBuffer;
		int initialIndex;
		bool expandable;
		bool streamClosed;
		int position;
		int dirty_bytes;

		public MemoryStream() : this(0) { }

		public MemoryStream(int capacity)
		{
			if (capacity < 0) throw new ArgumentOutOfRangeException("capacity");

			canWrite = true;

			this.capacity = capacity;
			internalBuffer = new byte[capacity];

			expandable = true;
			allowGetBuffer = true;
		}

		public MemoryStream(byte[] buffer)
		{
			if (buffer == null) throw new ArgumentNullException("buffer");

			InternalConstructor(buffer, 0, buffer.Length, true, false);
		}

		public MemoryStream(byte[] buffer, bool writable)
		{
			if (buffer == null) throw new ArgumentNullException("buffer");

			InternalConstructor(buffer, 0, buffer.Length, writable, false);
		}

		public MemoryStream(byte[] buffer, int index, int count) { InternalConstructor(buffer, index, count, true, false); }

		public MemoryStream(byte[] buffer, int index, int count, bool writable) { InternalConstructor(buffer, index, count, writable, false); }

		public MemoryStream(byte[] buffer, int index, int count, bool writable, bool publiclyVisible) { InternalConstructor(buffer, index, count, writable, publiclyVisible); }

		void InternalConstructor(byte[] buffer, int index, int count, bool writable, bool publicallyVisible)
		{
			if (buffer == null) throw new ArgumentNullException("buffer");

			if (index < 0 || count < 0) throw new ArgumentOutOfRangeException("index or count is less than 0.");

			if (buffer.Length - index < count) throw new ArgumentException("index+count", "The size of the buffer is less than index + count.");

			canWrite = writable;

			internalBuffer = buffer;
			capacity = count + index;
			length = capacity;
			position = index;
			initialIndex = index;

			allowGetBuffer = publicallyVisible;
			expandable = false;
		}

		void CheckIfClosedThrowDisposed() { if (streamClosed) throw new ObjectDisposedException("MemoryStream"); }

		public override bool CanRead { get { return !streamClosed; } }

		public override bool CanSeek { get { return !streamClosed; } }

		public override bool CanWrite { get { return (!streamClosed && canWrite); } }

		public virtual int Capacity
		{
			get
			{
				CheckIfClosedThrowDisposed();
				return capacity - initialIndex;
			}
			set
			{
				CheckIfClosedThrowDisposed();

				if (!expandable) throw new NotSupportedException("Cannot expand this MemoryStream");

				if (value < 0 || value < length) throw new ArgumentOutOfRangeException("value", "New capacity cannot be negative or less than the current capacity " + value + " " + capacity);

				if (internalBuffer != null && value == internalBuffer.Length) return;

				byte[] newBuffer = null;
				if (value != 0)
				{
					newBuffer = new byte[value];
					if (internalBuffer != null) Buffer.BlockCopy(internalBuffer, 0, newBuffer, 0, length);
				}

				dirty_bytes = 0; // discard any dirty area beyond previous length
				internalBuffer = newBuffer; // It's null when capacity is set to 0
				capacity = value;
			}
		}

		public override long Length
		{
			get
			{
				CheckIfClosedThrowDisposed();
				return length - initialIndex;
			}
		}

		public override long Position
		{
			get
			{
				CheckIfClosedThrowDisposed();
				return position - initialIndex;
			}
			set
			{
				CheckIfClosedThrowDisposed();
				if (value < 0) throw new ArgumentOutOfRangeException("value", "Position cannot be negative");

				if (value > Int32.MaxValue) throw new ArgumentOutOfRangeException("value", "Position must be non-negative and less than 2^31 - 1 - origin");

				position = initialIndex + (int)value;
			}
		}

		protected override void Dispose(bool disposing)
		{
			streamClosed = true;
			expandable = false;
		}

		public override void Flush()
		{
			// Do nothing
		}

		public virtual byte[] GetBuffer()
		{
			if (!allowGetBuffer) throw new UnauthorizedAccessException();

			return internalBuffer;
		}

		public override int Read([In, Out] byte[] buffer, int offset, int count)
		{
			if (buffer == null)
				throw new ArgumentNullException("buffer");

			if (offset < 0 || count < 0) throw new ArgumentOutOfRangeException("offset or count less than zero.");

			if (buffer.Length - offset < count) throw new ArgumentException("offset+count", "The size of the buffer is less than offset + count.");

			CheckIfClosedThrowDisposed();

			if (position >= length || count == 0) return 0;

			if (position > length - count) count = length - position;

			Buffer.BlockCopy(internalBuffer, position, buffer, offset, count);
			position += count;
			return count;
		}

		public override int ReadByte()
		{
			CheckIfClosedThrowDisposed();
			if (position >= length) return -1;
			return internalBuffer[position++];
		}

		public override long Seek(long offset, SeekOrigin loc)
		{
			CheckIfClosedThrowDisposed();

			if (offset > (long)Int32.MaxValue) throw new ArgumentOutOfRangeException("Offset out of range. " + offset);

			int refPoint;
			switch (loc)
			{
				case SeekOrigin.Begin:
					if (offset < 0) throw new IOException("Attempted to seek before start of MemoryStream.");
					refPoint = initialIndex;
					break;
				case SeekOrigin.Current:
					refPoint = position;
					break;
				case SeekOrigin.End:
					refPoint = length;
					break;
				default: throw new ArgumentException("loc", "Invalid SeekOrigin");
			}

			refPoint += (int)offset;
			if (refPoint < initialIndex) throw new IOException("Attempted to seek before start of MemoryStream.");

			position = refPoint;
			return position;
		}

		int CalculateNewCapacity(int minimum)
		{
			if (minimum < 256) minimum = 256;
			if (minimum < capacity * 2) minimum = capacity * 2;
			return minimum;
		}

		void Expand(int newSize)
		{
			if (newSize > capacity) Capacity = CalculateNewCapacity(newSize);
			else if (dirty_bytes > 0)
			{
				Array.Clear(internalBuffer, length, dirty_bytes);
				dirty_bytes = 0;
			}
		}

		public override void SetLength(long value)
		{
			if (!expandable && value > capacity) throw new NotSupportedException("Expanding this MemoryStream is not supported");

			CheckIfClosedThrowDisposed();

			if (!canWrite) throw new NotSupportedException("Cannot write to this MemoryStream");

			if (value < 0 || (value + initialIndex) > (long)Int32.MaxValue) throw new ArgumentOutOfRangeException();

			int newSize = (int)value + initialIndex;

			if (newSize > length) Expand(newSize);
			else if (newSize < length) dirty_bytes += length - newSize;

			length = newSize;
			if (position > length) position = length;
		}

		public virtual byte[] ToArray()
		{
			int l = length - initialIndex;
			byte[] outBuffer = new byte[l];

			if (internalBuffer != null) Buffer.BlockCopy(internalBuffer, initialIndex, outBuffer, 0, l);
			return outBuffer;
		}

		public override void Write(byte[] buffer, int offset, int count)
		{
			if (buffer == null) throw new ArgumentNullException("buffer");

			if (offset < 0 || count < 0) throw new ArgumentOutOfRangeException();

			if (buffer.Length - offset < count) throw new ArgumentException("offset+count", "The size of the buffer is less than offset + count.");

			CheckIfClosedThrowDisposed();

			if (!CanWrite) throw new NotSupportedException("Cannot write to this stream.");

			if (position > length - count) Expand(position + count);

			Buffer.BlockCopy(buffer, offset, internalBuffer, position, count);
			position += count;
			if (position >= length) length = position;
		}

		public override void WriteByte(byte value)
		{
			CheckIfClosedThrowDisposed();
			if (!canWrite) throw new NotSupportedException("Cannot write to this stream.");

			if (position >= length)
			{
				Expand(position + 1);
				length = position + 1;
			}

			internalBuffer[position++] = value;
		}

		public virtual void WriteTo(Stream stream)
		{
			CheckIfClosedThrowDisposed();

			if (stream == null) throw new ArgumentNullException("stream");

			stream.Write(internalBuffer, initialIndex, length - initialIndex);
		}
	}
}
