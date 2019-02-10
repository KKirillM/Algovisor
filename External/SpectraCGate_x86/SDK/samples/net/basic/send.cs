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
	Send - выставл€ет за€вку на FORTS. ¬ыводит в лог ответ торговой системы. ¬ходных аргументов нет.

	Send adds order to FORTS. Saves reply from the trading system into log file. The example does not have input parameters.
*/

using System;
using System.Collections.Generic;
using System.Text;

using System.Runtime.InteropServices;

using ru.micexrts.cgate;
using ru.micexrts.cgate.message;

namespace send
{
	class Send
	{

		static bool bExit = false;

		private static void sendRequest(Publisher pub, int counter)
		{
			SchemeDesc schemeDesc = pub.Scheme;
			MessageDesc messageDesc = schemeDesc.Messages[0]; //AddMM
			FieldDesc fieldDesc = messageDesc.Fields[3];

			Message sendMessage = pub.NewMessage(MessageKeyType.KeyName, "FutAddOrder");

			DataMessage smsg = (DataMessage)sendMessage;
			smsg.UserId = (uint)counter;
			smsg["broker_code"].set("HB00");
			smsg["isin"].set("RTS-6.12");
			smsg["client_code"].set("000");
			smsg["type"].set(1);
			smsg["dir"].set(1);
			smsg["amount"].set(1);
			smsg["price"].set("1700");
			smsg["ext_id"].set(0);

			System.Console.WriteLine("posting message dump: {0}", sendMessage);
			pub.Post(sendMessage, PublishFlag.NeedReply);
			sendMessage.Dispose();
		}

		public static int ClientMessageCallback(Connection conn, Listener listener, Message msg)
		{
			try
			{
				if (msg.Type == MessageType.MsgData)
				{
					Console.WriteLine(String.Format("Client received reply [id: {0}, data: {1}, user-id: {2}, name: {3}]", ((DataMessage)msg).MsgId, ((DataMessage)msg).Data, ((DataMessage)msg).UserId, ((DataMessage)msg).MsgName));
					{
						System.Console.WriteLine("client dump: {0}", msg);
					}
				}
				else if (msg.Type == MessageType.MsgP2MQTimeout)
				{
					Console.WriteLine("Client reply TIMEOUT");
				}
				else
				{
					Console.WriteLine(String.Format("Message {0}", msg.Type));
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
			int counter = 0;
			Console.CancelKeyPress += ConsoleCancelEventHandler;

			CGate.Open("ini=netsend.ini;key=11111111");
			CGate.LogInfo("test .Net log.");
			Connection conn = new Connection("p2tcp://127.0.0.1:4001;app_name=ntest_send");

			Publisher publisher = new Publisher(conn, "p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink;timeout=5000;scheme=|FILE|forts_messages.ini|message");
			Listener listener = new Listener(conn, "p2mqreply://;ref=srvlink");
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
						ErrorCode result = conn.Process(0);
						if (result != ErrorCode.Ok && result != ErrorCode.TimeOut)
						{
							CGate.LogError(String.Format("Warning: connection state request failed: {0}", CGate.GetErrorDesc(result)));
						}
					}
					else if (state == State.Active)
					{
						ErrorCode result = conn.Process(0);
						if (result != ErrorCode.Ok && result != ErrorCode.TimeOut)
						{
							CGate.LogError(String.Format("Warning: connection state request failed: {0}", CGate.GetErrorDesc(result)));
						}

						if (listener.State == State.Closed)
						{
							listener.Open("");
						}
						else if (listener.State == State.Error)
						{
							listener.Close();
						}
						if (publisher.State == State.Closed)
						{
							publisher.Open("");
						}
						else if (publisher.State == State.Error)
						{
							publisher.Close();
						}
						if (listener.State == State.Active && publisher.State == State.Active)
						{
							Console.WriteLine("Sending message to server.");
							sendRequest(publisher, counter++);
							System.Threading.Thread.Sleep(1000);
						}
					}
				}
				catch (CGateException e)
				{
					Console.WriteLine(e.Message);
				}
			}
			conn.Close();
			listener.Close();
			listener.Dispose();
			publisher.Close();
			publisher.Dispose();
			conn.Dispose();
			CGate.Close();
		}
	}
}
