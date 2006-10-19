#!/usr/bin/python

import sys
import readline
import threading
import thread
import time

readline.clear_history()
readline.set_history_length(0)

current = None
everything = None

####################################################################
#        ALL THE BLOODY FUNCTIONS GO HERE.
####################################################################  

class autosave(threading.Thread):
  def __init__(self, tofile, what):
    self.tofile = tofile
    self.what = what
    threading.Thread.__init__(self)
  def run(self):
    self._running = True
    tobesavedas = str(self.tofile)[12:-32] + "backup"
    while self._running:
      time.sleep(60)
      self.target = open(tobesavedas, "w")
      thread.start_new_thread(writeall, (self.target, self.what),)
  def stop(self):
    self._running = False
  def update(self, material):
    self.what = material[:]

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
    return output

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
  print ""
  print "  #" + node[0][1][0]
  for subnode in node:
    if subnode[0] == "tuxtalk":
      print "  Tux: " + subnode[1][1]
    elif subnode[0] == "reply":
      if count == 0:
        print "  A: " + subnode[1]
        count = count + 1
      else:
        print "     " + subnode[1]

def rawprint(everything):
  print ""
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
  print "\n" + 'End of chat dialog for character="XXXXX"'

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
      print "ChangeOption=" + str(something[1][0]) + " ChangeToValue=" + str(something[1][1])
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
  print "\n<< " + a + "\n",
  readline.clear_history()
  readline.add_history(a)
  b = raw_input(">> ")
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
  readline.clear_history()
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

def cu(node):
  readline.clear_history()
  spawn = int(raw_input("Which subnode? "))
  count = 0
  meaty = -1
  while meaty != spawn:
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
  if flag == 1:
    count = count + 3
  else: count = count + 2
  list.insert(node, count - 1 , ["reply.s", "Sorry_No_Voice_Sample_Yet_0.wav"])
  list.insert(node, count - 1 , ["reply", "<?>"])

def uc(node):
  readline.clear_history()
  spawn = int(raw_input("Which subnode? "))
  if spawn == 0:
    error()
    return
  else:
    count = 0
    meaty = -1
    while meaty != spawn:
      if node[count][0] == "tuxtalk":
        count = count + 1
        meaty = meaty + 1
      elif node[count][0] == "reply":
        count = count + 1
        meaty = meaty + 1
      elif node[count][0] == "switch":
        error()
        return
      else:
        count = count + 1
    del(node[count - 1])
    del(node[count - 1])

def links(node, everything, nodeindex):
  firstrun = False
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
      if say == "OFF":
        if bit[1][0] == node[0][1][0]:
          continue
      x = list.index(nodeindex, bit[1][0])
      print "  " + str(bit[1][0]) + " <- " + say + " Tux: " + everything[x][0][1][1]
      firstrun = False
  return firstrun

def addlink(node, noda, state):
  to = str(int(raw_input("To node? ")))
  for bit in node:
    if bit[0] == "switch":
      if str(int(bit[1][0])) == to:
        bit[1][1] = str(state)
        return
  x = 0
  for bit in node:
    x = x + 1
    if bit[0] == "switch":
      list.insert(node, x, ["switch", [to,str(state)]])
      return
  x = 0
  for bit in node:
    x = x + 1
    if bit[0] == "extra":
      x = x - 1
      list.insert(node, x, ["switch", [to,str(state)]])
      return
    elif bit[0] == "startup":
      x = x - 1
      list.insert(node, x, ["switch", [to,str(state)]])
      return
  
  hcf()
        
def killlink(node, everything):
  to = int(raw_input("To node? "))
  for bit in node:
    if bit[0] == "switch":
      if int(bit[1][0]) == to:
        list.remove(node, bit)
        return
  error()

