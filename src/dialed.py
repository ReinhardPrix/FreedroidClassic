#!/usr/bin/python

import sys
import readline
readline.clear_history()
readline.set_history_length(0)


####################################################################
#        ALL THE BLOODY FUNCTIONS GO HERE.
####################################################################

#I use ctrl+d anyway, but that one is for the other people.
def end():
  sys.exit()

def error():
  readline.clear_history()
  print "  ?",

#Loads a file.
def load(current):
  checkup = None
  character = "../dialogs/" + raw_input("What character? ") + ".dialog"
  checkup = file(character)
  if checkup != None:
    if current != None:
      file.close(current)  
    current = checkup
  return current

#Headers are a pain, I do not need them, so I just scroll past them.
def clip(current):
  line = ""
  while line != "Beginning of new chat dialog for character=\"XXXXX\"\n" :
    line = file.readline(current)
  home = file.tell(current) + 1
  return home


#Goes to the next node.
def nextnode(current):
  line = ''
  while line != "----------------------------------------------------------------------\n" :
    line = file.readline(current)
    if line == '' :
      break
    elif line == "End of chat dialog for character=\"XXXXX\"\n" :
      break
  file.readline(current)


#How many nodes do we have here? # Do I still need this?
def nodecount(current, home):
  nodecount = 0
  line2 = "a"
  while line2 != '' :
    line = line2 = file.readline(current)
    if line == "----------------------------------------------------------------------\n" :
      nodecount = nodecount + 1
  file.seek(current, home)
  return nodecount

def scanall(current, count):
  everything = doid(current, count)
#  print everything
  return everything

def doid(current, count):
  everything = []
  for number in range(count):   # Replace range with a list of node numbers. Write a bit to gather the node names.
    node = []
    crude = ['']
    while crude[0] != "startup":
      crude = whatisit(current)
      list.append(node, crude)
    list.append(everything, node)
    nextnode(current)
  return everything

#Behold my mighty engine!
def whatisit(current) :
    back = file.tell(current)
    unknown = file.read(current, 8)
    file.seek(current, back)
    if unknown == "Subtitle" :  #Reply. We want that.
      what = "reply"
      meat = str(file.readline(current))[10:-2]
    elif unknown == "ReplySam" : #Trivial.
      what = "reply.s"
      meat = str(file.readline(current))[13:-2]
    elif unknown == "Position" : #Coordinates to maintain backwards compatibility with the GTK editor.
      what = "coords"
      meat = ["No damn clue for now." , "No damn clue for now."]
      meat[0] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      meat[1] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[1] = meat[1] + character
      file.readline(current)

    elif unknown == "ChangeOp" :  #Another harder one. 
      what = "switch"
      meat = ["No damn clue for now." , "No damn clue for now."]
      meat[0] = ''
      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      meat[1] = file.readline(current)[-2:-1]

    elif unknown == "AlwaysEx" : # The last line.
      what = "startup"
      meat = str(file.readline(current))[43:-2]

    elif unknown == "New Opti" : #Tux speaking.
      what = "tuxtalk"
      meat=['' , '']

      character=''
      while character != "=" :
        character = file.read(current, 1)
      character = ''
      while character != " " :
        character = file.read(current, 1)
        if character == " " :
          file.read(current, 1)
          break
        meat[0] = meat[0] + character
      character=''
      while character != "\"" :
        character = file.read(current, 1)
      character = ''
      while character != "\"" :
        character = file.read(current, 1)
        if character == "\"" :
          file.read(current, 1)
          break
        meat[1] = meat[1] + character

    elif unknown == "OptionSa" : #Tux voice. Easy.
      what = "tuxtalk.s"
      meat =  str(file.readline(current))[14:-2]

    elif unknown == "OnCondit" : #Might be a linked node or a goto...
        what = "goto"               #Welcome to hell my friends. This will hurt a bit. This is a goto.
        meat = ['', '', '']
        character=''
        while character != "\"" :
          character = file.read(current, 1)
        character = ''
        while character != "\"" :
          character = file.read(current, 1)
          if character == "\"" :
            file.read(current, 1)
            break
          meat[0] = meat[0] + character
        character=''
        while character != "=" :
          character = file.read(current, 1)
        while character != " " :
          character = file.read(current, 1)
          if character == " " :
            file.read(current, 1)
            break
          meat[1] = meat[1] + character
        character=''
        while character != "=" :
          character = file.read(current, 1)
        while character != "\n" :
          character = file.read(current, 1)
          if character == "\n" :
            break
          meat[2] = meat[2] + character
        if meat[1] == meat[2]:
          what = "linked"

    elif unknown == "DoSometh" :
      what = "extra"
      meat =  str(file.readline(current))[18:-2]

    else :                    
      print "PAAAANIIIIIIIIC!!!"                    
      hcf()

    output = [what, meat]
