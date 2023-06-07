from ulab import numpy as np
import time

t = np.linspace(0,10,1024) # generate time array
waves = np.zeros(1024) # generate wave array

freq = [1,3,5]
for i in range(3): # add 3 random waves
    waves += np.sin(2*np.pi/freq[i]*t)

### Fast Fourier Transforms
a, b = np.fft.fft(waves)

for i in range(1024):
    time.sleep(0.05)
    print( (a[i],b[i]) )

