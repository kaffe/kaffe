/*
 * NNTPConnection.java
 * Copyright (C) 2002 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.nntp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Constructor;
import java.net.InetSocketAddress;
import java.net.ProtocolException;
import java.net.Socket;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.HashMap;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.security.auth.callback.CallbackHandler;
import javax.security.sasl.Sasl;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;

import gnu.inet.util.CRLFInputStream;
import gnu.inet.util.CRLFOutputStream;
import gnu.inet.util.LineInputStream;
import gnu.inet.util.MessageInputStream;
import gnu.inet.util.SaslCallbackHandler;
import gnu.inet.util.SaslInputStream;
import gnu.inet.util.SaslOutputStream;
import gnu.inet.util.TraceLevel;

/**
 * An NNTP client.
 * This object is used to establish and manage a connection to an NNTP
 * server.
 *
 * @author <a hef='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class NNTPConnection
  implements NNTPConstants
{

  /**
   * The logger used for NNTP protocol traces.
   */
  public static final Logger logger = Logger.getLogger("gnu.inet.nntp");

  /**
   * The network trace level.
   */
  public static final Level NNTP_TRACE = new TraceLevel("nntp");

  /**
   * The default NNTP port.
   */
  public static final int DEFAULT_PORT = 119;

  /**
   * The hostname of the host we are connected to.
   */
  protected String hostname;

  /**
   * The port on the host we are connected to.
   */
  protected int port;

  /**
   * The socket used for network communication.
   */
  protected Socket socket;

  /**
   * The socket input stream.
   */
  protected LineInputStream in;

  /**
   * The socket output stream.
   */
  protected CRLFOutputStream out;

  /**
   * Whether the host permits posting of articles.
   */
  protected boolean canPost;

  /**
   * The greeting issued by the host when we connected.
   */
  protected String welcome;

  /**
   * Pending data, if any.
   */
  protected PendingData pendingData;

  private static final String DOT = ".";
  private static final String US_ASCII = "US-ASCII";

  /**
   * Creates a new connection object.
   * @param hostname the hostname or IP address of the news server
   */
  public NNTPConnection(String hostname)
    throws IOException
  {
    this(hostname, DEFAULT_PORT, 0, 0);
  }

  /**
   * Creates a new connection object.
   * @param hostname the hostname or IP address of the news server
   * @param port the port to connect to
   */
  public NNTPConnection(String hostname, int port)
    throws IOException
  {
    this(hostname, port, 0, 0);
  }

  /**
   * Creates a new connection object.
   * @param hostname the hostname or IP address of the news server
   * @param port the port to connect to
   * @param connectionTimeout the socket connection timeout
   * @param timeout the read timeout on the socket
   */
  public NNTPConnection(String hostname, int port,
                        int connectionTimeout, int timeout)
    throws IOException
  {
    if (port < 0)
      {
        port = DEFAULT_PORT;
      }
    
    this.hostname = hostname;
    this.port = port;
    
    // Set up the socket and streams
    socket = new Socket();
    InetSocketAddress address = new InetSocketAddress(hostname, port);
    if (connectionTimeout > 0)
      {
        socket.connect(address, connectionTimeout);
      }
    else
      {
        socket.connect(address);
      }
    if (timeout > 0)
      {
        socket.setSoTimeout(timeout);
      }
    InputStream in = socket.getInputStream();
    in = new CRLFInputStream(in);
    this.in = new LineInputStream(in);
    OutputStream out = socket.getOutputStream();
    out = new BufferedOutputStream(out);
    this.out = new CRLFOutputStream(out);
    
    // Read the welcome message(RFC977:2.4.3)
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case POSTING_ALLOWED:
        canPost = true;
      case NO_POSTING_ALLOWED:
        welcome = response.getMessage();
        break;
      default:
        throw new NNTPException(response);
      }
  }
  
  /**
   * Return the welcome message sent by the server in reply to the initial
   * connection.
   * This message sometimes contains disclaimers or help information that
   * may be relevant to the user.
   */
  public String getWelcome()
  {
    return welcome;
  }
  
  /*
   * Returns an NNTP-format date string.
   * This is only required when clients use the NEWGROUPS or NEWNEWS
   * methods, therefore rarely: we don't cache any of the variables here.
   */
  String formatDate(Date date)
  {
    DateFormat df = new SimpleDateFormat("yyMMdd HHmmss 'GMT'");
    Calendar cal = new GregorianCalendar();
    TimeZone gmt = TimeZone.getTimeZone("GMT");
    cal.setTimeZone(gmt);
    df.setCalendar(cal);
    cal.setTime(date);
    return df.format(date);
  }

  /*
   * Parse the specfied NNTP date text.
   */
  Date parseDate(String text)
    throws ParseException
  {
    DateFormat df = new SimpleDateFormat("yyMMdd HHmmss 'GMT'");
    return df.parse(text);
  }

  // RFC977:3.1 The ARTICLE, BODY, HEAD, and STAT commands

  /**
   * Send an article retrieval request to the server.
   * @param articleNumber the article number of the article to retrieve
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article header and
   * body, separated by an empty line
   */
  public ArticleResponse article(int articleNumber)
    throws IOException
  {
    return articleImpl(ARTICLE, Integer.toString(articleNumber));
  }
  
  /**
   * Send an article retrieval request to the server.
   * @param messageId the message-id of the article to retrieve
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article header and
   * body, separated by an empty line
   */
  public ArticleResponse article(String messageId)
    throws IOException
  {
    return articleImpl(ARTICLE, messageId);
  }

  /**
   * Send an article head retrieval request to the server.
   * @param articleNumber the article number of the article to head
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article header
   */
  public ArticleResponse head(int articleNumber)
    throws IOException
  {
    return articleImpl(HEAD, Integer.toString(articleNumber));
  }

  /**
   * Send an article head retrieval request to the server.
   * @param messageId the message-id of the article to head
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article header
   */
  public ArticleResponse head(String messageId)
    throws IOException
  {
    return articleImpl(HEAD, messageId);
  }

  /**
   * Send an article body retrieval request to the server.
   * @param articleNumber the article number of the article to body
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article body
   */
  public ArticleResponse body(int articleNumber)
    throws IOException
  {
    return articleImpl(BODY, Integer.toString(articleNumber));
  }

  /**
   * Send an article body retrieval request to the server.
   * @param messageId the message-id of the article to body
   * @return an article response consisting of the article number and
   * message-id, and an iterator over the lines of the article body
   */
  public ArticleResponse body(String messageId)
    throws IOException
  {
    return articleImpl(BODY, messageId);
  }

  /**
   * Send an article status request to the server.
   * @param articleNumber the article number of the article to stat
   * @return an article response consisting of the article number and
   * message-id
   */
  public ArticleResponse stat(int articleNumber)
    throws IOException
  {
    return articleImpl(STAT, Integer.toString(articleNumber));
  }

  /**
   * Send an article status request to the server.
   * @param messageId the message-id of the article to stat
   * @return an article response consisting of the article number and
   * message-id
   */
  public ArticleResponse stat(String messageId)
    throws IOException
  {
    return articleImpl(STAT, messageId);
  }

  /**
   * Performs an ARTICLE, BODY, HEAD, or STAT command.
   * @param command one of the above commands
   * @param messageId the article-number or message-id in string form
   */
  protected ArticleResponse articleImpl(String command, String messageId)
    throws IOException
  {
    if (messageId != null)
      {
        StringBuffer line = new StringBuffer(command);
        line.append(' ');
        line.append(messageId);
        send(line.toString());
      }
    else
      {
        send(command);
      }
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case ARTICLE_FOLLOWS:
      case HEAD_FOLLOWS:
      case BODY_FOLLOWS:
        ArticleResponse aresponse = (ArticleResponse) response;
        ArticleStream astream =
          new ArticleStream(new MessageInputStream(in));
        pendingData = astream;
        aresponse.in = astream;
        return aresponse;
      case ARTICLE_RETRIEVED:
        return (ArticleResponse) response;
      default:
        // NO_GROUP_SELECTED
        // NO_ARTICLE_SELECTED
        // NO_SUCH_ARTICLE_NUMBER
        // NO_SUCH_ARTICLE
        // NO_PREVIOUS_ARTICLE
        // NO_NEXT_ARTICLE
        throw new NNTPException(response);
      }
  }
  
  // RFC977:3.2 The GROUP command

  /**
   * Send a group selection command to the server.
   * Returns a group status response.
   * @param name the name of the group to select
   */
  public GroupResponse group(String name)
    throws IOException
  {
    send(GROUP + ' ' + name);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case GROUP_SELECTED:
        return (GroupResponse) response;
      default:
        // NO_SUCH_GROUP
        throw new NNTPException(response);
      }
  }

  // RFC977:3.3 The HELP command

  /**
   * Requests a help listing.
   * @return an iterator over a collection of help lines.
   */
  public LineIterator help()
    throws IOException
  {
    send(HELP);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case HELP_TEXT:
        LineIterator li = new LineIterator(this);
        pendingData = li;
        return li;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC977:3.4 The IHAVE command

  /**
   * Sends an ihave command indicating that the client has an article with
   * the specified message-id.
   * @param messageId the article message-id
   * @return a PostStream if the server wants the specified article, null
   * otherwise
   */
  public PostStream ihave(String messageId)
    throws IOException
  {
    send(IHAVE + ' ' + messageId);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case SEND_TRANSFER_ARTICLE:
        return new PostStream(this, false);
      case ARTICLE_NOT_WANTED:
        return null;
      default:
        throw new NNTPException(response);
      }
  }

  // RFC(77:3.5 The LAST command

  /**
   * Sends a previous article positioning command to the server.
   * @return the article number/message-id pair associated with the new
   * article
   */
  public ArticleResponse last()
    throws IOException
  {
    return articleImpl(LAST, null);
  }

  // RFC977:3.6 The LIST command

  /**
   * Send a group listing command to the server.
   * Returns a GroupIterator. This must be read fully before other commands
   * are issued.
   */
  public GroupIterator list()
    throws IOException
  {
    return listImpl(LIST);
  }
  
  GroupIterator listImpl(String command)
    throws IOException
  {
    send(command);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case LIST_FOLLOWS:
        GroupIterator gi = new GroupIterator(this);
        pendingData = gi;
        return gi;
      default:
        throw new NNTPException(response);
      }
  }

  // RFC977:3.7 The NEWGROUPS command

  /**
   * Returns an iterator over the list of new groups on the server since the
   * specified date.
   * NB this method suffers from a minor millenium bug.
   * 
   * @param since the date from which to list new groups
   * @param distributions if non-null, an array of distributions to match
   */
  public LineIterator newGroups(Date since, String[]distributions)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(NEWGROUPS);
    buffer.append(' ');
    buffer.append(formatDate(since));
    if (distributions != null)
      {
        buffer.append(' ');
        for (int i = 0; i < distributions.length; i++)
          {
            if (i > 0)
              {
                buffer.append(',');
              }
            buffer.append(distributions[i]);
          }
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case NEWGROUPS_LIST_FOLLOWS:
        LineIterator li = new LineIterator(this);
        pendingData = li;
        return li;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC977:3.8 The NEWNEWS command

  /**
   * Returns an iterator over the list of message-ids posted or received to
   * the specified newsgroup(s) since the specified date.
   * NB this method suffers from a minor millenium bug.
   *
   * @param newsgroup the newsgroup wildmat
   * @param since the date from which to list new articles
   * @param distributions if non-null, a list of distributions to match
   */
  public LineIterator newNews(String newsgroup, Date since,
                              String[] distributions)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(NEWNEWS);
    buffer.append(' ');
    buffer.append(newsgroup);
    buffer.append(' ');
    buffer.append(formatDate(since));
    if (distributions != null)
      {
        buffer.append(' ');
        for (int i = 0; i < distributions.length; i++)
          {
            if (i > 0)
              {
                buffer.append(',');
              }
            buffer.append(distributions[i]);
          }
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case NEWNEWS_LIST_FOLLOWS:
        LineIterator li = new LineIterator(this);
        pendingData = li;
        return li;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC977:3.9 The NEXT command

  /**
   * Sends a next article positioning command to the server.
   * @return the article number/message-id pair associated with the new
   * article
   */
  public ArticleResponse next()
    throws IOException
  {
    return articleImpl(NEXT, null);
  }

  // RFC977:3.10 The POST command

  /**
   * Post an article. This is a two-stage process.
   * If successful, returns an output stream to write the article to.
   * Clients should call <code>write()</code> on the stream for all the
   * bytes of the article, and finally call <code>close()</code>
   * on the stream.
   * No other method should be called in between.
   * @see #postComplete
   */
  public OutputStream post()
    throws IOException
  {
    send(POST);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case SEND_ARTICLE:
        return new PostStream(this, false);
      default:
        // POSTING_NOT_ALLOWED
        throw new NNTPException(response);
      }
  }
  
  /**
   * Indicates that the client has finished writing all the bytes of the
   * article.
   * Called by the PostStream during <code>close()</code>.
   * @see #post
   */
  void postComplete()
    throws IOException
  {
    send(DOT);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case ARTICLE_POSTED:
      case ARTICLE_TRANSFERRED:
        return;
      default:
        // POSTING_FAILED
        // TRANSFER_FAILED
        // ARTICLE_REJECTED
        throw new NNTPException(response);
      }
  }

  // RFC977:3.11 The QUIT command

  /**
   * Close the connection.
   * After calling this method, no further calls on this object are valid.
   */
  public void quit()
    throws IOException
  {
    send(QUIT);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case CLOSING_CONNECTION:
        socket.close();
        return;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC977:3.12 The SLAVE command

  /**
   * Indicates to the server that this is a slave connection.
   */
  public void slave()
    throws IOException
  {
    send(SLAVE);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case SLAVE_ACKNOWLEDGED:
        break;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:1.1 The CHECK command

  public boolean check(String messageId)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(CHECK);
    buffer.append(' ');
    buffer.append(messageId);
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case SEND_ARTICLE_VIA_TAKETHIS:
        return true;
      case ARTICLE_NOT_WANTED_VIA_TAKETHIS:
        return false;
      default:
        // SERVICE_DISCONTINUED
        // TRY_AGAIN_LATER
        // TRANSFER_PERMISSION_DENIED
        // COMMAND_NOT_RECOGNIZED
        throw new NNTPException(response);
      }
  }

  // RFC2980:1.2 The MODE STREAM command

  /**
   * Attempt to initialise the connection in streaming mode.
   * This is generally used to bypass the lock step nature of NNTP in order
   * to perform a series of CHECK and TAKETHIS commands.
   *
   * @return true if the server supports streaming mode
   */
  public boolean modeStream()
    throws IOException
  {
    send(MODE_STREAM);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case STREAMING_OK:
        return true;
      default:
        // COMMAND_NOT_RECOGNIZED
        return false;
      }
  }
  
  // RFC2980:1.3 The TAKETHIS command

  /**
   * Implements the out-of-order takethis command.
   * The client uses the returned output stream to write all the bytes of the
   * article. When complete, it calls <code>close()</code> on the
   * stream.
   * @see #takethisComplete
   */
  public OutputStream takethis(String messageId)
    throws IOException
  {
    send(TAKETHIS + ' ' + messageId);
    return new PostStream(this, true);
  }

  /**
   * Completes a takethis transaction.
   * Called by PostStream.close().
   * @see #takethis
   */
  void takethisComplete()
    throws IOException
  {
    send(DOT);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case ARTICLE_TRANSFERRED_OK:
        return;
      default:
        // SERVICE_DISCONTINUED
        // ARTICLE_TRANSFER_FAILED
        // TRANSFER_PERMISSION_DENIED
        // COMMAND_NOT_RECOGNIZED
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:1.4 The XREPLIC command

  // TODO

  // RFC2980:2.1.2 The LIST ACTIVE command

  /**
   * Returns an iterator over the groups specified according to the wildmat
   * pattern. The iterator must be read fully before other commands are
   * issued.
   * @param wildmat the wildmat pattern. If null, returns all groups. If no
   * groups are matched, returns an empty iterator.
   */
  public GroupIterator listActive(String wildmat)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(LIST_ACTIVE);
    if (wildmat != null)
      {
        buffer.append(' ');
        buffer.append(wildmat);
      }
    return listImpl(buffer.toString());
  }
  
  // RFC2980:2.1.3 The LIST ACTIVE.TIMES command

  /**
   * Returns an iterator over the active.times file.
   * Each ActiveTime object returned provides details of who created the
   * newsgroup and when.
   */
  public ActiveTimesIterator listActiveTimes()
    throws IOException
  {
    send(LIST_ACTIVE_TIMES);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case LIST_FOLLOWS:
        return new ActiveTimesIterator(this);
      default:
        throw new NNTPException(response);
      }
  }

  // RFC2980:2.1.4 The LIST DISTRIBUTIONS command

  // TODO

  // RFC2980:2.1.5 The LIST DISTRIB.PATS command

  // TODO

  // RFC2980:2.1.6 The LIST NEWSGROUPS command

  /**
   * Returns an iterator over the group descriptions for the given groups.
   * @param wildmat if non-null, limits the groups in the iterator to the
   * specified pattern
   * @return an iterator over group name/description pairs
   * @see #xgtitle
   */
  public PairIterator listNewsgroups(String wildmat)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(LIST_NEWSGROUPS);
    if (wildmat != null)
      {
        buffer.append(' ');
        buffer.append(wildmat);
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case LIST_FOLLOWS:
        PairIterator pi = new PairIterator(this);
        pendingData = pi;
        return pi;
      default:
        throw new NNTPException(response);
      }
  }

  // RFC2980:2.1.7 The LIST OVERVIEW.FMT command

  /**
   * Returns an iterator over the order in which headers are stored in the
   * overview database.
   * Each line returned by the iterator contains one header field.
   * @see #xover
   */
  public LineIterator listOverviewFmt()
    throws IOException
  {
    send(LIST_OVERVIEW_FMT);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case LIST_FOLLOWS:
        LineIterator li = new LineIterator(this);
        pendingData = li;
        return li;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:2.1.8 The LIST SUBSCRIPTIONS command

  /**
   * Returns a list of newsgroups suitable for new users of the server.
   */
  public GroupIterator listSubscriptions()
    throws IOException
  {
    return listImpl(LIST_SUBSCRIPTIONS);
  }

  // RFC2980:2.2 The LISTGROUP command

  /**
   * Returns a listing of all the article numbers in the specified
   * newsgroup. If the <code>group</code> parameter is null, the currently
   * selected group is assumed.
   * @param group the name of the group to list articles for
   */
  public ArticleNumberIterator listGroup(String group)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(LISTGROUP);
    if (group != null)
      {
        buffer.append(' ');
        buffer.append(group);
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read(), true);
    switch (response.status)
      {
      case GROUP_SELECTED:
        ArticleNumberIterator ani = new ArticleNumberIterator(this);
        pendingData = ani;
        return ani;
      default:
        throw new NNTPException(response);
      }
  }

  // RFC2980:2.3 The MODE READER command

  /**
   * Indicates to the server that this is a user-agent.
   * @return true if posting is allowed, false otherwise.
   */
  public boolean modeReader()
    throws IOException
  {
    send(MODE_READER);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case POSTING_ALLOWED:
        canPost = true;
        return canPost;
      case POSTING_NOT_ALLOWED:
        canPost = false;
        return canPost;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:2.4 The XGTITLE command

  /**
   * Returns an iterator over the list of newsgroup descriptions.
   * @param wildmat if non-null, the newsgroups to match
   */
  public PairIterator xgtitle(String wildmat)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(XGTITLE);
    if (wildmat != null)
      {
        buffer.append(' ');
        buffer.append(wildmat);
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case XGTITLE_LIST_FOLLOWS:
        PairIterator pi = new PairIterator(this);
        pendingData = pi;
        return pi;
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:2.6 The XHDR command

  public HeaderIterator xhdr(String header, String range)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(XHDR);
    buffer.append(' ');
    buffer.append(header);
    if (range != null)
      {
        buffer.append(' ');
        buffer.append(range);
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case HEAD_FOLLOWS:
        HeaderIterator hi = new HeaderIterator(this);
        pendingData = hi;
        return hi;
      default:
        // NO_GROUP_SELECTED
        // NO_SUCH_ARTICLE
        throw new NNTPException(response);
      }
  }

  // RFC2980:2.7 The XINDEX command

  // TODO

  // RFC2980:2.8 The XOVER command

  public OverviewIterator xover(Range range)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(XOVER);
    if (range != null)
      {
        buffer.append(' ');
        buffer.append(range.toString());
      }
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case OVERVIEW_FOLLOWS:
        OverviewIterator oi = new OverviewIterator(this);
        pendingData = oi;
        return oi;
      default:
        // NO_GROUP_SELECTED
        // PERMISSION_DENIED
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:2.9 The XPAT command

  // TODO

  // RFC2980:2.10 The XPATH command

  // TODO

  // RFC2980:2.11 The XROVER command

  // TODO

  // RFC2980:2.12 The XTHREAD command

  // TODO

  // RFC2980:3.1.1 Original AUTHINFO

  /**
   * Basic authentication strategy.
   * @param username the user to authenticate
   * @param password the(cleartext) password
   * @return true on success, false on failure
   */
  public boolean authinfo(String username, String password)
    throws IOException
  {
    StringBuffer buffer = new StringBuffer(AUTHINFO_USER);
    buffer.append(' ');
    buffer.append(username);
    send(buffer.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case AUTHINFO_OK:
        return true;
      case SEND_AUTHINFOPASS:
        buffer.setLength(0);
        buffer.append(AUTHINFO_PASS);
        buffer.append(' ');
        buffer.append(password);
        send(buffer.toString());
        response = parseResponse(read());
        switch (response.status)
          {
          case AUTHINFO_OK:
            return true;
          case PERMISSION_DENIED:
            return false;
          default:
            throw new NNTPException(response);
          }
      default:
        // AUTHINFO_REJECTED
        throw new NNTPException(response);
      }
  }

  // RFC2980:3.1.2 AUTHINFO SIMPLE

  /**
   * Implementation of NNTP simple authentication.
   * Note that use of this authentication strategy is highly deprecated,
   * only use on servers that won't accept any other form of authentication.
   */
  public boolean authinfoSimple(String username, String password)
    throws IOException
  {
    send(AUTHINFO_SIMPLE);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case SEND_AUTHINFO_SIMPLE:
        StringBuffer buffer = new StringBuffer(username);
        buffer.append(' ');
        buffer.append(password);
        send(buffer.toString());
        response = parseResponse(read());
        switch (response.status)
          {
          case AUTHINFO_SIMPLE_OK:
            return true;
          case AUTHINFO_SIMPLE_DENIED:
            return false;
          default:throw new NNTPException(response);
          }
      default:
        throw new NNTPException(response);
      }
  }
  
  // RFC2980:3.1.3 AUTHINFO GENERIC

  /**
   * Authenticates the connection using the specified SASL mechanism,
   * username and password.
   * @param mechanism a SASL authentication mechanism, e.g. LOGIN, PLAIN,
   * CRAM-MD5, GSSAPI
   * @param username the authentication principal
   * @param password the authentication credentials
   */
  public boolean authinfoGeneric(String mechanism,
                                  String username, String password)
    throws IOException
  {
    String[] m = new String[] { mechanism };
    CallbackHandler ch = new SaslCallbackHandler(username, password);
    // Avoid lengthy callback procedure for GNU Crypto
    HashMap p = new HashMap();
    p.put("gnu.crypto.sasl.username", username);
    p.put("gnu.crypto.sasl.password", password);
    SaslClient sasl =
      Sasl.createSaslClient(m, null, "smtp",
                             socket.getInetAddress().getHostName(),
                             p, ch);
    if (sasl == null)
      {
        return false;
      }
    
    StringBuffer cmd = new StringBuffer(AUTHINFO_GENERIC);
    cmd.append(' ');
    cmd.append(mechanism);
    if (sasl.hasInitialResponse())
      {
        cmd.append(' ');
        byte[] init = sasl.evaluateChallenge(new byte[0]);
        cmd.append(new String(init, "US-ASCII"));
      }
    send(cmd.toString());
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case AUTHINFO_OK:
        String qop = (String) sasl.getNegotiatedProperty(Sasl.QOP);
        if ("auth-int".equalsIgnoreCase(qop)
            || "auth-conf".equalsIgnoreCase(qop))
          {
            InputStream in = socket.getInputStream();
            in = new BufferedInputStream(in);
            in = new SaslInputStream(sasl, in);
            in = new CRLFInputStream(in);
            this.in = new LineInputStream(in);
            OutputStream out = socket.getOutputStream();
            out = new BufferedOutputStream(out);
            out = new SaslOutputStream(sasl, out);
            this.out = new CRLFOutputStream(out);
          }
        return true;
      case PERMISSION_DENIED:
        return false;
      case COMMAND_NOT_RECOGNIZED:
      case SYNTAX_ERROR:
      case INTERNAL_ERROR:
      default:
        throw new NNTPException(response);
        // FIXME how does the server send continuations?
      }
  }
  
  // RFC2980:3.2 The DATE command

  /**
   * Returns the date on the server.
   */
  public Date date()
    throws IOException
  {
    send(DATE);
    StatusResponse response = parseResponse(read());
    switch (response.status)
      {
      case DATE_OK:
        String message = response.getMessage();
        try
          {
            DateFormat df = new SimpleDateFormat("yyyyMMddHHmmss");
            return df.parse(message);
          }
        catch (ParseException e)
          {
            throw new IOException("Invalid date: " + message);
          }
      default:
        throw new NNTPException(response);
      }
  }

  // -- Utility functions --

  /**
   * Parse a response object from a response line sent by the server.
   */
  protected StatusResponse parseResponse(String line)
    throws ProtocolException
  {
    return parseResponse(line, false);
  }
  
  /**
   * Parse a response object from a response line sent by the server.
   * @param isListGroup whether we are invoking the LISTGROUP command
   */
  protected StatusResponse parseResponse(String line, boolean isListGroup)
    throws ProtocolException
  {
    if (line == null)
      {
        throw new ProtocolException(hostname + " closed connection");
      }
    int start = 0, end;
    short status = -1;
    String statusText = line;
    String message = null;
    end = line.indexOf(' ', start);
    if (end > start)
      {
        statusText = line.substring(start, end);
        message = line.substring(end + 1);
      }
    try
      {
        status = Short.parseShort(statusText);
      }
    catch (NumberFormatException e)
      {
        throw new ProtocolException(line);
      }
    StatusResponse response;
    switch (status)
      {
      case ARTICLE_FOLLOWS:
      case HEAD_FOLLOWS:
      case BODY_FOLLOWS:
      case ARTICLE_RETRIEVED:
      case GROUP_SELECTED:
        /* The LISTGROUP command returns a list of articles with a 211,
         * instead of the newsgroup totals returned with the GROUP command.
         * Check for this case. */
        if (status != GROUP_SELECTED || isListGroup)
          {
            try
              {
                ArticleResponse aresponse =
                  new ArticleResponse(status, message);
                // article number
                start = end + 1;
                end = line.indexOf(' ', start);
                if (end > start)
                  {
                    aresponse.articleNumber =
                      Integer.parseInt(line.substring(start, end));
                  }
                // message-id
                start = end + 1;
                end = line.indexOf(' ', start);
                if (end > start)
                  {
                    aresponse.messageId = line.substring(start, end);
                  }
                else
                  {
                    aresponse.messageId = line.substring(start);
                  }
                response = aresponse;
              }
            catch (NumberFormatException e)
              {
                // This will happen for XHDR
                response = new StatusResponse(status, message);
              }
            break;
          }
        // This is the normal case for GROUP_SELECTED
        GroupResponse gresponse = new GroupResponse(status, message);
        try
          {
            // count
            start = end + 1;
            end = line.indexOf(' ', start);
            if (end > start)
              {
                gresponse.count =
                  Integer.parseInt(line.substring(start, end));
              }
            // first
            start = end + 1;
            end = line.indexOf(' ', start);
            if (end > start)
              {
                gresponse.first =
                  Integer.parseInt(line.substring(start, end));
              }
            // last
            start = end + 1;
            end = line.indexOf(' ', start);
            if (end > start)
              {
                gresponse.last =
                  Integer.parseInt(line.substring(start, end));
              }
            // group
            start = end + 1;
            end = line.indexOf(' ', start);
            if (end > start)
              {
                gresponse.group = line.substring(start, end);
              }
            else
              {
                gresponse.group = line.substring(start);
              }
          }
        catch (NumberFormatException e)
          {
            throw new ProtocolException(line);
          }
        response = gresponse;
        break;
      default:
        response = new StatusResponse(status, message);
      }
    return response;
  }
  
  /**
   * Send a single line to the server.
   * @param line the line to send
   */
  protected void send(String line)
    throws IOException
  {
    if (pendingData != null)
      {
        // Clear pending data
        pendingData.readToEOF();
        pendingData = null;
      }
    logger.log(NNTP_TRACE, ">" + line);
    byte[] data = line.getBytes(US_ASCII);
    out.write(data);
    out.writeln();
    out.flush();
  }
  
  /**
   * Read a single line from the server.
   * @return a line of text
   */
  protected String read()
    throws IOException
  {
    String line = in.readLine();
    if (line == null)
      {
        logger.log(NNTP_TRACE, "<EOF");
      }
    else
      {
        logger.log(NNTP_TRACE, "<" + line);
      }
    return line;
  }
  
}

