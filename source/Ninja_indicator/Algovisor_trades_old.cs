#region Using declarations
using System;
using System.Runtime;   // я добавил
using System.Runtime.InteropServices; // я добавил
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Xml.Serialization;
using System.Collections.Generic;
using NinjaTrader.Cbi;
using NinjaTrader.Data;
using System.Text;
//using System.Windows.Forms;
#endregion

// This namespace holds all indicators and is required. Do not change it.


namespace NinjaTrader.Indicator
{
    [Description("www.alogvisor.com")]
    public class Algovisor_Trades_only : Indicator
    {		
		[DllImport("User32.dll", EntryPoint = "SendMessage")]
    	public static extern int SendMessage(IntPtr hWnd, int Msg, int wParam, ref COPYDATASTRUCT lParam);
		
		[DllImport("Kernel32.dll")]
    	public static extern void GetLocalTime([In, Out] SystemTime st);

        [DllImport("user32.dll", CharSet = CharSet.Unicode)]
        public static extern IntPtr FindWindow(string lpClassName, string lpWindowName);
		
		public override string ToString()
		{
			return "Algovisor_Trades_only";
		}
		
        #region Variables
		public Instrument_DATA id;
		public Trade_DATA td;
		public BBO_DATA bd;
		public DOM_DATA dd;
		public DELETE_INSTRUMENT di;
		public SystemTime st = new SystemTime();
		public IntPtr hwnd;
		public COPYDATASTRUCT cds = new COPYDATASTRUCT();
		const int WM_COPYDATA = 0x004A;
		public string AlgoWindow = "Algovisor";
		public string ticker;
		#endregion
		
		#region Initialize
        protected override void Initialize()
        {
			CalculateOnBarClose	= true;
            Overlay				= true;
			DrawOnPricePanel	= true;
			PaintPriceMarkers 	= false;
            PriceTypeSupported	= false;
			PlotsConfigurable 	= false;
			
			hwnd = FindWindow(null, AlgoWindow);
		}
		#endregion
		
		#region StartUp
		protected override void OnStartUp()
		{
			if (hwnd == IntPtr.Zero)
				return;		
			
			id.ID = Instrument.Id;
			ticker = Instrument.FullName;
			id.ticker = ticker;
			id.expiration = Instrument.Expiry.ToLongDateString();
			id.description = Instrument.MasterInstrument.Description.Normalize();			
			id.tickSize = Instrument.MasterInstrument.TickSize;
			id.exchange = Instrument.Exchange.ToString();
			id.type = false;
			id.securityString = "Algovisor-Ninjatrader";
						
			cds.dwData = (IntPtr)101;
           	cds.cbData = Marshal.SizeOf(id);
			
			IntPtr structPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(id));
            Marshal.StructureToPtr(id, structPtr, false);
			cds.lpData = structPtr;	
			
			int res = SendMessage(hwnd, WM_COPYDATA, 0, ref cds);
			if (res == 0)
				hwnd = IntPtr.Zero;
			
			Marshal.FreeCoTaskMem(structPtr);
		}
		#endregion
	
		#region On Event Updates
		protected override void OnBarUpdate()
		{
			
		}
		
        protected override void OnMarketData(MarketDataEventArgs e)
        {
			if (hwnd == IntPtr.Zero)
			{
				hwnd = FindWindow(null, AlgoWindow);
				OnStartUp();		
			}
			hwnd = FindWindow(null, AlgoWindow);			
			
			if (e.MarketDataType == MarketDataType.Last)
			{
				td.ticker = ticker;
				td.Price = e.Price;
				td.Volume = e.Volume;
				GetLocalTime(st);
				td.hour = st.hour;
				td.minute = st.minute;
				td.second = st.second;
				td.millisecond = st.millisecond;
				Print (td.hour + " " + td.minute + " " + td.second);
				
				cds.dwData = (IntPtr)102;
				cds.cbData = Marshal.SizeOf(td);
				
				IntPtr structPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(td));
				Marshal.StructureToPtr(td, structPtr, false);
				cds.lpData = structPtr;	
				
				int res = SendMessage(hwnd, WM_COPYDATA, 0, ref cds);
				if (res == 0)
					hwnd = IntPtr.Zero;
				
				Marshal.FreeCoTaskMem(structPtr);
			}
			else if (e.MarketDataType == MarketDataType.Ask)
			{
				bd.ticker = Instrument.FullName;
				bd.Price = e.Price;
				
				cds.dwData = (IntPtr)103;
				cds.cbData = Marshal.SizeOf(bd);
				
				IntPtr structPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(bd));
				Marshal.StructureToPtr(bd, structPtr, false);
				cds.lpData = structPtr;	
				
