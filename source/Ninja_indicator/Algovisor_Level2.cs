#region Using declarations
using System;
using System.Runtime;   // я добавил
using System.Runtime.InteropServices; // я добавил
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Xml.Serialization;
using System.Collections.Generic;
using System.IO.Pipes;
using NinjaTrader.Cbi;
using NinjaTrader.Data;
using System.Text;
using System.IO;
using System.Threading;
//using System.Windows.Forms;
#endregion

// This namespace holds all indicators and is required. Do not change it.


namespace NinjaTrader.Indicator
{
    [Description("www.alogvisor.com")]
    public class Algovisor_Level2 : Indicator
    {		
		[DllImport("Kernel32.dll")]
    	public static extern void GetLocalTime([In, Out] SystemTime st);

		public override string ToString()
		{
			return "Algovisor_Level2";
		}
		
		byte [] StructureToByteArray(object obj)
		{
			int len = Marshal.SizeOf(obj);
			byte [] arr = new byte[len];
			IntPtr ptr = Marshal.AllocHGlobal(len);
			Marshal.StructureToPtr(obj, ptr, true);
			Marshal.Copy(ptr, arr, 0, len);
			Marshal.FreeHGlobal(ptr);
			return arr;
		}

        private void AsyncSend(IAsyncResult iar)
        {
            try
            {
                // Get the pipe
                NamedPipeClientStream pipeStream = (NamedPipeClientStream)iar.AsyncState;

                // End the write
                pipeStream.EndWrite(iar);
               // pipeStream.Flush();
               // pipeStream.Close();
               // pipeStream.Dispose();
            }
            catch (Exception oEX)
            {
                //Debug.WriteLine(oEX.Message);
				//pipeStream = null;
				return;
            }
        }
		
		private void AsyncSendOnTermination(IAsyncResult iar)
        {
            try
            {
                // Get the pipe
                NamedPipeClientStream pipeStream = (NamedPipeClientStream)iar.AsyncState;

                // End the write
                pipeStream.EndWrite(iar);
                pipeStream.Flush();
                pipeStream.Close();
                pipeStream.Dispose();
            }
            catch (Exception oEX)
            {
                //Debug.WriteLine(oEX.Message);
				//pipeStream = null;
				return;
            }
        }
		
        #region Variables
		private Instrument_DATA id;
		private Trade_DATA td;
		private BBO_DATA bd;
		private DOM_DATA dd;
		private DELETE_INSTRUMENT di;
		private SystemTime st;
		private string AlgoWindow;
		private string ticker;
		private NamedPipeClientStream pipeStream;
		private bool firstSend;
		private	bool secondIndicator;
		private Mutex mutexObj;
        private bool mutexOwner;
		private string pipeName;
		#endregion

//		#region Properties		
//		[XmlIgnore()]
//		[GridCategory("Parameters")]
//		public Instrument_DATA SerializeFunction
//		{
//			get { return id; }
//		}
		
//		[Browsable(false)]
//		public Instrument_DATA SerializeFunction
//		{
//			get { return id; }
//		}
//        #endregion
//		[Description("If \"Enable\" than market depth data available in Algovisor")]	
//		[Category("Instrument Data type")]
//		[NinjaTrader.Gui.Design.DisplayName("MarketDepth Data")]		
//		public bool DataType
//		{
//			get{ return DOMDATA; }
//			set
//			{ 
//				DOMDATA = value; 
//				OnStartUp();
//			}
//		}
        //#endregion
		
		#region Initialize
        protected override void Initialize()
        {
			CalculateOnBarClose	= true;
            Overlay				= true;
			DrawOnPricePanel	= true;
			PaintPriceMarkers 	= false;
            PriceTypeSupported	= false;
			PlotsConfigurable 	= false;
			pipeName = "Global\\ALGOVISOR_PIPE_SERVER";
			pipeStream = new NamedPipeClientStream(".", pipeName, PipeDirection.Out, PipeOptions.WriteThrough);
			st = new SystemTime();
			AlgoWindow = "Algovisor";
			firstSend = false;
            mutexOwner = false;
			mutexObj = null;
			
		}
		#endregion
		
		#region StartUp
		
