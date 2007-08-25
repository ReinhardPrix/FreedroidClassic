#!/usr/bin/python


import sys, os.path

from PyQt4.QtGui import *
from PyQt4.QtCore import *

a = QApplication(sys.argv)
a.setStyle(QStyleFactory.create("Cleanlooks"))

fn = "../graphics/obstacles/iso_obstacle_0114"

def getCoords(fn):
    f = open(fn, "r")
    x = 0
    y = 0
    for rivi in f.readlines():
        if rivi[:7] == "OffsetX":
            x = int(rivi.split("=")[1][:-1])
            
        if rivi[:7] == "OffsetY":
            y = int(rivi.split("=")[1][:-1])
            
    f.close()
    return x, y

class gs(QGraphicsScene):
    def __init__(self, *args):
        QGraphicsScene.__init__(self, *args)
        
        
    def mousePressEvent(self, e):
        if e.button() == Qt.LeftButton:
            print e.buttonDownScenePos(Qt.LeftButton).x(), e.buttonDownScenePos(Qt.LeftButton).y()
            self.emit(SIGNAL("klik(QPointF)"), e.buttonDownScenePos(Qt.LeftButton))                 
       

class ui(QDialog):
    def __init__(self, *args):
        QDialog.__init__(self, *args)
        self.setObjectName("ikkuna")
        #self.resize(QSize(QRect(0,0,600,600).size()).expandedTo(self.minimumSizeHint()))
               
        self.vboxlayout = QVBoxLayout(self)
        self.vboxlayout.setObjectName("vboxlayout")             
        self.hboxlayout = QHBoxLayout(self)
        self.hboxlayout.setObjectName("hboxlayout")
        
        self.nappu = QPushButton("open pic", self)       
        self.connect(self.nappu, SIGNAL("clicked()"), self.avaa)
        self.hboxlayout.addWidget(self.nappu)
        
        self.nappu2 = QPushButton("save offset", self)       
        self.connect(self.nappu2, SIGNAL("clicked()"), self.tallenna)
        self.hboxlayout.addWidget(self.nappu2)
        
        self.label = QLabel("tekstii", self)
        self.hboxlayout.addWidget(self.label)
        
        spacerItem = QSpacerItem(50,20, QSizePolicy.Expanding, QSizePolicy.Minimum)
        self.hboxlayout.addItem(spacerItem)

        self.vboxlayout.addLayout(self.hboxlayout)
                       
        self.scene = gs()
        self.scene.setItemIndexMethod(QGraphicsScene.BspTreeIndex)
        self.kuva = QPixmap()
        self.pix = self.scene.addPixmap(self.kuva)
        
        self.view = QGraphicsView(self.scene)
        self.view.setObjectName("theview")
        self.view.resize(QSize(QRect(50,50,400,400).size()).expandedTo(self.minimumSizeHint()))
        self.view.setInteractive(True)
        
        self.fn = ""
        self.ofsfn = ""
        self.nx = 0
        self.ny = 0
        self.ox = 0
        self.oy = 0
        self.lev = 25
        
        self.yviiva = self.scene.addLine(QLineF(0, 0, 0, 0), QPen(QColor("red")))
        self.xviiva = self.scene.addLine(QLineF(0, 0, 0, 0), QPen(QColor("red")))
        
        try:
            self.paivitaKuva(fn + ".png")
        except:
            pass
        self.vboxlayout.addWidget(self.view)
        
        self.connect(self.scene, SIGNAL("klik(QPointF)"), self.kliksor)
     
    def kliksor(self, p):
        print p.x(), p.y()
        self.paivitaRuksi(p.x(), p.y())       

               
    def paivitaRuksi(self, nx, ny):       
        self.xviiva.setLine(QLineF(nx, ny-self.lev, nx, ny+self.lev))
        self.yviiva.setLine(QLineF(nx-self.lev, ny, nx+self.lev, ny))
        self.label.setText("ox=%s, oy=%s, nx=%s, ny=%s" % (self.ox, self.oy, nx, ny))
        self.nx = nx
        self.ny = ny
        
        
    
    def paivitaKuva(self, fn):        
        self.kuva = QPixmap(fn)
        self.pix.setPixmap(self.kuva)
        
        nx = self.kuva.width() - (self.kuva.width() + self.ox)
        ny = self.kuva.height() - (self.kuva.height() + self.oy)
                  
        self.paivitaRuksi(nx, ny)
        
        #self.label.setText("ox=%s, oy=%s, nx=%s, ny=%s" % (self.ox, self.oy, nx, ny))

    def tallenna(self):
        
        
        outp = """
\n** Start of iso_image offset file **\n\n
OffsetX=-%s\n
OffsetY=-%s\n
GraphicsFileName=%s\n\n
** End of iso_image offset file **\n\n
""" % (int(self.nx), int(self.ny), os.path.basename(str(self.ofsfn)))
        
        try:
            f = open(self.ofsfn, "w+")
            f.write(outp)
            f.close()
            QMessageBox.information(self, ":)", "%s was updated, happy happy joy joy.." % (str(self.ofsfn)))
        except:
            QMessageBox.critical(self, "Error", "Yikes, offset file (%s) could not be saved. \nwho knows why.. " % (str(self.ofsfn)))
        

    def avaa(self):
        
        fn = QFileDialog.getOpenFileName(self, "choose image", "../graphics/obstacles/", "Images (*.jpg *.png)")
        
        self.fn = str(fn)
        self.ofsfn = str(fn).replace(".png", ".offset")            
       
        self.ox, self.oy = getCoords(self.ofsfn)
        self.paivitaKuva(self.fn)
        
        
w = ui()
w.show()


a.exec_()
