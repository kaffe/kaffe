/*
 * NNTPConstants.java
 * Copyright (C) 2002 The free Software Foundation
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

/**
 * NNTP status response codes.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public interface NNTPConstants
{

  /**
   * Indicates a line of help text.
   */
  public static final short HELP_TEXT = 100;

  /**
   * Indicates a DATE response.
   */
  public static final short DATE_OK = 111;

  /**
   * Indicates that the server is ready and posting is allowed.
   */
  public static final short POSTING_ALLOWED = 200;

  /**
   * Indicates that the server is ready but posting is not allowed.
   */
  public static final short NO_POSTING_ALLOWED = 201;

  /**
   * Indicates that the server has noted the slave status of the connection.
   */
  public static final short SLAVE_ACKNOWLEDGED = 202;

  /**
   * Indicates that the server has accepted streaming mode.
   */
  public static final short STREAMING_OK = 203;

  /**
   * Indicates that the server is closing the connection.
   */
  public static final short CLOSING_CONNECTION = 205;

  /**
   * Indicates that the newsgroup was successfully selected.
   * Format of the message is &quot;<tt>n f l s</tt> xxx&quot;
   * (<tt>n</tt> = estimated number of articles in group,
   * <tt>f</tt> = first article number in the group,
   * <tt>l</tt> = last article number in the group,
   * <tt>s</tt> = name of the group.)
   */
  public static final short GROUP_SELECTED = 211;

  /**
   * Indicates that a list of valid newsgroups follows.
   * The format of each following line is &quot;<tt>g l f p</tt>&quot;
   * (<tt>g</tt> = newsgroup name,
   * <tt>l</tt> = last article number in group,
   * <tt>f</tt> = first article number in group,
   * <tt>p</tt> = &apos;y&apos; if posting to the group is allowed,
   * &apos;n&apos; otherwise)
   */
  public static final short LIST_FOLLOWS = 215;

  /**
   * Indicates that the article has been retrieved.
   * The head and body of the article follow.
   */
  public static final short ARTICLE_FOLLOWS = 220;

  /**
   * Indicates that the article has been retrieved.
   * The head of the article follows.
   */
  public static final short HEAD_FOLLOWS = 221;

  /**
   * Indicates that the article has been retrieved.
   * The body of the article follows.
   */
  public static final short BODY_FOLLOWS = 222;

  /**
   * Indicates that the article has been retrieved.
   * The text of the article must be requested separately.
   */
  public static final short ARTICLE_RETRIEVED = 223;

  /**
   * Indicates that a listing of overview information follows.
   */
  public static final short OVERVIEW_FOLLOWS = 224;

  /**
   * Indicates that a list of new articles by message-id follows.
   */
  public static final short NEWNEWS_LIST_FOLLOWS = 230;

  /**
   * Indicates that a list of new newsgroups follows.
   * This code is issued following a successful NEWGROUPS command. The
   * format of the listing is the same as for code 215 (list follows).
   */
  public static final short NEWGROUPS_LIST_FOLLOWS = 231;

  /**
   * Indicates that the article was correctly transferred.
   */
  public static final short ARTICLE_TRANSFERRED = 235;

  /**
   * Indicates that the server does not have the specified article and would
   * like it to be transferred via TAKETHIS.
   */
  public static final short SEND_ARTICLE_VIA_TAKETHIS = 238;

  /**
   * Indicates that the server accepted the article transferred by a
   * TAKETHIS command.
   */
  public static final short ARTICLE_TRANSFERRED_OK = 239;

  /**
   * Indicates that the article was successfully posted.
   */
  public static final short ARTICLE_POSTED = 240;

  /**
   * Indicates success of an AUTHINFO SIMPLE transaction.
   */
  public static final short AUTHINFO_SIMPLE_OK = 350;

  /**
   * Indicates that AUTHINFO authentication was successful.
   */
  public static final short AUTHINFO_OK = 281;

  /**
   * Indicates that the article to be transferred should be sent by the
   * client. It should end with a CRLF-dot-CRLF sequence, i.e. a dot on a
   * line by itself.
   */
  public static final short SEND_TRANSFER_ARTICLE = 335;

  /**
   * Indicates that the article to be posted should be sent by the
   * client. It should end with a CRLF-dot-CRLF sequence, i.e. a dot on a
   * line by itself.
   */
  public static final short SEND_ARTICLE = 340;

  /**
   * Instructs the client to send a username/password pair according to the
   * AUTHINFO SIMPLE specification.
   */
  public static final short SEND_AUTHINFO_SIMPLE = 350;

  /**
   * Indicates that the server is ready to accept the AUTHINFO password.
   */
  public static final short SEND_AUTHINFOPASS = 381;

  /**
   * Indicates that the service has been discontinued.
   */
  public static final short SERVICE_DISCONTINUED = 400;

  /**
   * Indicates that no such newsgroup exists.
   */
  public static final short NO_SUCH_GROUP = 411;

  /**
   * Indicates that no newsgroup has been selected.
   */
  public static final short NO_GROUP_SELECTED = 412;

  /**
   * Indicates that no article has been selected.
   */
  public static final short NO_ARTICLE_SELECTED = 420;

  /**
   * Indicates that there is no next article in this newsgroup.
   */
  public static final short NO_NEXT_ARTICLE = 421;

  /**
   * Indicates that there is no previous article in this newsgroup.
   */
  public static final short NO_PREVIOUS_ARTICLE = 422;

  /**
   * Indicates that no article with the specified number exists in this
   * newsgroup.
   */
  public static final short NO_SUCH_ARTICLE_NUMBER = 423;

  /**
   * Indicates that the specified article could not be found.
   */
  public static final short NO_SUCH_ARTICLE = 430;

  /**
   * Indicates that the server is not currently in a state to accept an
   * article, but may become so at a later stage.
   */
  public static final short TRY_AGAIN_LATER = 431;

  /**
   * Indicates that the server does not want the specified article.
   * The client should not send the article.
   */
  public static final short ARTICLE_NOT_WANTED = 435;

  /**
   * Indicates that transfer of the specified article failed.
   * The client should try to send the article again later.
   */
  public static final short TRANSFER_FAILED = 436;

  /**
   * Indicates that the specified article was rejected.
   * The client should not attempt to send the article again.
   */
  public static final short ARTICLE_REJECTED = 437;

  /**
   * Indicates that the server already has the specified article, and
   * therefore doesn't want it sent using TAKETHIS.
   */
  public static final short ARTICLE_NOT_WANTED_VIA_TAKETHIS = 438;

  /**
   * Indicates that an article transferred by a TAKETHIS command failed.
   */
  public static final short ARTICLE_TRANSFER_FAILED = 439;

  /**
   * Indicates that posting is not allowed.
   */
  public static final short POSTING_NOT_ALLOWED = 440;

  /**
   * Indicates that posting of the article failed.
   * The client may attempt to post the article again.
   */
  public static final short POSTING_FAILED = 441;

  /**
   * Indicates that authentication via the AUTHINFO SIMPLE strategy is
   * required.
   */
  public static final short AUTHINFO_SIMPLE_REQUIRED = 450;

  /**
   * Indicates an authentication failure using AUTHINFO SIMPLE.
   */
  public static final short AUTHINFO_SIMPLE_DENIED = 452;

  /**
   * Indicates that the client does not have the appropriate authorization
   * to transfer an article.
   */
  public static final short TRANSFER_PERMISSION_DENIED = 480;

  /**
   * Indicates that an XGTITLE listing follows.
   */
  public static final short XGTITLE_LIST_FOLLOWS = 481;

  /**
   * Indicates the the authentication information supplied was not accepted
   * by the server.
   */
  public static final short AUTHINFO_REJECTED = 482;

  /**
   * Indicates that the command sent by the client was not understood by the
   * server.
   */
  public static final short COMMAND_NOT_RECOGNIZED = 500;

  /**
   * Indicates that the command sent by the client was not a valid NNTP
   * command.
   */
  public static final short SYNTAX_ERROR = 501;

  /**
   * Indicates that access restrictions deny permission to execute the
   * command sent by the client.
   */
  public static final short PERMISSION_DENIED = 502;

  /**
   * Indicates that the server was unable to perform the command due to an
   * internal error.
   */
  public static final short INTERNAL_ERROR = 503;

  // -- Client commands --

  public static final String ARTICLE = "ARTICLE";
  public static final String AUTHINFO_USER = "AUTHINFO USER";
  public static final String AUTHINFO_PASS = "AUTHINFO PASS";
  public static final String AUTHINFO_SIMPLE = "AUTHINFO SIMPLE";
  public static final String AUTHINFO_GENERIC = "AUTHINFO GENERIC";
  public static final String BODY = "BODY";
  public static final String CHECK = "CHECK";
  public static final String DATE = "DATE";
  public static final String HEAD = "HEAD";
  public static final String STAT = "STAT";
  public static final String GROUP = "GROUP";
  public static final String HELP = "HELP";
  public static final String IHAVE = "IHAVE";
  public static final String LAST = "LAST";
  public static final String LIST = "LIST";
  public static final String LIST_ACTIVE = "LIST ACTIVE";
  public static final String LIST_ACTIVE_TIMES = "LIST ACTIVE.TIMES";
  public static final String LIST_DISTRIBUTIONS = "LIST DISTRIBUTIONS";
  public static final String LIST_DISTRIB_PATS = "LIST DISTRIB.PATS";
  public static final String LIST_NEWSGROUPS = "LIST NEWSGROUPS";
  public static final String LIST_OVERVIEW_FMT = "LIST OVERVIEW.FMT";
  public static final String LIST_SUBSCRIPTIONS = "LIST SUBSCRIPTIONS";
  public static final String LISTGROUP = "LISTGROUP";
  public static final String MODE_READER = "MODE READER";
  public static final String MODE_STREAM = "MODE STREAM";
  public static final String NEWGROUPS = "NEWGROUPS";
  public static final String NEWNEWS = "NEWNEWS";
  public static final String NEXT = "NEXT";
  public static final String POST = "POST";
  public static final String QUIT = "QUIT";
  public static final String SLAVE = "SLAVE";
  public static final String TAKETHIS = "TAKETHIS";
  public static final String XGTITLE = "XGTITLE";
  public static final String XHDR = "XHDR";
  public static final String XINDEX = "XINDEX";
  public static final String XOVER = "XOVER";
  public static final String XPAT = "XPAT";
  public static final String XPATH = "XPATH";
  public static final String XREPLIC = "XREPLIC";
  public static final String XROVER = "XROVER";

}

