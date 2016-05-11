import Tkinter as tk
import cv2
import cv2.cv as cv
import sys
import numpy as np


 
class ExampleApp(tk.Frame):
    ''' An example application for TkInter.  Instantiate
        and call the run method to run. '''
    def __init__(self, master):
        # Initialize window using the parent's constructor
        self.cap = cv2.VideoCapture(0)
        self.cap1 = cv2.VideoCapture(1)
        tk.Frame.__init__(self,
                          master,
                          width=500,
                          height=200)
        # Set the title
        self.master.title('Cam param')
	self.iteration = 0 
        # This allows the size specification to take effect
        self.pack_propagate(0)
 
        # We'll use the flexible pack layout manager
        self.pack()
 
        # The greeting selector
        # Use a StringVar to access the selector's value
        self.SADWindowSize = tk.IntVar()
        self.SADWindowSize = 9
        self.slider_SADWindowSize = tk.Scale( self, from_=2, to=16,  variable = self.SADWindowSize)
        self.slider_SADWindowSize.pack(fill=tk.X, side=tk.LEFT)
        self.slider_SADWindowSize.set(4)
        self.preFilterType = tk.IntVar()
        self.preFilterType = 1
        self.slider_preFilterType = tk.Scale( self, from_=0, to=1, variable = self.preFilterType)
        self.slider_preFilterType.pack(fill=tk.X, side=tk.LEFT)
        self.slider_preFilterType.set(1)
        self.preFilterSize = tk.IntVar()
        self.preFilterSize = 3
        self.slider_preFilterSize = tk.Scale( self, from_=2, to=4, variable = self.preFilterSize)
        self.slider_preFilterSize.pack(fill=tk.X, side=tk.LEFT)
        self.slider_preFilterSize.set(3)
        self.preFilterCap = tk.IntVar()
        self.preFilterCap = 61
        self.slider_preFilterCap = tk.Scale( self, from_=1, to=63, variable = self.preFilterCap)
        self.slider_preFilterCap.pack(fill=tk.X, side=tk.LEFT)
        self.slider_preFilterCap.set(61)
        self.minDisparity = tk.IntVar()
        self.minDisparity = 31
        self.slider_minDisparity = tk.Scale( self, from_=1, to=63, variable = self.minDisparity)
        self.slider_minDisparity.pack(fill=tk.X, side=tk.LEFT)
        self.slider_minDisparity.set(31)
        self.numberOfDisparities = tk.IntVar()
        self.numberOfDisparities = 7
        self.slider_numberOfDisparities = tk.Scale( self, from_=1, to=16, variable = self.numberOfDisparities)
        self.slider_numberOfDisparities.pack(fill=tk.X, side=tk.LEFT)
        self.slider_numberOfDisparities.set(7)
        self.textureThreshold = tk.IntVar()
        self.textureThreshold = 507
        self.slider_textureThreshold = tk.Scale( self, from_=1, to=1000, variable = self.textureThreshold)
        self.slider_textureThreshold.pack(fill=tk.X, side=tk.LEFT)
        self.slider_textureThreshold.set(507)
        self.speckleRange = tk.IntVar()
        self.speckleRange = 8
        self.slider_speckleRange = tk.Scale( self, from_=0, to=10, variable = self.speckleRange)
        self.slider_speckleRange.pack(fill=tk.X, side=tk.LEFT)
        self.slider_speckleRange.set(8)
        self.uniquenessRatio = tk.IntVar()
        self.uniquenessRatio = 0
        self.slider_uniquenessRatio = tk.Scale( self, from_=0, to=10, variable = self.uniquenessRatio)
        self.slider_uniquenessRatio.pack(fill=tk.X, side=tk.LEFT)
        self.slider_uniquenessRatio.set(0)
        self.speckleWindowSize = tk.IntVar()
        self.speckleWindowSize = 0
        self.slider_speckleWindowSize = tk.Scale( self, from_=0, to=30, variable = self.speckleWindowSize)
        self.slider_speckleWindowSize.pack(fill=tk.X, side=tk.LEFT)
        self.slider_speckleWindowSize.set(20)
      
    def cam_update(self):
        ret, imgLeft = self.cap.read()
        ret1, imgRight = self.cap1.read()
        disparity = self.getDisparity(imgLeft, imgRight)
        cv2.imshow('right', imgRight)
        cv2.imshow('left', imgLeft)
        cv2.imshow('disparity', disparity)
	#np.savetxt('data_' + str(self.iteration) , disparity, fmt='%.4e')
	self.iteration += 1
	cv2.waitKey(200)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            self.cap.release()
            self.cap1.release()
            cv2.destroyAllWindows()
        self.after(10, self.cam_update) 

    def run(self):
        ''' Run the app '''
        self.after(0, self.cam_update)
        self.mainloop()

    def getDisparity(self, imgLeft, imgRight, method="BM"):
        
	#imgLeft = cv2.fastNlMeansDenoisingColored(imgLeft, None, 10, 10, 7, 21)
	#imgRight = cv2.fastNlMeansDenoisingColored(imgRight, None, 10, 10, 7, 21)
	gray_left = cv2.cvtColor(imgLeft, cv.CV_BGR2GRAY)
        gray_right = cv2.cvtColor(imgRight, cv.CV_BGR2GRAY)
        #print gray_left.shape
        c, r = gray_left.shape
        if method == "BM":
            sbm = cv.CreateStereoBMState()
            disparity = cv.CreateMat(c, r, cv.CV_32F)
            sbm.SADWindowSize = 2 * self.slider_SADWindowSize.get() + 1
            sbm.preFilterType = self.slider_preFilterType.get()
            sbm.preFilterSize = 2 * self.slider_preFilterSize.get() + 1
            sbm.preFilterCap = self.slider_preFilterCap.get()
            sbm.minDisparity = - self.slider_minDisparity.get()
            sbm.numberOfDisparities = 16 * self.slider_numberOfDisparities.get()
            sbm.textureThreshold = self.slider_textureThreshold.get()
            sbm.uniquenessRatio = self.slider_uniquenessRatio.get()
            sbm.speckleRange = self.slider_speckleRange.get()
            sbm.speckleWindowSize = self.slider_speckleWindowSize.get()

            gray_left = cv.fromarray(gray_left)
            gray_right = cv.fromarray(gray_right)

            cv.FindStereoCorrespondenceBM(gray_left, gray_right, disparity, sbm)
            disparity_visual = cv.CreateMat(c, r, cv.CV_8U)
            cv.Normalize(disparity, disparity_visual, 0, 255, cv.CV_MINMAX)
            disparity_visual = np.array(disparity_visual)

        elif method == "SGBM":
            sbm = cv2.StereoSGBM()
            sbm.SADWindowSize = 2 * self.slider_SADWindowSize.get()
            sbm.numberOfDisparities = 9 # self.slider_numberOfDisparities.get()
            sbm.preFilterCap = self.slider_preFilterCap.get()
            sbm.minDisparity = - self.slider_minDisparity.get()
            sbm.uniquenessRatio = self.slider_uniquenessRatio.get()
            sbm.speckleWindowSize = self.slider_speckleWindowSize.get()
            sbm.speckleRange = self.slider_speckleRange.get()
            sbm.disp12MaxDiff = 1;
            sbm.fullDP = False;

            disparity = sbm.compute(gray_left, gray_right)
            disparity_visual = cv2.normalize(disparity, alpha=0, beta=255, norm_type=cv2.cv.CV_MINMAX, dtype=cv2.cv.CV_8U)

        return disparity_visual







app = ExampleApp(tk.Tk())
app.run()
