set clk_period $::env(ENV_CLK_PERIOD)
set virtual_clock $::env(ENV_VIRTUAL_CLOCK)
set clk_port $::env(ENV_CLK_PORT)
set reset_port $::env(ENV_RST_PORT)
if {$virtual_clock=="false"} {

	create_clock $clk_port -name i_clk -period $clk_period
#	set_false_path -from $reset_port
	set_input_delay 0.00 -clock  i_clk [all_inputs]
	set_output_delay 0.00 -clock  i_clk [all_outputs]

} else {
	create_clock -name i_clk -period $clk_period {i_clk}
	set_input_delay 0.00 -clock  i_clk [all_inputs]
	set_output_delay 0.00 -clock  i_clk [all_outputs]
}