				int res = SendMessage(hwnd, WM_COPYDATA, 0, ref cds);
				if (res == 0)
					hwnd = IntPtr.Zero;
				
				Marshal.FreeCoTaskMem(structPtr);
			}
			else if (e.MarketDataType == MarketDataType.Bid)
			{
				bd.ticker = Instrument.FullName;
				bd.Price = e.Price;
				
				cds.dwData = (IntPtr)104;
				cds.cbData = Marshal.SizeOf(bd);
				
				IntPtr structPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(bd));
				Marshal.StructureToPtr(bd, structPtr, false);
				cds.lpData = structPtr;	
				
				int res = SendMessage(hwnd, WM_COPYDATA, 0, ref cds);
				if (res == 0)
					hwnd = IntPtr.Zero;
				
				Marshal.FreeCoTaskMem(structPtr);
			}
        }		
		#endregion
		
		#region Termination
		protected override void OnTermination()
		{
			di.ticker = ticker;
	
			cds.dwData = (IntPtr)106;
			cds.cbData = Marshal.SizeOf(di);
			
			IntPtr structPtr = Marshal.AllocCoTaskMem(Marshal.SizeOf(di));
			Marshal.StructureToPtr(di, structPtr, false);
			cds.lpData = structPtr;	
			
			int res = SendMessage(hwnd, WM_COPYDATA, 0, ref cds);
			if (res == 0)
				hwnd = IntPtr.Zero;
		}
		#endregion
		
		#region Support Methods
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct Instrument_DATA
		{
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
			public double Price;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet=CharSet.Auto)]
		public struct DOM_DATA
		{
			public ushort hour;
			public ushort minute;
			public ushort second;
			public ushort millisecond;
			public double Price;
			public long Volume;	
			public char agressor;
			public int pos;			
			public char type;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst=50)]
			public string ticker;
		}
		
		[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Auto)]
		public struct DELETE_INSTRUMENT
		{
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
        private Algovisor_Trades_only[] cacheAlgovisor_Trades_only = null;

        private static Algovisor_Trades_only checkAlgovisor_Trades_only = new Algovisor_Trades_only();

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Algovisor_Trades_only Algovisor_Trades_only()
        {
            return Algovisor_Trades_only(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Algovisor_Trades_only Algovisor_Trades_only(Data.IDataSeries input)
        {
            if (cacheAlgovisor_Trades_only != null)
                for (int idx = 0; idx < cacheAlgovisor_Trades_only.Length; idx++)
                    if (cacheAlgovisor_Trades_only[idx].EqualsInput(input))
                        return cacheAlgovisor_Trades_only[idx];

            lock (checkAlgovisor_Trades_only)
            {
                if (cacheAlgovisor_Trades_only != null)
                    for (int idx = 0; idx < cacheAlgovisor_Trades_only.Length; idx++)
                        if (cacheAlgovisor_Trades_only[idx].EqualsInput(input))
                            return cacheAlgovisor_Trades_only[idx];

                Algovisor_Trades_only indicator = new Algovisor_Trades_only();
                indicator.BarsRequired = BarsRequired;
                indicator.CalculateOnBarClose = CalculateOnBarClose;
#if NT7
                indicator.ForceMaximumBarsLookBack256 = ForceMaximumBarsLookBack256;
                indicator.MaximumBarsLookBack = MaximumBarsLookBack;
#endif
                indicator.Input = input;
                Indicators.Add(indicator);
                indicator.SetUp();

                Algovisor_Trades_only[] tmp = new Algovisor_Trades_only[cacheAlgovisor_Trades_only == null ? 1 : cacheAlgovisor_Trades_only.Length + 1];
                if (cacheAlgovisor_Trades_only != null)
                    cacheAlgovisor_Trades_only.CopyTo(tmp, 0);
                tmp[tmp.Length - 1] = indicator;
                cacheAlgovisor_Trades_only = tmp;
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
        public Indicator.Algovisor_Trades_only Algovisor_Trades_only()
        {
            return _indicator.Algovisor_Trades_only(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Indicator.Algovisor_Trades_only Algovisor_Trades_only(Data.IDataSeries input)
        {
            return _indicator.Algovisor_Trades_only(input);
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
        public Indicator.Algovisor_Trades_only Algovisor_Trades_only()
        {
            return _indicator.Algovisor_Trades_only(Input);
        }

        /// <summary>
        /// www.alogvisor.com
        /// </summary>
        /// <returns></returns>
        public Indicator.Algovisor_Trades_only Algovisor_Trades_only(Data.IDataSeries input)
        {
            if (InInitialize && input == null)
                throw new ArgumentException("You only can access an indicator with the default input/bar series from within the 'Initialize()' method");

            return _indicator.Algovisor_Trades_only(input);
        }
    }
}
#endregion
