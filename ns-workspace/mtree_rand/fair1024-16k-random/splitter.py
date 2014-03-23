import os

def output2f(fname, line):
  if os.path.exists(fname):
    fout = file(fname, 'a')
  else:
    fout = file(fname, 'w')
    fout.write('cmd arg1 v1 arg2 v2 arg3 v3 agr4 v4\n')
    # fout.write('cmd mean low high\n')
  fout.write(line)
  fout.close()

def analyzer(fin, error, arg, name):
  stat = []
  sum = 0
  while True:
    line = fin.readline()
    if len(line) == 0:
      break
    tuples = line.split()
    if tuples[0] == 'dump-fair' and not (float(tuples[4]) == 1.0 or float(tuples[4]) == 0.0):
      stat.append(float(tuples[4]))
      sum = sum + float(tuples[4])
  stat.sort()
  bar2 = int(error * len(stat))
  line = '%d\t%f\t%f\t%f\n' % (arg, sum / len(stat), stat[bar2], stat[len(stat) - 1 - bar2])
  output2f(name, line)

e = 0.
fin = file('0.txt', 'r')
analyzer(fin, e, 1, 'fair-stat.txt')
fin = file('10k.txt', 'r')
analyzer(fin, e, 2, 'fair-stat.txt')
fin = file('100k.txt', 'r')
analyzer(fin, e, 3, 'fair-stat.txt')
fin = file('1M.txt', 'r')
analyzer(fin, e, 4, 'fair-stat.txt')