#    print output
    return output

  #  else !!!


def dumpnodes(everything):
  node = []
  subnode = []
  print ""
  for node in everything:
    for subnode in node:
      if subnode[0] == "tuxtalk":
        if len(subnode[1][0]) == 1:
          print "  " + subnode[1][0] + "  Tux: " + subnode[1][1]
        else:
          print "  " + subnode[1][0] + " Tux: " + subnode[1][1]
#      elif subnode[0] == "reply":
#        print "A: " + subnode[1]

def dumpnoded(node):
  count = 0
  snn = 0
  print ""
  print "       #" + node[0][1][0]
  for subnode in node:
    if subnode[0] == "tuxtalk":
      print "  " + str(snn),
      snn = snn + 1
      print "   Tux: " + subnode[1][1]
    elif subnode[0] == "reply":
      if count == 0:
        print "  " + str(snn),
        snn = snn + 1
        print "   A: " + subnode[1]
        count = count + 1
      else:
        print "  " + str(snn),
        snn = snn + 1
        print "      " + subnode[1]



def dumpnode(node):
  count = 0
  snn = 0
  print ""
  print "  #" + node[0][1][0]
  for subnode in node:
    if subnode[0] == "tuxtalk":
      snn = snn + 1
      print "  Tux: " + subnode[1][1]
    elif subnode[0] == "reply":
      if count == 0:
        print "  A: " + subnode[1]
        count = count + 1
      else:
        print "     " + subnode[1]




def rawprint(everything):
  print """% -*- mode: flyspell; mode: fill -*-
----------------------------------------------------------------------
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
----------------------------------------------------------------------

This file was generated using dialed.
If you have questions concerning FreedroidRPG, please send an email to:

freedroid-discussion@lists.sourceforge.net

BEGIN OF AUTORS NOTES """  
  print " "
  print """END OF AUTORS NOTES


Beginning of new chat dialog for character="XXXXX" """

  for nodo in range(count):
    print ""
    raw(everything[nodo])
    print "\n----------------------------------------------------------------------"
  print "\n" + 'End of chat dialog for character="XXXXX"' + "\n"

def raw(node):
  for something in node:
    if something[0] == "tuxtalk" :
      print "New Option Nr=" + something[1][0] + '  OptionText="' + something[1][1] + "\""
    elif something[0] == "tuxtalk.s" :
      print "OptionSample=\"" + something[1] + "\""
    elif something[0] == "reply" :
      print "Subtitle=\"" + something[1] + "\""
    elif something[0] == "reply.s" :
      print "ReplySample=\"" + something[1] + "\""
    elif something[0] == "coords" :
      print "PositionX=" + something[1][0] + "  PositionY=" + something[1][1] + "  "
    elif something[0] == "switch" :
      print "ChangeOption=" + something[1][0] + " ChangeToValue=" + something[1][1]
    elif something[0] == "startup" :
      print "AlwaysExecuteThisOptionPriorToDialogStart=\"" + something[1] + "\""
    elif something[0] == "goto" :
      print 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][2]
    elif something[0] == "linked" :
      print 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][1]
    elif something[0] == "extra" :
      print 'DoSomethingExtra="' + something[1] + '"'

def et(node, pick):
  count = 0
  meaty = -1
  while meaty != pick:
    if node[count][0] == "tuxtalk":
      count = count + 1
      meaty = meaty + 1
      flag = 1
    elif node[count][0] == "reply":
      count = count + 1
      meaty = meaty + 1
      flag = 0
    elif node[count][0] == "switch":
      error()
      return
    else:
      count = count + 1
      flag = 0
  count = count - 1
  if flag == 0:
    a = node[count][1]
  else:
    a = node[count][1][1]
  print a
  print "\n<< " + a + "\n",
  readline.clear_history()
  readline.add_history(a)
  b = raw_input(">> ")
  readline.clear_history()
  if flag == 0:
    node[count][1] = b
  else:
    node[count][1][1] = b

