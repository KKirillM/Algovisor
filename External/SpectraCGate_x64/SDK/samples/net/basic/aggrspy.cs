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
	Настоящий пример показывает получение агрегированного стакана для определенного инструмента,
	чей isin_id указан в аргументах командной строки.
	Также указываются максимальное количество выводимых заявок (глубина стакана, 
	0 - для вывода без ограничений), название выходного файла и признак реверсивной сортировки (r).

	This sample represents getting an aggregate order book for specific instrument 
	whose isin_id is set in command line arguments.
	The other arguments are: order book depth (max quantity of elements in output, 0 for unlimited output),
	output file name and reverse sort flag (r).
*/

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

using ru.micexrts.cgate;
using System.Runtime.InteropServices;
using ru.micexrts.cgate.message;

namespace aggrspy
{
	enum Direction
	{
		Buy, Sell
	};

	// номер ревизии таблицы. соответствует данным CG_MSG_P2REPL_CLEARDELETED
	// table revision number. corresponding to CG_MSG_P2REPL_CLEARDELETED data
	class RevisionInfo
	{
		private UInt32 tableIndex;
		private Int64 tableRevision;
	
		public UInt32 TableIndex
		{
			get
			{
				return tableIndex;
			}
		}
	
		public Int64 TableRevision
		{
			get
			{
				return tableRevision;
			}
		}
	
		public RevisionInfo(UInt32 index, Int64 revision)
		{
			this.tableIndex = index;
			this.tableRevision = revision;
		}
	
		public RevisionInfo(P2ReplClearDeletedMessage message)
		{
			this.tableIndex = message.TableIdx;
			this.tableRevision = message.TableRev;
		}
	
		public override string ToString() 
		{
			return String.Format("table = {0}, rev = {1}",
				this.tableIndex, this.tableRevision);
		}
	};

	class CGateDisconnectException : CGateException
	{
		public CGateDisconnectException(string message)
			: base(message) { }
		public CGateDisconnectException(ErrorCode errorCode, string message)
			: base(errorCode, message) { }
		public CGateDisconnectException(int errorCode, string message)
			: base(errorCode, message) { }
	};

	// вспомогательный класс для создания логов
	//
	// helper class used for log messages creation
	class MessageLogHelper
	{
		static string MessagePrefix(MessageType type)
		{
			return type.ToString() + ". ";
		}

		public static string CreateMessageString(Message message)
		{
			return MessagePrefix(message.Type);
		}

		public static string CreateMessageString(P2ReplClearDeletedMessage message)
		{
			return MessagePrefix(message.Type) + new RevisionInfo(message).ToString();
		}

		public static string CreateMessageString(StreamDataMessage message)
		{
			return CreateMessageString(new OrderAggr(message));
		}

		public static string CreateMessageString(OrderAggr order)
		{
			return MessagePrefix(MessageType.MsgStreamData) + order.ToString();
		}
	};

	// даннные агрегированной заявки
	//
	// aggregate order data
	class OrderAggr
	{
		double price;
		Int32 amount;
		RevisionInfo revision;
		Direction direction;
		Int32 isinId;
		Int64 replId;
		Int64 replAct;

		public OrderAggr(double price, Int32 amount, RevisionInfo rev, 
			Direction direction, Int32 isinId, Int64 replId, Int64 replAct)
		{
	
			this.price = price;
			this.amount = amount;
			this.revision = rev;
			this.direction = direction;
			this.isinId = isinId;
			this.replId = replId;
			this.replAct = replAct;
		}

