import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy

measured_tests = []

"""
Documentation de la communication python
"""

#Lists all ports
ports = serial.tools.list_ports.comports()

open_ports = ""
descr = ""

for port, desc, hwid in sorted(ports):
		##print("{}: {} [{}]".format(port, desc, hwid))
		##print("{}".format(port))
		descr = "{}".format(desc)
		if (descr == "STM32 STLink - ST-Link VCP Ctrl"):
			open_ports = "{}".format(port)

print("Open ports : ", open_ports)

data_serial = serial.Serial(
	port=open_ports,
	baudrate=921600,#576000,#115200,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE,
	bytesize=serial.EIGHTBITS

)
data_serial.writeTimeout = 0 

# =================================
# IDs TX/RX
# =================================

"""
Liste des IDs de communication
"""

ID_RX_START = b'0x01' 
"""ID START DES INTERRUPTIONS"""
ID_RX_STOP = b'0x02' 
"""ID STOP DES INTERRUPTIONS"""
ID_RX_PWM_START = b'0x03' 
"""ID START DU PWM"""
ID_RX_PWM_STOP = b'0x04' 
"""ID STOP DU PWM"""
ID_RX_DUTY_SET = b'0x05' 
"""ID SET DU DUTY CYCLE"""
ID_RX_DIR = b'0x06' 
"""ID CHOIX DE LA DIRECTION"""
ID_RX_VOLTAGE_SET = b'0x07' 
"""ID SET DU VOLTAGE"""
ID_RX_SET_MAG = b'0x08' 
"""ID SET DU MAGNET"""
ID_RX_SET_COMMAND = b'0x09' 
"""ID SET DE LA COMMANDE EN PID"""
ID_RX_ACK = b'0xFF' 
"""ID ENVOI DU ACKNOWLEDGE"""
ID_RX_CALIBRATE = b'0xFE' 
"""ID ENVOI DE LA COMMANDE CALIBRATE"""
ID_RX_CALIB_ZEROG = b'0xFA' 
"""ID ENVOI DE LA COMMANDE CALIBRATE A 0G"""
ID_RX_CALIB_MINUSG = b'0xFB' 
"""ID ENVOI DE LA COMMANDE CALIBRATE A -1G"""
ID_RX_CALIB_PLUSG = b'0xFC' 
"""ID ENVOI DE LA COMMANDE CALIBRATE A +1G"""
ID_RX_SEND_KP = b'0x0A' 
"""ID ENVOI DE LA DEMANDE DE LA VALEUR ACTUELLE DE KP"""
ID_RX_SET_KP = b'0x0B' 
"""ID SET DE LA VALEUR DE KP"""
ID_RX_SEND_KI = b'0x0C' 
"""ID ENVOI DE LA DEMANDE DE LA VALEUR ACTUELLE DE KI"""
ID_RX_SET_KI = b'0x0D' 
"""ID SET DE LA VALEUR DE KI"""
ID_RX_SEND_KD = b'0x0E' 
"""ID ENVOI DE LA DEMANDE DE LA VALEUR ACTUELLE DE KD"""
ID_RX_SET_KD = b'0x0F' 
"""ID SET DE LA VALEUR DE KD"""

# =================================
# trames
# =================================

"""
Description :
Produit une trame de message en bytes, compatible avec la stm32.
Insert à un ID de message une valeur, produisant une ligne de caractères dont la longueur est limité par max_size.
Erreur si la taille de la trame est inférieur a 6 bytes.

Paramètres :
- msg_id : ID du message, fourni plus haut.
- value : la valeur à insérer. 
- max_size : la longueur en bytes du message.

Retour :
Retourne une trame en bytes (b'').
"""
def define_trame(msg_id, value, max_size):
	#init
	trame = b''
	byte_length = b''
	
	byte_length = bytes(str(value),'utf-8')
	float_byte_size = len(byte_length)
	index_max_byte = max_size-len(msg_id)-2
	print("index_max_byte :",index_max_byte, " max_size :", max_size)
	trame = msg_id+byte_length[0:index_max_byte]+b';'
	
	#zero filler
	zero_filler = b'0'*(max_size - len(trame)-1)
	trame+= zero_filler
	#EOF Character to end the line, counts as 1 bytes
	trame += bytes('\n', 'utf-8')
	return trame

# =================================
# fonctions d'envoi
# =================================

MSG_MAX_SIZE = 16

def uart_send_start():
	"""Envoi via uart d'une trame avec l'ID start"""
	data_serial.write(define_trame(ID_RX_START, 1, MSG_MAX_SIZE))
	
