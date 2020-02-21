#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

struct SATURATING_COUNTER {
	SATURATING_COUNTER() {
		this->counter = bitset<2> ("11");
	}
	bitset<2> counter;
};

struct BPB {
	BPB(int m) {
		this->m = m;
		this->bpb.resize(pow(2, m));
	}
	int m;
	vector<SATURATING_COUNTER> bpb;
};

struct PREDICTOR {
	PREDICTOR(int m, int k) {
		this->m = m;
		this->k = k;
		this->bhr = pow(2, k) - 1;
		this->pht.assign(pow(2, k), BPB(m));
	}
	int m;
	int k;
	unsigned int bhr;
	vector<BPB> pht;
};

bool counterPredict(bitset<2> counter) {
	if (counter == bitset<2> ("11") || counter == bitset<2> ("10")) {
		return 1;
	} else {
		return 0;
	}
}

void updatePHT(PREDICTOR &predictor, int bhrIndex, int bpbIndex, bool result) {
	if (result) {
		if (predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter.to_string() == "00") {
			predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter = bitset<2> ("01");
		} else {
			predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter = bitset<2> ("11");
		}
	} else {
		if (predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter.to_string() == "11") {
			predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter = bitset<2> ("10");
		} else {
			predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter = bitset<2> ("00");
		}
	}
}

void updateBHR(PREDICTOR &predictor, bool prevResult) {
	predictor.bhr = predictor.bhr >> 1;
	if (prevResult == 1) {
		predictor.bhr |= (1<<(predictor.k-1));
	} else {
		predictor.bhr &= (~(1<<(predictor.k-1)));
	}
}


int main (int argc, char** argv) {
	ifstream config;
	config.open(argv[1]);

	int m, k;
	config >> m >> k;
 
	config.close();

	ofstream out;
	string out_file_name = string(argv[2]) + ".out";
	out.open(out_file_name.c_str());
	
	ifstream trace;
	trace.open(argv[2]);
	unsigned long pc; bool taken;
	trace >> std::hex >> pc >> taken;

	bool prediction;
	int bhrIndex, bpbIndex;
	PREDICTOR predictor = PREDICTOR(m, k);

	while (!trace.eof()) {
		bhrIndex = predictor.bhr;
		bpbIndex = pc % (int) pow(2, m);
		bitset<2> branchCounter = predictor.pht.at(bhrIndex).bpb.at(bpbIndex).counter;
		prediction = counterPredict(branchCounter);
		updatePHT(predictor, bhrIndex, bpbIndex, taken);
		if (predictor.k != 0) {
			updateBHR(predictor, taken);
		}
		out << prediction << endl;
		trace >> std::hex >> pc >> taken;
	}
	 
	trace.close();	
	out.close();
}
