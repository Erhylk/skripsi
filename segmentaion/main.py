import cv2
import os
import numpy as np
import pyrebase
from urllib.request import urlopen
from PIL import Image
import requests

config = {
  "apiKey": "AIzaSyBinHQj79l7UQ5Gs6b5mdbk1xJ88E2KfAg",
  "authDomain": "skripsi-2022-363703.firebaseapp.com",
  "databaseURL": "https://skripsi-2022-363703-default-rtdb.asia-southeast1.firebasedatabase.app/",
  "storageBucket": "skripsi-2022-363703.appspot.com",
  "serviceAccount": "skripsi-2022-363703-firebase-adminsdk-gmal9-80b03c1e17.json"
}

def url_to_image(url, readFlag=cv2.IMREAD_COLOR):
    # download the image, convert it to a NumPy array, and then read
    # it into Opencv2 format
    resp = urlopen(url)
    image = np.asarray(bytearray(resp.read()), dtype="uint8")
    image = cv2.imdecode(image, readFlag)

    # return the image
    return image

def segment_send_telegram(event, context):
  firebase = pyrebase.initialize_app(config)
  auth = firebase.auth()
  user = auth.sign_in_with_email_and_password('syammk16d@student.unhas.ac.id','123qweasd')
  token = user['idToken']
  image_list = firebase.storage().child('images').list_files()
  latest_image = ''
  for file in image_list:            
      try:
          latest_image = file.name
      except:
          print('something happen')
          
  image_url = firebase.storage().child(latest_image).get_url(token)

  image1 = url_to_image(image_url)
  print(image1)
  image2 = cv2.cvtColor(image1, cv2.COLOR_RGB2GRAY)
  blur = cv2.GaussianBlur(image2,(5,5),0)
  otsu_threshold, result = cv2.threshold(blur, 0, 255,cv2.THRESH_BINARY_INV+cv2.THRESH_OTSU)
  otsu_threshold
  result = 255 - result

  kernel = np.uint8([[0,1,0],[1,1,1],[0,1,0]])
  kernel

  dilation = cv2.dilate(result, kernel, iterations=18)

  erotion = cv2.erode(dilation, kernel, iterations = 25)

  contours, hierarchy = cv2.findContours(erotion,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
  # print(len(contours))
  width = int(image1.shape[1])
  height = int(image1.shape[0])

  # find largest area contour
  max_area = -1
  for i in range(len(contours)):
      area = cv2.contourArea(contours[i])
      x,y,w,h = cv2.boundingRect(contours[i])
      # print(area)
      if area>max_area:
          cnt = contours[i]
          max_area = area



  cv2.drawContours(image2, contours, -1, (0, 255, 0), 3)

  img = cv2.cvtColor(image2, cv2.COLOR_GRAY2RGB)
  im_pil = Image.fromarray(img)

  tmp_path = '/tmp/'

  if (os.path.isdir(tmp_path+'images') != True):
    os.makedirs(tmp_path+'images')
  im_pil.save(tmp_path+latest_image, format="PNG")

  firebase.storage().child('archive/'+latest_image).put(tmp_path+latest_image)
  segmented_url = firebase.storage().child('archive/'+latest_image).get_url(token)
  r = requests.get(segmented_url)
  if (r.status_code == 200):
    os.remove(tmp_path+latest_image)

  # Firebase RTDB
  rtdb_data = {'img_file': latest_image, 'value': max_area}
  is_image_exist = firebase.database().child("segmented").order_by_child('img_file').equal_to(latest_image).get().val()
  prev_val = max_area
  if (len(is_image_exist) == 0):
    firebase.database().child('segmented').push(rtdb_data)
  
  has_data = firebase.database().child('segmented').get().val()
  if (len(has_data) > 1):
    prev_data = firebase.database().child('segmented').order_by_child('img_file').limit_to_last(2).get().val()
    prev_val = list(prev_data.values())[0].get('value')

  perkembangan = (prev_val-max_area)/(prev_val)*(-100)
  print(perkembangan)

  # print("Jumlah pixel dari objek =" , max_area)
  # print(image2.shape)
  # # print(x,y,w,h)
  # print("ukuran gambar w*h = ", width, height)
  # print("total pixel gambar= ", (width*height))
  # print(max_area/(width*height)*100)
  # #CONTOH LUAS OBJEK
  # print("luas objek = ", 900 / (width * height) * max_area)

  #TELEGRAM BOT
  TOKEN = "5731267048:AAGObbWGwAb7SJ4Ez5puj98v5GrPHMap7NM"
  chat_id = "475837333"
  chat_id2 = "670631346"
  message = "luas objek hari ini = " + str(max_area) + " piksel \nluas objek sebelumnya = "+ str(prev_val) +" piksel \nperkembangan = "+ str('{0:.2f}'.format(perkembangan)) + '%'
  url_tele_image = f"https://api.telegram.org/bot{TOKEN}/sendPhoto"
  data = dict(chat_id = chat_id, caption=message, photo = segmented_url)
  data2 = dict(chat_id = chat_id2, caption=message, photo = segmented_url)
  print(requests.post(url_tele_image, data = data).json()) # this sends the message
  print(requests.post(url_tele_image, data = data2).json()) # this sends the message