		// чтение данных заявки из stream_data message
		//
		// reading order data from stream_data message
		public OrderAggr(StreamDataMessage message)
		{
			this.price = message[AggrSpyApp.SchemeInfo.FieldNames.Price].asDouble();
			this.amount = message[AggrSpyApp.SchemeInfo.FieldNames.Amount].asInt();
			Int64 tableRevision = message[AggrSpyApp.SchemeInfo.FieldNames.ReplRevision].asLong();
			this.revision = new RevisionInfo((uint)message.MsgIndex, tableRevision);
		
			Byte intDir = message[AggrSpyApp.SchemeInfo.FieldNames.Direction].asByte();
			this.direction = (intDir == 1) ? Direction.Buy : Direction.Sell;
			this.isinId = message[AggrSpyApp.SchemeInfo.FieldNames.IsinId].asInt();
			this.replAct = message[AggrSpyApp.SchemeInfo.FieldNames.ReplAct].asLong();
			this.replId = message[AggrSpyApp.SchemeInfo.FieldNames.ReplId].asLong();
		}
				
		public double Price
		{
			get 
			{
				return this.price;
			}
		}
	
		public Int32 Amount
		{
			get
			{
				return this.amount;
			}
			set
			{
				this.amount = value;
			}
		}
	
		public RevisionInfo Revision
		{
			get
			{
				return this.revision;
			}
		}
	
		public Direction Direction
		{
			get
			{
				return this.direction;
			}
		}
	
		public Int32 IsinId
		{
			get
			{
				return this.isinId;
			}
		}

		public Int64 ReplId
		{
			get
			{
				return this.replId;
			}
		}

		public Int64 ReplAct
		{
			get
			{
				return this.replAct;
			}
		}
	
		public override string ToString()
		{
			return String.Format(
				"Price = {0:F2}, amount = {1}, table = {2}, rev = {3}, Dir = {4}, IsinId = {5}, ReplAct = {6}",
				this.price, 
				this.amount, 
				this.revision.TableIndex, 
				this.revision.TableRevision,
				this.direction, 
				this.isinId, 
				this.replAct);
		}
	};

	// вспомогательная функция для сравнения цены покупки. лучшая цена - самая большая
	//
	// helper function object for buying price comparison. bigger price is better
	class OrderComparatorBuy : IComparer<double>
	{
		public int Compare(double x, double y)
		{
			return -Comparer<double>.Default.Compare(x, y);
		}
	};

	// вспомогательная функция для сравнения цены продажи. лучшая цена - самая маленькая
	//
	// helper function object for selling price comparison. less price is better
	class OrderComparatorSell : IComparer<double>
	{
		public int Compare(double x, double y)
		{
			return Comparer<double>.Default.Compare(x, y);
		}
	};

	// сортированный в указанном направлении контейнер для агрегированных заявок
	//
	// sorted in specified direction container for aggregate orders 
	class Orders
	{
		static IComparer<double>[] comparers;
		Direction direction;
		// содержит агрегированные заявки (суммарный объем заявок с одинаковой ценой)
		// по соображениям производительности сортировка 
		// контейнера происходит только при печати содержимого
		// 
		// aggregate orders
		// by perfomance reasons container sorting occurs only when printing
		Dictionary<Int64, OrderAggr> aggregateOrders;
	
		static Orders()
		{
			comparers = new IComparer<double>[2];
			comparers[(int)Direction.Buy] = new OrderComparatorBuy();
			comparers[(int)Direction.Sell] = new OrderComparatorSell();
		}
	
		public Orders(Direction dir)
		{
			this.aggregateOrders = new Dictionary<Int64, OrderAggr>();
			this.direction = dir;
		}
	
		//private OrderAggr FindOrderByPrice(double price)
		//{
		//	OrderAggr order = null;
		//	try
		//	{
		//		order = this.aggregateOrders[price];
		//	}
		//	catch (KeyNotFoundException) { }
		//	return order;
		//}

		// добавление агрегированной заявки или изменение существующей, 
		// если найдется заявка с равной ценой
		//
		// place new aggregate order or alter existing if found equal price orderFAmount
		public void Add(OrderAggr order)
		{
			if (order.ReplAct != 0)
			{
				CGate.LogDebug(String.Format("Delete record {0}.", order.Price));
				this.aggregateOrders.Remove(order.ReplId);
			}
			try
			{
				OrderAggr existingOrder = this.aggregateOrders[order.ReplId];
				if (order.Amount == 0)
					this.aggregateOrders.Remove(order.ReplId);
				else
					this.aggregateOrders[order.ReplId] = order;
			}
			catch(KeyNotFoundException )
			{
				this.aggregateOrders.Add(order.ReplId, order);
			}
		}

