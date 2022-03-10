#!/usr/bin/python

import sys, getopt, re
import matplotlib.pyplot as plt
import matplotlib.patches as patches

colorarray = ['brown','green','fuchsia','orange','blue','lime']

def parseopt(argv):
    path=""
    try:
      opts, args = getopt.getopt(argv,"hp:",["path="])
    except getopt.GetoptError:
      print ('show_rtree.py -p <input filepath>')
      sys.exit(2)
    for opt, arg in opts:
      if opt == '-h':
         print ('show_rtree.py -p <input filepath>')
         sys.exit()
      elif opt in ("-p"):
         path = arg
         print ('input path is',path)
      else:
         print ('show_rtree.py -p <input filepath>')
         sys.exit()
    return path

def readfile(path):
    lines = []
    print(path)    
    with open(str(path)) as f:
        lines = f.readlines()
    result = []
    for line in lines:
        m = re.match(r'\[RTREE\] This is node ([-+]?\d+) at depth: ([-+]?\d+), the bounding box is \(([-+]?\d+\.?\d*),([-+]?\d+\.?\d*)\), \(([-+]?\d+\.?\d*),([-+]?\d+\.?\d*)\), leaf: (\d+), edge: (\d+)'+'\n', line)
        print(m.groups())
        index = int(m.groups()[0])
        depth = int(m.groups()[1])
        xmin  = float(m.groups()[2])
        ymin  = float(m.groups()[3])
        xmax  = float(m.groups()[4])
        ymax  = float(m.groups()[5])
        leaf  = int(m.groups()[6])
        edge  = int(m.groups()[7])
        #print(line)
        result.append([index, depth, xmin, ymin, xmax, ymax, leaf, edge])

    return result

def plotrect(bbs):
    maxdepth = 0
    for bb in bbs:
        if bb[1] > maxdepth: 
            maxdepth = bb[1]

    fig, ax = plt.subplots(maxdepth+1)

    for bb in bbs:
        index = int(bb[0])
        depth = int(bb[1])
        xmin  = float(bb[2])
        ymin  = float(bb[3])
        xmax  = float(bb[4])
        ymax  = float(bb[5])
        leaf  = int(bb[6])
        edge  = int(bb[7])
        #color = colorarray[depth%2],
        ec = ''
        fc = ''
        ap = 1
        if edge == 1:
            ec = 'black'
            fc = 'white'
            ap = 1
        else:
            ec = 'red'
            fc = 'red'
            ap = 0.4

        ax[depth].add_patch(
             patches.Rectangle(
                (xmin, ymin),
                xmax-xmin,
                ymax-ymin,
                edgecolor = ec,
                facecolor = fc,
                alpha = ap
             ) )
    for axi in ax:
        axi.plot()
    plt.show()

if __name__ == "__main__":
    path = parseopt(sys.argv[1:])
    result = readfile(path)
    #print(result)
    plotrect(result)

