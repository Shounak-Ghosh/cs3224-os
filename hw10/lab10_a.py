import sys
import random

class PageTable:
    def __init__(self):
        self.frame_number = -1
        self.valid = False
        self.reference = False

def page_trace_generator(n, p):
    return [random.randint(0, p-1) for _ in range(n)]

def find_victim(page_table, clock_hand, f):
    while True:
        if page_table[clock_hand].reference:
            page_table[clock_hand].reference = False
        else:
            victim = clock_hand
            clock_hand = (clock_hand + 1) % f
            return victim, clock_hand
        clock_hand = (clock_hand + 1) % f

def simulate(page_trace, f, p):
    page_table = [PageTable() for _ in range(p)]
    num_page_faults = 0
    num_frames_avail = f
    clock_hand = 0

    for page in page_trace:
        if not page_table[page].valid:
            num_page_faults += 1
            if num_frames_avail > 0:
                page_table[page].frame_number = f - num_frames_avail
                num_frames_avail -= 1
            else:
                victim, clock_hand = find_victim(page_table, clock_hand, f)
                for i in range(p):
                    if page_table[i].frame_number == victim:
                        page_table[i].valid = False
                        break
                page_table[page].frame_number = victim
            page_table[page].valid = True
        page_table[page].reference = True

    return num_page_faults

def main():
    if len(sys.argv) != 3:
        print("Usage: python script.py <n> <p>")
        sys.exit(1)

    n = int(sys.argv[1])
    p = int(sys.argv[2])

    if n < 16 or p < 8:
        print("Error: n must be >= 16 and p must be >= 8")
        sys.exit(1)

    page_trace = page_trace_generator(n, p)

    print("Frames,Page Faults")
    for f in range(4, p + 1):
        faults = simulate(page_trace, f, p)
        print(f"{f},{faults}")

if __name__ == "__main__":
    main()