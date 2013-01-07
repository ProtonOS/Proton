namespace System.Collections.Generic
{
    public interface IList<T> : ICollection<T>
    {
        int IndexOf(T item);

        void Insert(int index, T item);

        void RemoveAt(int index);

        T this[int index] { get; set; }
    }
}
