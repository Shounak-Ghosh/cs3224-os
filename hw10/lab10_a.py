import random
import sys

def page_trace_generator(n, p):
    """Generate a random page trace of length n with page numbers from 0 to p-1."""
    return [random.randint(0, p-1) for _ in range(n)]

def find_victim(page_table):
    """Find a victim page to replace using the second-chance algorithm."""
    while True:
        for page, entry in page_table.items():
            if entry['valid'] and entry['reference'] == 0:
                return page
            elif entry['valid']:
                entry['reference'] = 0  # Reset the reference bit
    return None  # Shouldn't reach here

def simulate(page_trace, f, p):
    """Simulate the second-chance LRU algorithm."""
    num_page_faults = 0
    frames = []
    page_table = {i: {'frame_number': -1, 'valid': False, 'reference': 0} for i in range(p)}

    for page in page_trace:
        # Check if the page is in a frame (no page fault)
        if page_table[page]['valid']:
            page_table[page]['reference'] = 1  # Mark page as referenced
        else:
            # Page fault occurs
            num_page_faults += 1
            if len(frames) < f:
                # Allocate an empty frame if available
                page_table[page]['frame_number'] = len(frames)
                page_table[page]['valid'] = True
                page_table[page]['reference'] = 1
                frames.append(page)
            else:
                # Replace a page using the second-chance algorithm
                victim = find_victim(page_table)
                victim_frame = page_table[victim]['frame_number']
                page_table[victim] = {'frame_number': -1, 'valid': False, 'reference': 0}
                page_table[page]['frame_number'] = victim_frame
                page_table[page]['valid'] = True
                page_table[page]['reference'] = 1
                frames[victim_frame] = page
    return num_page_faults

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <n> <p>")
        sys.exit(1)
    
    n = int(sys.argv[1])
    p = int(sys.argv[2])
    
    if n < 16 or p < 8:
        print("Error: n must be >= 16 and p must be >= 8.")
        sys.exit(1)

    page_trace = page_trace_generator(n, p)
    print("Generated Page Trace:", page_trace)
    # Simulate for f values from 4 to p and record results
    results = {}
    for f in range(4, p + 1):
        num_page_faults = simulate(page_trace, f, p)
        results[f] = num_page_faults

    # Print results
    print("Number of Page Faults for each frame size:")
    for f, faults in results.items():
        print(f"Frames: {f}, Page Faults: {faults}")
