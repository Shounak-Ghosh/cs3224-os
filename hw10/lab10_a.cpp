#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

class PageTable {
public:
    int frame_number;
    bool valid;
    bool reference;
    
    PageTable() : frame_number(-1), valid(false), reference(false) {}
};

std::vector<int> PageTraceGenerator(int n, int p) {
    std::vector<int> trace;
    for (int i = 0; i < n; ++i) {
        trace.push_back(rand() % p);
    }
    return trace;
}

int FindVictim(std::vector<PageTable>& page_table, int& clock_hand, int f) {
    while (true) {
        if (page_table[clock_hand].reference) {
            page_table[clock_hand].reference = false;
        } else {
            int victim = clock_hand;
            clock_hand = (clock_hand + 1) % f;
            return victim;
        }
        clock_hand = (clock_hand + 1) % f;
    }
}

int Simulate(const std::vector<int>& page_trace, int f, int p) {
    std::vector<PageTable> page_table(p);
    int num_page_faults = 0;
    int num_frames_avail = f;
    int clock_hand = 0;

    for (int page : page_trace) {
        if (!page_table[page].valid) {
            num_page_faults++;
            if (num_frames_avail > 0) {
                page_table[page].frame_number = f - num_frames_avail;
                num_frames_avail--;
            } else {
                int victim = FindVictim(page_table, clock_hand, f);
                for (int i = 0; i < p; ++i) {
                    if (page_table[i].frame_number == victim) {
                        page_table[i].valid = false;
                        break;
                    }
                }
                page_table[page].frame_number = victim;
            }
            page_table[page].valid = true;
        }
        page_table[page].reference = true;
    }
    return num_page_faults;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <n> <p>" << std::endl;
        return 1;
    }

    int n = std::atoi(argv[1]);
    int p = std::atoi(argv[2]);

    if (n < 16 || p < 8) {
        std::cerr << "Error: n must be >= 16 and p must be >= 8" << std::endl;
        return 1;
    }

    srand(time(nullptr));
    std::vector<int> page_trace = PageTraceGenerator(n, p);

    std::cout << "Frames,Page Faults" << std::endl;
    for (int f = 4; f <= p; ++f) {
        int faults = Simulate(page_trace, f, p);
        std::cout << f << "," << faults << std::endl;
    }

    return 0;
}