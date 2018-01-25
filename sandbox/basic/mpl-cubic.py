import numpy as np
import matplotlib.pyplot as plt

x = np.linspace(-5,5,100);

plt.plot(x, np.power(x,3) + -5.*np.power(x,2) + 1.*x + 1., '-')
plt.plot(x, 0.*x, '-', color='black')

plt.show();