def gotolist(node, everything, nodeindex):
  for bit in node:
    if bit[0] == "goto":
      print ""
      print "  IF: " + bit[1][0]
      print "  T -> " + bit[1][1] + "  " + everything[list.index(nodeindex, bit[1][1])][0][1][1]
      print "  F -> " + bit[1][2] + "  " + everything[list.index(nodeindex, bit[1][2])][0][1][1]
      return

def linkednode(everything, node, nodeindex):
  for bit in node:
    if bit[0] == "linked":
      print ""
      print "  -LINKED- "
      chop = everything[list.index(nodeindex, bit[1][1])]
      dumpnode(chop)
      linkednode(everything, chop, nodeindex)
      return

def cheaplinkednode(everything,node, nodeindex):
  for bit in node:
    if bit[0] == "linked":
      print ""
      print "  LN -> " +  bit[1][1] + "  Tux: " + everything[list.index(nodeindex, bit[1][1])][0][1][1]
      return  

def makeindex(everything):
  nodeindex = []
  for bit in everything:
    list.append(nodeindex, bit[0][1][0])
  return nodeindex
  
def killgoto(node):
  for bit in node:
    if bit[0] == "goto":
      list.remove(node, bit)

def newgoto(node):
  x = 0
  readline.add_history("MissionComplete:")
  readline.add_history("PointsToDistributeAtLeast")
  readline.add_history("GoldIsLessThan:")
  readline.add_history("MeleeSkillLesserThan:")
  readline.add_history("CookieIsPlante")
  readline.add_history("MissionAssigned:")
  readline.add_history("HaveItemWithCode:")
  readline.add_history("OldTownMissionScoreAtLeast:")
  condition = raw_input("Condition? ")
  readline.clear_history()
  truth = str(int(raw_input("On true to? ")))
  readline.clear_history()
  lies = str(int(raw_input("On false to? ")))
  if truth == lies:
    type = "linked"
  else:
    type = "goto"
  for bit in node:
    x = x + 1
    if bit[0] == "goto":
      list.remove(node, bit)
      bit = [type, [condition, truth, lies]]
      list.insert(node, x - 1, bit)
      return
    elif bit[0] == "linked":
      list.remove(node, bit)
      bit = [type, [condition, truth, lies]]
      list.insert(node, x - 1, bit)
      return
    elif bit[0] == "extra":
      list.insert(node, x - 1 , [type, [condition, truth, lies]])
      return
    elif bit[0] == "startup":
      list.insert(node, x - 1 , [type, [condition, truth, lies]])
      return
  hcf()

def editgoto(node):
  x = 0
  for bit in node:
    x = x + 1
    if bit[0] == "goto":
      readline.add_history(bit[1][0])
      condition = raw_input("Condition? ")
      readline.clear_history()
      truth = raw_input("On true? ")
      readline.clear_history()
      lies = raw_input("On false? ")
      newthing = ["goto", [condition, truth, lies]]
      list.remove(node, bit)
      list.insert(node, x - 1, newthing)
      return
  error()
  
def showextra(node):
  x = 0
  for bit in node:
    if bit[0] == "extra" :
      print ""
      for bit in node:
        if bit[0] == "extra" :
          print "  " + str(x) + " Do:  " + bit[1]
          x = x + 1
      return

def killextra(node):
  y = 0
  for bit in node:
    if bit[0] == "extra":
      y = 1
      break
  if y == 0:
    error()
    return
  victim = int(raw_input("Which extra? "))
  x = 0
  for bit in node:
    if bit[0] == "extra":
      if x == victim:
        list.remove(node, bit)
      x = x + 1

