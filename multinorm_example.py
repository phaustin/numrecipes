import numpy as np
import multinorm as mn
import matplotlib.pyplot as plt

invalues={'meanx':450.,
          'stdx':50,
          'meany':-180,
          'stdy':40,
          'rho':0.8}


def get_random(meanx=None,stdx=None,meany=None,stdy=None,rho=None,nvars=1):
    corr=np.array([(stdx**2., rho*stdx*stdy), (rho*stdx*stdy, stdy**2.)])
    meanvec=np.array([meanx,meany])
    multi_class=mn.multi_norm(1234,meanvec,corr)
    out_vals=multi_class.get_devs(nvars)
    return out_vals

invalues['nvars']=4000
out_vec=get_random(**invalues)

fig=plt.figure(1)
fig.clf()
ax1=fig.add_subplot(111)
ax1.plot(out_vec[0,:],out_vec[1,:],'b.')
plt.show()
 