		// удалить все заявки
		//
		// delete all orders
		public void Clear()
		{
			this.aggregateOrders.Clear();
		}

		// обработчик CG_MSG_P2REPL_CLEARDELETED
		// удалить все заявки с ревизией, старшей, чем указанная
		//
		// CG_MSG_P2REPL_CLEARDELETED handler
		// delete all orders with revision older than specified
		public void OnClearDeleted(RevisionInfo rev)
		{
			List<Int64> keysToRemove = new List<Int64>();
			foreach (KeyValuePair<Int64, OrderAggr> p in this.aggregateOrders)
			{
				if ((p.Value.Revision.TableIndex == rev.TableIndex) &&
					(p.Value.Revision.TableRevision < rev.TableRevision))
				{
					keysToRemove.Add(p.Key);
				}
			}
			foreach (Int64 key in keysToRemove)
			{
				this.aggregateOrders.Remove(key);
			}
		}

		// записать содержимое агрегированных заявок в текстовый файл
		//
		// write aggregate contents to text file
		public void Print(TextWriter writer, uint depth, bool reverse)
		{
			if (depth == 0)
			{
				depth = UInt32.MaxValue;
			}

			SortedDictionary<double, Int32> sortedAggr = new SortedDictionary<double,int>();

			foreach (KeyValuePair<Int64, OrderAggr> item in this.aggregateOrders)
			{
				if (item.Value.Price > 0)
					sortedAggr.Add(item.Value.Price, item.Value.Amount);
			}

			foreach (KeyValuePair<double, Int32> item in sortedAggr)
			{
				writer.WriteLine("\t\tprice = {0:F2}\tamount = {1}", item.Key, item.Value);
			}
		}
	};

	// стакан для определенного инструмента
	// содержит по одному контейнеру 'Orders' для заявок на покупку и продажу
	//
	// order book for single instrument
	// contains one 'Orders' container for buy orders and one for sell orders
	class OrderBook
	{
		Int32 isinId;
		Orders[] orders;
	
		public OrderBook(Int32 isinId)
		{
			this.isinId = isinId;
			this.orders = new Orders[2];
			this.orders[(int)Direction.Buy] = new Orders(Direction.Buy);
			this.orders[(int)Direction.Sell] = new Orders(Direction.Sell);
		}
	
		public void Add(OrderAggr order)
		{
			this.orders[(int)order.Direction].Add(order);
		}
	
		public void Clear()
		{
			this.orders[(int)Direction.Buy].Clear();
			this.orders[(int)Direction.Sell].Clear();
		}
	
		public void OnClearDeleted(RevisionInfo rev)
		{
			this.orders[(int)Direction.Buy].OnClearDeleted(rev);
			this.orders[(int)Direction.Sell].OnClearDeleted(rev);
		}

		public void Print(Direction direction, TextWriter writer, uint depth, bool reverse)
		{
			this.orders[(int)direction].Print(writer, depth, reverse);
		}
	};
	
	// основной объект - приложение AggrSpyApp
	// реализует логику обработки сообщений
	//
	// main aggr spy application
	// implements message callback and other message handling functions
	public class AggrSpyApp
	{
		static bool bExit = false;
	
		int sessionId;
		Connection connection;
		Listener listener;
		TextWriter msgLogFile;
		OrderBook orderBook;
		uint targetIsinId;
		uint depth;
		string outfile;
		bool reverse;
		Int64 lastRevision;

		// описание серверной схемы
		//
		// server scheme description
		public struct SchemeInfo
		{
			// индексы таблиц в схеме
			// 
			// indices of tables in scheme
			public struct TableIndices
			{
				public long OrdersAggr;
			};
			public TableIndices Indices;

			// названия таблиц в схеме
			//
			// names of tables in scheme
			public struct TableNames
			{
				public const string OrdersAggr = "orders_aggr";
			};