def uart_send_stop():
	"""Envoi via uart d'une trame avec l'ID stop"""
	data_serial.write(define_trame(ID_RX_STOP, 1, MSG_MAX_SIZE))
	
def uart_send_pwmStart():
	"""Envoi via uart d'une trame avec l'ID PWM Start"""
	data_serial.write(define_trame(ID_RX_PWM_START, 1, MSG_MAX_SIZE))
	
def uart_send_pwmStop():
	"""Envoi via uart d'une trame avec l'ID PWM Stop"""
	data_serial.write(define_trame(ID_RX_PWM_STOP, 1, MSG_MAX_SIZE))
	
def uart_send_dutyValue(duty_value):
	"""Envoi via uart d'une trame avec l'ID DUTY SET et de la valeur choisie"""
	data_serial.write(define_trame(ID_RX_DUTY_SET, duty_value, MSG_MAX_SIZE))
	
def uart_send_command(command):
	"""Envoi via uart d'une trame avec l'ID SET COMMAND et de la commande choisie"""
	data_serial.write(define_trame(ID_RX_SET_COMMAND, command, MSG_MAX_SIZE))
	
def uart_send_direction(dir_value):
	"""Envoi via uart d'une trame avec l'ID DIR et de la direction choisie"""
	data_serial.write(define_trame(ID_RX_DIR, dir_value, MSG_MAX_SIZE))
	
def uart_send_voltageValue(voltage_value):
	"""Envoi via uart d'une trame avec l'ID VOLTAGE SET et de la valeur de voltage choisie"""
	print("b'' sent : ",data_serial.write(define_trame(ID_RX_VOLTAGE_SET, voltage_value, MSG_MAX_SIZE)))
	
def uart_send_kpAsk(kp_value):
	"""Envoi via uart d'une trame avec l'ID SET KP et de la valeur de KP choisie"""
	data_serial.write(define_trame(ID_RX_SET_KP, kp_value, MSG_MAX_SIZE))
	
def uart_send_kpValue():
	data_serial.write(define_trame(ID_RX_SEND_KP, 1, MSG_MAX_SIZE))
	
def uart_send_kiAsk(ki_value):
	"""Envoi via uart d'une trame avec l'ID SET KI et de la valeur de KI choisie"""
	data_serial.write(define_trame(ID_RX_SET_KI, ki_value, MSG_MAX_SIZE))
	
def uart_send_kiValue():
	data_serial.write(define_trame(ID_RX_SEND_KI, 1, MSG_MAX_SIZE))
	
def uart_send_kdAsk(kd_value):
	"""Envoi via uart d'une trame avec l'ID SET KD et de la valeur de KD choisie"""
	data_serial.write(define_trame(ID_RX_SET_KD, kd_value, MSG_MAX_SIZE))
	
def uart_send_kdValue():
	data_serial.write(define_trame(ID_RX_SEND_KD, 1, MSG_MAX_SIZE))
	
def uart_send_acknowledge():
	data_serial.write(define_trame(ID_RX_ACK, 1, MSG_MAX_SIZE))
	
def uart_send_calibrate():
	"""Envoi via uart d'une trame avec l'ID CALIBRATE"""
	data_serial.write(define_trame(ID_RX_CALIBRATE, 1, MSG_MAX_SIZE))
	
def uart_send_calibzeroG():
	"""Envoi via uart d'une trame avec l'ID CALIB ZERO"""
	data_serial.write(define_trame(ID_RX_CALIB_ZEROG, 1, MSG_MAX_SIZE))
	
def uart_send_calibminusG():
	"""Envoi via uart d'une trame avec l'ID CALIB MINUS G"""
	data_serial.write(define_trame(ID_RX_CALIB_MINUSG, 1, MSG_MAX_SIZE))
	
def uart_send_calibplusG():
	"""Envoi via uart d'une trame avec l'ID CALIB PLUS G"""
	data_serial.write(define_trame(ID_RX_CALIB_PLUSG, 1, MSG_MAX_SIZE))
	
def uart_send_mag(state_int):
	"""Envoi via uart d'une trame avec l'ID SET MAG et de la valeur du magnet"""
	data_serial.write(define_trame(ID_RX_SET_MAG, state_int, MSG_MAX_SIZE))


# =================================
# Read
# =================================


