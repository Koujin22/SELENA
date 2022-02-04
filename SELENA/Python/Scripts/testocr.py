import numpy as np
from PIL import ImageGrab, Image
import cv2
import time
import pytesseract
import pyautogui
import time
from datetime import datetime, timedelta
import os
from pynput.keyboard import Key, Listener, KeyCode

def main():
    infiniteLoop = [True]
    inQueue = True
    setEstimatedQueue = False
    startQueue()
    
    lastCorrectExpectedTime = ''
    queTxt= ''
    resetCount = 0
    totalWaitTime = getTime('0:0');
    
    listener = Listener(
        on_press=on_press(infiniteLoop))
    listener.start()
    
    pytesseract.pytesseract.tesseract_cmd ='C:\\Program Files\\Tesseract-OCR\\tesseract.exe'
    while(infiniteLoop[0]):
        if(getAcceptData()):
            print("Found game! Wait time: {queTxt}")
            acceptQueue()
            inQueue = False
            infiniteLoop[0] = False
        elif(inQueue):
            if(not setEstimatedQueue):
                estTxt, estValid = getEstimatedData()
            else:
                estTxt = "Stoped"
                estValid = False
        
            queTxt, queValid = getQueueData()
                
            #Log time
            print('\x1b[1K\r', end='', flush=True);
            print(f"Current queue time: {queTxt} | Estimated: {lastCorrectExpectedTime} {getOcr(estTxt)} | Times reseted: {resetCount} | Total wait time {getTotalWaitTime(totalWaitTime, queValid, queTxt)}", end='',flush=True)

            if(estValid and not setEstimatedQueue):
                lastCorrectExpectedTime, setEstimatedQueue = updateEstimatedData(estTxt);
                
                
            if((queValid and
                setEstimatedQueue and
                getTime(queTxt) == getTime(lastCorrectExpectedTime))or
               (queValid and
                getTime(queTxt) == getTime("2:00"))):
                totalWaitTime += getTime(queTxt)
                cancelQueue()
                resetCount += 1
                inQueue = False
        else:
            time.sleep(1)
            startQueue()
            inQueue = True
            setEstimatedQueue = False
    cv2.destroyAllWindows()
    if(inQueue):
        cancelQueue()

def getOcr(estTxt):
    if(estTxt == 'Stoped'):
        return ''
    else:
        return  f'(OCR: {estTxt})'

def getTotalWaitTime(totalWaitTime, queValid, queTxt):
    if(queValid):
        return totalWaitTime+getTime(queTxt)
    else:
        return totalWaitTime

def cancelQueue():
    pyautogui.click(725, 860)

def startQueue():
    pyautogui.click(775, 860)  
    
def acceptQueue():
    pyautogui.click(1000, 725)

def updateEstimatedData(estTxt):
    tmp = getTime(estTxt)
    if(tmp.seconds >= 120):
        tmp = tmp - timedelta(seconds=15)

    s = tmp.seconds
    hours, remainder = divmod(s, 3600)
    minutes, seconds = divmod(remainder, 60)
    
    return '{:02}:{:02}'.format(int(minutes), int(seconds)), True

def getAcceptData():
    #Get queue time and print
    acceptImage = getAceptedImage()
    #printImageToScreen(acceptImage, "Acepted")


    #Binarize image
    acceptBinImage = binarizeImage(acceptImage)
                
    #Print binarize image
    #printImageToScreen(acceptBinImage, "Bin Acept")
            

    #extractText
    aceptTxt = extractText(acceptBinImage)

    return "ACCEPT!" == aceptTxt

def getQueueData():
    #Get queue time and print
    queueTimeImage = getQueueTimeImage()
    #printImageToScreen(queueTimeImage)


    #Binarize image
    queueTimeBinImage = binarizeImage(queueTimeImage)
                
    #Print binarize image
    #printImageToScreen(queueTimeBinImage, "Bin")
            

    #extractText
    queueTimeText = extractText(queueTimeBinImage)

    #Validate extracted text
    isQueueTimeValid = isTimeValid(queueTimeText)

    return queueTimeText, isQueueTimeValid;
            
def getEstimatedData():
    #Get estimated queue time and print
    estimatedQueueImage = getEstimatedQueueTimeImage()
    #printImageToScreen(estimatedQueueImage, "estimated")
    
    #Binarize image
    estimatedQueueBinImage = binarizeImage(estimatedQueueImage, True)
    
    #Print binarize image
    #printImageToScreen(estimatedQueueBinImage, "Bin est")

    #extractText
    estimatedQueueTimeText = extractText(estimatedQueueBinImage)

    
    #Validate extracted text
    isEstimatedTimeValid = isTimeValid(estimatedQueueTimeText)

    return estimatedQueueTimeText, isEstimatedTimeValid;

def getQueueTimeImage():
    return ImageGrab.grab(bbox=(1425,290,1500,325))

def getAceptedImage():
    return ImageGrab.grab(bbox=(900,720,1010,750))

def getEstimatedQueueTimeImage():
    return ImageGrab.grab(bbox=(1490,320,1530,340))

def printImageToScreen(image, name='window'):
    printscreen =  np.array(image)
    cv2.imshow(name,cv2.cvtColor(printscreen, cv2.COLOR_BGR2RGB  ))

def extractText(image):
    return pytesseract.image_to_string(
                    image, 
                    lang ='eng').rstrip("\n")

def binarizeImage(image, resize=False):
    if(resize):
        basewidth = 300
        wpercent = (basewidth/float(image.size[0]))
        hsize = int((float(image.size[1])*float(wpercent)))
        
        image = image.resize((basewidth,hsize), Image.ANTIALIAS)
        blurNumber = 7
    else:
        blurNumber = 1
    # convert the image to grayscale and blur it slightly
    filters = cv2.cvtColor(np.array( image), cv2.COLOR_BGR2XYZ   )
    gray = cv2.cvtColor(filters, cv2.COLOR_BGR2GRAY )
    
    blurred = cv2.GaussianBlur(gray, (blurNumber, blurNumber), 0)


    
    (T, threshInv) = cv2.threshold(blurred, 0, 255,
	cv2.THRESH_BINARY_INV | cv2.THRESH_OTSU)
    return threshInv;

def isTimeValid(timetxt):
    try:
        time.strptime(timetxt, '%M:%S')
        return True
    except ValueError:
        if(timetxt == ''):
            return False
        else:
            tmp = timetxt[:-2] + ':' + timetxt[-2:]
            try:
                time.strptime(tmp, '%M:%S')
                return True
            except ValueError:
                return False
            
def getTime(timetxt):
    try:
        t = datetime.strptime(timetxt, '%M:%S')
        return timedelta(minutes=t.minute, seconds=t.second)
    except ValueError:
        tmp = timetxt[:-2] + ':' + timetxt[-2:]
        t = datetime.strptime(tmp, '%M:%S')
        return timedelta(minutes=t.minute, seconds=t.second)

def on_press(infiniteLoop):
    def on_press_handle(key):
        if(key == KeyCode.from_char('q')):
            nonlocal infiniteLoop
            infiniteLoop[0] = False
    return on_press_handle
    
if __name__ == '__main__':
    main()
