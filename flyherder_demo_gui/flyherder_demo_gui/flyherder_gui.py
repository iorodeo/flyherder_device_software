import sys, platform, serial, time, math
from PyQt4 import QtCore
from PyQt4 import QtGui
from flyherder_serial import FlyHerder
from flyherder_gui_ui import Ui_MainWindow 

DFLT_PORT_WINDOWS = 'com1' 
DFLT_PORT_LINUX = '/dev/ttyUSB0' 
MIN_SPEED = 1 # mm/s
MAX_SPEED = 90 # mm/s
MIN_SEP_MM = int(0)  # mm
MAX_SEP_MM = int(12*25.4) # mm
MIN_POS = 0 # mm
TIMER_INTERVAL_MS = 250 # ms
DEFAULT_SPEED = 10 # mm/s
DEFAULT_SEP = 200 # mm
NUM_AXIS = 4
INDEX_TO_AXIS = {0:'x0', 1: 'y0', 2: 'x1', 3: 'y1'}
DEFAULT_POS = {'x0':-1,'y0':-1,'x1':-1,'y1':-1}
DEFAULT_POS_LABELS = {'x0':'','y0':'','x1':'','y1':''}
DEFAULT_ORIENT = {'x0':'-','y0':'-','x1':'+','y1':'+'}

