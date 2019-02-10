/*
		Copyright (c) 2014, MOEX. All rights reserved.
		
		Plaza-2 Client Gate API Usage Sample.
		Replication DataStream Client.
		
		All the software and documentation included in this and any
		other MOEX CGate Releasese is copyrighted by MOEX.
		
		Redistribution and use in source and binary forms, with or without
		modification, are permitted only by the terms of a valid
		software license agreement with MOEX.
		
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
	P2sys - пример авторизации роутера из cgate. ѕовтор€ет в цикле следующие действи€:
	i. ѕосылает ошибочный набор (login, pwd), в ответ получает сообщение logon failed;
	ii. ѕосле этого посылает правильный набор (login, pwd);
	iii. Ќа сообщение об успешной авторизации посылаетс€ запрос на logout;
	iv. ¬озврат к пункту 1.

	P2sys is used for authorize router via cgate. Runs the following commands in loop:
	i. Sends erroneous command set ('login', 'pwd'), receives the 'logon failed' message in reply;
	ii. Sends the correct command set ('login', 'pwd');
	iii. Sends the 'logout' request in reply on successful authorisation message;
	iv. Returns to 1.
*/

using System;
using System.Collections.Generic;
using System.Text;

using System.Runtime.InteropServices;

using ru.micexrts.cgate;
using ru.micexrts.cgate.message;
using System.Threading;

namespace p2sys
{
	class P2Sys
	{
		const UInt32 RouterLogin_msgid = 1;
		const UInt32 RouterLogout_msgid = 2;

		const UInt32 RouterConnected_msgid = 1;
		const UInt32 RouterDisconnected_msgid = 2;
		const UInt32 ConnectionConnected_msgid = 3;
		const UInt32 ConnectionDisconnected_msgid = 4;
		const UInt32 LogonFailed_msgid = 5;

		static string userLogin = null;
		static string userPwd = null;
		static string userLogin2 = null;
		static string userPwd2 = null;

		static bool bExit = false;

		static Publisher publisher = null;

		private static void sendLogoutRequest(Publisher pub)
		{
			Message sendMessage;
			sendMessage = pub.NewMessage(MessageKeyType.KeyId, RouterLogout_msgid);
			DataMessage smsg = (DataMessage)sendMessage;
			smsg["state"].set(1);
			System.Console.WriteLine("posting message dump: {0}", sendMessage);
			pub.Post(sendMessage, 0);
			sendMessage.Dispose();
			Thread.Sleep(1000);
		}

		private static bool sendLoginRequest(Publisher pub, uint cntr)
		{
			string currentLogin, currentPwd;
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
			string loginMsgContents = string.Format(
					"USERNAME={0};PASSWORD={1}",
					currentLogin,
					currentPwd);

			return sendLoginRequest(pub, loginMsgContents);
		}

		private static bool sendLoginRequest(Publisher pub, string login)
		{
			Message sendMessage;
			sendMessage = pub.NewMessage(MessageKeyType.KeyId, RouterLogin_msgid);
			DataMessage smsg = (DataMessage)sendMessage;
			smsg["loginstr"].set(login);
			System.Console.WriteLine("posting message dump: {0}", sendMessage);
			try
			{
				pub.Post(sendMessage, 0);
			}
			catch (CGateException ex)
			{
				if (ex.ErrCode == ErrorCode.More)
				{
					return false;
				}
				else
				{
					throw;
				}
			}
			sendMessage.Dispose();
			Thread.Sleep(1000);
			return true;
		}

		static UInt32 RequestsCntr = 0;