def read_data(nline):

	"""
	Description :
	Wrapper de la fonction readline().
	Fonction bloquante a cause de readline().
	La fonction lit n+50 lignes, puis les décode et retourne les [50:max] lignes.
	Malgré un reset_input_buffer(), des résidus de lecture apparaissent dans la data recue,
	d'où la lecture de +50 lignes.

	Paramètres :
	- nline : Quantité en entier naturel de lignes lues et retournées.

	Retour :
	Une liste de nline entrées décodées mais pas traitées.
	"""
	data_serial.reset_input_buffer()
	data_serial.reset_output_buffer()
	
	it = 0
	print("start reading")
	##error while reading previous values
	##kinda flush
	data_table = []
	while (it != nline+50):
		# Read data out of the buffer until a carraige return / new line is found
		data = (data_serial.readline()).decode()
		data = data.split(";")
		#print(data)
		
		data_table.append(data)
		it += 1;
		#print(it)
	print("out")
	data_table = data_table[50:nline+1]
	return data_table

# =================================
# Processing
# =================================

def compute_data_table(dt):
	"""
	Description :
	Prend une liste de données brutes et les traite.
	Découpe le *string* en plusieurs substring, puis remet les données numériques en *float* ou *int*.

	Paramètres :
	- dt : une data table de strings.

	Retour :
	Retourne une table découpée et traitée.
	"""
	ndt = []
	tables = []
	
	for data_line in dt:
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			data_line[0] = data_line[0].lstrip('\r') #supprime le \r caractere
			
			for i in range(0, len(data_line)):
				if ((i%2) == 1):
					data_line[i] = float(data_line[i])
		ndt.append(data_line)
	return ndt

# =================================
# Slicing
# =================================

def time_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la table des valeurs de temps.
	Une attention est portée au temps d'origine. La première valeur devient la reférence dont l'ensemble des valeurs se soustrait.
	Dès lors, le temps traité est relatif.
	
	Paramètres :
	- dt : data table traitée.

	Retour :
	Table de temps.
	"""
	ndt = []
	for data_line in dt:
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")): # "0x01" in data_line[k] only for string
					if ("0x01" in data_line[k]):
						ndt.append((data_line[k+1])/1000)
	
	init_time = ndt[0]
	for j in range(len(ndt)):
		ndt[j] = round(ndt[j] - init_time, 3) #3 digit after . as it is divised by 10e3
	
	return ndt

def pwm_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la table des valeurs de commande en PWM.
	La valeur du PWM est un pourcentage de 0.0 à 100.0 (0.x de précision).
	
	Paramètres :
	- dt : data table traitée.

	Retour :
	Table de PWM.
	"""
	ndt = []
	for data_line in dt:
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			##print(data_line)
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")):
					if ("0x11" in data_line[k]):
						ndt.append(data_line[k+1])
					
	return ndt

def adc_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la table des mesures de l'ADC.
	Aucun post-traitement de la valeur de l'ADC.

	Paramètres :
	- dt : data table traitée.

	Retour :
	Table de mesures de l'ADC.
	"""
	ndt = []
	for data_line in dt:
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")):
					if ("0x09" in data_line[k]):
						ndt.append(data_line[k+1])
					
	return ndt

def accel_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la table des accelerations.

	Paramètres :
	- dt : data table traitée.

	Retour :
	Table d'accelerations.
	"""
	ndt = []
	for data_line in dt:
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")):
					if ("0x10" in data_line[k]):
						ndt.append(data_line[k+1])
					
	return ndt