def writeall(current, everything):
  c = str(current)[12:-26]
  file.close(current)
  d = open(c, 'w')
  file.write(d, """% -*- mode: flyspell; mode: fill -*-
----------------------------------------------------------------------
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
----------------------------------------------------------------------

This file was generated using dialed.
If you have questions concerning FreedroidRPG, please send an email to:

freedroid-discussion@lists.sourceforge.net

BEGIN OF AUTORS NOTES

END OF AUTORS NOTES 


Beginning of new chat dialog for character="XXXXX"

""")

  for node in everything:
    for something in node:
      if something[0] == "tuxtalk" :
        file.write(d, "New Option Nr=" + something[1][0] + '  OptionText="' + something[1][1] + "\"\n")
      elif something[0] == "tuxtalk.s" :
        file.write(d, "OptionSample=\"" + something[1] + "\"\n")
      elif something[0] == "reply" :
        file.write(d, "Subtitle=\"" + something[1] + "\"\n")
      elif something[0] == "reply.s" :
        file.write(d, "ReplySample=\"" + something[1] + "\"\n")
      elif something[0] == "coords" :
        file.write(d, "PositionX=" + something[1][0] + "  PositionY=" + something[1][1] + "  \n")
      elif something[0] == "switch" :
        file.write(d, "ChangeOption=" + something[1][0] + " ChangeToValue=" + something[1][1] + "\n")
      elif something[0] == "startup" :
        file.write(d, "AlwaysExecuteThisOptionPriorToDialogStart=\"" + something[1] + "\"\n")
      elif something[0] == "goto" :
        file.write(d, 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][2] + "\n")
      elif something[0] == "linked" :
        file.write(d, 'OnCondition="' + something[1][0] + '" JumpToOption=' + something[1][1] + ' ElseGoto=' + something[1][1] + "\n")
      elif something[0] == "extra" :
        file.write(d, 'DoSomethingExtra="' + something[1] + '"' + "\n")
    file.write(d, "\n----------------------------------------------------------------------\n\n")
  file.write(d, 'End of chat dialog for character="XXXXX"' + "\n")
  file.close(d)
  return file(str(current)[14:-26])
  
  

def inject(everything):
  desired = raw_input("New node number? ")
  for checking in everything:
    if checking[0][1][0] == desired:
      error()
      return everything
    else:
      pass
  readline.clear_history()
  node = ['','','','','','','']
  node[0] = ['','']
  node[0][0] = "tuxtalk"
  node[0][1] = ['','']
  node[0][1][0] = desired
  node[0][1][1] = "<?>"
  node[1] = ['','']
  node[1][0] = "tuxtalk.s"
  node[1][1] = "Sorry_No_Voice_Sample_Yet_0.wav"
  node[2] = ['','']
  node[2][0] = "coords"
  node[2][1] = ['','']
  node[2][1][0] = "200"
  node[2][1][1] = "200"
  node[3] = ['','']
  node[3][0] = "reply"
  node[3][1] = "<?>"
  node[4] = ['','']
  node[4][0] = "reply.s"
  node[4][1] = "Sorry_No_Voice_Sample_Yet_0.wav"
  node[5] = ['','']
  node[5][0] = "switch"
  node[5][1] = ['','']
  node[5][1][0] = desired
  node[5][1][1] = "0"
  node[6] = ['','']
  node[6][0] = "startup"
  node[6][1] = "no"
  list.append(everything, node)
  return everything

def sort(everything, count):
  pile = []
  for bit in range(100):
    for bite in range(count):
      if int(everything[bite][0][1][0]) == bit :
        list.append(pile, everything[bite])
  everything = pile
  return everything

def cu(everything, noda):
  x = 0
  while int(everything[x][0][1][0]) != noda:
    x = x + 1
  spawn = int(raw_input("Which subnode? "))
  count = 0
  meaty = -1
  while meaty != spawn:
    if everything[x][count][0] == "tuxtalk":
      count = count + 1
      meaty = meaty + 1
      flag = 1
    elif everything[x][count][0] == "reply":
      count = count + 1
      meaty = meaty + 1
      flag = 0
    elif everything[x][count][0] == "switch":
      error()
      return everything
    else:
      count = count + 1
  if flag == 1:
    count = count + 3
  else: count = count + 2
  list.insert(everything[x], count - 1 , ["reply.s", "Sorry_No_Voice_Sample_Yet_0.wav"])
  list.insert(everything[x], count - 1 , ["reply", "<?>"])
  return everything

def uc(everything, noda):
  x = 0
  while int(everything[x][0][1][0]) != noda:
    x = x + 1
  spawn = int(raw_input("Which subnode? "))
  if spawn == 0:
    error()
    return everything
  else:
    count = 0
    meaty = -1
    while meaty != spawn:
      if everything[x][count][0] == "tuxtalk":
        count = count + 1
        meaty = meaty + 1
      elif everything[x][count][0] == "reply":
        count = count + 1
        meaty = meaty + 1
      elif everything[x][count][0] == "switch":
        error()
        return everything
      else:
        count = count + 1
    del(everything[x][count - 1])
    del(everything[x][count - 1])
    return everything

