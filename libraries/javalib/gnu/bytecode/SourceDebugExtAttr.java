// Copyright (c) 2004  Per M.A. Bothner.
// This is free software;  for terms and warranty disclaimer see ./COPYING.

package gnu.bytecode;

/**
  * Represents the contents of a JSR-45 "SourceDebugExtension" attribute.
  * We only support generating a single "stratum".
  */

public class SourceDebugExtAttr  extends Attribute
{
  byte[] data;
  int dlength;

  private String outputFileName;
  private String defaultStratumId;

  /** Number of files in the <code>FileSection</code>.  The active (used)
   * length of the <code>fileNames</code> and <code>fileIDs</code> arrays. */
  int fileCount;
  /** For each entry in the <code>FileSection</code>:
   * <code>2*id+with_path</code>.
   * The <code>id</code> is the <code>FileID</code> written in the SMAP;
   * the <code>with_path</code> is 1 if we also write the full path. */
  int[] fileIDs;
  /** For eachentry its <code>FileName</code>.
   * If the <code>with_path</code> bit is set,
   * <code>FileName '\n' AbsoluteFileName</code>. */
  String[] fileNames;

  int lineCount;
  /** A table of LineInfo entries. */
  int[] lines;

  //int curMinLine;  int curMaxLine;  int curLineAdjust;
  int curLineIndex = -1;
  int curFileIndex = -1;
  int maxFileID;
  String curFileName;

  private int fixLine (int sourceLine, int index)
  {
    int sourceMin = lines[index];
    int repeat = lines[index+2];
    if (sourceLine < sourceMin)
      {
	if (index > 0)
	  return -1;
	int sourceMax = sourceMin + repeat - 1;
	lines[index] = sourceLine;
	lines[index+2] = sourceMax - sourceLine + 1;
	lines[index+3] = sourceLine;
	sourceMin = sourceLine;
      }
    int delta = lines[index+3] - sourceMin;
    if (sourceLine < sourceMin + repeat)
      return sourceLine + delta;
    else if (index == 5 * (lineCount - 1)
	     || (index == 0 && sourceLine < lines[5+3]))
      { // If last LineInfo entry, we can "extend" its range.
	lines[index+2] = sourceLine - sourceMin + 1; // Fix RepeatCount.
	return sourceLine + delta;
      }
    return -1;
  }

  int fixLine (int sourceLine)
  {
    int outLine;
    if (curLineIndex >= 0)
      {
	outLine = fixLine(sourceLine, curLineIndex);
	if (outLine >= 0)
	  return outLine;
      }
    int i5 = 0;
    int findex = curFileIndex;
    for (int i = 0;  i < lineCount;  i++)
      {
	if (i5 != curLineIndex && findex == lines[i5+1])
	  {
	    outLine = fixLine(sourceLine, i5);
	    if (outLine >= 0)
	      {
		curLineIndex = i5;
		return outLine;
	      }
	  }
	i5 += 5;
      }
    if (lines == null)
      lines = new int[20];
    else if (i5 >= lines.length)
      {
	int[] newLines = new int[2 * i5];
	System.arraycopy(lines, 0, newLines, 0, i5);
	lines = newLines;
      }
    int outputStartLine;
    int inputStartLine = sourceLine;
    if (i5 == 0)
      outputStartLine = sourceLine;
    else
      {
	outputStartLine = lines[i5-5+3] + lines[i5-5+2];
	if (i5 == 5 && outputStartLine < 10000)
	  {
	    // Reserve the first 10k lines for the main file.
	    // This is for the sake of non-JSR-45-capable tools.
	    outputStartLine = 10000;
	  }
	sourceLine = outputStartLine;
      }
    lines[i5] = inputStartLine;
    lines[i5+1] = findex;
    lines[i5+2] = 1;
    lines[i5+3] = outputStartLine;
    lines[i5+4] = 1;
    curLineIndex = i5;
    lineCount++;
    return sourceLine;
  }