def addextra(node):
  print ""
  readline.add_history("BreakOffAndBecomeHostile")
  readline.add_history("IncreaseMeleeWeaponSkill")
  readline.add_history("IncreaseRangedWeaponSkill")
  readline.add_history("IncreaseSpellcastingSkill")
  readline.add_history("IncreaseHackingSkill")
  readline.add_history("GiveItem:")
  readline.add_history("ExecuteActionWithLabel:")
  readline.add_history("PlantCookie:")
  readline.add_history("InitTradeWithCharacter:")
  readline.add_history("AssignMission:")
  readline.add_history("MarkMissionComplete:")
  readline.add_history("AddExperienceBonus:")
  readline.add_history("AddBigScreenMessageBUT_WITH_TERMINATION_CHARACTER_PLEASE:")
  readline.add_history("AddBaseMagic:")
  readline.add_history("AddBaseDexterity:")
  readline.add_history("SubtractPointsToDistribute:")
  readline.add_history("SubtractGold:")
  readline.add_history("AddGold:")
  readline.add_history("DeleteAllInventoryItemsWithCode:")
  readline.add_history("DeleteAllInventoryItemsOfType:")
  readline.add_history("ExecuteSubdialog:")
  readline.add_history("EverybodyBecomesHostile")
  readline.add_history("MakeTuxTownGuardMember")
  readline.add_history("ForceBotRespawnOnLevel:")
  readline.add_history("CompletelyHealTux")
  readline.add_history("OpenQuestDiaryEntry:")
  readline.add_history("EndDialog")
  newextra = raw_input(">> ")
  x = 0
  for bit in node:
    if bit[0] != "startup":
      x = x + 1
  list.insert(node, x, ["extra", newextra]) 
  
def startup(node, state):
  list.remove(node, node[-1])
  list.append(node, ["startup", state])
  
def showstartup(node):
  if node[-1][1] == "yes":
    print ""
    print "  Autostarting"

def backlinks(node, everything):
  flag = 0
  for byte in everything:
    for bit in byte:
      if bit[0] == "switch":
        if bit[1][0] == node[0][1][0]:
          if byte == node:
              continue
          if flag == 0:
            flag = 1
            print ""
          if bit[1][1] == "0":
            print "  " + byte[0][1][0] + " -> OFF Tux: " + byte[0][1][1]
          else:
            print "  " + byte[0][1][0] + " -> ON  Tux: " + byte[0][1][1]

def reprirority(everything, node, nodeindex, count, direction):
  numthing = 0
  freezer = ""
  new = ""
  if direction == "up":
    if node[0][1][0] == nodeindex[0]:
      error()
    else:
      freezer = (everything[list.index(nodeindex, node[0][1][0]) - 1][0][1][0])[:]
      everything[list.index(nodeindex, node[0][1][0]) - 1][0][1][0] = (everything[list.index(nodeindex, node[0][1][0])][0][1][0])[:]
      everything[list.index(nodeindex, node[0][1][0])][0][1][0] = freezer[:]
      everything = sort(everything, count)
      rebuild(everything, freezer, everything[list.index(nodeindex, node[0][1][0]) + 1][0][1][0])
  elif direction == "down":
    if node[0][1][0] == nodeindex[-1]:
      error()
    else:
      freezer = (everything[list.index(nodeindex, node[0][1][0]) + 1][0][1][0])[:]
      everything[list.index(nodeindex, node[0][1][0]) + 1][0][1][0] = (everything[list.index(nodeindex, node[0][1][0])][0][1][0])[:]
      everything[list.index(nodeindex, node[0][1][0])][0][1][0] = freezer[:]
      everything = sort(everything, count)
      rebuild(everything, freezer, everything[list.index(nodeindex, node[0][1][0]) - 1][0][1][0])
  
def rebuild(everything, old, new):
  print old
  print new
  for node in everything:
    for bit in node:
      if bit[0] == "goto":
        if bit[1][1] == old:
          bit[1][1] = new
        elif bit[1][1] == new:
          bit[1][1] = old
        if bit[1][2] == old:
          bit[1][2] = new
        elif bit[1][2] == new:
          bit[1][2] = old
      if bit[0] == "switch":
        if bit[1][0] == old:
          bit[1][0] = new
        elif bit[1][0] == new:
          bit[1][0] = old
      if bit[0] == "linked":
        if bit[1][1] == old:
          bit[1][1] = new
          bit[1][2] = new
        elif bit[1][1] == new:
          bit[1][1] = old
          bit[1][2] = old
  
