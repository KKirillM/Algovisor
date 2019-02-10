//
// Copyright (C) 2006, NinjaTrader LLC <ninjatrader@ninjatrader.com>.
// NinjaTrader reserves the right to modify or overwrite this NinjaScript component with each release.
//

#region Using declarations
using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Xml.Serialization;
using NinjaTrader.Data;
using NinjaTrader.Gui.Chart;
#endregion

#region NinjaScript generated code. Neither change nor remove.
// This namespace holds all indicators and is required. Do not change it.
namespace NinjaTrader.Indicator
{
    public partial class Indicator : IndicatorBase
    {
        private Algovisor_Level2[] cacheAlgovisor_Level2 = null;
        private Algovisor_Trades_only[] cacheAlgovisor_Trades_only = null;

        private static Algovisor_Level2 checkAlgovisor_Level2 = new Algovisor_Level2();
        private static Algovisor_Trades_only checkAlgovisor_Trades_only = new Algovisor_Trades_only();

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
