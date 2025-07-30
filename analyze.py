import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import sosfilt, butter

def analyze_ofdm_spectrum(fname):
    """
    Analyze the OFDM spectrum by reading the log file and plotting the frequency spectrum.
    """

    # Read the log file
    data = np.loadtxt(fname, delimiter=',')
    data = np.array([x + 1j*y for x, y in data])

    # Calculate the FFT of the data in chunks of 64 samples
    freq = []
    for i in range(0, len(data), 64):
        chunk = data[i:i+64]
        if len(chunk) < 64:
            continue
        fft_chunk = np.fft.fft(chunk)
        freq.append(fft_chunk)

    # Plot
    x_axis = np.arange(-31, 33)
    
    plt.figure()
    plt.plot(x_axis, 20 * np.log10(np.roll(np.sum(np.abs(freq), axis=0), 31)))
    plt.title('OFDM Spectrum (dB)')
    plt.xlabel('Frequency Bin')
    plt.ylabel('Magnitude (dB)')
    plt.show(block=False)

    plt.figure()
    plt.scatter(np.real(np.array(freq).flatten()), np.imag(np.array(freq).flatten()), s=1)
    plt.xlim([-1.5, 1.5])
    plt.ylim([-1.5, 1.5])
    plt.show()

def analyze_demod_spectrum(fname):
    # Read the log file
    data = np.loadtxt(fname, delimiter=',')
    data = np.array([x + 1j*y for x, y in data])

    # Calculate the FFT of the data in chunks of 64 samples
    freq = []
    for i in range(0, len(data), 64):
        chunk = data[i:i+64]
        if len(chunk) < 64:
            continue
        freq.append(chunk)

    # Plot
    x_axis = np.arange(-31, 33)
    data_out = np.array(freq).flatten()
    # data_out = np.array(freq)[:, 7]
    data_out = data_out[2048:]
    print(len(data_out))
    
    # plt.figure()
    # plt.plot(np.fft.fftfreq(len(data), 1/100e9), np.abs(np.fft.fft(data)))
    # plt.show(block=False)

    plt.figure()
    plt.plot(x_axis, 20 * np.log10(np.roll(np.sum(np.abs(freq), axis=0), 31)))
    plt.title('OFDM Spectrum (dB)')
    plt.xlabel('Frequency Bin')
    plt.ylabel('Magnitude (dB)')
    plt.show(block=False)

    data_syms = np.zeros((np.array(freq).shape[0], 48), dtype=np.complex128)
    insert_i = 0
    for i in range(1, 64):
        if i not in [7, 21, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 43, 57]:
            data_syms[:, insert_i] = np.array(freq)[:, i]
            insert_i += 1

    plt.figure()
    plt.scatter(np.real(data_syms.flatten()), np.imag(data_syms.flatten()), s=1)
    plt.xlim([-1.5, 1.5])
    plt.ylim([-1.5, 1.5])
    plt.show(block=False)

    plt.figure()
    plt.scatter(np.real(np.array(freq)[:, 7]), np.imag(np.array(freq)[:, 7]), s=1)
    plt.scatter(np.real(np.array(freq)[:, 21]), np.imag(np.array(freq)[:, 21]), s=1)
    plt.scatter(np.real(np.array(freq)[:, 43]), np.imag(np.array(freq)[:, 43]), s=1)
    plt.scatter(np.real(np.array(freq)[:, 57]), np.imag(np.array(freq)[:, 57]), s=1)
    plt.xlim([-1.5, 1.5])
    plt.ylim([-1.5, 1.5])
    plt.show()

def plot_signal(fname):
    """
    Plot the time-domain signal from the log file.
    """
    time, data = np.split(np.loadtxt(fname, delimiter=','), 2, axis=1)
    time = time.flatten()
    data = data.flatten()
    # plt.figure()
    # plt.plot(time, data)
    # plt.title('Transmitter Time-Domain Signal')
    # plt.show(block=False)

    print(np.var(data))

    plt.figure()
    plt.plot(np.fft.fftfreq(len(data), 1/100e9), np.abs(np.fft.fft(data)))
    plt.title('Transmitter Frequency-Domain Signal')
    plt.show()

def demod_signal(fname):
    """
    Demodulate the signal from the log file.
    This function is a placeholder and should be implemented based on the specific demodulation requirements.
    """
    data = np.loadtxt(fname, delimiter=',')
    i = 0
    t = np.array(data)[:, 0]
    data = np.array(data)[:, 1]
    baseband = data * (np.cos(2 * np.pi * 2.4e9 * t) +
                        1j * np.sin(2 * np.pi * 2.4e9 * t))
    sampled = []
    t = 0.5/20e6

    sos = butter(20, 10e6, 'low', fs=100e9, output='sos')
    baseband = sosfilt(sos, baseband.real) + 1j * sosfilt(sos, baseband.imag)

    while i < len(data):
        # Convert to baseband signal at 2.4 GHz and sample
        sampled.append(baseband[i])

        t += 1.0 / 20e6  # Sample at 20 MHz
        i = int(t * 100e9)


    # Calculate the FFT of the data in chunks of 64 samples
    freq = []
    for i in range(0, len(data), 64):
        chunk = sampled[i:i+64]
        if len(chunk) < 64:
            continue
        fft_chunk = np.fft.fft(chunk)
        freq.append(fft_chunk)

    # Plot
    x_axis = np.arange(-31, 33)
    
    # plt.figure()
    # plt.plot(np.fft.fftfreq(len(baseband), 1/100e9), np.abs(np.fft.fft(baseband)))
    # plt.show(block=False)

    # plt.figure()
    # plt.plot(np.real(np.array(baseband)))
    # plt.show(block=False)

    # plt.figure()
    # plt.plot(np.imag(np.array(baseband)))
    # plt.show(block=False)

    plt.figure()
    plt.plot(x_axis, 20 * np.log10(np.roll(np.sum(np.abs(freq), axis=0), 31)))
    plt.title('OFDM Spectrum (dB)')
    plt.xlabel('Frequency Bin')
    plt.ylabel('Magnitude (dB)')
    plt.show(block=False)

    data_out = np.array(freq).flatten()
    # data_out = data_out[512:]

    plt.figure()
    plt.plot(np.imag(data_out))
    plt.show(block=False)

    plt.figure()
    plt.scatter(data_out.real, data_out.imag, s=1)
    plt.xlim([-1.5, 1.5])
    plt.ylim([-1.5, 1.5])
    plt.show()
    

if __name__ == "__main__":
    # analyze_ofdm_spectrum('build/ofdm_modulate.log')
    # plot_signal('build/transmitter.log')
    # demod_signal('build/transmitter.log')
    analyze_demod_spectrum('build/ofdm_demodulate.log')