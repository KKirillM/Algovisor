/*
        Copyright (c) 2012, MICEX-RTS. All rights reserved.

        Plaza-2 Client Gate API Usage Sample.
        Replication DataStream Client.

        All the software and documentation included in this and any
        other MICEX-RTS CGate Releasese is copyrighted by MICEX-RTS.

        Redistribution and use in source and binary forms, with or without
        modification, are permitted only by the terms of a valid
        software license agreement with MICEX-RTS.

        THIS SOFTWARE IS PROVIDED "AS IS" AND MICEX-RTS DISCLAIMS ALL WARRANTIES
        EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY IMPLIED WARRANTIES OF
        NON-INFRINGEMENT, MERCHANTABILITY OR FITNESS FOR A PARTICULAR
        PURPOSE.  MICEX-RTS DOES NOT WARRANT THAT USE OF THE SOFTWARE WILL BE
        UNINTERRUPTED OR ERROR-FREE.  MICEX-RTS SHALL NOT, UNDER ANY CIRCUMSTANCES, BE
        LIABLE TO LICENSEE FOR LOST PROFITS, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR
        INDIRECT DAMAGES ARISING OUT OF OR RELATED TO THIS AGREEMENT OR THE
        TRANSACTIONS CONTEMPLATED HEREUNDER, EVEN IF MICEX-RTS HAS BEEN APPRISED OF
        THE LIKELIHOOD OF SUCH DAMAGES.
*/

/*
	P2sys - пример авторизации роутера из cgate. Повторяет в цикле следующие действия:
	i. Посылает ошибочный набор (login, pwd), в ответ получает сообщение logon failed;
	ii. После этого посылает правильный набор (login, pwd);
	iii. На сообщение об успешной авторизации посылается запрос на logout;
	iv. Возврат к пункту 1.

	P2sys is used for authorize router via cgate. Runs the following commands in loop:
	i. Sends erroneous command set ('login', 'pwd'), receives the 'logon failed' message in reply;
	ii. Sends the correct command set ('login', 'pwd');
	iii. Sends the 'logout' request in reply on successful authorisation message;
	iv. Returns to 1.
*/


import ru.micexrts.cgate.*;
import ru.micexrts.cgate.messages.*;


public class P2SysSample {
	
	private class P2SysListener implements ISubscriber{

		private Publisher publisher;
		
		public P2SysListener(Publisher pub)
		{
			publisher = pub;
		}
		
		private int requestsCntr = 0;
		
		@Override
		public int onMessage(Connection conn, Listener listener, Message message) {
			switch (message.getType())
			{
			case MessageType.MSG_DATA:
				DataMessage dataMsg = (DataMessage)message;
				if (dataMsg.getMsgId() == RouterConnected_msgid)
				{
					CGate.logInfo("Router connected");
					sendLogoutRequest(publisher);
				}
				else if (dataMsg.getMsgId() == RouterDisconnected_msgid)
				{
					CGate.logInfo("Router disconnected");
					if (sendLoginRequest(publisher, requestsCntr) == true)
					{
						requestsCntr++;
					}
				}
				else if (dataMsg.getMsgId() == ConnectionConnected_msgid)
				{
					CGate.logInfo("Connection connected");
				}
				else if (dataMsg.getMsgId() == ConnectionDisconnected_msgid)
				{
					CGate.logInfo("Connection disconnected");
				}
				else if (dataMsg.getMsgId() == LogonFailed_msgid)
				{
					CGate.logInfo("Logon failed");
					if (sendLoginRequest(publisher, requestsCntr) == true)
					{
						requestsCntr++;
					}
				}
				CGate.logInfo(String.format("Reply message. ID=%d, User-Id=%d, content=%s", 
							dataMsg.getMsgId(), dataMsg.getUserId(), message));
				break;
			case MessageType.MSG_P2MQ_TIMEOUT:
				CGate.logInfo("Timeout");
				break;
			case MessageType.MSG_OPEN:
				CGate.logInfo("Msg open");
				break;
			case MessageType.MSG_CLOSE:
				CGate.logInfo("Msg close");
				break;
			default:
				CGate.logInfo(message.toString());
			}

			
            return 0;
		}					
	}
	
	private Connection conn;
	private Publisher pub;
	private Listener lsn;
	
	final static int RouterLogin_msgid = 1;
	final static int RouterLogout_msgid = 2;

	final static int RouterConnected_msgid = 1;
	final static int RouterDisconnected_msgid = 2;
	final static int ConnectionConnected_msgid = 3;
	final static int ConnectionDisconnected_msgid = 4;
	final static int LogonFailed_msgid = 5;
	
	static String userLogin;
	static String userPwd;
	static String userLogin2;
	static String userPwd2;

	public P2SysSample(String login, String pwd) {
		userLogin = login;
		userPwd = pwd;
	}
	
	public P2SysSample(String login, String pwd, String login2, String pwd2) {
		userLogin = login;
		userPwd = pwd;
		userLogin2 = login2;
		userPwd2 = pwd2;
	} 

