package gnu.bytecode;

public interface Filter
{
  /** Returns true if parameter is selected by this filter. */
  public boolean select(Object value);
}