def sound(node):
  pick = int(raw_input("Which subnode? "))
  readline.clear_history()
  meaty = -1
  for bit in node:
    if bit[0] == "tuxtalk.s":
      meaty = meaty + 1
    elif bit[0] == "reply.s":
      meaty = meaty + 1
    if meaty == pick:
      print "\n<< " + bit[1] + "\n",
      readline.add_history("Sorry_No_Voice_Sample_Yet_0.wav")
      readline.add_history("NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER")
      readline.add_history(bit[1])
      bit[1] = raw_input(">> ")
      return
  error()
  return
  
def dumpsound(node):
  for bit in node:
    if bit[0] == "tuxtalk":
      print ""
      print "  Tux: " + bit[1][1]
    elif bit[0] == "tuxtalk.s":
      print "     --> " + bit[1]
    elif bit[0] == "reply":
      print ""
      print "  " + bit[1]
    elif bit[0] == "reply.s":
      print "     --> " + bit[1]

####################################################################
#        THE SIM BITS ARE HERE.
####################################################################

def simain(everything, nodeindex):
  halting = ''
  x = -1
  ranbefore = False
  running = False
  invite = " ## "
  conditionslist = gatherconditions(everything)
  thosegofirst = startupnodes(everything)
  startlist = getnodesready(everything, nodeindex[:])[:]
  while True:
    if running == False:
      print ""
      y = 0
      for condition in conditionslist:
        print "  " + str(y) + "  " + condition[0] + " = " + str(condition[1])
        y = y + 1
    else:
      printactivenodes(startlist, nodeindex)
    print ""
    command = raw_input(invite)
    if running == False:
      if command == "q":
        return
      elif command == "go":
        running = True
        invite = " #> "
#        if ranbefore == False:
        for bit in thosegofirst:
            halting = process(everything, bit, startlist, nodeindex, conditionslist)
#        ranbefore = True
      elif int(command) in range(y):
        if conditionslist[int(command)][1] == True:
          conditionslist[int(command)][1] = False
        elif conditionslist[int(command)][1] == False:
          conditionslist[int(command)][1] = True
      else:
        error()
    else:
      if command == "q":
        return
      elif command in startlist:
        halting = process(everything, command, startlist, nodeindex, conditionslist)[:]
        if halting == "yes":
          running = False
          invite = " ## "
      elif command == "ha":
        running = False
        invite = " ## "
      elif command == "re":
        ranbefore = False
        running = False
        invite = " ## "
        conditionslist = gatherconditions(everything)
        thosegofirst = startupnodes(everything)
        startlist = getnodesready(everything, nodeindex[:])[:]

def gatherconditions(everything):
  conditionslist = []
  for bite in everything:
    for bit in bite:
      if bit[0] == "goto":
        if [bit[1][0], False] not in conditionslist:
          list.append(conditionslist, [bit[1][0], False])
  return conditionslist

def process(everything, do, startlist, nodeindex, conditionslist):
  halting = "no"
  aim = list.index(nodeindex, do)
  firstsay = 1
  for ittybit in everything[aim]:
    if ittybit[0] == "tuxtalk":
      petition = "  Tux: " + ittybit[1][1]
    if ittybit[0] == "tuxtalk.s":
      if ittybit[1] == "NO_SAMPLE_HERE_AND_DONT_WAIT_EITHER":
        pass
      else:
        print ""
        print petition
    elif ittybit[0] == "reply":
      if firstsay == 1:
        print "  A:",
        firstsay = 0
      else:
        print "    ",
      print ittybit[1]
    elif ittybit[0] == "switch":
      if ittybit[1][1] == "0":
        if ittybit[1][0] in startlist:
          list.remove(startlist, ittybit[1][0])
      else:
        if ittybit[1][0] not in startlist:
          list.append(startlist, ittybit[1][0])
      list.sort(startlist)
    elif ittybit[0] == "linked":
      print "\n  - LINKED -"
      halting = process(everything, ittybit[1][1], startlist, nodeindex, conditionslist)
    elif ittybit[0] == "goto":
      if [ittybit[1][0], False] in conditionslist:
        halting = process(everything, ittybit[1][2], startlist, nodeindex, conditionslist)
      elif [ittybit[1][0], True] in conditionslist:
        halting = process(everything, ittybit[1][1], startlist, nodeindex, conditionslist)
    elif ittybit[0] == "extra":
      if ittybit[1] == "EndDialog":
        halting = "yes"
  return halting

