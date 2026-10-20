# STM32 Code Documentation

## Architecture(s) du projet

Le projet est basé sur la carte STM32F411, fournie au préalable pour le projet.
Le but de la carte est principalement celui d'acquérir des mesures, avec un contrôle de certaines parties pour y arriver.
Il est donc naturellement orienté autour d'une boucle périodique d'acquisition des données suivi de l'envoi des données.
La partie contrôle se base sur des changements -de valeurs- limités.

#### La boucle principale

La boucle principale est assurée par le TIM2, à une fréquence de 0.1ms.
La période est fixée elle par l'utilisateur, à l'aide de tickTimer_Init(uint32_t ms).
A chaque fin de période, une interruption (dont la priorité est la plus haute) est appelée et c'est là que se situe les appels aux fonctions.


```c
// in main.c

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM2){
		ms_value++; //= HAL_GetTick();

		if ((ms_value % 1) == 0)
		{
			... //code tous les period_ms
		}

		if ((ms_value % 10) == 0)
		{
			... //code tous les 10*period_ms
		}

		... //autres codes
	}
}
```

#### ADC

L'acquisition de l'accéléromètre analogique est assuré par l'ADC (**A**nalog to **D**igital **C**onverter).
L'appel à la conversion est normalement bloquant si c'est le processeur qui le fait.
Pour pallier au problème, on utilise le DMA, qui appelle à la conversion, puis transfert dans la mémoire le résultat à l'aide d'une interruption.

```c
// Paramètrages de base

ADC1->SMPR2 |= (uint32_t)(0x7); //SET SAMPLING TIME TO 480 CYCLES, FCONV = 42.7 kHz, TCONV = 2.342 10^-5 s
HAL_ADC_Start_DMA (&hadc1, &ADC_DATA_BUFFER, 1);

// Interruption, conversion roulante sur 8 valeurs

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   acc_rawIntData[conv_index] = ADC_DATA_BUFFER;
   //ADC1->CR2 |= ADC_CR2_DDS;
   conv_index++;
   if (conv_index >= N_CONVERSIONS)
   {
	   conv_index = 0;
   }

}

// code bloquant

void acc_ReadDataBlocking(uint32_t* destination)
{
	for (int it = 0; it < (int)N_CONVERSIONS; it++)
	{
		//start ADC Conversion
		HAL_ADC_Start(&hadc1);
		//fetch value
		HAL_ADC_PollForConversion(&hadc1, 100);
		test = HAL_ADC_GetValue(&hadc1);
		destination[it] = (uint32_t)(0x0000FFFF & HAL_ADC_GetValue(&hadc1));
		HAL_ADC_Stop(&hadc1);
	}
}
```

#### Le moteur

Le moteur est à la fois un contrôlant et un contrôlé : il y a un encodeur et un PWM.

