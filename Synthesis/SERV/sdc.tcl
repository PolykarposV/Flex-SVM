set clk_period $::env(ENV_CLK_PERIOD)
set virtual_clock $::env(ENV_VIRTUAL_CLOCK)
set clk_port $::env(ENV_CLK_PORT)
set reset_port $::env(ENV_RST_PORT)
if {$virtual_clock=="false"} {

	create_clock $clk_port -name clk -period $clk_period
	set_input_delay 0.00 -clock  clk [all_inputs]
	set_output_delay 0.00 -clock  clk [all_outputs]

} else {
	create_clock -name clk -period $clk_period {clk}
	set_input_delay 0.00 -clock  clk [all_inputs]
	set_output_delay 0.00 -clock  clk [all_outputs]
}

