import pandas as pd
import numpy as np
import lmfit

df = pd.DataFrame({
  'n'      : pd.Series([1024, 2048, 4096 , 8192]*4),
  'p'      : pd.Series([1, 1, 1, 1, 2, 2, 2, 2, 4, 4, 4, 4, 8, 8, 8, 8]),
  't_par'  : pd.Series([1.58214537e-05, 3.064394e-05,  6.87956822e-05 ,  0.000125017164,1.09386417e-05 , 1.88255289e-05 , 5.77306725e-05 , 6.33239711e-05, 2.1030905e-05 , 2.27093698e-05 , 2.16841662e-05 , 3.60274319e-05,1.62243825e-05 , 3.75008588e-05 , 2.96688098e-05 , 2.9134753e-05])
})

print "p: " + str(df['p'])
print "n: " + str(df['n'])
print "t_par: " +str(df['t_par'])

def fun(A, B, p1 = 1, p2 = 1):
    return p1 * np.exp(A) + p2 * np.exp(B)

def t_parallel(n, p, a = 1, b = 1):
    return a * (n/p) + b*np.log2(p)

model = lmfit.Model(t_parallel, independent_vars=['n', 'p'])
fit = model.fit(df['t_par'], n = df['n'], p = df['p'])

print fit.fit_report()
print fit.values