def printactivenodes(startlist, nodeindex):
  print ""
  for bit in startlist:
    lookup = list.index(nodeindex, bit)
    print " " + bit +  " " + everything[lookup][0][1][1]
  
def getnodesready(everything, listing):
  for chop in everything:
    for subnode in chop:
      if subnode[0] == "switch":
        if subnode[1][1] == "1":
          if subnode[1][0] in listing:
            if subnode[1][0] != chop[0][1][0]:
              list.remove(listing, subnode[1][0])
      elif subnode[0] == "goto":
        if subnode[1][1] in listing:
          list.remove(listing, subnode[1][1])
        if subnode[1][2] in listing:
          list.remove(listing, subnode[1][2])
      elif subnode[0] == "linked":
        if subnode[1][1] in listing:
          list.remove(listing, subnode[1][1])
      elif subnode[0] == "startup":
        if subnode[1] == "yes":
          if chop[0][1][0] in listing:
            list.remove(listing, chop[0][1][0])
  return listing

def startupnodes(everything):
  earlybirds = []
  for bite in everything:
    for bit in bite:
      if bit[0] == "startup":
        if bit[1] == "yes":
          list.append(earlybirds, bite[0][1][0])
  return earlybirds

####################################################################
#    HERE STARTS THE PROGRAM.
####################################################################
#Preparing the needed variables.
#GLOBAL? EEEK! I never wanted that! I will recode that later, when I feel like it.
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
oncebefore = False

