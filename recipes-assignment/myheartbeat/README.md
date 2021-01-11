# Heartbeat Monitor Application

This application works with the PPG Kernel Module and, relying on two threads that act as **producer/consumer**, it computes the mean heart rate within a given frequency. In particular, one thread reads data from the (virtual) ppg sensor with a sampling frequency of 50Hz and puts it in a shared circular buffer. Instead, in order to save resources, the second thread sleeps on a conditional variable. It is woken up only when there is at least one element in the shared buffer and it moves all data produced in its internal array. When all values are collected, the consumer thread performs a 2048-point Fast Fourier Transformate, computes the Power Spectral Density and prints the heart rate in beat-per-minute.

In order to guarantees concurrency and thread safety, the shared buffer between the two threads works with a mutex and a conditional variable. It is based on a **templated circular buffer**, so its type and static size are defined at compile time, simply changing the template parameters. I've chosen this structure in order to let the app to be as flexible as possible: the size of the buffer can be changed with respect to the frequency of read operations and device performances, in order to continue to read data with the same frequency and not to loose them during the long FFT operations. Otherwise, if we are focused on memory saving, we can choose a smaller size, but during heavier operations least recent data will be overwritten by newer ones, with sligthly less precise measurements.

For example, in the default implementation that targets a Raspberry Pi3, the ring is only 4 elements large and it is totally lossless, due to the difference of about one order of magnitude between the read operation and FFT computing time.

Regarding the customization of sizes and timings, it is possible to change them simply editing the `main.cpp` file:
* In `circular_buffer` templated class the first parameters specifies the type of data content, while the second one will define the buffer maximum capacity.
* In `producer` templated constructor it is possible to define the sleep time (expressed in microseconds) between read operations and the reference to the shared buffer, inheriting statically its type and dimension.
* Also `consumer` class has a templated constructor and acccepts only one parameter, the reference to the buffer.
