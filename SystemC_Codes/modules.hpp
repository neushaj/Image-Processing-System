#include "systemc.h"
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>

using namespace std;

class put_if : public sc_interface {
public:
	virtual void put(sc_lv<8> data) = 0;
};

class get_if : public sc_interface {
public:
	virtual void get(sc_lv<8> & data) = 0;
};

SC_MODULE(GrayScaler) {
	sc_lv <8> Array[512 * 8];
	sc_lv <8> ArrayR[512 * 512];
	sc_lv <8> ArrayG[512 * 512];
	sc_lv <8> ArrayB[512 * 512];
	sc_port <put_if> out;
	sc_in <sc_logic> clk;
	int last_row;
	ofstream outFile;
	SC_HAS_PROCESS(GrayScaler);
	GrayScaler(sc_module_name, string Rfile, string Gfile, string Bfile);
	void putting();
	void process();
};

class channel : public put_if, public get_if{
	bool full;
	sc_lv <8> saved;
	sc_event put_event, get_event;
	public:
		channel() : full(false) {};
		void put(sc_lv<8> data);
		void get(sc_lv<8> & data);
};

SC_MODULE(EdgeDetectorAccelerator) {
	sc_port <get_if> in;
	sc_in <sc_logic> clk;
	sc_lv <8> gotten_data[512 * 8];
	//sc_lv <8> data[514 * 10];
	sc_lv <8> data[512 * 512];
	sc_lv <8> result[512 * 512];
	sc_lv <8> result_median[512 * 512];
	sc_lv <8> pre_row[514];
	long last_idx;
	int idx;
	ofstream outFile;
	ofstream outfile_median;
	SC_CTOR(EdgeDetectorAccelerator) {
		fill(pre_row, pre_row + 514, 0);
		fill(result, result + 512*512, 0);
		last_idx = 0;
		idx = 0;
		outFile.open("edgedetector_value.txt");
		outfile_median.open("median.txt");
		SC_THREAD(getting);
		sensitive << clk;
	}
	void getting();
	void process(int start, int end);
};

SC_MODULE(testbench) {
	sc_signal<sc_logic> clk;
	GrayScaler* m1;
	channel* c;
	EdgeDetectorAccelerator* m2;
	SC_CTOR(testbench) {
		c = new channel();
		m1 = new GrayScaler("m1", "red_pixel.txt", "green_pixel.txt", "blue_pixel.txt");
		m1->clk(clk);
		m1->out(*c);
		m2 = new EdgeDetectorAccelerator("m2");
		m2->clk(clk);
		m2->in(*c);
		SC_THREAD(clk_generate);
	}
	void clk_generate();
};



