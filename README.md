# Archive
Dossier contenant les programmes utilisé et les procédures à suivre

Prendre une photo : (testé ok)
Si caméra non active :
        terminal : $ sudo raspi-config
        aller a « enable camera »
        puis reboot la RP
Si caméra active :
        terminal : $ raspistill -o *.png                (* nom de l'image)

→ Cette fonction allume la caméra pendant 5 sec et prend la derière image.

Enregistrer une vidéo: (testé ok)
        terminal : $ raspivid -o *.h264         (* nom de la vidéo)

enregistrer une image via python
        terminal : $ python
        ensuite :       import picamera
                        camera = picamera.PiCamera()
                        camera.capture('*.png')         (* Nom de la video)
$mémoirePrendre une photo : (testé ok)
