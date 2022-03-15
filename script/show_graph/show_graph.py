#!/usr/bin/python

import sys, getopt, re
import matplotlib.pyplot as plt
import matplotlib.patches as patches
from matplotlib.animation import FuncAnimation 

colorarray = ['brown','green','fuchsia','orange','blue','lime']
noth = 0
vert = 1
edge = 2
path = 3

def parseopt(argv):
    path=""
    try:
      opts, args = getopt.getopt(argv,"hp:",["path="])
    except getopt.GetoptError:
      print ('show_graph.py -p <input filepath>')
      sys.exit(2)
    for opt, arg in opts:
      if opt == '-h':
         print ('show_graph.py -p <input filepath>')
         sys.exit()
      elif opt in ("-p"):
         path = arg
         print ('input path is',path)
      else:
         print ('show_graph.py -p <input filepath>')
         sys.exit()
    return path

def readfile(path):
    lines = []
    print(path)
    sec = noth
    with open(str(path)) as f:
        lines = f.readlines()
    v = []
    e = []
    p = []
    for line in lines:
        # determine section
        if (line == '------- Print the vertex here -------\n'):
            sec = vert
        elif (line == '------- Print the edge here -------\n'):
            sec = edge
        elif (line == '------- Eular Circuit -------\n'):
            sec = path
        # Parse text due to the section number
        else:
            if sec == vert:
                m = re.match(r'index: ([-+]?\d+), x: ([-+]?\d+\.?\d*), y: ([-+]?\d+\.?\d*)'+'\n', line)
                # print(m.groups())
                index = int(m.groups()[0])
                px = float(m.groups()[1])
                py = float(m.groups()[2])
                v.append([index, px, py])
            elif sec == edge:
                m = re.match(r'index pair: ([-+]?\d+), ([-+]?\d+)'+'\n', line)
                # print(m.groups())
                ia = int(m.groups()[0])
                ib = int(m.groups()[1])
                e.append([ia, ib])
            elif sec == path:
                m = re.match(r'([-+]?\d+)'+'\n', line)
                # print(m.groups())
                ip = int(m.groups()[0])
                p.append(ip)
    return v, e, p

def plotrect(v, e, p, path):
    fig, axis = plt.subplots(1, 2)
    for i, x, y in v:
        # print(str(i)+','+str(x)+','+str(y))
        axis[0].plot(x, y, 'bo', markersize = 4)
    for a, b in e:
        ia, ax, ay = v[a]
        ib, bx, by = v[b]
        axis[0].plot([ax, bx], [ay, by], 'green')
    def init():
        for i, x, y in v:
            ao = axis[1].plot(x, y, 'bo', markersize = 2)
        return ao
    def update(i):
        plt.cla()
        for k, x, y in v:
            ao = axis[1].plot(x, y, 'bo', markersize = 2)
        for j in range(i):
            # print(str(i)+', '+str(j))
            jm = p[j]
            jn = p[j+1]
            no , mx, my = v[jm]
            no , nx, ny = v[jn]
            ao = axis[1].plot([mx, nx], [my, ny], 'green')
        ip1 = p[i]
        ip2 = p[i+1]
        ia, ax, ay = v[ip1]
        ib, bx, by = v[ip2]
        dx = bx-ax
        dy = by-ay
        ao = axis[1].arrow(ax, ay, dx, dy, 
          head_width = 1, 
          head_length = 2, 
          length_includes_head = True, 
          width = 0.2,
          ec = 'red', 
          fc = 'red')
        # ao = axis[1].plot([ax, bx], [ay, by], 'green')
        return ao
    
    anim = FuncAnimation(fig, update, init_func = init,
                     frames = len(p)-1, interval = 400, repeat=False) 
    anim.save(path + '.mp4', fps=3, extra_args=['-vcodec', 'libx264'])
    plt.show()

if __name__ == "__main__":
    path = parseopt(sys.argv[1:])
    v, e, p = readfile(path)
    plotrect(v, e, p, path)