			// названия полей в таблицах
			//
			// names of fields in tables
			public struct FieldNames
			{
				public const string ReplId = "replID";
				public const string ReplRevision = "replRev";
				public const string ReplAct = "replAct";
				public const string IsinId = "isin_id";
				public const string Price = "price";
				public const string Amount = "volume";	// The database table has a field containing amount data but called as "volume"
				public const string Moment = "moment";
				public const string Direction = "dir";
				public const string Timestamp = "timestamp";
				public const string SessionId = "sess_id";
			};
	
			public bool Ready;
			
			public const Int32 StatusMessageNonSystem = 0x4;
			public const Int32 StatusEndBusinessTransaction = 0x1000;
	
		};
		SchemeInfo schemeInfo;
	
		SchemeDesc Scheme
		{
			get
			{
				return this.listener.Scheme;
			}
		}
	
		public static bool Exit
		{
			get
			{
				return bExit;
			}
		}

		public AggrSpyApp(Connection conn, String settings, uint targetIsinId, 
			uint depth, string outfile, bool reverse)
		{
			this.orderBook = new OrderBook((int)targetIsinId);
			this.sessionId = 0;
			this.schemeInfo.Ready = false;
			this.schemeInfo.Indices.OrdersAggr = -1;
	
			this.connection = conn;
			this.listener = new Listener(conn, settings);
			this.listener.Handler += new Listener.MessageHandler(OrdbookMessageHandler);
			this.listener.Open();
			if ((this.listener.State != State.Active) && ((this.listener.State != State.Opening)))
			{
				throw new CGateException("Unable to connect");
			}
	
			this.msgLogFile = new StreamWriter("aggrspy.NET msg.log");
			this.targetIsinId = targetIsinId;
			this.depth = depth;
			this.outfile = outfile;
			this.reverse = reverse;
			this.lastRevision = 0;
		}
	
		~AggrSpyApp()
		{
			try 
			{
				this.listener.Close();
			}
			catch(CGateException ex)
			{
				CGate.LogError(ex.Message);
			}
			catch(Exception)    {   }
	
			this.Cleanup();
		}
	
		public static void ConsoleCancelEventHandler(object sender, ConsoleCancelEventArgs e)
		{
			bExit = true;
		}
	
#region logging
		void LogMessageToFile(Message message)
		{
			this.msgLogFile.WriteLine(MessageLogHelper.CreateMessageString(message));
		}
	
		void LogMessageToFile(P2ReplClearDeletedMessage message)
		{
			this.msgLogFile.WriteLine(MessageLogHelper.CreateMessageString(message));
		}
	
		void LogMessageToFile(StreamDataMessage message)
		{
			this.msgLogFile.WriteLine(MessageLogHelper.CreateMessageString(message));
		}
	
		void LogMessageToFile(OrderAggr order)
		{
			this.msgLogFile.WriteLine(MessageLogHelper.CreateMessageString(order));
		}
	
		void LogMessage(Message message)
		{
			CGate.LogInfo(MessageLogHelper.CreateMessageString(message));
			LogMessageToFile(message);
		}
	
		void LogMessage(P2ReplClearDeletedMessage message)
		{
			CGate.LogInfo(MessageLogHelper.CreateMessageString(message));
			LogMessageToFile(message);
		}
	
		void LogMessage(StreamDataMessage message)
		{
			CGate.LogInfo(MessageLogHelper.CreateMessageString(message));
			LogMessageToFile(message);
		}
	
		void LogMessage(OrderAggr order)
		{
			CGate.LogInfo(MessageLogHelper.CreateMessageString(order));
			LogMessageToFile(order);
		}
#endregion logging

