a = [[1.58214537e-05, 3.064394e-05,  6.87956822e-05 ,  0.000125017164],
    [ 1.09386417e-05 , 1.88255289e-05 , 5.77306725e-05 , 6.33239711e-05],
    [ 2.1030905e-05 , 2.27093698e-05 , 2.16841662e-05 , 3.60274319e-05],
    [1.62243825e-05 , 3.75008588e-05 , 2.96688098e-05 , 2.9134753e-05]]

print "speedup mean"
b = []
for i in range(len(a)):
    b.append([])
    for j in range(len(a[i])):
        b[i].append(a[0][j]/a[i][j])
        print str(b[i][j]) + " ",
    print "\n"

print "eficiency mean"
e = []
for i in range(len(a)):
    e.append([])
    for j in range(len(a[i])):
        e[i].append(a[0][j]/((2**i)*a[i][j]))
        print str(e[i][j]) + " ",
    print "\n"
