set_time_format -unit ns -decimal_places 3
create_clock -period 49.152MHz -name clk_sys
create_clock -period 49.152MHz -name ADC_CLK

derive_pll_clocks -create_base_clocks
derive_clock_uncertainty