		// основной обработчик сообщений
		//
		// main message callback function
		int OrdbookMessageHandler(Connection connection, Listener listener, Message message)
		{
			switch (message.Type)
			{
				// открытие нового соединения с сервером
				// инициализировать схему
				//
				// opening new server connection
				// initialize scheme description
				case MessageType.MsgOpen:
					{
						this.LogMessage(message);
						this.Cleanup();
	
						for (int i = 0; i < this.listener.Scheme.Messages.Length; ++i)
						{
							MessageDesc md = this.listener.Scheme.Messages[i];
							if (md.Name == SchemeInfo.TableNames.OrdersAggr)
							{
								this.schemeInfo.Indices.OrdersAggr = i;
							}
						}
	
						if (this.schemeInfo.Indices.OrdersAggr >= 0)
						{
							this.schemeInfo.Ready = true;
						}
						break;
					}

				// обрабатывать только таблицу FUTAGGR50.orders_aggr
				// пропускать остальные таблицы
				// пропускать сообщения для инструментов, отличных от _target_isin_id
				//
				// process only FUTAGGR50.orders_aggrtable
				// skip other tables
				// skip messages for instruments other than _target_isin_id
				case MessageType.MsgStreamData:
					{
						if (!this.schemeInfo.Ready)
						{
							break;
						}
						StreamDataMessage sdMessage = (StreamDataMessage)message;
						if (sdMessage.MsgIndex == this.schemeInfo.Indices.OrdersAggr)
						{
							UInt32 isinId = sdMessage[SchemeInfo.FieldNames.IsinId].asUInt();
							if (isinId != this.targetIsinId)
							{
								break;
							}

							Int32 sessionId = sdMessage[SchemeInfo.FieldNames.SessionId].asInt();
							if (sessionId < this.sessionId)
							{
								break;
							}
							this.UpdateSessionId(sessionId);
	
							OrderAggr order = new OrderAggr(sdMessage);
							this.LogMessage(order);
	
							this.orderBook.Add(order);
							this.lastRevision = order.Revision.TableRevision;
						}
						break;
					}

				// данное сообщение говорит о получении полной реплики базы данных
				// с этого момента мы будем получать обновления
				//
				// this message tells that we have completed database replica
				// since this moment we shall receive updates only
				case MessageType.MsgP2ReplOnline:
					{
						this.Print("Online.txt");
						this.LogMessage(message);
						break;
					}

				// очистить все данные при смене номера жизни
				//
				// clear all data when life number is changed
				case MessageType.MsgP2ReplLifeNum:
					{
						this.LogMessage(message);
						this.Cleanup();
						break;
					}

				// удалить все заявки с ревизией, старшей, чем указанная
				//
				// clear older orders
				case MessageType.MsgP2ReplClearDeleted:
					{
						P2ReplClearDeletedMessage cdMessage = (P2ReplClearDeletedMessage)message;
						this.LogMessage(cdMessage);
						
						if (cdMessage.TableIdx == this.schemeInfo.Indices.OrdersAggr)
						{
							RevisionInfo revision = new RevisionInfo(cdMessage.TableIdx, cdMessage.TableRev);

							this.orderBook.OnClearDeleted(revision);

							if (cdMessage.TableRev == P2ReplClearDeletedMessage.MaxRevision)
							{
								this.lastRevision = 0;
							}
	
							//this.Print(String.Format(
							//	"aggrspy.NET ClearDeleted table {0} revision {1}.log",
							//	revision.TableIndex,
							//	revision.TableRevision));
						}
						break;
					}

				// закрытие соединения 
				// записать содержимое стакана в файл
				// произвести очистку всех данных
				//
				// closing current connection
				// write order book contents into file
				// perform complete cleanup
				case MessageType.MsgClose:
					{
						this.LogMessage(message);
						this.Print(this.outfile);
						this.Cleanup();
						break;
					}
			}
	
			return (int)ErrorCode.Ok;
		}
	
		void Cleanup()
		{
			this.orderBook.Clear();
		}
	
		bool UpdateSessionId(Int32 id)
		{
			// если номер сессии сменился на следующий
			// if sess_id is changed to next
			if (id > this.sessionId)
			{
				// исключаем случай, когда sess_id еще не был инициализирован
				// удаляем все данные с предыдущей сессии
				// avoiding an occurence when sess_id not being initialized yet
				// deleting all data from previous session
				if (this.sessionId > 0)
				{
					this.Print(String.Format(
						"aggrspy.NET session {0}.log", this.sessionId));
					this.Cleanup();
				}
				this.sessionId = id;
				return true;
			}
			else
			{
				return false;
			}
		}