		private bool IsSecond()
		{
			try 
			{
				Mutex m = Mutex.OpenExisting(Instrument.Id);
				m.Close();
				if (!mutexOwner)
				{
					secondIndicator = true;
					return true;
				}
				else
					return false;
			}
			catch(WaitHandleCannotBeOpenedException oEx)
			{
                mutexObj = new Mutex(true, Instrument.Id, out mutexOwner);
				return false;
			}
		}
			
		protected override void OnStartUp()
		{
            if (IsSecond())
				return;
			
            try
            {
                Mutex m = Mutex.OpenExisting("Algovisor-Ninjatrader");
				m.Close();
            }
            catch (WaitHandleCannotBeOpenedException oEx)
            {
				if (pipeStream != null)
				{
					pipeStream.Close();
					pipeStream = null;
				}
                firstSend = false;				
                return;
            }
			
			if (pipeStream == null)
				pipeStream = new NamedPipeClientStream(".", pipeName, PipeDirection.Out, PipeOptions.WriteThrough);
			
			if (!pipeStream.IsConnected)
			{
				try
				{
					pipeStream.Connect(1);
				}
				catch (TimeoutException oEX)
				{
					return;
				}
			}
				
			if (pipeStream != null && pipeStream.IsConnected)
			{

				id.value = 101;
				id.ID = Instrument.Id;
				ticker = Instrument.FullName;
				id.ticker = ticker;
				id.expiration = Instrument.Expiry.ToLongDateString();
				id.description = Instrument.MasterInstrument.Description.Normalize();
				id.tickSize = Instrument.MasterInstrument.TickSize;
				id.exchange = Instrument.Exchange.ToString();
				id.type = true;
				id.securityString = "Algovisor-Ninjatrader";

				try
				{
					//Write from client to server
					//StreamWriter sw = new StreamWriter(pipeStream);
					//sw.WriteLine("Hello");

					//string SendStr = "Test1";
					//byte[] _buffer = Encoding.UTF8.GetBytes(SendStr);
					byte[] buffer = StructureToByteArray(id);
					pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSend), pipeStream);
				}
				catch (TimeoutException oEX)
				{
					return;
				}
				
