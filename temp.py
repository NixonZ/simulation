import random
import math
import numpy as np
from collections import Counter

T = 1440

def lambd(t):
    if t<420:
        return 0.3/10
    elif t<420+3*60:
        return (0.3+(t-420)/180)/10
    else:
        return (1.3-(t-(420+180))/14*60)/10

def C(t):
    if t<10:
        return 3
    elif t<120:
        return 2
    elif t<7*60+30:
        return 1
    elif t<10*60:
        return 2
    elif t<10*60+20:
        return 3
    elif t<14*60+30:
        return 4
    elif t<18*60 +30:
        return 5
    elif t<20*60:
        return 4
    else:
        return 3


def non_homo_pos(t):
    h = lambd(t)
    U = random.random()

    t = t - math.log(U)/h

    while not(U <= h/0.13):
        U = random.random()
        t = t - math.log(U)/h

    return t
def markovian():
    U = random.random()
    return -math.log(U)/0.5

t = 0
n = 0 # Number of patient in ED currently at time t.
c = 3 # Number of Server in ED currently at time t.
server_list = [-1,-1,-1,-1,-1] # Number of people in each server
                               #-1 if server is disconnected,
                               # 0 if connected and empty,
                               # 1 if connected and filled,
                               # 2 if disconnected,but filled.

for i in range(c):
    server_list[i] = 0

counter_variable = [0,0] #{Number of patients in ED by time t,Number of departures from ED by time t}
ta = non_homo_pos(0)

td = [-1,-1,-1,-1,-1] # ta,td,tc : time at which server number changes
for server,i in zip(server_list,range(len(server_list))):
    if server!=-1:
        td[i]=1441
print(td)
A = []
D = []

while True:
    t%=1440
    print("---System Status---")
    print("Present time:"+str(t))
    print("Number of People Present:"+str(n))
    print("Cummulative Number of Arrivals by t:" + str(counter_variable[0]) )
    print("Cummulative Number of Departures by t:"+ str(counter_variable[1]) )
    print("Time for next arrival:"+str(ta))

    print("Number of Server:"+str(c))

    non_empty_servers = [(i,td[i]) for i in range(len(server_list)) if server_list[i]==1 or server_list[i]==2]
    non_empty_servers.sort(key=lambda x: x[1])

    print('Non-empty Servers: '+str(non_empty_servers))
    print('Server List: '+str(server_list))
    print('td: '+str(td))

    if non_empty_servers == []:
        print("---Arrival happening---")
        t = ta
        if c < C(t):
            opened = 0
            for i in range((len(server_list))):
                if server_list[i] == -1:
                    server_list[i] = 0
                    opened+=1
                if server_list[i] == 2:
                    server_list[i] = 1
                    opened+=1
                if opened ==C(t)-c:
                    break

        elif c > C(t):
            closed = 0
            # find c-C(t) minimum time servers in server_list and close them accordingly
            # please do this
            departure_copy = [ (td[i],i) for i in range(len(td)) if td[i]!=-1 ]
            departure_copy.sort(key = lambda x: x[0])
            print('Closing time departure copy'+str(departure_copy))
            for i in range(c-C(t)):
                if server_list[departure_copy[i][1]] == 1:
                    server_list[departure_copy[i][1]] = 2
                elif server_list[departure_copy[i][1]] == 0:
                    server_list[departure_copy[i][1]] = -1
        c = C(t)
        counter_variable[0] += 1
        n += 1
        ta = non_homo_pos(t)

        empty_servers_index = [i for i in range(len(server_list)) if server_list[i]==0]
        print('empty servers: '+str(empty_servers_index))
        if empty_servers_index != []:
            server_list[empty_servers_index[0]] = 1
            td[empty_servers_index[0]] = t + markovian()

        A.append(t)
        input()
        continue

    min_dept_idx = non_empty_servers[0][0]
    print("Time for next departure:"+str(non_empty_servers[0][1]) )
    if ta <= td[min_dept_idx] and ta<=T:
        print("---Arrival happening---")
        t = ta
        if c < C(t):
            opened = 0
            for i in range((len(server_list))):
                if server_list[i] == -1:
                    server_list[i] = 0
                    opened+=1
                if server_list[i] == 2:
                    server_list[i] = 1
                    opened+=1
                if opened ==C(t)-c:
                    break

        elif c > C(t):
            closed = 0
            # find c-C(t) minimum time servers in server_list and close them accordingly
            # please do this
            departure_copy = [ (td[i],i) for i in range(len(td)) ]
            departure_copy.sort(key = lambda x: x[0])
            for i in range(c-C(t)):
                if server_list[departure_copy[i][1]] == 1:
                    server_list[departure_copy[i][1]] = 2
                elif server_list[departure_copy[i][1]] == 0:
                    server_list[departure_copy[i][1]] = -1

        c = C(t)
        counter_variable[0] += 1
        n += 1
        ta = non_homo_pos(t)

        empty_servers_index = [i for i in range(len(server_list)) if server_list[i]==0]
        print('empty servers: '+str(empty_servers_index))
        if empty_servers_index != []:
            server_list[empty_servers_index[0]] = 1
            td[empty_servers_index[0]] = t + markovian()

        A.append(t)

    elif non_empty_servers[0][1]<ta and min(td)<=T:
        print("---Departure happening at server"+str(min_dept_idx)+"---")
        t = td[min_dept_idx]
        counter_variable[1] += 1
        n -= 1

        save_dept = min_dept_idx
        td[min_dept_idx] = 1441
        if server_list[min_dept_idx] == 2:
            server_list[min_dept_idx] = -1
        else:
            server_list[min_dept_idx] = 0

        if c < C(t):
            opened = 0
            for i in range((len(server_list))):
                if server_list[i] == -1:
                    server_list[i] = 0
                    opened+=1
                if server_list[i] == 2:
                    server_list[i] = 1
                    opened+=1
                if opened ==C(t)-c:
                    break

        elif c > C(t):
            closed = 0
            # find c-C(t) minimum time servers in server_list and close them accordingly
            # please do this
            departure_copy = [ (td[i],i) for i in range(len(td)) ]
            departure_copy.sort(key = lambda x: x[0])
            for i in range(c-C(t)):
                if server_list[departure_copy[i][1]] == 1:
                    server_list[departure_copy[i][1]] = 2
                elif server_list[departure_copy[i][1]] == 0:
                    server_list[departure_copy[i][1]] = -1
        c = C(t)

        q = n - Counter(server_list)[1] - Counter(server_list)[2]
        if server_list[save_dept] == 0 and q>0:
            server_list[save_dept] = 1
            td[save_dept] = t + markovian()
        D.append(t)

    input()

average_waiting_time = 0
for i in range(len(A)):
    average_waiting_time += (D[i]-A[i])

average_waiting_time/=count
print(average_waiting_time)
