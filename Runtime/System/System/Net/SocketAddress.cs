// Copyright (c) 2009 DotNetAnywhere
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;

namespace System.Net {
	public class SocketAddress {
		private byte[] data;

		public SocketAddress(AddressFamily family, int size) {
			if (size < 2) {
				throw new ArgumentOutOfRangeException("size is too small");
			}

			data = new byte[size];
			data[0] = (byte)family;
			data[1] = (byte)((int)family >> 8);
		}

		public SocketAddress(AddressFamily family) : this(family, 32) { }

		public AddressFamily Family {
			get {
				return (AddressFamily)(data[0] + (data[1] << 8));
			}
		}

		public int Size {
			get {
				return data.Length;
			}
		}

		public byte this[int offset] {
			get {
				return (data[offset]);
			}

			set {
				data[offset] = value;
			}
		}

		public override string ToString() {
			string af = ((AddressFamily)data[0]).ToString();
			int size = data.Length;
			string ret = af + ":" + size + ":{";

			for (int i = 2; i < size; i++) {
				int val = (int)data[i];
				ret = ret + val;
				if (i < size - 1) {
					ret = ret + ",";
				}
			}

			ret = ret + "}";

			return (ret);
		}

		public override bool Equals(object obj) {
			if (obj is System.Net.SocketAddress &&
				((System.Net.SocketAddress)obj).data.Length == data.Length) {
				byte[] otherData = ((System.Net.SocketAddress)obj).data;
				for (int i = 0; i < data.Length; i++)
					if (otherData[i] != data[i])
						return false;

				return true;
			}

			return false;
		}

		public override int GetHashCode() {
			int code = 0;

			for (int i = 0; i < data.Length; i++)
				code += data[i] + i;

			return code;
		}
	}
}