		public static int ClientMessageCallback(Connection conn, Listener listener, Message msg)
		{
			try
			{
				if (msg.Type == MessageType.MsgData)
				{
					DataMessage dataMsg = (DataMessage)msg;
					if (dataMsg.MsgId == RouterConnected_msgid)
					{
						CGate.LogInfo("Router connected");
						sendLogoutRequest(publisher);
					}
					else if (dataMsg.MsgId == RouterDisconnected_msgid)
					{
						CGate.LogInfo("Router disconnected");
						if (sendLoginRequest(publisher, RequestsCntr) == true)
						{
							RequestsCntr++;
						}
					}
					else if (dataMsg.MsgId == ConnectionConnected_msgid)
					{
						CGate.LogInfo("Connection connected");
					}
					else if (dataMsg.MsgId == ConnectionDisconnected_msgid)
					{
						CGate.LogInfo("Connection disconnected");
					}
					else if (dataMsg.MsgId == LogonFailed_msgid)
					{
						CGate.LogInfo("Logon failed");
						if (sendLoginRequest(publisher, RequestsCntr) == true)
						{
							RequestsCntr++;
						}
					}
				}
				else if (msg.Type == MessageType.MsgOpen)
				{
					CGate.LogInfo("Msg Open");
				}
				else if (msg.Type == MessageType.MsgClose)
				{
					CGate.LogInfo("Msg close");
				}
				else
				{
					CGate.LogInfo(String.Format("Message {0}", msg.Type));
				}
				return 0;
			}
			catch (CGateException e)
			{
				return (int)e.ErrCode;
			}
		}

		public static void ConsoleCancelEventHandler(object sender, ConsoleCancelEventArgs e)
		{
			bExit = true;
		}

		public static void Main(string[] args)
		{
			if (args.Length < 2)
			{
				System.Console.WriteLine("Usage: 'user login' 'user pwd' 'user login 2' (optional) 'user pwd 2' (optional)");
				return;
			}

			// Init router login, router password
			userLogin = args[0];
			userPwd = args[1];
			if (args.Length >= 4)
			{
				userLogin2 = args[2];
				userPwd2 = args[3];
			}

			Console.CancelKeyPress += ConsoleCancelEventHandler;

			// Environment is initialized using provided INI-file
			CGate.Open("ini=netp2sys.ini;key=11111111");
	
			CGate.LogInfo("P2Sys example: login, logout router every second.");

			// Defines connection with P2MQRouter which is located at
			// the same machine and listening on port 4001 using TCP protocol
			// Plaza-2 "application name" will be "test_p2sys"
			Connection conn = new Connection("p2sys://127.0.0.1:4001;app_name=test_p2sys");

			// Defines Plaza-2 publisher which allow to connect router to Plaza-2 network
			publisher = new Publisher(conn, "p2sys://;name=p2sys_pub");
			// Defines Plaza-2 router state listener
			Listener listener = new Listener(conn, "p2sys://;name=p2sys_lsn");
			listener.Handler += new Listener.MessageHandler(ClientMessageCallback);

			while (!bExit)
			{
				try
				{
					State state = conn.State;
					if (state == State.Error)
					{
						conn.Close();
					}
					else if (state == State.Closed)
					{
						conn.Open("");
					}
					else if (state == State.Opening)
					{
						ErrorCode result = conn.Process(1);
						if (result != ErrorCode.Ok && result != ErrorCode.TimeOut)
						{
							CGate.LogError(String.Format("Warning: connection state request failed: {0}", CGate.GetErrorDesc(result)));
						}
					}
					else if (state == State.Active)
					{
						ErrorCode result = conn.Process(1);
						if (result != ErrorCode.Ok && result != ErrorCode.TimeOut)
						{
							CGate.LogError(String.Format("Warning: connection state request failed: {0}", CGate.GetErrorDesc(result)));
						}
						// Check publisher state
						if (publisher.State == State.Closed)
						{
							publisher.Open("");
						}
						else if (publisher.State == State.Error)
						{
							publisher.Close();
						}
						if (publisher.State != State.Active)
						{
							continue;
						}

						// Check listener state
						if (listener.State == State.Closed)
						{
							listener.Open("");
						}
						else if (listener.State == State.Error)
						{
							listener.Close();
						}
					}
				}
				catch (CGateException e)
				{
					Console.WriteLine(e.Message);
				}
			}

			if (listener.State != State.Closed)
			{
				listener.Close();
			}
			listener.Dispose();
			if (publisher.State != State.Closed)
			{
				publisher.Close();
			}
			publisher.Dispose();
			if (conn.State != State.Closed)
			{
				conn.Close();
			}
			conn.Dispose();
			CGate.Close();
		}
	}
}
