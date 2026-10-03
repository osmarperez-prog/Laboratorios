#*****************************************************************************
# LIBRERIAS
#*****************************************************************************
import RPi.GPIO as GPIO
import time

#*****************************************************************************
# DEFINICION DE PINES
#*****************************************************************************
BUZZER = 18
BOTON_ENCENDER = 24
BOTON_APAGAR = 25

#*****************************************************************************
# CONFIGURACION INICIAL
#*****************************************************************************
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

GPIO.setup(BUZZER, GPIO.OUT)
GPIO.setup(BOTON_ENCENDER, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(BOTON_APAGAR, GPIO.IN, pull_up_down=GPIO.PUD_UP)

buzzer_pwm = GPIO.PWM(BUZZER, 2000)
buzzer_pwm.start(0)

#*****************************************************************************
# BUCLE PRINCIPAL
#*****************************************************************************
while True:
    if GPIO.input(BOTON_ENCENDER) == GPIO.LOW:
        buzzer_pwm.ChangeDutyCycle(50)
        
    if GPIO.input(BOTON_APAGAR) == GPIO.LOW:
        buzzer_pwm.ChangeDutyCycle(0)
        
    time.sleep(0.05)