Le code de l'encodeur est une modification de [celui de M. Kerhoas](https://web.enib.fr/~kerhoas/quadEncoder.html).
De même pour le [code du hacheur](https://web.enib.fr/~kerhoas/motorCommand.html).

Les modifications portées au code de l'encodeur sont les suivantes :
- On se sépare de l'Index pour compter les tours, puisqu'il n'est pas proposé par l'encodeur.
- Un compteur global (au fichier speed.c) le remplace.
- Les fonctions pour l'asservissement ne sont pas prises.

Les modifications portées au code du PWM sont les suivantes :
- Un degré de précision (0.x) est pris
- Des fonctions plus poussées que simplement le SetDuty sont proposées.

On rajoute un contrôle de direction sous la forme de 2 signaux logiques directement aux bornes du hacheur.
Ils sont mutuellement exclusifs.

## Documentation

### accelerometer.c

```c
void acc_Init(void)
{
	/*
	 * Tconv = (Sampling_Time + 12Bits)/(APB2_CLOCK/4) *
	 */
	/* accelerodata */
	acc_LastVoltage;
	acc_zeroG = 1.63f;
	acc_minusG = 1.93f;
	acc_plusG = 1.32f;
	acc_VperG = 0.305f;

	ADC1->SMPR2 |= (uint32_t)(0x7); //SET SAMPLING TIME TO 480 CYCLES, FCONV = 42.7 kHz, TCONV = 2.342 10^-5 s
	HAL_ADC_Start_DMA (&hadc1, &ADC_DATA_BUFFER, 1);
}
```
**Description :**
	Cette fonction permet de démarrer les conversions.
	Une modification est portée au registre **SMPR2** pour faire en sorte qu'une conversion prenne 480 coups de cloches. La fréquence passe de 1.75 MHz (ce qui étouffe le processeur avec toutes les interruptions) à 42.7 kHz.

**Paramètres :**
	*None*

**Retourne :**
	*None*



```c
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
   acc_rawIntData[conv_index] = ADC_DATA_BUFFER;
   //ADC1->CR2 |= ADC_CR2_DDS;
   conv_index++;
   if (conv_index >= N_CONVERSIONS)
   {
	   conv_index = 0;
   }

}
```
**Description :**
	Après une conversion réussie, le DMA appelle ce Callback pour transférer la valeur dans la mémoire accessible au processeur. Ici, la mémoire est un tableau circulaire de 8 valeurs.

**Paramètres :**
	hadc : La *struct* handler de l'ADC

**Retourne :**
	*None*

```c
void acc_ReadDataBlocking(uint32_t* destination)
{
	for (int it = 0; it < (int)N_CONVERSIONS; it++)
	{
		//start ADC Conversion
		HAL_ADC_Start(&hadc1);
		//fetch value
		HAL_ADC_PollForConversion(&hadc1, 100);
		test = HAL_ADC_GetValue(&hadc1);
		destination[it] = (uint32_t)(0x0000FFFF & HAL_ADC_GetValue(&hadc1));
		HAL_ADC_Stop(&hadc1);
	}
}
```
**Description :**
	Débute 8 conversions consécutives.
	Pour chaque conversion, démarre la lecture, attend la fin de la conversion puis transmet la valeur avant de conclure la lecture.

**Paramètres :**
	destination : Tableau de valeur de destination.

**Retourne :**
	*None*

```c
/* Converts int 0-4095 to float 0-3.3, representing the voltage */
void acc_int2flt(uint32_t* source, float* destination)
{
	for (int n = 0; n < N_CONVERSIONS; n++)
	{
		destination[n] = ((float)(source[n]*10*V_REF)/((float)(MAX_VALUE*10)));
	}
}
```
**Description :**
	Convertis chaque valeur entière d'origine en réel qui représente mieux la valeur.

**Paramètres :**
	1. source : Tableau de valeurs d'origine, sont les valeurs entières issus de l'ADC.
	2. destination : Tableau de valeurs convertis en float /valeurs réelles.

**Retourne :**
	*None*
```c
//For best accuracy
// The higher the value of alpha, the more closely the EWMA tracks the original time series
float acc_EWMAverage(float* source, float alpha)
{
	//EWMA Formula :
	//EWMA[x] = alpha*source[x] + (1-alpha)*EWMA[x-1]
	//EWMA[0] & init
	float avgd_value = (float)(alpha*source[0])/10.0f;

	for (int i = 1; i < N_CONVERSIONS; i++)
	{
			avgd_value = (float)(alpha*source[i]+(1-alpha)*avgd_value);
	}
	return avgd_value;
}
```
**Description :**
	EWMA pour Exponentially Weighted Moving Average.
	L'objectif est de donner une moyenne qui prend les derniers résultats avec un plus grand poids par rapport aux premières arrivées. Le nombre de 8 mesures vient de là, il s'agit pour les systèmes binaires d'une valeur n'entraînant pas de sur-calculs.
	[Source](https://corporatefinanceinstitute.com/resources/career-map/sell-side/capital-markets/exponentially-weighted-moving-average-ewma/).

**Paramètres :**
	1. source : Tableau de float issus de *acc_int2flt*.
	2. alpha : coefficient exponentiel e [0,1]. Plus la valeur est proche de 1, plus le résultat sera proche de la courbe d'origine avec 0 qui donne comme résultat de moyenne 0 et 1 la courbe donnée en paramètre.

**Retourne :**
	*None*

```c
//example value
void acc_GetValue(void)
{
	acc_int2flt(acc_rawIntData, acc_fltData);
	acc_LastVoltage = acc_EWMAverage(acc_fltData, 0.5f);
}
```
**Description :**
	Fonction principal de *accelerometer.c*.
	Appelle à la conversion des entiers en réels (voir acc_int2flt), puis assigne à la valeur globale acc_LastVoltage le résultat de l'EWMA des 8 dernières valeurs.
	Destinée uniquement à être appelée dans *main.c*.

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void acc_Voltage2Accel(void)
{
	//Prendre Valeur Voltage
	acc_GetValue();
	//acc_LastAccel = ((acc_zeroG-acc_LastVoltage)/acc_VperG);
	acc_LastAccel = (float)((1.63f-acc_LastVoltage)/0.305f);
}
```

**Description :**
Convertis la dernière tension en acceleration selon les paramètres mis dans Init. Ils sont 3 : 
		La tension à 0G (à vide).
		La tension à 1G (quand le +x du capteur pointe vers le sol).
		La tension à -1G (quand le -x du capyeur pointe vers le sol).
Assigne la valeur globale acc_LastAccel.

**Paramètres :**
	*None*

**Retourne :**
	*None*

### pidFilter.c

```c
void pidFilter_Init(void)
{
	/* PID values */
	Kp = 0.01216f;
	Ki = 0.02f;
	Kd = -0.005f;
	te_ms = 10;
	Ti = (Kp/Ki)*(float)te_ms;
	Td = (Kd/Kp)*(float)te_ms;
	command = 0.0f;
	debug_tension = 0.0f;

	/* PID coefficients init */

	b2 = Kp+Kd+Ki;
	b1 = -2*Kd-Kp;
	b0 = Kd;
	a2 = 1;
	a1 = -1;
}
```
**Description :**
	Initialise les valeurs/poids du PID.

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void pidFilter_SetKp(float new_Kp)
{
	Kp = new_Kp;
}
```
**Description :**
	Assigne au poids Kp du PID une nouvelle valeur choisie par l'utilisateur.

**Paramètres :**
	new_Kp : nouvelle valeur en float.

**Retourne :**
	*None*

```c
void pidFilter_SetKi(float new_Ki)
{
	Ki = new_Ki;
}
```
**Description :**
	Assigne au poids Ki du PID une nouvelle valeur choisie par l'utilisateur.

**Paramètres :**
	new_Ki : nouvelle valeur en float.

**Retourne :**
	*None*

```c
void pidFilter_SetKd(float new_Kd)
{
	Kd = new_Kd;
}
```
**Description :**
	Assigne au poids Kd du PID une nouvelle valeur choisie par l'utilisateur.

**Paramètres :**
	new_Kd : nouvelle valeur en float.

**Retourne :**
	*None*

```c
void pidFilter_SetCommand(float new_command)
{
	command = new_command;
}
```
**Description :**
	Assigne une nouvelle valeur de commande (vitesse à atteindre) choisie par l'utilisateur.

**Paramètres :**
	new_command : nouvelle valeur en float, correspond à la vitesse.

**Retourne :**
	*None*

```c
void pidFilter_SetCommandVoltage(float new_voltage)
{
	/* use rated speed of 3000 rpm */
	float new_command = 280*(new_voltage/12);
	pidFilter_SetCommand(new_command);
}
```
**Description :**
	Assigne une nouvelle valeur de commande choisie par l'utilisateur.
	Contrairement à SetCommand, la nouvelle commande est déduite d'une valeur de tension voulue à l'entrée du moteur. Le calcul prend la tension et la vitesse nominales du moteur.

**Paramètres :**
	new_voltage : nouvelle valeur en float.

**Retourne :**
	*None*

```c
void pidFilter_SetOuterSpeedCommand(float new_speed)
{
	/* 0 <= new_speed <= 3800/15.88 */
	pidFilter_SetCommand(new_speed*15.88f);
}
```
**Description :**
	Assigne une nouvelle valeur de commande choisie par l'utilisateur.
	Contrairement à SetCommand, la nouvelle commande est déduite d'une valeur de vitesse après le reducteur. C'est tout naturellement que cette valeur soit multiplié par le rapport du réducteur.

**Paramètres :**
	new_Kp : nouvelle valeur en float. Faire attention à ce que la valeur choisie soit inférieur ou égale à la vitesse nominale divisée par le rapport du réducteur.

**Retourne :**
	*None*

```c
void pidFilter_SetFrequency(float freq)
{
	float new_command = 60.0f*(freq/(float)TS_BY_TE);
	pidFilter_SetCommand(new_command);
}
```
**Description :**
	Assigne une nouvelle valeur de commande choisie par l'utilisateur.
	Contrairement à SetCommand, la nouvelle commande est déduite d'une fréquence de rotation du moteur. Cette fréquence correspond au nombre de rotations par seconde (càd RPM/60).

**Paramètres :**
	new_Kp : nouvelle valeur en float. Faire attention à ce que la valeur choisie soit inférieur ou égale à la vitesse nominale divisée par 60.

**Retourne :**
	*None*

```c
void pidFilter_Compute(void)
{
	/* set k-1 */
	Oe_k[2] = Oe_k[1];
	Oe_k[1] = Oe_k[0];
	Oe_k[0] = command - (float)fabs(output_speed);

	Uc_k[1] = Uc_k[0];

	/* calculate Uc(z) */
	Uc_k[0] = (b2/a2)*Oe_k[0] + (b1/a2)*Oe_k[1] + (b0/a2)*Oe_k[2] - (a1/a2)*Uc_k[1];

	/* no negative voltage */
	if (Uc_k[0] < 0.0f)
	{
		Uc_k[0] = 0.0f;
	}
	else if (Uc_k[0] > 24.0f) /* no voltage superior to 24, both as a command & status*/
	{
		Uc_k[0] = 24.0f;
	}
	/* PWM new duty */
	pwm_SetVoltage(Uc_k[0]);

	debug_tension = Uc_k[0];
}
```
**Description :**
Fonction principale de ce fichier.
Calcule à partir des paramètres du PID (Kp, Kd, Ki) la **tension** de sortie aux bornes du moteur. La formule de Uc_k[0] est simplement le résultat du développement de la formule initiale du PID, puis de sa conversion dans l'espace discret.
*fabs()* est utilisée puisque seul la vitesse compte dans le calcul, le signe donne par contre la direction (controlée dans mccCommand.c). Destinée uniquement à être appelée dans *main.c* 

**Paramètres :**
	*None*

**Retourne :**
	*None*

### mccCommand.c

```c
void mcc_SetCW(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_RESET);
	isCW = 1;
}
```
**Description :**
	Fonction de bas niveau qui met le moteur en rotation horaire (ClockWise).

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void mcc_SetCCW(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_SET);
	isCW = 0;
}
```
**Description :**
	Fonction de bas niveau qui met le moteur en rotation anti-horaire (CounterClockWise).

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void mcc_SetMagnet(int state)
{
	if (state == 1)
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
		isMagOn = 1;
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
		isMagOn = 0;
	}

}
```
**Description :**
	Fonction de commande qui choisie l'état de rotation du moteur.

**Paramètres :**
	state : Int de valeur 1 ou 0.

**Retourne :**
	*None*

```c
void mcc_ToggleDir(void)
{
	if (isCW == 0)
	{
		mcc_SetCCW();
		isCW = 1;
	}
	else
	{
		mcc_SetCW();
		isCW = 0;
	}

}

```
**Description :**
	Fonction de contrôle qui inverse le sens de rotation du moteur.

**Paramètres :**
	*None*

**Retourne :**
	*None*

### pwmCommand.c

```c
uint32_t pulseCalc(float val)
{
	uint32_t pulseVal = 0;

	if (val < 0.0f)
	{
		//If val inf to 0, pulseVal = 0
		pulseVal = (uint32_t)(0x0);
	}
	else if (val > 100.0f)
	{
		//If val sup to 100, pulseVal = 1000
		pulseVal = (uint32_t)(0x3E8);
	}
	else if ((val >= 0.0f) & (val <= 100.0f))
	{
		/* CCR = duty_cycle*ARR <=> CCR = duty_cycle * 100 * ARR(1000)  /100.0f*/
		pulseVal = (uint32_t)(10*val*(1000)/100.0f); // val*10 to keep 0.x precision
		pulseVal = pulseVal/10; // val/10 to be on point
	}
	return pulseVal;
}
```
**Description :**
Produit à partir d'une valeur de pulsation allant de 0.0 a 100.0 la valeur de CCR1 correspondante. Contient en plus de cela une sécurité sous la forme d'assignements à 0 ou 100 si la valeur en entrée dépasse ces 2 bornes.

**Paramètres :**
	val : float correspondant à la valeur de commande. Va de 0.0 à 100.0.

**Retourne :**
	La valeur de **val** multipliée par 10, puis castée en *uint32_t*.

```c
void pwm_Init(void){

	PWM_Toggle = 0;
	PWM_Value = 0;
	TIM11->CCR1 = 10;
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
}
```
**Description :**
Initialise le PWM du moteur ainsi que les variables PWM_Toggle et PWM_Value. Lance ensuite la génération du signal PWM via HAL.

**Paramètres :**
	*None*

**Retourne :**
	*None*


```c
void pwm_SetDuty(float duty) //
{
	//Formula duty : Pulse Value = duty*(2^16 - 1)/100
	//Float = 32bits

	//__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_1, pulseCalc(duty));
	PWM_Value = pulseCalc(duty);
	TIM11->CCR1 = PWM_Value;
}
```
**Description :**
	Définie la valeur du registre CCR1, qui est le registre de référence pour définir l'état logique de sortie du PWM. Le signal est haut tant que le compte du TIM11 est inférieur ou égal à CCR1. Fait usage de pulseCalc pour la transformation d'un float en valeur utilisable.

**Paramètres :**
	duty : Valeur de duty cycle qui sera utilisée. Allant de 0.0 à 100.0.

**Retourne :**
	*None*

```c

void pwm_SetVoltage(float voltage)
{
	// Voltage e [0.0,24.0]
	float duty = 0;

	if (voltage < 0.0f)
	{
		//If val inf to 0, duty = 0
		duty = 0.0f;
	}
	else if (voltage > 24.0f)
	{
		//If val sup to 24, duty = 100
		duty = 24.0f;
	}
	else if ((voltage >= 0.0f) & (voltage <= 24.0f))
	{
		duty = (voltage*100.0f)/(24.0f);
	}
	PWM_Value = pulseCalc(duty);
	TIM11->CCR1 = PWM_Value;
}
```
**Description :**
	Définie la valeur du duty cycle, non pas à partir d'un pourcentage mais d'une tension. Une seconde sécurité est mise en place sous la forme d'un anti-dépassement de la tension max du moteur.

**Paramètres :**
	voltage : Valeur de tension dont sera issue le duty cycle. En float pour permettre de la précision qui peut être cependant supprimée par le passage en int.

**Retourne :**
	*None*

```c
void pwm_Enable(void)
{
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
	PWM_Toggle = 1;
}
```
**Description :**
	Fonction de bas niveau qui autorise l'émission du PWM.

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void pwm_Disable(void)
{
	HAL_TIM_PWM_Stop(&htim11, TIM_CHANNEL_1);
	PWM_Toggle = 0;
}
```
**Description :**
	Fonction de bas niveau qui bloque l'émission du PWM.

**Paramètres :**
	*None*

**Retourne :**
	*None*

```c
void pwm_Toggle(void)
{
	/*PWM_Toggle :
	* 0 = Off
	* 1 = On
	*/
	if (PWM_Toggle == 0)
	{
		pwm_Enable();
		PWM_Toggle = 1;
	}
	else
	{
		pwm_Disable();
		PWM_Toggle = 0;
	}
}
```
**Description :**
	Fonction qui inverse l'état de l'émission du PWM.

**Paramètres :**
	*None*

**Retourne :**
	*None*

### comm.c
```c
void slice_str(const char * str, char * buffer, size_t start, size_t end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}
```
**Description :**
Découpe un string dont le début du découpage est *start* et la fin du découpage est *stop*. Le résultat n'est pas renvoyé, ni même la quantité de caractères traités. Dépose le résultat dans *buffer*.


**Paramètres :**
	- str : String (pointeur de char) issu d'une variable OU directement mis en entrée avec "...".
	- buffer : String où le résultat est remis.
	- start : N'importe quel entier d'où le découpage démarre.
	- stop : N'importe quel entier où se finit le découpage. A noter que si *end* est supérieur ou égal au nombre de caractères de str, alors end ne fait pas effet.

**Retourne :**
	*None*

```c
char * slice_str_r(const char * str, size_t start, size_t end)
{
	char * buffer;
	size_t j = 0;
	for (size_t i = start; i <= end; ++i ) {
	      buffer[j++] = str[i];
	}
	buffer[j] = 0;
	return buffer;
}
```
**Description :**
Découpe un string dont le début du découpage est *start* et la fin du découpage est *stop*. Le résultat est **renvoyé**, sous forme d'un tableau de caractères complet.

**Paramètres :**
	- str : String (pointeur de char) issu d'une variable OU directement mis en entrée avec "...".
	- start : N'importe quel entier d'où le découpage démarre.
	- stop : N'importe quel entier où se finit le découpage.

**Retourne :**
	char * dont la taille (en nombre de caractères) est exactement celle de *end - start*.

```c
uint8_t get_used_bytes(const char *source)
{
	for (int i = 0; i < 64; i++)
	{
		if (source[i] == '\r')
		{
			return i;
		}
	}
	return 0;
}
```
**Description :**
Donne le nombre de caractères qui sont réellement utilisés dans un tableau de caractères. La fin d'un tableau est signalée par un caractère EOL.

**Paramètres :**
	source : Tableau analysé.

**Retourne :**
	Taille en entier du nombre de caractères utilisés.

```c
void comm_SendData(uint32_t msvalue)
{
	//DATA_FORMAT : tx_data[x] = [ID_TX_TypeOfData]+[TypeOfData_0]+...+[TypeOfData_n]+[ID_TX_TypeOfData...]+...

	sprintf(tx_data,"%s;%i;%s;%.3f;%s;%.3f;%s;%i;%s;%.2f;\n\r", //Only 1 line for sprintf
			ID_TX_TIME, (int)HAL_GetTick(),//msvalue,
			ID_TX_ADC, acc_LastVoltage,
			ID_TX_ACC, acc_LastAccel,
			ID_TX_PWM, (int)PWM_Value,
			ID_TX_RPM, output_speed);

	uint8_t used_len = get_used_bytes(tx_data);

	HAL_UART_Transmit_DMA(&huart2,
			(uint8_t *)slice_str_r(tx_data, 0, used_len+2),
			(used_len+2));
}
```
**Description :**
Fonction principale du module **comm.c**.
Récupère les données, y associe des identifiants puis envoie les données.
Fait appel à *get_used_bytes()* pour déterminer la quantité de données à transmettre et ainsi ne transmettre que cette quantité, suivi de *slice_str_r* pour découper. S'appuie sur sprintf pour produire un string prêt à l'émission, puis l'émêt via DMA. Cette association permet de passer sous les 1ms de temps d'exécution pour cette fonction.


**Paramètres :**
	ms_value : uint32_t correspondant au temps de l'appel de la fonction pour l'émission de données.

**Retourne :**
	*None*

```c
void comm_SliceInput(const char * input, char * identifier, char *value)
{
	char tmp_value[RX_BUFFER_SIZE-4];
	char *tmp_slice;
	char *tmp_out_value;
	/* Slice input string from 0 to 3 == the identifier of shape 0xXX */
	slice_str(input, identifier, 0, 3);

	/* Slice input string from 5 to buffer size -1 == the value*/
	slice_str(input, tmp_value, 4, RX_BUFFER_SIZE-1); // rest of the string

	/* cast from char*[RX_BUFFER_SIZE-4], incompatible with strsep, to char * */
	tmp_slice = (char *)tmp_value;
	/* SEParate the string with ; as delimiter, keeps the string before ; */
	tmp_out_value = strsep(&tmp_slice, ";");
	strcpy(value,tmp_out_value);
}
```
**Description :**
	Fonction utilisé à la réception d'une trame depuis le PC.
	A partir de *input*, découpe *identifier* et *value*.


**Paramètres :**
	- input : String reçu, sans taille.
	- identifier : Variable externe à la fonction sensée recevoir l'identifiant du message.
	- value : Variable externe sensée recevoir la valeur du message. 

**Retourne :**
	*None*

```c
void comm_ComputeInput(char * identifier, char * value)
{
	/*
	 * str 1 == str 2 compare the address
	 * strcmp(str1,str2) compare the content
	 */
	if (strcmp(identifier,ID_RX_ACK) == 0)
	{
		comm_ack = 1;
	}
	else if (strcmp(identifier,ID_RX_START) == 0)
	{
		comm_start = 1;
		tickTimer_Start();
		comm_start = 0;
	}
	else if (strcmp(identifier,ID_RX_STOP) == 0)
		{
			comm_start = 1;
			tickTimer_Stop();
			comm_start = 0;
		}
	else if (strcmp(identifier,ID_RX_CALIBRATE) == 0)
	{
		comm_ack = 0;
		//acc_Calibrate();
	}
	else if (strcmp(identifier,ID_RX_CALIB_ZEROG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationZeroG();
	}
	else if (strcmp(identifier,ID_RX_CALIB_MINUSG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationMinusG();
	}
	else if (strcmp(identifier,ID_RX_CALIB_PLUSG) == 0)
	{
		comm_ack = 0;
		//acc_CalibrationPlusG();
	}
	else if (strcmp(identifier,ID_RX_STOP) == 0)
	{
		comm_stop = 1;
		tickTimer_Stop();
		comm_stop = 0;
	}
	else if (strcmp(identifier,ID_RX_PWM_START) == 0)
	{
		pwm_Enable();
	}
	else if (strcmp(identifier,ID_RX_PWM_STOP) == 0)
	{
		pwm_Disable();
	}
	else if (strcmp(identifier,ID_RX_DUTY_SET) == 0)
	{
		/* Data sanitization done in SetVoltage */
		pwm_SetDuty((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_DIR) == 0)
	{
		int tmp_dir = atoi(value);
		/* data sanitization */
		if (tmp_dir < 0)
		{
			tmp_dir = 0;
		}
		else if (tmp_dir > 1)
		{
			tmp_dir = 1;
		}
		/* Change of direction */
		if (tmp_dir == 0)
		{
				mcc_SetCCW();
		}
		else
		{
				mcc_SetCW();
		}
	}
	else if (strcmp(identifier,ID_RX_VOLTAGE_SET) == 0)
	{
		/* Data sanitization done in SetVoltage */
		pwm_SetVoltage((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KP) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKp((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KI) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKi((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_KD) == 0)
	{
		/* No data sanitization */
		pidFilter_SetKd((float)atof(value));
	}
	else if (strcmp(identifier,ID_RX_SET_MAG) == 0)
		{
			/* No data sanitization */
			mcc_SetMagnet(atoi(value));
		}
	else
	{

	}
}
```
**Description :**
Fonction de réaction appelée à la suite de *comm_SliceInput()*.
En fonction de l'identifiant, appelle la fonction correspondante à la fonctionnalité souhaitée. Utilise *strcmp()* pour choisir quelle réaction adopter, puisque cette fonction est très souple. *value* est utilisée si et si seulement la fonction est demandée.

**Paramètres :**
	- identifier : Identifiant du message. String.
	- value : Valeur. String

**Retourne :**
	*None*