  void addFile (String fname)
  {
    if (curFileName == fname || (fname != null && fname.equals(curFileName)))
      return;
    curFileName = fname;
    fname = SourceFileAttr.fixSourceFile(fname);
    String fentry;
    int slash = fname.lastIndexOf('/');
    if (slash >= 0)
      {
	String fpath = fname;
	fname = fname.substring(slash+1);
	fentry = fname + '\n' + fpath;
      }
    else
      fentry = fname;

    if (curFileIndex >= 0 && fentry.equals(fileNames[curFileIndex]))
      return;
    
    int n = fileCount;
    for (int i = 0;  i < n;  i++)
      {
	if (i != curFileIndex && fentry.equals(fileNames[i]))
	  {
	    curFileIndex = i;
	    curLineIndex = -1;
	    return;
	  }
      }

    if (fileIDs == null)
      {
	fileIDs = new int[5];
	fileNames = new String[5];
      }
    else if (n >= fileIDs.length)
      {
	int[] newIDs = new int[2 * n];
	String[] newNames = new String[2 * n];
	System.arraycopy(fileIDs, 0, newIDs, 0, n);
	System.arraycopy(fileNames, 0, newNames, 0, n);
	fileIDs = newIDs;
	fileNames = newNames;
      }


    fileCount++;
    int id = ++maxFileID;
    id = id << 1;
    if (slash >= 0)
      id++;
    fileNames[n] = fentry;
    if (outputFileName == null)
      outputFileName = fname;
    fileIDs[n] = id;
    curFileIndex = n;
    curLineIndex = -1;
  }

  public void addStratum (String name)
  {
    defaultStratumId = name;
  }

  /** Add a new InnerClassesAttr to a ClassType. */
  public SourceDebugExtAttr (ClassType cl)
  {
    super("SourceDebugExtension");
    addToFrontOf(cl);
  }

  void nonAsteriskString(String str, StringBuffer sbuf)
  {
    if (str == null || str.length() == 0 || str.charAt(0) == '*')
      sbuf.append(' ');
    sbuf.append(str);
  }

  public void assignConstants (ClassType cl)
  {
    super.assignConstants(cl);

    StringBuffer sbuf = new StringBuffer();
    // Append SMAP Header:
    sbuf.append("SMAP\n");
    nonAsteriskString(outputFileName, sbuf); sbuf.append('\n');
    String stratum = defaultStratumId == null ? "Java" : defaultStratumId;
    nonAsteriskString(stratum, sbuf);
    sbuf.append('\n');
    // Append StratumSection.
    sbuf.append("*S ");
    sbuf.append(stratum);
    sbuf.append('\n');
    // Append FileSection:
    sbuf.append("*F\n");
    for (int i = 0;  i < fileCount;  i++)
      {
	int id = fileIDs[i];
	boolean with_path = (id & 1) != 0;
	id >>= 1;
	if (with_path)
	  sbuf.append("+ ");
	sbuf.append(id);  sbuf.append(' ');
	sbuf.append(fileNames[i]);  sbuf.append('\n');
      }
    // Append LineSection:
    if (lineCount > 0)
      {
	int prevFileID = 0;
	sbuf.append("*L\n");
	int i = 0, i5 = 0;
	do
	  {
	    int inputStartLine = lines[i5];
	    int lineFileID = fileIDs[lines[i5+1]] >> 1;
	    int repeatCount = lines[i5+2];
	    int outputStartLine = lines[i5+3];
	    int outputLineIncrement = lines[i5+4];
	    sbuf.append(inputStartLine);
	    if (lineFileID != prevFileID)
	      {
		sbuf.append('#');
		sbuf.append(lineFileID);
		prevFileID = lineFileID;
	      }
	    if (repeatCount != 1)
	      {
		sbuf.append(',');
		sbuf.append(repeatCount);
	      }
	    sbuf.append(':');
	    sbuf.append(outputStartLine);
	    if (outputLineIncrement != 1)
	      {
		sbuf.append(',');
		sbuf.append(outputLineIncrement);
	      }
	    sbuf.append('\n');
	    i5 += 5;
	  }
	while (++i < lineCount);
      }
    // Append EndSection:
    sbuf.append("*E\n");
    try
      {
	data = sbuf.toString().getBytes("UTF-8");
      }
    catch (Exception ex)
      {
	throw new RuntimeException(ex.toString());
      }
    dlength = data.length;
  }

  /** Return the length of the attribute in bytes.
    * Does not include the 6-byte header (for the name_index and the length).*/
    public int getLength() { return dlength; }

  /** Write out the contents of the Attribute.
    * Does not write the 6-byte attribute header. */
  public void write (java.io.DataOutputStream dstr)
    throws java.io.IOException
  {
    dstr.write(data, 0, dlength);
  }

  public void print (ClassTypeWriter dst) 
  {
    ClassType ctype = (ClassType) container;
    ConstantPool constants = ctype.getConstants();
    dst.print("Attribute \"");
    dst.print(getName());
    dst.print("\", length:");
    dst.println(dlength);
    try
      {
	dst.print(new String(data, 0, dlength, "UTF-8"));
      }
    catch (Exception ex)
      {
	dst.print("(Caught ");  dst.print(ex);  dst.println(')');
      }
    if (dlength > 0 && data[dlength-1] != '\r' && data[dlength-1]!= '\n')
      dst.println();
  }
}
