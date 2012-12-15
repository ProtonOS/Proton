﻿using System;
using System.Collections.Generic;

namespace Proton.Nernel
{
	public class SomeOtherBase<T> { }

	public class SomeBase<T> : SomeOtherBase<T> { }

	public class SomeBase<T, T2> : SomeOtherBase<T> { }

	public class SomeClass1 : SomeBase<string> { }

	public class SomeClass2<T> : SomeBase<T> { }

	public class SomeClass3<T> : SomeBase<string, T> { }

	public class SomeClass4<T>
	{
		public T SomeField;

		public T SomeMethod() { return SomeField; }
	}

	public abstract class SomeClass5
	{
		public abstract M SomeMethod<M>();
	}


	public abstract class SomeClass6<T>
	{
		public abstract void SomeMethod<M>(T someParameter1, M someParameter2);
	}

	public class SomeClass7<T> : SomeClass6<T>
	{
		public override void SomeMethod<M>(T someParameter1, M someParameter2)
		{
			T someLocal1 = default(T);
			M someLocal2 = default(M);
		}
	}


	public class SomeClass<T> : SomeBase<T>
	{
		public T SomeField;

		public Converter<T, M> SomeMethod<M>(T someParameter1, M someParameter2)
		{
			T someLocal1 = default(T);
			M someLocal2 = default(M);
			T[] someArray1 = new T[10];
			M[] someArray2 = new M[10];
			Converter<T, M> someConverter = new Converter<T, M>(t => default(M));
			return someConverter;
		}
	}

	internal static class Program
	{
		private static void Main(string[] pCommandLine)
		{
			new SomeClass<SomeClass6<SomeClass5>>().SomeMethod(new SomeClass7<SomeClass5>(), new SomeClass4<int>());
		}
	}
}