	private static void sendLogoutRequest(Publisher pub) 
	{
		DataMessage smsg = null;
		try {
			smsg = (DataMessage)pub.newMessage(MessageKeyType.KEY_ID, RouterLogout_msgid);
			smsg.getField("state").set(1);
			ru.micexrts.cgate.impl.CGateImpl.log_infostr(String.format("posting message dump: %s", smsg));
			pub.post(smsg, 0);
		} 
		catch (CGateException e) {
		}
		finally{
			try {
				if (smsg != null) 
					smsg.dispose();
			} catch (CGateException e) {
			}
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
		}
	}

	private static boolean sendLoginRequest(Publisher pub, int cntr)
	{
		String currentLogin = "", currentPwd = "";
		
		if (userLogin2 != null && userPwd2 != null)
		{
			if (cntr % 3 == 0)
			{
				currentLogin = "wrong login";
				currentPwd = "wrong pwd";
			}
			else if ((cntr % 3) == 1)
			{
				currentLogin = userLogin;
				currentPwd = userPwd;
			}
			else
			{
				currentLogin = userLogin2;
				currentPwd = userPwd2;
			}
		}
		else
		{
			if (cntr % 2 == 0)
			{
				currentLogin = "wrong login";
				currentPwd = "wrong pwd";
			}
			else
			{
				currentLogin = userLogin;
				currentPwd = userPwd;
			}
		}
		
		String loginMsgContents = String.format("USERNAME=%s;PASSWORD=%s", currentLogin, currentPwd);
		
		return sendLoginRequest(pub, loginMsgContents);
	}
	
	private static boolean sendLoginRequest(Publisher pub, String login) 
	{
		boolean bResult = false;
		DataMessage smsg = null;
		try{
			smsg = (DataMessage)pub.newMessage(MessageKeyType.KEY_ID, RouterLogin_msgid);
			smsg.getField("loginstr").set(login);
			ru.micexrts.cgate.impl.CGateImpl.log_infostr(String.format("posting message dump: %s", smsg));
			pub.post(smsg, 0);
			bResult = true;
		} 
		catch (CGateException e) {
		}
		finally{
			try {
				if (smsg != null) 
					smsg.dispose();
			} 
			catch (CGateException e) {
			}
		}
		try {
			Thread.sleep(1000);
		} catch (InterruptedException e) {
		}
		return bResult;
	}

	
	private static volatile boolean exitFlag = false;
	private static volatile boolean cleanedUp = false;
	
	public void run() throws CGateException {	
		Runtime.getRuntime(). addShutdownHook(new Thread() {
			public void run() {
				exitFlag = true;
				while (!cleanedUp);
			}
		});

		try
		{
			CGate.open("ini=jp2sys.ini;key=11111111");
			conn = new Connection("p2sys://127.0.0.1:4001;app_name=jtest_p2sys");
			pub = new Publisher(conn, "p2sys://;name=p2sys_pub");
			lsn = new Listener(conn, "p2sys://;name=p2sys_lsn", new P2SysListener(pub));
			
			while (!exitFlag)
			{
				try
				{
					int state = conn.getState();
					if (state == State.ERROR)
					{
						conn.close();
					}
					else if (state == State.CLOSED)
					{
						conn.open("");
					}
					else if (state == State.OPENING)
					{
						int result = conn.process(1);
						if (result != ErrorCode.OK && result != ErrorCode.TIMEOUT)
						{
							ru.micexrts.cgate.impl.CGateImpl.log_errorstr(String.format("Warning: connection state request failed: %d", result));
						}
					}
					else if (state == State.ACTIVE)
					{
						int result = conn.process(1);
						if (result != ErrorCode.OK && result != ErrorCode.TIMEOUT)
						{
							ru.micexrts.cgate.impl.CGateImpl.log_errorstr(String.format("Warning: connection state request failed: %d", result));
						}
						// Check publisher state
						int pubState = pub.getState();
						if (pubState == State.CLOSED)
						{
							pub.open("");
						}
						else if (pubState == State.ERROR)
						{
							pub.close();
						}
						if (pubState != State.ACTIVE)
						{
							continue;
						}
						// Check listener state
						int lsnState = lsn.getState();
						if (lsnState == State.CLOSED)
						{
							lsn.open("");
						}
						else if (lsnState == State.ERROR)
						{
							lsn.close();
						}
					}
				}
				catch (CGateException cgex)
				{
					CGate.logInfo("Exception: " + cgex);
				}
			}
		}
		finally {
			if (lsn != null) {
				try { lsn.close(); } catch (CGateException cgex) {}
				try { lsn.dispose(); } catch (CGateException cgex) {}
			}
			if (pub != null) {
				try { pub.close(); } catch (CGateException cgex) {}
				try { pub.dispose(); } catch (CGateException cgex) {}
			}
			if (conn != null) {
				try { conn.close(); } catch (CGateException cgex) {}
				try { conn.dispose(); } catch (CGateException cgex) {}
			}
			CGate.close();
			cleanedUp = true;
		}
	}
	
	public static void main(String[] args) throws CGateException, InterruptedException{
		if (args.length < 2)
		{			
			System.out.println("Usage: 'user login' 'user pwd'");
			return;
		}
		if (args.length < 4)
		{
			new P2SysSample(args[0], args[1]).run();
		}
		else
		{
			new P2SysSample(args[0], args[1], args[2], args[3]).run();
		}
	}

}