				firstSend = true;
			}
		}
		#endregion
		
		#region On Event Updates
		protected override void OnBarUpdate()
		{
		}
		
        protected override void OnMarketData(MarketDataEventArgs e)
        {	
			if (secondIndicator)
				return;
			
			try
            {
                Mutex m = Mutex.OpenExisting("Algovisor-Ninjatrader");
				m.Close();
            }
            catch (WaitHandleCannotBeOpenedException oEx)
            {
                if (pipeStream != null)
				{
					pipeStream.Close();
					pipeStream = null;
				}
                firstSend = false;
                return;
            }
				
			if (!firstSend)
				OnStartUp();
			else 
			{
				if (pipeStream == null)
					pipeStream = new NamedPipeClientStream(".", pipeName, PipeDirection.Out, PipeOptions.WriteThrough);
				
				if (!pipeStream.IsConnected)
				{
					try
					{
						pipeStream.Connect(1);
					}
					catch (TimeoutException oEX)
					{
						//pipeStream.Close();
						//pipeStream = null;
						return;
					}
				}
			}
			
			if (pipeStream != null && pipeStream.IsConnected)
			{
				if (e.MarketDataType == MarketDataType.Last)
				{
					td.value = 102;
					td.ticker = ticker;
					td.Price = e.Price;
					td.Volume = e.Volume;
					GetLocalTime(st);
					td.hour = st.hour;
					td.minute = st.minute;
					td.second = st.second;
					td.millisecond = st.millisecond;
					
					try
					{
						byte[] buffer = StructureToByteArray(td);
						pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSend), pipeStream);
					}
					catch (TimeoutException oEX)
					{
						return;
					}					
				}
				else if (e.MarketDataType == MarketDataType.Ask)
				{
					bd.value = 103;
					bd.ticker = Instrument.FullName;
					bd.Price = e.Price;
					
					try
					{
						byte[] buffer = StructureToByteArray(bd);
						pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSend), pipeStream);
					}
					catch (TimeoutException oEX)
					{
						return;
					}
				}
				else if (e.MarketDataType == MarketDataType.Bid)
				{
					bd.value = 104;
					bd.ticker = Instrument.FullName;
					bd.Price = e.Price;
					
					try
					{
						byte[] buffer = StructureToByteArray(bd);
						pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSend), pipeStream);
					}
					catch (TimeoutException oEX)
					{
						return;
					}
				}
			}					
        }
		
		protected override void OnMarketDepth(MarketDepthEventArgs e)
        {
			if (secondIndicator)
				return;
			
			try
            {
                Mutex m = Mutex.OpenExisting("Algovisor-Ninjatrader");
				m.Close();
            }
            catch (WaitHandleCannotBeOpenedException oEx)
            {
               	if (pipeStream != null)
				{
					pipeStream.Close();
					pipeStream = null;
				}
                firstSend = false;
                return;
            }
				
			if (!firstSend)
				OnStartUp();
			else
			{
				if (pipeStream == null)
					pipeStream = new NamedPipeClientStream(".", pipeName, PipeDirection.Out, PipeOptions.WriteThrough);
				
				if (!pipeStream.IsConnected)
				{
					try
					{
						pipeStream.Connect(1);
					}
					catch (TimeoutException oEX)
					{
						//pipeStream.Close();
						//pipeStream = null;
						return;
					}
				}
			}
		
			if (pipeStream != null && pipeStream.IsConnected)
			{
				dd.value = 105;
				dd.ticker = Instrument.FullName;		
				if (e.MarketDataType == MarketDataType.Ask)
					dd.agressor = 'a';				
				else if (e.MarketDataType == MarketDataType.Bid)
					dd.agressor = 'b';	
				dd.pos = e.Position;
				dd.Price = e.Price;
				dd.Volume = e.Volume;
				GetLocalTime(st);				
				dd.hour = st.hour;
				dd.minute = st.minute;
				dd.second = st.second;
				dd.millisecond = st.millisecond;
				dd.type = '-';
				if (e.Operation == Operation.Insert)
					dd.type = 'i';
				else if (e.Operation == Operation.Remove)
					dd.type = 'r';
				else if (e.Operation == Operation.Update)
					dd.type = 'u';		
			
				try
				{
					byte[] buffer = StructureToByteArray(dd);
					pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSend), pipeStream);
				}
				catch (TimeoutException oEX)
				{
					return;
				}
			}
        }
		#endregion
		
		#region Termination
		protected override void OnTermination()
		{
			if (mutexObj != null)
			{
				mutexObj.ReleaseMutex();
				mutexObj.Close();
			}
				
			if (secondIndicator)
				return;
			
			try
            {
                Mutex m = Mutex.OpenExisting("Algovisor-Ninjatrader");
				m.Close();
            }
            catch (WaitHandleCannotBeOpenedException oEx)
            {
                if (pipeStream != null)
				{
					pipeStream.Close();
					pipeStream = null;
				}
                firstSend = false;
                return;
            }
				
			//if (!firstSend)
			//	OnStartUp();
			//else
			{
				if (pipeStream == null)
					pipeStream = new NamedPipeClientStream(".", pipeName, PipeDirection.Out, PipeOptions.WriteThrough);
				
				if (!pipeStream.IsConnected)
				{
					try
					{
						pipeStream.Connect(1);
					}
					catch (TimeoutException oEX)
					{
						//pipeStream.Close();
						//pipeStream = null;
						return;
					}	
				}
			}
			
			if (pipeStream != null && pipeStream.IsConnected)
			{
				di.value = 106;
				di.ticker = ticker;//Instrument.FullName;
	
				try
				{
					byte[] buffer = StructureToByteArray(di);
					pipeStream.BeginWrite(buffer, 0, buffer.Length, new AsyncCallback(AsyncSendOnTermination), pipeStream);
					//Thread.Sleep(1000);
				}
				catch (TimeoutException oEX)
				{
					return;
				}
				
				//pipeStream.Flush();
				//pipeStream.Close();
				//pipeStream.Dispose();
			}
		}
		#endregion
			
		#region Support Methods
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct Instrument_DATA
		{
			public int value;	
			public double tickSize;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ID;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string description;		
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string expiration;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string exchange;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
            public string securityString;
			public bool type;           
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct Trade_DATA
		{
			public int value;
			public ushort hour;
			public ushort minute;
			public ushort second;
			public ushort millisecond;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
			public double Price;
			public long Volume;					
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct BBO_DATA
		{
			public int value;
			public double Price;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
		public struct DOM_DATA
		{
			public int value;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
			public ushort hour;
			public ushort minute;
			public ushort second;
			public ushort millisecond;
			public double Price;
			public long Volume;	
			public int pos;			
			public char type;	
			public char agressor;
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct DELETE_INSTRUMENT
		{
			public int value;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
		}
		
		[StructLayout(LayoutKind.Sequential)]
		public class SystemTime
		{
			public ushort year;
			public ushort month;
			public ushort weekday;
			public ushort day;
			public ushort hour;
			public ushort minute;
			public ushort second;
			public ushort millisecond;
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet=CharSet.Ansi)]
		public struct COPYDATASTRUCT
		{			
			public IntPtr dwData;
			public int cbData;
			public IntPtr lpData;
		}
		#endregion
    }
}

#region NinjaScript generated code. Neither change nor remove.
// This namespace holds all indicators and is required. Do not change it.
namespace NinjaTrader.Indicator
{
    public partial class Indicator : IndicatorBase
    {
        private Algovisor_Level2[] cacheAlgovisor_Level2 = null;

        private static Algovisor_Level2 checkAlgovisor_Level2 = new Algovisor_Level2();

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Algovisor_Level2 Algovisor_Level2()
        {
            return Algovisor_Level2(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Algovisor_Level2 Algovisor_Level2(Data.IDataSeries input)
        {
            if (cacheAlgovisor_Level2 != null)
                for (int idx = 0; idx < cacheAlgovisor_Level2.Length; idx++)
                    if (cacheAlgovisor_Level2[idx].EqualsInput(input))
                        return cacheAlgovisor_Level2[idx];

            lock (checkAlgovisor_Level2)
            {
                if (cacheAlgovisor_Level2 != null)
                    for (int idx = 0; idx < cacheAlgovisor_Level2.Length; idx++)
                        if (cacheAlgovisor_Level2[idx].EqualsInput(input))
                            return cacheAlgovisor_Level2[idx];

                Algovisor_Level2 indicator = new Algovisor_Level2();
                indicator.BarsRequired = BarsRequired;
                indicator.CalculateOnBarClose = CalculateOnBarClose;
#if NT7
                indicator.ForceMaximumBarsLookBack256 = ForceMaximumBarsLookBack256;
                indicator.MaximumBarsLookBack = MaximumBarsLookBack;
#endif
                indicator.Input = input;
                Indicators.Add(indicator);
                indicator.SetUp();

                Algovisor_Level2[] tmp = new Algovisor_Level2[cacheAlgovisor_Level2 == null ? 1 : cacheAlgovisor_Level2.Length + 1];
                if (cacheAlgovisor_Level2 != null)
                    cacheAlgovisor_Level2.CopyTo(tmp, 0);
                tmp[tmp.Length - 1] = indicator;
                cacheAlgovisor_Level2 = tmp;
                return indicator;
            }
        }
    }
}

// This namespace holds all market analyzer column definitions and is required. Do not change it.
namespace NinjaTrader.MarketAnalyzer
{
    public partial class Column : ColumnBase
    {
        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        [Gui.Design.WizardCondition("Indicator")]
        public Indicator.Algovisor_Level2 Algovisor_Level2()
        {
            return _indicator.Algovisor_Level2(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Indicator.Algovisor_Level2 Algovisor_Level2(Data.IDataSeries input)
        {
            return _indicator.Algovisor_Level2(input);
        }
    }
}

// This namespace holds all strategies and is required. Do not change it.
namespace NinjaTrader.Strategy
{
    public partial class Strategy : StrategyBase
    {
        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        [Gui.Design.WizardCondition("Indicator")]
        public Indicator.Algovisor_Level2 Algovisor_Level2()
        {
            return _indicator.Algovisor_Level2(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Indicator.Algovisor_Level2 Algovisor_Level2(Data.IDataSeries input)
        {
            if (InInitialize && input == null)
                throw new ArgumentException("You only can access an indicator with the default input/bar series from within the 'Initialize()' method");

            return _indicator.Algovisor_Level2(input);
        }
    }
}
#endregion
