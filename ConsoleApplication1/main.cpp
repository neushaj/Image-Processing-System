#include "modules.hpp"
int sc_main(int argc, char** argv){
	testbench* Test = new testbench("Test");
	sc_report_handler::set_actions(SC_WARNING, SC_DO_NOTHING);
	sc_start(3000000, SC_NS);
	return 0;
}