		// основной цикл приложения
		//
		// main application loop body
		ErrorCode Run()
		{
			if (this.connection.State != State.Active)
			{
				CGate.LogError(String.Format(
					"Error: connection state is {0}", this.connection.State));
			}
			ErrorCode result = ErrorCode.Ok;
			switch (this.connection.State)
			{
				case State.Error:
					{
						this.connection.Close();
						result = ErrorCode.IncorrectState;
						break;
					}
	
				case State.Closed:
					{
						try
						{
							this.connection.Open();
						}
						catch(CGateException ex)
						{
							CGate.LogError(ex.Message);
							result = ErrorCode.Internal;
						}
						break;
					}
	
				case State.Active:
					{
						result = this.connection.Process(0);
						if ((result != ErrorCode.Ok) && (result != ErrorCode.TimeOut))
						{
							CGate.LogInfo(String.Format(
								"Warning: connection state request failed: {0}", result));
						}
	
						switch (this.listener.State)
						{
							case State.Closed:
								this.listener.Open();
								break;
	
							case State.Error:
								this.listener.Close();
								throw new CGateDisconnectException("Disconnect");
						}
						break;
					}
					
			}
			return result;
		}

		// записать содержимое стакана в текстовый файл
		//
		// write current order books content to text file
		void Print(string fileName)
		{
			TextWriter writer = new StreamWriter(fileName);
			writer.WriteLine("BUY:");
			this.orderBook.Print(Direction.Buy, writer, this.depth, this.reverse);
			writer.WriteLine("SELL:");
			this.orderBook.Print(Direction.Sell, writer, this.depth, this.reverse);
			writer.Close();
		}

		// записать содержимое стакана в текстовый файл
		// добавить в название файла номер текущей ревизии
		//
		// write current order books content to text file
		// add current revision number to file name
		void PrintLastRevision()
		{
			this.Print("aggrspy " + this.targetIsinId + "." + this.lastRevision + ".log");
		}

		public static void Main(string[] args)
		{
			uint targetIsinId = 0;
			uint depth = 0;
			string outfile = "";
			try
			{
				targetIsinId = Convert.ToUInt32(args[0]);
				depth = Convert.ToUInt32(args[1]);
				outfile = args[2];
			}
			catch (Exception)
			{
				Console.WriteLine("Usage:\n\aggrspy ISIN_ID depth outfile r(optional)\n");
				Console.WriteLine("where\n\tISIN_ID is unique instrument ID (e.g. 167566)");
				Console.WriteLine("\tdepth is max orderbook size (0 for unlimited size)");
				Console.WriteLine("\toutfile is output file path");
				Console.WriteLine("\tr for reverse sorting");
				return;
			}
			bool reverse = false;
			try
			{
				reverse = (args[3] == "r");
			}
			catch (Exception) { }

			Console.CancelKeyPress += ConsoleCancelEventHandler;

			CGate.Open("ini=netaggrspy.ini;key=11111111;minloglevel=trace");
			Connection conn = new Connection("p2tcp://127.0.0.1:4001;app_name=aggrspy_net");
			try
			{
				conn.Open();
			}
			catch (CGateException ex)
			{
				CGate.LogError(ex.Message);
				return;
			}

			String settings = "p2repl://FORTS_FUTAGGR50_REPL";

			AggrSpyApp theApp = new AggrSpyApp(
				conn, settings, targetIsinId, depth, outfile, reverse);
	
			while (!Exit)
			{
				try
				{
					theApp.Run();
				}
				catch(CGateDisconnectException)
				{
					CGate.LogInfo("Disconnection");
				}
				if (Console.KeyAvailable)
				{
					// print OrderBook on "Enter"
					if (Console.ReadKey().KeyChar == '\r')
					{
						theApp.PrintLastRevision();
					}
				}
			}
		}
	};
}