def links(node, everything):
  no = 0
  pre = 0
  n = 0

  for bit in node:
    if bit[0] == "switch":
      pre = pre + 1
  if pre == 1:
    for bit in node:
      if bit[0] == "switch":
        if bit[1][1] == "0":
          return False

  for bit in node:
    no = no + 1
    if bit[0] == "switch":
      if bit[1][1] == "0":
        say = "OFF"
      else:
        say = "ON "
      if n == 0:
        print ""
        n = 1
       
      y = 0
      while int(everything[y][0][1][0]) != bit[1][0]:
        if everything[y][0][1][0] == bit[1][0]:
          break
        y = y + 1
      if everything[y] == node:
        if say == "OFF":
          firstrun = True
        else:
          print "  " + str(bit[1][0]) + " -> " + say + " Tux: " + everything[int(y)][0][1][1]

      else:
        x = 0
        while int(everything[x][0][1][0]) != bit[1][0]:
          if everything[x][0][1][0] == bit[1][0]:
            break
          x = x + 1
        print "  " + str(bit[1][0]) + " -> " + say + " Tux: " + everything[int(x)][0][1][1]
        firstrun = False
  return firstrun





####################################################################
#    HERE STARTS THE PROGRAM.
####################################################################
#Preparing the needed varialbles.
current = None
node = None
home = None
count = None
subnode = None
everything = []
locked = True
firstrun = True
prompt = ""
previous = []
backup = None

# current = load(current)
# home = clip(current)
# count = nodecount(current, home)
# everything = scanall(current, count)
# rebuild(everything)

while True :
 try:
#  print everything
  if previous != everything:
    backup = previous[:]
    previous = everything[:]
  prompt = ""
  readline.clear_history()
  if firstrun == True:
    firstrun = False
  else:
    print ""
  if current != None :
    a = str(current)
    if locked == True:
      prompt = "f: " + a[12:-26] + " "
    else:
      prompt = "F: " + a[12:-26] + " "
  if node != None :
    prompt = prompt + "n:" + str(noda) + " "
  if subnode != None :
    prompt = prompt + "s:" + str(sub) + " "
  prompt = str(prompt) + " >> "
  command = raw_input(str(prompt))
  if command == "q":
    end()
  elif command == "s":
    readline.clear_history()
    node = None
    subnode = None
    current = load(current)
    home = clip(current)
    count = nodecount(current, home)
    everything = scanall(current, count)
    backup = everything[:]
    previous = everything[:]
    dumpnodes(everything)
  elif command == "n":
    readline.clear_history()
    if everything == None:
      error()
    else:
      subnode = None
      x = 0
      justone = []
      noda = int(raw_input("What node? "))
      while int(everything[x][0][1][0]) != noda:
        if everything[x][0][1][0] == noda:
          break
        x = x + 1
      node = everything[x]
      dumpnode(node)
      links(node, everything)
  elif command == "p":
    readline.clear_history()
    if node == None:
      error()
    else:
      dumpnode(node)
  elif command == "pp":
    readline.clear_history()
    if node == None:
      error()
    else:
      dumpnoded(node)
  elif command == "d":
    readline.clear_history()
    if everything == None:
      error()
    else:
      dumpnodes(everything)
  elif command == "dd":
    readline.clear_history()
    if everything == None:
      error()
    else:
      for x in everything:
        dumpnode(x)
  elif command == "ddd":
    readline.clear_history()
    if everything == None:
      error()
    else:
      rawprint(everything)
  elif command == "et":   #Edit Text.
    readline.clear_history()
    if node == None:
      error()
    else:
      pick = int(raw_input("Which subnode? "))
      et(node, pick)
  elif command == "es":
    readline.clear_history()
    if node == None:
      error()
    else:
      pass
  elif command == "ed":
    readline.clear_history()
    if node == None:
      error()
    else:
      pass
  elif command == "ne":
    if current == None:
      error()
    else:
      readline.clear_history()
      everything = inject(everything)
      count = 0
      for spam in everything:
        count = count + 1
      everything = sort(everything, count)
  elif command == "w":
    readline.clear_history()
    if locked == True:
      error()
    elif current == None:
      error()
    else:
      current = writeall(current, everything)
  elif command == "l":
    readline.clear_history()
    if current == None:
      error()
    else:
      if locked == True:
        locked = False
      else:
        locked = True
  elif command == "so":
    readline.clear_history()
    if current == None:
      error()
    else:
      everything = sort(everything, count)
  elif command == "mu":
    readline.clear_history()
    if node == None:
      error()
    else:
      x = 0
      while int(everything[x][0][1][0]) != noda:
        x = x + 1
      list.remove(everything, everything[x])
      count = count - 1
      node = None
      noda = None
  elif command == "cu":
    readline.clear_history()
    if node == None:
      error()
    else:
      cu(everything, noda)
  elif command == "uc":
    readline.clear_history()
    if node == None:
      error()
    else:
      everything = uc(everything, noda)
  elif command == "!":
    everything = backup
  elif command == "!!":
    if current == None:
      error()
    else:
      everything = None
      file.seek(current, 0)
      home = clip(current)
      count = nodecount(current, home)
      everything = scanall(current, count)
  elif command == "nk":
    if node == None:
      error()
    else:
      links(node, everything)



  else:
    error()

 except KeyboardInterrupt:
  print""
  error()
 except IOError:
   error()
