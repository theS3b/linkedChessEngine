#include "utils.h"

using namespace std;

void append_to_array(array<uint64_t, 5> & arr, const uint64_t& value) {
	arr[0] = arr[1];
	arr[1] = arr[2];
	arr[2] = arr[3];
	arr[3] = arr[4];
	arr[4] = value;
}

std::string casenb_to_coo(const short& casenb)
{
	short nb = int(casenb / 8) + 1;
	std::string letters = "hgfedcba";
	std::string ret = letters[(int)((casenb-1) % 8)] + to_string(nb);
	return ret;
}
