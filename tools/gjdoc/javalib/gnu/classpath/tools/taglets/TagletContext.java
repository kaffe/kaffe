package gnu.classpath.tools.taglets;

public class TagletContext
{
   public static final TagletContext OVERVIEW = new TagletContext("overview");
   public static final TagletContext PACKAGE = new TagletContext("package");
   public static final TagletContext TYPE = new TagletContext("type");
   public static final TagletContext CONSTRUCTOR = new TagletContext("constructor");
   public static final TagletContext METHOD = new TagletContext("method");
   public static final TagletContext FIELD = new TagletContext("field");

   private String name;

   private TagletContext(String name)
   {
      this.name = name;
   }

   public String getName()
   {
      return name;
   }

   public String toString()
   {
      return "TagletContext{name=" + name + "}";
   }
}
