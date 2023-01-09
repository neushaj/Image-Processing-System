#include "modules.hpp"

GrayScaler::GrayScaler(sc_module_name, string Rfile, string Gfile, string Bfile) {
	ifstream Rf, Gf, Bf;
	last_row = 0;
	int temp1, temp2, temp3;
	Rf.open(Rfile);
	Gf.open(Gfile);
	Bf.open(Bfile);
	outFile.open("1.txt");
	if (!Rf || !Gf || !Bf) {
		cout << "Unable to open file!!! :(" << endl;
		exit(1);
	}
	int i = 0;
	while (Rf >> temp1 && Gf >> temp2 && Bf >> temp3) {
		ArrayR[i] = temp1;
		ArrayG[i] = temp2;
		ArrayB[i] = temp3;
		i++;
	}
	SC_THREAD(putting);
	sensitive << clk;
}

void GrayScaler::process() {
	for (int i = 0; i < 512 * 8; i++) {
		int temp;
		temp = ((ArrayR[i+512*last_row].to_uint() * 1) + (ArrayG[i+512 * last_row].to_uint() * 1) + (ArrayB[i+ 512 * last_row].to_uint() * 1)) / 3;
		Array[i] = temp;
		outFile <<temp << endl;
	}
}

void GrayScaler::putting() {
	while (true) {
		process();
		sc_lv <8> DataToPut;
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 512; j++) {
				wait(clk.posedge_event());
				DataToPut = Array[(512 * last_row) + j];
				out->put(DataToPut);
			}
			last_row++;
		}
		if (last_row == 512) {
			wait(100, SC_NS);
			exit(1);
		}
	}
}

void channel::put(sc_lv<8> data) {
	if (full) wait(get_event);
	saved = data;
	full = true;
	put_event.notify();
}

void channel::get(sc_lv<8> & data) {
	if (!full) wait(put_event);
	data = saved;
	full = false;
	get_event.notify();
}

void EdgeDetectorAccelerator::getting() {
	while (true) {
		sc_lv <8> DataToGet;
		for (int i = 0; i < 512 * 8; i++) {
			wait(clk.posedge_event());
			in->get(DataToGet);
			gotten_data[i] = DataToGet;
			data[idx * 8 * 512 + i] = DataToGet;
		}
		process(7 * idx, 7 * idx + 6);
		idx++;
		if (idx == 64) {
			break;
		}
	}
	process(448, 511);
	for (int i = 0; i < 512 * 512; i++) {
		outFile << result[i].to_uint() << endl;
		outfile_median << result_median[i].to_uint() << endl;
	}
}

void EdgeDetectorAccelerator::process(int start, int end) {
	cout << end << endl;
	for (int i = start; i <= end; i++) {
		for (int j = 0; j < 512; j++) {
			if (i == 0 || i == 511 || j == 0 || j == 511) {
				continue;
			}
			int ARR[9] = {data[(i - 1) * 512 + (j - 1)].to_uint(),
			 data[(i) * 512 + (j - 1)].to_uint(),
			 data[(i + 1) * 512 + (j - 1)].to_uint() ,
			 data[(i - 1) * 512 + (j)].to_uint(),
			 data[(i) * 512 + (j)].to_uint(),
			 data[(i + 1) * 512 + (j)].to_uint(),
			 data[(i - 1) * 512 + (j + 1)].to_uint(),
			data[(i) * 512 + (j + 1)].to_uint(),
			data[(i + 1) * 512 + (j + 1)].to_uint()};
			sort(ARR, ARR + 9);
			result_median[i * 512 + j] = ARR[4];
			int GX = -1 * data[(i - 1) * 512 + (j - 1)].to_uint() + (-2) * data[(i - 1) * 512 + (j)].to_uint() + (-1) * data[(i - 1) * 512 + (j + 1)].to_uint() +
				1 * data[(i + 1) * 512 + (j - 1)].to_uint() + (2) * data[(i + 1) * 512 + (j)].to_uint() + (1) * data[(i + 1) * 512 + (j + 1)].to_uint();
			int GY = -1 * data[(i - 1) * 512 + (j - 1)].to_uint() + (-2) * data[(i) * 512 + (j - 1)].to_uint() + (-1) * data[(i + 1) * 512 + (j - 1)].to_uint() +
				1 * data[(i - 1) * 512 + (j + 1)].to_uint() + (2) * data[(i) * 512 + (j + 1)].to_uint() + (1) * data[(i + 1) * 512 + (j + 1)].to_uint();
				result[i * 512 + j] = abs(GX) + abs(GY);

		}
	}
}

void testbench::clk_generate() {
	while (true) {
		clk.write(SC_LOGIC_0);
		wait(0.5, SC_NS);
		clk.write(SC_LOGIC_1);
		wait(0.5, SC_NS);
	}
}