class FlyHerderMainWindow(QtGui.QMainWindow,Ui_MainWindow):

    def __init__(self,parent=None):
        super(FlyHerderMainWindow,self).__init__(parent)
        self.setupUi(self)
        self.setupTimer()
        self.initialize()
        self.connectActions()

    def setupTimer(self):
        self.timer = QtCore.QTimer()
        self.timer.setInterval(TIMER_INTERVAL_MS)
        try:
            self.timer.timeout.connect(self.timer_Callback)
        except AttributeError:
            self.connect(self.timer,QtCore.SIGNAL("timeout()"),self.timer_Callback)

    def connectActions(self):
        self.portLineEdit.editingFinished.connect(self.portChanged_Callback)
        self.pwrCheckBox.toggled.connect(self.pwrClicked_Callback)
        self.boundsCheckBox.toggled.connect(self.setBoundsCheck_Callback)
        self.moveSpeedLineEdit.editingFinished.connect(self.setMoveSpeed_Callback)
        self.homeSpeedLineEdit.editingFinished.connect(self.setHomeSpeed_Callback)
        self.xSepLineEdit.editingFinished.connect(self.updateSep_Callback)
        self.ySepLineEdit.editingFinished.connect(self.updateSep_Callback)
        self.connectPushButton.pressed.connect(self.connectPressed_Callback)
        self.connectPushButton.clicked.connect(self.connectClicked_Callback)
        self.homePushButton.clicked.connect(self.homeClicked_Callback)
        self.stopPushButton.clicked.connect(self.stopClicked_Callback)
        self.movePushButton.clicked.connect(self.moveClicked_Callback)

    def initialize(self):
        osType = platform.system()
        if osType == 'Linux': 
            self.port = DFLT_PORT_LINUX 
        else: 
            self.port = DFLT_PORT_WINDOWS 
        self.portLineEdit.setText(self.port) 
        self.dev = None
        self.NUM_AXIS = NUM_AXIS
        self.axisPos = dict(DEFAULT_POS)
        self.axisOrient = dict(DEFAULT_ORIENT)
        self.maxPos = {'x':0,'y':0}
        self.isHomed = False
        self.isInitialized = False
        self.moveSpeed = self.moveSpeedLineEdit.text()
        self.homeSpeed = self.homeSpeedLineEdit.text()
        self.moveSpeedValidator = QtGui.QIntValidator(MIN_SPEED,MAX_SPEED,self.moveSpeedLineEdit)
        self.moveSpeedLineEdit.setValidator(self.moveSpeedValidator)
        self.homeSpeedValidator = QtGui.QIntValidator(MIN_SPEED,MAX_SPEED,self.homeSpeedLineEdit)
        self.homeSpeedLineEdit.setValidator(self.homeSpeedValidator)
        self.xSepValidator = QtGui.QIntValidator(MIN_SEP_MM,MAX_SEP_MM,self.xSepLineEdit)
        self.xSepLineEdit.setValidator(self.xSepValidator)
        self.ySepValidator = QtGui.QIntValidator(MIN_SEP_MM,MAX_SEP_MM,self.ySepLineEdit)
        self.ySepLineEdit.setValidator(self.ySepValidator)
        self.setWidgetEnabledOnDisconnect()

    def closeEvent(self, event):
        if not self.dev == None:
            if self.dev.isDrivePowerOn():
                QtGui.QMessageBox.warning(self,'Warning','Shutting off power to the drive...')
            try:
                self.cleanUpAndCloseDevice()
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e))

    def portChanged_Callback(self):
        self.port = str(self.portLineEdit.text())

    def connectPressed_Callback(self):
        if self.dev == None:
            self.connectPushButton.setText('Disconnect')
            self.portLineEdit.setEnabled(False)
            self.statusbar.showMessage('Connecting...')

    def connectClicked_Callback(self):
        if self.dev == None:
            try:
                self.dev = FlyHerder(self.port)
                connected = True
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e))
                self.connectPushButton.setText('Connect')
                self.statusbar.showMessage('Not Connected')
                self.portLineEdit.setEnabled(True)
                connected = False
        else:
            try:
                self.cleanUpAndCloseDevice()
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e))
            connected = False

        if connected:
            self.setWidgetEnabledOnConnect()
        else:
            self.setWidgetEnabledOnDisconnect()

    def powerOnDrive(self):
        if not self.isInitialized:
            n = self.NUM_AXIS
            self.maxPos['x'] = int(self.xSepLineEdit.text())
            self.maxPos['y'] = int(self.ySepLineEdit.text())
            for i in range(n):
                le = getattr(self,'posLineEdit_{0}'.format(i))
                le.editingFinished.connect(self.updatePos_Callback)
                axis = INDEX_TO_AXIS[i]
                if (axis.find('y')>=0):
                    posValidator = QtGui.QIntValidator(MIN_POS,self.maxPos['y'],le)
                else:
                    posValidator = QtGui.QIntValidator(MIN_POS,self.maxPos['x'],le)
                le.setValidator(posValidator)
                le.setEnabled(False)

                cb_orient = getattr(self,'orientCheckBox_{0}'.format(i))
                cb_orient.toggled.connect(self.updateOrient_Callback)
                if cb_orient.isChecked():
                    self.axisOrient[axis] = '-'
                else:
                    self.axisOrient[axis] = '+'

                le_pos = getattr(self,'posLineEdit_{0}'.format(i))
                self.axisPos[axis] = int(le_pos.text())
            self.isInitialized = True
        try:
            self.dev.stop()
            self.dev.enableBoundsCheck()
            self.dev.setDrivePowerOn()
        except Exception, e:
            QtGui.QMessageBox.critical(self,'Error', str(e))

    def powerOffDrive(self):
        self.updatePosLineEdits(DEFAULT_POS)
        self.updatePosLabels(DEFAULT_POS_LABELS)
        self.moveSpeedLineEdit.setText(str(DEFAULT_SPEED))
        self.homeSpeedLineEdit.setText(str(DEFAULT_SPEED))
        self.xSepLineEdit.setText(str(DEFAULT_SEP))
        self.ySepLineEdit.setText(str(DEFAULT_SEP))
        try:
            self.dev.setDrivePowerOff()
            while self.dev.isDrivePowerOn():
                continue
        except Exception, e:
            QtGui.QMessageBox.critical(self,'Error', str(e))
        self.isHomed = False
        self.homePushButton.setEnabled(False)
        self.stopPushButton.setEnabled(False)
        self.setPosLineEditsEnabled(False)

    def setMoveSpeed_Callback(self):
        moveSpeed = self.moveSpeedLineEdit.text()
        if len(moveSpeed) > 0:
            self.moveSpeed = int(moveSpeed)

    def setHomeSpeed_Callback(self):
        homeSpeed = self.homeSpeedLineEdit.text()
        if len(homeSpeed) > 0:
            self.homeSpeed = int(homeSpeed)

    def setBoundsCheck_Callback(self):
        if self.dev is None:
            return
        if self.boundsCheckBox.isChecked():
            try:
                self.dev.enableBoundsCheck()        
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e))
        else:
            try:
                self.dev.disableBoundsCheck()        
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e))
            
    def updateSep_Callback(self):
        xSep = self.xSepLineEdit.text()
        ySep = self.ySepLineEdit.text()
        if (len(xSep) > 0) and (len(ySep) > 0):
            self.maxPos['x'] = int(self.xSepLineEdit.text())
            self.maxPos['y'] = int(self.ySepLineEdit.text())
            n = self.NUM_AXIS
            for i in range(n):
                le = getattr(self,'posLineEdit_{0}'.format(i))
                le.textChanged.connect(self.updatePos_Callback)
                axis = INDEX_TO_AXIS[i]
                if (axis.find('y')>=0):
                    posValidator = QtGui.QIntValidator(MIN_POS,self.maxPos['y'],le)
                else:
                    posValidator = QtGui.QIntValidator(MIN_POS,self.maxPos['x'],le)
                le.setValidator(posValidator)

            if (int(xSep)<MIN_SEP_MM) or (int(ySep)<MIN_SEP_MM):
                QtGui.QMessageBox.warning(self,'Warning','Please check separation values...')
                return
            try:
                self.dev.setMaxSeparation(self.maxPos)        
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e).title())

    def pwrClicked_Callback(self):
        if self.dev is None:
            return
        if self.pwrCheckBox.isChecked():
            self.statusbar.showMessage('Connected: Enabling...')
            self.pwrCheckBox.setEnabled(False)
            self.powerOnDrive()
            self.paramGroupBox.setEnabled(True)
            self.pwrCheckBox.setEnabled(True)
            self.statusbar.showMessage('Connected: Power On, Not Homed')
            self.moveGroupBox.setEnabled(False)
            self.positionGroupBox.setEnabled(True)
            self.movePushButton.setEnabled(False)
            self.homePushButton.setEnabled(True)
            self.stopPushButton.setEnabled(True)
            try:
                pos = self.dev.getPosition()
            except Exception, e:
                QtGui.QMessageBox.critical(self,'Error', str(e).title())
            self.updatePosLabels(pos)
        else:
            self.powerOffDrive()
            self.statusbar.showMessage('Connected: Power Off')
            self.paramGroupBox.setEnabled(False)
            self.moveGroupBox.setEnabled(False)
            self.movePushButton.setEnabled(False)

    def updatePos_Callback(self):
        for i in range(self.NUM_AXIS):
            le_pos = getattr(self,'posLineEdit_{0}'.format(i))
            axis = INDEX_TO_AXIS[i]
            pos = le_pos.text()
            if not (len(pos)>0):
                return
            self.axisPos[axis] = int(pos)

    def updateOrient_Callback(self):
        for i in range(self.NUM_AXIS):
            cb_orient = getattr(self,'orientCheckBox_{0}'.format(i))
            axis = INDEX_TO_AXIS[i]
            if cb_orient.isChecked():
                self.axisOrient[axis] = '-'
            else:
                self.axisOrient[axis] = '+'

    def guiValuesCheck(self):
        rspFailed = False
        rspSuccess = True
        for i in range(self.NUM_AXIS):
            le_pos = getattr(self,'posLineEdit_{0}'.format(i))
            pos = le_pos.text()
            if not (len(pos)>0):
                return rspFailed
        xSep = self.xSepLineEdit.text()
        ySep = self.ySepLineEdit.text()
        moveSpeed = self.moveSpeedLineEdit.text()
        homeSpeed = self.homeSpeedLineEdit.text()
        if not ( (len(ySep)>0)and(len(xSep)>0)and(len(moveSpeed)>0)and(len(homeSpeed)>0) ):
            return rspFailed

        return rspSuccess

    def homeClicked_Callback(self):
        if self.dev is None:
            return
        self.updateSep_Callback()
        self.setHomeSpeed_Callback()
        self.setMoveSpeed_Callback()
        if not self.guiValuesCheck():
            QtGui.QMessageBox.warning(self,'Warning','Please check GUI values...')
            return
        try:
            self.timer.start()
            self.statusbar.showMessage('Connected: Homing...')
            self.dev.setOrientation(self.axisOrient)        
            self.dev.setSpeed(self.homeSpeed)        
            self.dev.moveToHome()
        except IOError, e:
            msgTitle = 'Unable to home motors:'
            QtGui.QMessageBox.warning(self,msgTitle, str(e).title())           
            self.statusbar.showMessage('Connected: Drive On')

    def moveClicked_Callback(self):
        if self.dev is None:
            return
        self.updateSep_Callback()
        self.setHomeSpeed_Callback()
        self.setMoveSpeed_Callback()
        if not self.guiValuesCheck():
            QtGui.QMessageBox.warning(self,'Warning','Please check GUI values...')
            return
        if self.isHomed:
            try:
                self.timer.start()
                self.statusbar.showMessage('Connected: Moving...')
                self.dev.setOrientation(self.axisOrient)        
                self.dev.setSpeed(self.moveSpeed)        
                self.dev.moveToPosition(self.axisPos)
            except IOError, e:
                msgTitle = 'Warning'
                QtGui.QMessageBox.warning(self,msgTitle, str(e).title())           
                self.statusbar.showMessage('Connected: Drive On')

    def updatePosLineEdits(self,pos):
        self.posLineEdit_0.setText(str(int(round(pos['x0']))))
        self.posLineEdit_1.setText(str(int(round(pos['y0']))))
        self.posLineEdit_2.setText(str(int(round(pos['x1']))))
        self.posLineEdit_3.setText(str(int(round(pos['y1']))))
            
    def updatePosLabels(self,pos):
        if self.pwrCheckBox.isChecked():
            try:
                self.posLabel_0.setText(str(int(round(pos['x0']))))
                self.posLabel_1.setText(str(int(round(pos['y0']))))
                self.posLabel_2.setText(str(int(round(pos['x1']))))
                self.posLabel_3.setText(str(int(round(pos['y1']))))
                return
            except TypeError:
                pass
        self.posLabel_0.setText('')
        self.posLabel_1.setText('')
        self.posLabel_2.setText('')
        self.posLabel_3.setText('')

    def stopClicked_Callback(self):
        if self.dev is None:
            return
        pos = None
        try:
            self.dev.stop()        
            pos = self.dev.getPosition()
        except IOError, e:
            msgTitle = 'Unable to stop:'
            QtGui.QMessageBox.warning(self,msgTitle, e)           
            if self.isHomed:
                self.statusbar.showMessage('Connected: Power On, Homed')
            else:
                self.statusbar.showMessage('Connected: Power On, Not Homed')
        if pos is not None:
            self.updatePosLabels(pos)

    def timer_Callback(self):
        running = True
        try:
            if not self.dev.isRunning():
                running = False
                self.timer.stop()
        except IOError, e:
            msgTitle = 'Unable to query the device'
            QtGui.QMessageBox.warning(self,msgTitle, e)           
            running = False
            self.timer.stop()
        if not running: 
            try:
                pos = self.dev.getPosition()
                isInHomePosition = self.dev.isInHomePosition()
            except IOError, e:
                msgTitle = 'Unable to query the device'
                QtGui.QMessageBox.warning(self,msgTitle, e)           
                return

            if isInHomePosition and (not self.isHomed):
                self.isHomed = True
            self.updatePosLabels(pos)
            self.setWidgetEnabledOnStop()
        else:
            self.setWidgetDisabledOnRun()

    def setWidgetDisabledOnRun(self):
        self.paramGroupBox.setEnabled(False)
        self.connectPushButton.setEnabled(False)
        self.movePushButton.setEnabled(False)
        self.homePushButton.setEnabled(False)
        self.setPosLineEditsEnabled(False)

    def setWidgetEnabledOnStop(self):
        self.paramGroupBox.setEnabled(True)
        self.connectPushButton.setEnabled(True)
        self.homePushButton.setEnabled(True)
        if self.isHomed:
            self.moveGroupBox.setEnabled(True)
            self.movePushButton.setEnabled(True)
            self.statusbar.showMessage('Connected: Power On, Homed')
            self.setPosLineEditsEnabled(True)

    def setPosLineEditsEnabled(self,b):
        n = self.NUM_AXIS
        for i in range(n):
            le = getattr(self,'posLineEdit_{0}'.format(i))
            le.setEnabled(b)

    def setWidgetEnabledOnDisconnect(self):
        self.pwrCheckBox.setChecked(False)
        self.portLineEdit.setEnabled(True)
        self.pwrCheckBox.setEnabled(False)
        self.moveGroupBox.setEnabled(False)
        self.paramGroupBox.setEnabled(False)
        self.positionGroupBox.setEnabled(False)
        self.connectPushButton.setText('Connect')
        self.statusbar.showMessage('Not Connected')

    def setWidgetEnabledOnConnect(self):
        self.portLineEdit.setEnabled(False)
        self.paramGroupBox.setEnabled(True)
        self.pwrCheckBox.setEnabled(True)
        self.statusbar.showMessage('Connected: Power Off')

    def cleanUpAndCloseDevice(self):
        self.powerOffDrive()
        self.pwrCheckBox.setChecked(False)
        self.dev.close()
        self.dev = None

    def main(self):
        self.show()

def flyHerderMain():
    app = QtGui.QApplication(sys.argv)
    mainWindow = FlyHerderMainWindow()
    mainWindow.main()
    app.exec_()

# -----------------------------------------------------------------------------
if __name__ == '__main__':
    flyHerderMain()
