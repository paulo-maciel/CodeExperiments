import cv2 as cv
import numpy as np
import math

def createMask( cnt, mask):
  # print("Creating mask ...")
  cv.drawContours(mask, [cnt], -1, 0, 5)

#works well
# img = cv.imread('shapesbw.png')
# imgray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
# ret, thresh = cv.threshold(imgray, 200, 255, 0)
# contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

# cv.drawContours(img, contours, -1, (0,0,255), 5, cv.LINE_8)
# cv.imwrite("contours.png", img)

imagem = cv.imread("shapesdraw.png", cv.IMREAD_COLOR)
img = imagem.copy()
imgray = cv.cvtColor(imagem, cv.COLOR_BGR2GRAY)
_, threshold = cv.threshold(imgray, 200, 255, cv.THRESH_BINARY)

# invert the image
threshold = cv.bitwise_not(threshold)
contours, _ = cv.findContours(threshold, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)

for cnt in contours:
    approx = cv.approxPolyDP(cnt,0.01*cv.arcLength(cnt,True),True)
    # print("length: ",len(approx))
    if len(approx)==5:
        # print("pentagon")
        cv.drawContours(img,[cnt],0,(0,0,0),-1)
    elif len(approx)==3:
        # print("triangle")
        cv.drawContours(img,[cnt],0,(0,255,0),-1)
    elif len(approx)==4:
        # print("square")
        cv.drawContours(img,[cnt],0,(255, 255,0),-1)
    elif len(approx) == 9:
        # print("half-circle")
        cv.drawContours(img,[cnt],0,(255,0,255),-1)
    elif len(approx) > 15:
        # print("circle: length: ", len(approx))
        cv.drawContours(img,[cnt],0,(255,0,255),-1)
    elif len(approx) > 6 and len(approx) < 15:
        # print("ellipse: length: ", len(approx))
        cv.drawContours(img,[cnt],0,(0,255,255),-1)

# for i in range(len(contours)):
#   print("Contour",i,(0,0,255))
#   cv.drawContours(img, contours, i, (0,0,255), 10)
cv.drawContours(img, contours, -1, (0,0,255), 5, cv.LINE_8)

# cv.imwrite("shapesdetected.png", img)
# print("Contour image type", img.dtype)


# Remove the circles.

# Initialize a mask to ones.
mask = np.ones(img.shape[:2], dtype="uint8") * 255
circles = []
for cnt in contours:
    approx = cv.approxPolyDP(cnt,0.01*cv.arcLength(cnt,True),True)

    # Remove ellipse
    if len(approx) > 6 and len(approx) < 15:
      cv.drawContours(mask,[cnt],0,(0,255,255),-1)
      createMask(cnt, mask)
      moment = cv.moments(cnt)
      cx = int(moment['m10']/moment['m00'])
      cy = int(moment['m01']/moment['m00'])
      center = cx,cy
      # print("Center: ", center)
      peri = cv.arcLength(cnt, True)
      radius = peri/(2*math.pi)
      # print("Radius: ", radius)
      circles.append([center,radius])

# print("Circles are: ", circles)


# invert the mask
mask = cv.bitwise_not(mask)
# cv.imwrite("mask.png", mask)

mask = cv.cvtColor(mask, cv.COLOR_GRAY2BGR)
mask = img + mask
combined = mask

# cv.imwrite("combined.png", mask)

mask2 = cv.cvtColor(mask, cv.COLOR_BGR2GRAY)
img = cv.bitwise_and(img, img, mask=mask2)
# cv.imwrite("shaperemove.png", img)

# center = (circles[1][0][0], circles[1][0][0])
# radius = circles[1][1]
# print("center:", center)
# print("radius:", radius)

cv.circle(combined, center, int(radius), (0,0,255), 5)

for circle in circles:
  cv.circle(combined, circle[0], int(circle[1]), (0,0,255), 5)



cv.imwrite("addedcircle.png", combined)