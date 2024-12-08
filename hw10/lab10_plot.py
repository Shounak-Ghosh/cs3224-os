import matplotlib.pyplot as plt
import lab10_a

def main():
    n = 64  # Length of page trace
    p = 16  # Total number of pages
    f_min = 4
    f_max = 16

    # Generate page trace using the imported function
    frames = list(range(4, p + 1))
    page_trace = lab10_a.page_trace_generator(n, p)
    page_faults = []

    # Simulate for each number of frames
    for f in range(f_min, f_max + 1):
        faults = lab10_a.simulate(page_trace, f, p)
        page_faults.append(faults)

    # Plot results
    plt.figure(figsize=(10, 6))
    plt.plot(range(f_min, f_max + 1), page_faults, marker='o')
    plt.title('Page Faults vs. Number of Frames')
    plt.xlabel('Number of Frames')
    plt.ylabel('Number of Page Faults')
    plt.grid(True)

    # Add origin and axes
    plt.axhline(0, color='black', linewidth=0.8)  # X-axis
    plt.axvline(0, color='black', linewidth=0.8)  # Y-axis

    # Extend x and y limits to include origin
    plt.xlim(0, f_max + 1)
    plt.ylim(0, max(page_faults) + 1)

    # Display plot
    plt.show()

if __name__ == "__main__":
    main()
