package gnu.xml.xpath;

import org.w3c.dom.Node;

class NegativeExpr
extends Expr
{

  final Expr expr;

  NegativeExpr (Expr expr)
  {
    this.expr = expr;
  }

  public Object evaluate (Node context)
  {
    Object val = expr.evaluate (context);
    double n = _number (context, val);
    return new Double (-n);
  }

  public String toString ()
  {
    return "-" + expr;
  }
  
}