while True :
 try:
  if oncebefore == True:
    autosaver.update(everything)
  prompt = ""
  readline.clear_history()
  if firstrun == True:
    firstrun = False
  else:
    print ""
  if current != None :
    nodeindex = makeindex(everything)
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
  readline.clear_history()
  if command == "q":
    end()
  elif command == "s":
    protectme = load(current)
    current = protectme
    node = None
    subnode = None
    home = clip(current)
    count = nodecount(current, home)
    everything = scanall(current, count)
    backup = everything[:]
    previous = everything[:]
    dumpnodes(everything)
    nodeindex = makeindex(everything)
    if oncebefore == True:
      autosaver.stop()
    autosaver = autosave(current, everything)
    autosaver.setDaemon(True)
    autosaver.setName("autosave")
    autosaver.start()
    oncebefore = True
  elif command == "n":
    if current == None:
      error()
    else:
      buffering = str(int(raw_input("What node? ")))
      if buffering not in nodeindex:
        error()
        continue
      x = list.index(nodeindex, buffering)
      noda = buffering
      node = everything[x]
      dumpnode(node)
      cheaplinkednode(everything, node, nodeindex)
      links(node, everything, nodeindex)
      backlinks(node, everything)
      gotolist(node, everything, nodeindex)
      showextra(node)
      showstartup(node)
  elif command == "p":
    if node == None:
      error()
    else:
      dumpnode(node)
  elif command == "pp":
    if node == None:
      error()
    else:
      dumpnoded(node)
  elif command == "d":
    if current == None:
      error()
    else:
      dumpnodes(everything)
  elif command == "dd":
    if current == None:
      error()
    else:
      for x in everything:
        dumpnode(x)
  elif command == "ddd":
    if current == None:
      error()
    else:
      rawprint(everything)
  elif command == "et":   #Edit Text.
    if node == None:
      error()
    else:
      pick = int(raw_input("Which subnode? "))
      et(node, pick)
  elif command == "da":
    if node == None:
      error()
    else:
      cheaplinkednode(everything, node, nodeindex)
      links(node, everything, nodeindex)
      backlinks(node, everything)
      gotolist(node, everything, nodeindex)
      showextra(node)
      showstartup(node)
  elif command == "de":
    if node == None:
      error()
    else:
      dumpnode(node)
      cheaplinkednode(everything, node, nodeindex)
      links(node, everything, nodeindex)
      backlinks(node, everything)
      gotolist(node, everything, nodeindex)
      showextra(node)
      showstartup(node)
  elif command == "ne":
    if current == None:
      error()
    else:
      everything = inject(everything)
      count = 0
      for spam in everything:
        count = count + 1
      everything = sort(everything, count)
  elif command == "w":
    if locked == True:
      error()
    elif current == None:
      error()
    else:
      current = writeall(current, everything)
  elif command == "l":
    if current == None:
      error()
    else:
      if locked == True:
        locked = False
      else:
        locked = True
  elif command == "so":
    if current == None:
      error()
    else:
      everything = sort(everything, count)
  elif command == "mu":
    if node == None:
      error()
    else:
      list.remove(everything, node)
      count = count - 1
      node = None
      noda = None
  elif command == "cu":
    if node == None:
      error()
    else:
      cu(node)
  elif command == "uc":
    if node == None:
      error()
    else:
      uc(node)
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
      links(node, everything, nodeindex)
  elif command == "on":
    if node == None:
      error()
    else:
      addlink(node, everything, 1)
  elif command == "no":
    if node == None:
      error()
    else:
      addlink(node, everything, 0)
  elif command == "kn":
    if node == None:
      error()
    else:
      killlink(node, everything)
  elif command == "go":
    if node == None:
      error()
    else:
      gotolist(node, everything, nodeindex)
  elif command == "og":
    if node == None:
      error()
    else:
      killgoto(node)
  elif command == "ng":
    if node == None:
      error()
    else:
      newgoto(node)
  elif command == "eg":
    if node == None:
      error()
    else:
      editgoto(node)
  elif command == "es":
    if node == None:
      error()
    else:
      showextra(node)
  elif command == "ec":
    if node == None:
      error()
    else:
      killextra(node)
  elif command == "ce":
    if node == None:
      error()
    else:
      addextra(node)
  elif command == "su":
    if node == None:
      error()
    else:
      startup(node, "yes")
  elif command == "us":
    if node == None:
      error()
    else:
      startup(node, "no")
  elif command == "se":
    if node == None:
      error()
    else:
      showstartup(node)
  elif command == "la":
    if node == None:
      error()
    else:
      backlinks(node, everything)
  elif command == "si":
    if current == None:
      error()
    else:
      simain(everything, nodeindex)
  elif command == "lo":
    if node == None:
      error()
    else:
      dumpnode(node)
      linkednode(everything, node, nodeindex)
  elif command == "up":
    if node == None:
      error()
    else:
      reprirority(everything, node, nodeindex, count, "up")
      noda = node[0][1][0]
      everything = sort(everything, count)
  elif command == "lo":
    if node == None:
      error()
    else:
      reprirority(everything, node, nodeindex, count, "down")
      noda = node[0][1][0]
      everything = sort(everything, count)
  elif command == "un":
    if node == None:
      error()
    else:
      sound(node)
  elif command == "ps":
    if node == None:
      error()
    else:
      dumpsound(node)
  else:
    error()
  
 except KeyboardInterrupt:
   print ""
   error()
 except IOError:
   error()
 except ValueError:
   error()
 except EOFError:
   print ""
   error()