def speed_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la table des valeurs de vitesse.

	Paramètres :
	- dt : data table traitée.

	Retour :
	Table des vitesses.
	"""
	ndt = []
	for data_line in dt:
		#print("DATA LINE :", data_line, len(data_line))
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")):
					if ("0x12" in data_line[k]):
						ndt.append(data_line[k+1])
					
	return ndt

def all_getTable(dt):
	"""
	Description :
	Extrait à partir d'une table de données traitée la totalité des données.

	Paramètres :
	- dt : data table traitée.

	Retour :
	Table de données traitées.
	L'ensemble est une liste de n lignes, elles mêmes de 5 valeurs.
	"""
	ndt = []
	
	for data_line in dt:
		ndtline = [0]*5
		if (((len(data_line)-1) % 2 == 0) and (len(data_line) > 1)):
			for k in range(len(data_line)):
				if (type(data_line[k]) == type("0x0")):
					if ("0x01" in data_line[k]):
						ndtline[0] = ((data_line[k+1])/1000)
					if ("0x09" in data_line[k]):
						ndtline[1] = (data_line[k+1])
					if ("0x10" in data_line[k]):
						ndtline[2] = (data_line[k+1])
					if ("0x11" in data_line[k]):
						ndtline[3] = (data_line[k+1])
					if ("0x12" in data_line[k]):
						ndtline[4] = (data_line[k+1])
						
		ndt.append(ndtline)
		
	init_time = ndt[0][0]
	for j in range(len(ndt)):
		ndt[j][0] = round(ndt[j][0] - init_time, 3) #3 digit after . as it is divised by 10e3
	
	return ndt

	import time
import threading

# =================================
# Mesure des vibrations en relâche
# =================================

time_table = []
speed_table = []
adc_table = []
accel_table = []
pwm_table = []

def release_accel(te_ms, period_ms):
	"""
	Description :
	Fonction de threading.
	Gère l'envoi en parallèle du relâchement du magnet.

	Paramètres :
	- te_ms : Temps en ms de la longueur de l'échantillonage.
	- period_ms : Temps en ms de l'échantillonage.

	Retour :
	*None*
	"""
	print("Thread %s: starting")
	time.sleep((te_ms/period_ms)*0.1)
	uart_send_mag(0)
	print("Thread %s: finishing")

def measure_release(te_ms, period_ms):
	"""
	Description :
	Fonction pour lancer une mesure du relâchement de la réglette.
	En parallèle d'une lecture des données, un thread de relachement est lancée.
	Une attente de 10% de l'échantillonage est respectée pour avoir un 0 de réference dans la mesure.

	Paramètres :
	- dt : data table traitée.

	Retour :
	Table de données traitées.
	*Voir all_getTable*.
	"""
	#reboot input buffer
	global time_table, speed_table, adc_table, accel_table, pwm_table
	initial_measure = []
	core_measure = []
	thread_release = threading.Thread(target=release_accel, args=(te_ms,period_ms), daemon = True)
	# =================================
	# Mesures
	# =================================
	print("=== Lancement des mesures ===")
	print("Temps : "+str(te_ms)+"ms")
	print("--> Relaĉhe de l'aimant")
	thread_release.start()
	core_measure = read_data(int((te_ms/period_ms)))
	
	print("--> Fin de la mesure")
	#thread_release.stop()
	# =================================
	# Traitements
	# =================================
	print("=== Traitement des mesures ===")
	print("--> Traitement principal")
	computed_data = (compute_data_table(core_measure))
	print("--> Découpage des tables")
	computed_data = (all_getTable(computed_data))
	return computed_data
	print("--> Retour")

import time
import math
import threading

# =================================
# Constantes
# =================================

max_motor_speed = 166.667 #tr/s

# =================================
# Mesure des vibrations en forcée
# =================================

time_table = []
speed_table = []
adc_table = []
accel_table = []
pwm_table = []

def freq2duty(freq):
	"""
	Description :
	Transforme une fréquence de commande en un duty cycle (de 0.x de précision).
	La réference est la vitesse (en tr/s) donc la fréquence max.

	Paramètres :
	- freq : Fréquence à transformer, en Hz.

	Retour :
	Duty Cycle.
	"""	
	duty = int(round(freq/max_motor_speed, 4)*1000)
	# duty is an integer between 0 and 100 (0.x precision when %)
	return duty


def tx_command(length_ms, max_freq, freq_step, min_freq):
	"""
	Description :
	Fonction de thread gérant la commande de la vitesse moteur.
	Voir **measure_forced_vibrations** pour le fonctionnement. 

	Paramètres :
	- length_ms : temps de maintien à une vitesse moteur donnée, en ms.
	- max_freq : Fréquence d'excitation initiale ET maximale, en Hz.
	- freq_step : Pas de fréquence, en Hz.
	- min_freq : Fréquence d'excitation finale Et minimale, en Hz.

	Retour :
	*None*.
	"""
	print("Thread %s: starting")
	#nb of points
	max_points = int( (max_freq-min_freq)/freq_step ) + 1 #+1 for step @ 0 Hz
	#initial period of null freq
	uart_send_command(freq2duty(0.0))
	for step in range(0, max_points):
		time.sleep(length_ms/1000)
		freq = max_freq - step*freq_step
		uart_send_command(freq2duty(freq))
		
	#final period of null freq to finish the measure
	uart_send_command(freq2duty(0.0))
	print("Thread %s: finishing")
	 

def measure_forced_vibrations(te_ms, length_ms, max_freq, freq_step, min_freq):
	"""
	Description :
	Fonction pour lancer une mesure de l'excitation forcée de la réglette.
	En parallèle d'une lecture des données, un thread de commandes de PID est lancée.
	Une attente de 10% de l'échantillonage est respectée pour avoir un 0 de réference dans la mesure.
	Etant donnée la longueur de l'expérience, le temps de la mesure (pouvant trèèès vite monter) est donné.
	Le test se fait de la manière suivante :
	1. Une attente à 0 Hz pendant *length_ms*.
	2. La commande moteur saute à *max_freq* Hz, puis diminue de *freq_step* jusqu'à *min_freq*.
	3. Une deuxième attente à 0 Hz est effectuée.

	Paramètres :
	- te_ms : periode d'échantillonage, en ms.
	- length_ms : temps de maintien à une vitesse moteur donnée, en ms.
	- max_freq : Fréquence d'excitation initiale ET maximale, en Hz.
	- freq_step : Pas de fréquence, en Hz.
	- min_freq : Fréquence d'excitation finale Et minimale, en Hz.

	Retour :
	Table de données traitées.
	*Voir all_getTable*.
	"""
	#reboot input buffer
	global time_table, speed_table, adc_table, accel_table, pwm_table
	core_measure = []
	
	measuring_time = (int( (max_freq-min_freq)/freq_step ) + 1)*(length_ms/1000)
	reading_points = int(measuring_time*1000/te_ms)
	
	thread_command = threading.Thread(target=tx_command, 
									  args=(length_ms, max_freq, freq_step, min_freq), 
									  daemon = True)
	# =================================
	# Mesures
	# =================================
	print("=== Lancement des mesures ===")
	print("Temps total: "+str(measuring_time)+"s"+" | "+str(measuring_time/60)+ "min")
	print("--> Début de la mesure")
	thread_command.start()
	core_measure = read_data(reading_points)
	
	print("--> Fin de la mesure")
	# =================================
	# Traitements
	# =================================
	print("=== Traitement des mesures ===")
	print("--> Traitement principal")
	computed_data = (compute_data_table(core_measure))
	print("--> Découpage des tables")
	return (all_getTable(computed_data))
	print("--> Retour")

# ==========================
# Plot data
# ==========================

def extract_table(dt, index):
	"""
	Description :
	A partir d'une table de données traitées et complète, produit une table de donnée d'un seul paramètre. 

	Paramètres :
	- dt : Data Table.
	- index : la position de l'information dans une ligne de données.

	Retour :
	Table de données d'un seul type.
	"""
	ndt = [0]*len(dt)
	for i in range(len(dt)):
		ndt[i] = dt[i][index]
	return ndt

def plot_speed(dt):
	"""
	Description :
	Affichage de la commande et du résultat de la vitesse dans le temps.
	Matplotlib.

	Paramètres :
	- dt : Data Table.

	Retour :
	*None*.
	"""
	time_table = extract_table(dt, 0)
	pwm_table = extract_table(dt, 3)
	speed_table = extract_table(dt, 4)
	max_len = min(len(time_table), min(len(pwm_table),len(speed_table)))
	plt.plot(time_table[0:max_len],pwm_table[0:max_len], label = "Commande")
	plt.plot(time_table[0:max_len], speed_table[0:max_len], label = "Vitesse")
	plt.xlabel("Temps (s)")
	plt.ylim([-100, 500])
	plt.ylabel("Vitesse (rpm)")
	plt.title(str(len(time_table))+" mesures d'ADC en série")
	plt.legend()
	
def plot_adc(dt):
	"""
	Description :
	Affichage de l'adc dans le temps.
	Matplotlib.

	Paramètres :
	- dt : Data Table.

	Retour :
	*None*.
	"""
	time_table = extract_table(dt, 0)
	print(len(time_table))
	accel_table = extract_table(dt, 1)
	print(len(accel_table))
	max_len = min(len(time_table), len(accel_table))
	plt.step(time_table[0:max_len],accel_table[0:max_len], label = "Mesure ADC")
	plt.xlabel("Temps (s)")
	plt.ylim([-0.2, 3.5])
	plt.ylabel("Amplitude (V)")
	plt.title(str(len(time_table))+" mesures d'ADC")
	plt.legend()
	
def plot_accel(dt):
	"""
	Description :
	Affichage de l'acceleration dans le temps.
	Matplotlib.

	Paramètres :
	- dt : Data Table.

	Retour :
	*None*.
	"""
	time_table = extract_table(dt, 0)
	print(len(time_table))
	accel_table = extract_table(dt, 2)
	print(len(accel_table))
	max_len = min(len(time_table), len(accel_table))
	plt.step(time_table[0:max_len],accel_table[0:max_len], label = "Mesure d'accelero")
	plt.xlabel("Temps (s)")
	plt.ylim([-3.2, 3.2])
	plt.ylabel("Amplitude (G)")
	plt.title(str(len(time_table))+" mesures d'accelero")
	plt.legend()