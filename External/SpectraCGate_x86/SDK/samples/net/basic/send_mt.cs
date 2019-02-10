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
	Send_mt - многопоточный пример посылки заявки. (Примечание: компилируется только под компиляторами, поддерживающими C+
	+11.). В треде 1 посылаются заявки.В треде 2 обрабатываются reply на посылаемые заявки.

	Send_mt - a multistream example of sending order. (Attention! This example can be compiled only by compilers supporting C++11.)
	Stream 1 contains orders. Stream 2 contains replies for the orders sent.
*/

using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

using System.Runtime.InteropServices;

using ru.micexrts.cgate;
using ru.micexrts.cgate.message;

namespace send
{
	
	class MessageBase
	{
		protected static bool bExit = false;
		static private Object thisLock = new Object();

		static public bool BExit
		{
			get
			{
				lock (thisLock)
				{
					return BExit;
				}
			}
			set 
			{
				lock (thisLock)
				{
					bExit = value;
				}
			}
		}
	}
	
	class MessageReplyListener : MessageBase
	{
		const int defaultTimeout = 1;
		int cnt = 0;
		public MessageReplyListener(string connSettings, string lsnSettings)
		{
			conn = new Connection(connSettings);
			listener = new Listener(conn, lsnSettings);
			listener.Handler += new Listener.MessageHandler(this.DataEvent);
		}
	
		public void Close()
		{
			listener.Close();
			listener.Dispose();
			conn.Close();
			conn.Dispose();
		}
	
	
		void processListener()
		{
			State state = listener.State;
			if (state == State.Error)
				listener.Close();
			else if (state == State.Closed)
				listener.Open();
		}
	
		void processConnection()
		{
			State state = conn.State;
			if (state == State.Error)
				conn.Close();
			else if (state == State.Closed)
				conn.Open();
			else if (state == State.Active)
			{
				conn.Process(defaultTimeout);
				processListener();
			}
		}
	
		public void run()
		{
			DateTime lastTime = DateTime.Now;
			TimeSpan oneSecond = new TimeSpan(0, 0, 1);
			while(!bExit)
			{
				try
				{
					processConnection();
				}
				catch(CGateException e)
				{
					CGate.LogError(String.Format("Listener run: cgate exception %{0}",e.ToString()));
				}

				TimeSpan span = DateTime.Now - lastTime;
				if (span >= oneSecond)
				{
					CGate.LogInfo("Receive: messages per second " + cnt);
					cnt = 0;
					lastTime = DateTime.Now;
				}
			}
			CGate.LogInfo("Listener: finished");
		}
	
		public int DataEvent(Connection conn, Listener listener, Message msg)
		{
			if (msg.Type == MessageType.MsgData)
			{
				CGate.LogInfo(String.Format("message reply dump: {0}", msg));
				cnt++;
			}
			return 0;
		}
	
		public Connection getConnection()
		{
			return conn;
		}
	
		Connection conn;
		Listener listener;
	}
	
	class MessagePublisher : MessageBase
	{
	
		int cnt = 0;
		public MessagePublisher(Connection conn, string pubSettings)
		{
			this.conn = conn;
			publisher = new Publisher(conn, pubSettings);
		}
	
		public void Close()
		{
			publisher.Close();
			publisher.Dispose();
		}
	
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
	
			CGate.LogInfo(String.Format("posting message dump: {0}", sendMessage));
			pub.Post(sendMessage, PublishFlag.NeedReply);
			sendMessage.Dispose();
		}
	
		void DoLogic()
		{
			try
			{
				sendRequest(publisher, cnt);
				cnt = cnt + 1;
			}
			catch (CGateException e)
			{
				CGate.LogError(String.Format("Publisher do logic: cgate exception %{0}", e.ToString()));
				cnt = cnt - 1;
			}
		}
	
		void processPublisher()
		{
			State state = publisher.State;
			if (state == State.Error)
				publisher.Close();
			else if (state == State.Closed)
				publisher.Open();
			else if (state == State.Active)
				DoLogic();
		}
	
		public void run(Object arg)
		{
			CountdownEvent startEvent = (CountdownEvent)arg;
			startEvent.Signal();
			startEvent.Wait();

			DateTime lastTime = DateTime.Now;
			TimeSpan oneSecond = new TimeSpan(0, 0, 1);
			CGate.LogInfo("Thread started");
			while (!bExit)
			{
				try
				{
					processPublisher();
				}
				catch (CGateException e)
				{
					CGate.LogError(String.Format("Publisher run: cgate exception %{0}", e.ToString()));
				}

				TimeSpan span = DateTime.Now - lastTime;
				if (span >= oneSecond)
				{
					CGate.LogInfo("Post: messages per second " + cnt);
					cnt = 0;
					lastTime = DateTime.Now;
				}
			}
			CGate.LogInfo("Publisher: finished");
		}
		
		Connection conn;
		Publisher publisher;
	}
		
	class SendMt
	{
	
		static MessageReplyListener replyListener = null;
		const int nThreads = 2;
		static CountdownEvent startEvent = new CountdownEvent(nThreads);
	
		public static void ConsoleCancelEventHandler(object sender, ConsoleCancelEventArgs e)
		{
			MessageBase.BExit = true;
			CGate.LogInfo(" - - - BREAK - - - ");
		}
	
		public static void Main(string[] args)
		{
			Console.CancelKeyPress += ConsoleCancelEventHandler;
			CGate.Open("ini=netsend_mt.ini;key=11111111");
			CGate.LogInfo("test .Net log.");

			MessageReplyListener replyListener = new MessageReplyListener("p2tcp://127.0.0.1:4001;app_name=send_mt", "p2mqreply://;ref=srvlink0");
			MessagePublisher[] publishers = new MessagePublisher[nThreads];
			for (int i = 0; i < nThreads; ++i)
			{
				String name = String.Format("p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink{0};timeout=5000;scheme=|FILE|forts_messages.ini|message", i);
				publishers[i] = new MessagePublisher(
					replyListener.getConnection(), 
					String.Format("p2mq://FORTS_SRV;category=FORTS_MSG;name=srvlink{0};timeout=5000;scheme=|FILE|forts_messages.ini|message", i));
			}
			
			try
			{
				Thread replyThread = new Thread(new ThreadStart(replyListener.run));
				replyThread.Start();

				Thread[] sendThreads = new Thread[nThreads];
				for (int i = 0; i < nThreads; ++i)
				{
					sendThreads[i] = new Thread(new ParameterizedThreadStart(publishers[i].run));
					sendThreads[i].Start(startEvent);
				}

				replyThread.Join();
				for (int i = 0; i < nThreads; ++i)
				{
					sendThreads[i].Join();
				}
				CGate.LogInfo("All threads joined");
			}
			catch (CGateException e)
			{
				CGate.LogError(String.Format("Main: cgate exception %{0}", e.ToString()));
			}

			for (int i = 0; i < nThreads; ++i)
			{
				publishers[i].Close();
			}
			replyListener.Close();
			
			CGate.LogInfo("Finish");
			CGate.Close();
		}
	}
}
