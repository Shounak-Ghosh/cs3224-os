import matplotlib.pyplot as plt
from lab10_a import page_trace_generator, simulate

def main():
    n, p = 64, 16
    page_trace = page_trace_generator(n, p)

    frames = list(range(4, p + 1))
    faults = [simulate(page_trace, f, p) for f in frames]

    plt.figure(figsize=(10, 6))
    plt.plot(frames, faults, marker='o')
    plt.title('Page Faults vs. Number of Frames')
    plt.xlabel('Number of Frames')
    plt.ylabel('Number of Page Faults')
    plt.grid(True)
    plt.savefig('page_faults_vs_frames.png')
    plt.show()

    print("Frames,Page Faults")
    for f, fault in zip(frames, faults):
        print(f"{f},{fault}")

if __name__ == "__main__":
    main()