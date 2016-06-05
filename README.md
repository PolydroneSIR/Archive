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

Stream video: 
VLC : 
sur la raspberry : $ raspivid -t 0 -n --width 1280 --height 720 -o - | cvlc stream:///dev/stdin --sout '#standard{access=http,mux=ts,dst=:8090}' :demux=h264

cela active l'envois du stream vidéo (sur le port 8090)

sur le périphérique:  $ cvlc http://adresse_ip_du_pi:8090
(normalement l'adresse ip est statique, 172.20.10.2)


